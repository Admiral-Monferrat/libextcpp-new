//
//  pthread_allocator.h
//  libextc++
//
//  Created by 李志航 on 2022/9/8.
//

#ifndef libextcpp_pthread_allocator_h
#define libextcpp_pthread_allocator_h
#include <extc++/memory.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(concurrent)


#define CONCURRENT_DATA_ALIGNMENT 8


union pthread_alloc_object {
    union pthread_alloc_object* free_list_link;
    char client_data[CONCURRENT_DATA_ALIGNMENT];
};


template <size_t max_size>
class pthread_alloc_template;


template <size_t max_size>
struct pthread_alloc_per_thread_state {
    typedef pthread_alloc_object object;
    enum { NFREELISTS = max_size / CONCURRENT_DATA_ALIGNMENT };
    pthread_alloc_object * volatile free_list[NFREELISTS];
    pthread_alloc_per_thread_state<max_size>* next;
    
    pthread_alloc_per_thread_state() : next(nullptr) {
        memset((void *)free_list, 0, (size_t)NFREELISTS * sizeof(object*));
    }
    
    void* refill(size_t n) {
        int nobjs = 128;
        char * chunk = pthread_alloc_template<max_size>::chunk_alloc(n, nobjs);
        object * volatile * my_free_list;
        object * result;
        object * current_object, * next_object;
        int i;
        if (nobjs == 1)
            return chunk;
        my_free_list = free_list + pthread_alloc_template<max_size>::freelist_index(n);
        result = (object *)chunk;
        *my_free_list = next_object = (object *)(chunk + n);
        for (i = 1; ; i++) {
            current_object = next_object;
            next_object = (object *)((char *)next_object + n);
            if (i == nobjs - 1) {
                current_object->free_list_link = nullptr;
                break;
            } else {
                current_object->free_list_link = next_object;
            }
        }
        return result;
    }
};


template <size_t max_size = 128>
class pthread_alloc_template {
public:
    typedef pthread_alloc_object object;
    
    static char* chunk_alloc(size_t size, int& nobjs) {
        {
            char * result;
            size_t total_bytes;
            size_t bytes_left;
            lock lock_instance;
            total_bytes = size * nobjs;
            bytes_left = end_free - start_free;
            if (bytes_left >= total_bytes) {
                result = start_free;
                start_free += total_bytes;
                return(result);
            } else if (bytes_left >= size) {
                nobjs = int(bytes_left / size);
                total_bytes = size * nobjs;
                result = start_free;
                start_free += total_bytes;
                return(result);
            } else {
                size_t bytes_to_get = (total_bytes << 1) + round_up(heap_size >> 4);
                if (bytes_left > 0) {
                    pthread_alloc_per_thread_state<max_size>* state = (pthread_alloc_per_thread_state<max_size>*)pthread_getspecific(key);
                    object * volatile * my_free_list = state->free_list + freelist_index(bytes_left);
                    ((object *)start_free)->free_list_link = *my_free_list;
                    *my_free_list = (object *)start_free;
                }
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
                heap_size += bytes_to_get;
                end_free = start_free + bytes_to_get;
            }
        }
        return chunk_alloc(size, nobjs);
    }

    
    enum { ALIGN = CONCURRENT_DATA_ALIGNMENT };
    
    static size_t round_up(size_t bytes) {
        return (((bytes) + (int)ALIGN - 1) & ~((int)ALIGN - 1));
    }
    
    static size_t freelist_index(size_t bytes) {
        return (((bytes) + (int)ALIGN - 1) / ((int)ALIGN - 1));
    }
private:
    static pthread_mutex_t chunk_allocator_lock;
    static char *start_free;
    static char *end_free;
    static size_t heap_size;
    static pthread_alloc_per_thread_state<max_size>* free_per_thread_states;
    static pthread_key_t key;
    static bool key_initialized;
    
    static void destructor(void *instance) {
        lock lock_instance;
        pthread_alloc_per_thread_state<max_size>* state = (pthread_alloc_per_thread_state<max_size>*)instance;
        state->next = free_per_thread_states;
        free_per_thread_states = state;
    }
    
    static pthread_alloc_per_thread_state<max_size> *new_per_thread_state() {
        if (free_per_thread_states != nullptr) {
            pthread_alloc_per_thread_state<max_size>* result = free_per_thread_states;
            free_per_thread_states = free_per_thread_states->next;
            return result;
        } else {
            return new pthread_alloc_per_thread_state<max_size>;
        }
    }
    
    static pthread_alloc_per_thread_state<max_size> *get_per_thread_state() {
        lock lock_instance;
        int return_code;
        pthread_alloc_per_thread_state<max_size>* result;
        if (!key_initialized) {
            if (pthread_key_create(&key, destructor)) {
                throw std::bad_alloc();
            }
            key_initialized = true;
        }
        result = new_per_thread_state();
        return_code = pthread_setspecific(key, result);
        if (return_code) {
            if (return_code == ENOMEM) {
                throw std::bad_alloc();
            } else {
                abort();
            }
        }
        return result;
    }
    
    class lock;
    friend class lock;
    class lock {
    public:
        lock(){ pthread_mutex_lock(&chunk_allocator_lock); }
        ~lock(){ pthread_mutex_unlock(&chunk_allocator_lock); }
    };
public:
    static void* allocate(size_t n) {
        object * volatile * my_free_list;
        object * result;
        pthread_alloc_per_thread_state<max_size>* state;
        if (n > max_size)
            return ext::malloc_alloc::allocate(n);
        if (!key_initialized || !(state = (pthread_alloc_per_thread_state<max_size>*)pthread_getspecific(key)))
            state = get_per_thread_state();
        my_free_list = state->free_list + freelist_index(n);
        result = *my_free_list;
        if (result == nullptr) {
            void *r = state->refill(round_up(n));
            return r;
        }
        *my_free_list = result->free_list_link;
        return result;
    }
    
    static void deallocate(void *p, size_t n) {
        object *q = (object *)p;
        object * volatile * my_free_list;
        pthread_alloc_per_thread_state<max_size>* state;
        if (n > max_size) {
            ext::malloc_alloc::deallocate(p, n);
            return;
        }
        if (!key_initialized || !(state = (pthread_alloc_per_thread_state<max_size>*)pthread_getspecific(key)))
            state = get_per_thread_state();
        my_free_list = state->free_list + freelist_index(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
};


template <size_t max_size>
pthread_alloc_per_thread_state<max_size>* pthread_alloc_template<max_size>::free_per_thread_states = nullptr;


template <size_t max_size>
pthread_key_t pthread_alloc_template<max_size>::key;


template <size_t max_size>
bool pthread_alloc_template<max_size>::key_initialized = false;


template <size_t max_size>
pthread_mutex_t pthread_alloc_template<max_size>::chunk_allocator_lock = PTHREAD_MUTEX_INITIALIZER;


template <size_t max_size>
char * pthread_alloc_template<max_size>::start_free = nullptr;


template <size_t max_size>
char * pthread_alloc_template<max_size>::end_free = nullptr;


template <size_t max_size>
size_t pthread_alloc_template<max_size>::heap_size = 0;


typedef pthread_alloc_template<> pthread_alloc;


template <class Tp>
class pthread_allocator {
private:
    typedef pthread_alloc pool;
public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef Tp*         pointer;
    typedef const Tp*   const_pointer;
    typedef Tp&         reference;
    typedef const Tp*   const_reference;
    typedef Tp          value_type;
    
    template <class Up>
    struct rebind {
        typedef pthread_allocator<Up> other;
    };
    
    pthread_allocator() {}
    pthread_allocator(const pthread_allocator& other) {}
    template <class Up>
    pthread_allocator(const pthread_allocator<Up>&) {}
    ~pthread_allocator() {}
    
    Tp* allocate(size_type n) {
        return n != 0 ? static_cast<Tp*>(pool::allocate(n * sizeof(Tp))) : nullptr;
    }
    
    void deallocate(pointer p, size_type n) {
        pool::deallocate(p, n * sizeof(Tp));
    }
};


LIBEXTCPP_END_NAMESPACE /* namespace parallel */
LIBEXTCPP_END_NAMESPACE /* namespace ext */
#endif /* pthread_allocator_h */
