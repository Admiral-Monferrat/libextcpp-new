//
//  memory.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/3/13.
//

#ifndef libextcpp_memory_h
#define libextcpp_memory_h
#include <new>
#include <pthread.h>
#include <cstring>
#include <extc++/type_traits.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


using std::random_access_iterator_tag;


template <class Alloc>
class allocator_traits;


template <class Ip>
struct iterator_traits;


template <class Tp>
class allocator;


template <class Tp>
inline Tp* addressof(Tp& object) noexcept {
    return reinterpret_cast<Tp*>(const_cast<char*>(&reinterpret_cast<const volatile char &>(object)));
}


template <class T>
inline T* allocate(ptrdiff_t size, T*) {
    std::set_new_handler(nullptr);
    T* tmp = (T*)(::operator new(static_cast<size_t>(size * sizeof(T))));
    if (tmp == nullptr)
        throw std::bad_alloc();
    return tmp;
}


template <class T>
inline void deallocate(T* p) {
    ::operator delete(p);
}


//template <class Up, class... Args>
//inline void construct_at(Up* p, Args&& ...args) {
//    ::new((void*)p) Up(ext::forward<Args>(args)...);
//}

//I didn't get it , I was just in awe
template <class Up, class... Args>
inline void construct_at(Up* p, Args&& ...args) {
    ::new(const_cast<void*>(static_cast<const void*>(p))) Up(ext::forward<Args>(args)...);
}


template <class T>
inline void destory_at(T *pointer) {
    pointer->~T();
}


template <class Alloc, class Category, bool Trivial>
struct destory_switch {
    template <class ForwardIterator>
    static void destory(ForwardIterator first, ForwardIterator last, Alloc& alloc) {
        typedef allocator_traits<Alloc> alloc_traits;
        for (; first != last; ++first)
            alloc_traits::destory(alloc, ext::addressof(*first));
    }
};


template <class Alloc, bool Trivial>
struct destory_switch<Alloc, random_access_iterator_tag, Trivial> {
    template <class RandomAccessIterator>
    static void destory(RandomAccessIterator first, RandomAccessIterator last, Alloc& alloc) {
        typedef allocator_traits<Alloc> alloc_traits;
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        for (difference_type n = last - first; n > 0; --n) {
            alloc_traits::destory(alloc, ext::addressof(*first));
            ++first;
        }
    }
};


template <class Alloc, class Category>
struct destory_switch<Alloc, Category, true> {
    template <class ForwardIterator>
    static void destory(ForwardIterator first, ForwardIterator last, Alloc& alloc) {}
};


template <class ForwardIterator, class Alloc>
void destory(ForwardIterator first, ForwardIterator last, Alloc& alloc) {
    ext::destory_switch <
        Alloc,
        typename iterator_traits<ForwardIterator>::iterator_category,
        is_trivially_destructible<
            typename iterator_traits<ForwardIterator>::value_type
        >::value
    >::destory(first, last, alloc);
}


template <class ForwardIterator>
void destory(ForwardIterator first, ForwardIterator last) {
    allocator<typename iterator_traits<ForwardIterator>::value_type> alloc;
    ext::destory(first, last, alloc);
}


inline void destory(char*, char*) {}
inline void destory(wchar_t*, wchar_t*) {}
inline void destory(int*, int*) {}
inline void destory(double*, double*) {}
inline void destory(long*, long*) {}
inline void destory(float*, float*) {}


template <int inst>
class malloc_alloc_template {
private:
    static void *oom_malloc(size_t n) {
        void (* my_malloc_handler)();
        void *result;
        while (true) {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (my_malloc_handler == nullptr) { exit(1);}
            (*my_malloc_handler)();
            result = malloc(n);
            if (result) return result;
        }
    }
    static void *oom_realloc(void *p, size_t n) {
        void (* my_malloc_handler)();
        void *result;
        while (true) {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (my_malloc_handler == nullptr) { exit(1);}
            (*my_malloc_handler)();
            result = realloc(p, n);
            if (result) return result;
        }
    }
    static void (*__malloc_alloc_oom_handler)();
public:
    static void * allocate(size_t n) {
        void *result = malloc(n);
        //if (result == nullptr) result = oom_malloc(n);
        return result;
    }
    static void deallocate(void *p, size_t) { free(p); }
    static void * reallocate(void *p, size_t, size_t new_sz) {
        void *result = realloc(p, new_sz);
        //if (result == nullptr) result = oom_realloc(p, new_sz);
        return result;
    }
    static void (* set_malloc_handler(void(*f)()))() {
        void (* old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return (old);
    }
};

/* for single thread allocation only */
typedef malloc_alloc_template<0> malloc_alloc;


template <bool threads, int inst>
class pool_alloc {
private:
    enum {ALIGN = 8};
    enum {MAX_BYTES = 128};
    enum {NFREELISTS = MAX_BYTES / ALIGN};
    static size_t round_up(size_t bytes) {
        return (((bytes) + ALIGN - 1) & ~ (ALIGN - 1));
    }
private:
    union object {
        union object* free_list_link;
        char client_data[1];
    };
#ifdef LIBEXTCPP_THREADS
    friend class lock;
    class lock {
        lock() { pthread_mutex_lock(&chunk_allocator_mutex); }
        ~lock() { pthread_mutex_unlock(&chunk_allocator_mutex); }
    };
#endif
private:
    static object * volatile free_list[NFREELISTS];
#ifdef LIBEXTCPP_THREADS
    static pthread_mutex_t chunk_allocator_mutex;
#endif
    
    static size_t freelist_index(size_t bytes) {
        return (((bytes) + ALIGN - 1) / ALIGN - 1);
    }
    
    static void *refill(size_t n) {
        int nobjs = 20;
        char * chunk = chunk_alloc(n, nobjs);
        object * volatile * my_free_list;
        object * result;
        object * current_obj, * next_obj;
        int i;
        if (nobjs == 1) return chunk;
        my_free_list = free_list + freelist_index(n);
        result = (object *) chunk;
        *my_free_list = next_obj = (object *)(chunk + n);
        for (i = 1; ; i++) {
            current_obj = next_obj;
            next_obj = (object *)((char *)next_obj + n);
            if (i == nobjs - 1) {
                current_obj->free_list_link = nullptr;
                break;
            } else
                current_obj->free_list_link = next_obj;
        }
        return(result);
    }
    
    static char *chunk_alloc(size_t size, int &nobjs) {
        char * result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;
        if (bytes_left >= total_bytes) {
            result = start_free;
            start_free += total_bytes;
            return(result);
        } else if (bytes_left >= size) {
            nobjs = (int)(bytes_left / size);
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return(result);
        } else {
            size_t bytes_to_get = (total_bytes << 1) + round_up(heap_size >> 4);
            if (bytes_left > 0) {
                object * volatile * my_free_list = free_list + freelist_index(bytes_left);
                ((object *)start_free)->free_list_link = *my_free_list;
            }
            start_free = (char *)malloc(bytes_to_get);
            if (start_free == nullptr) {
                int i;
                object * volatile * my_free_list, *p;
                for (i = (int) size; i <= MAX_BYTES; i += ALIGN) {
                    my_free_list = free_list + freelist_index(i);
                    p = *my_free_list;
                    if (p != nullptr) {
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs);
                    }
                }
                end_free = nullptr;
                start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, nobjs);
        }
    }
    
    static char *start_free;
    static char *end_free;
    static size_t heap_size;
public:
    static void * allocate(size_t n) {
        object * volatile * my_free_list;
        object * result;
        if (n > (size_t) MAX_BYTES)
            return(malloc_alloc::allocate(n));
        my_free_list = free_list + freelist_index(n);
        result = *my_free_list;
        if (result == nullptr) {
            void *r = refill(round_up(n));
            return r;
        }
        *my_free_list = result->free_list_link;
        return(result);
    }
    
    static void deallocate(void *p, size_t n) {
        if (n > (size_t) MAX_BYTES)
            malloc_alloc::deallocate(p, n);
        else {
            object * volatile * my_free_list = free_list + freelist_index(n);
            object * q = (object*) p;
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
    }
    
    static void * reallocate(void *p, size_t old_sz, size_t new_sz) {
        void* result;
        size_t copy_size;
        if (old_sz > (size_t) MAX_BYTES && new_sz > (size_t) MAX_BYTES) return(realloc(p, new_sz));
        if (round_up(old_sz) == round_up(new_sz)) return p;
        result = allocate(new_sz);
        copy_size = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_size);
        deallocate(p, old_sz);
        return(result);
    }
};


template <bool threads, int inst>
char *pool_alloc<threads, inst>::start_free = nullptr;


template <bool threads, int inst>
char *pool_alloc<threads, inst>::end_free = nullptr;


template <bool threads, int inst>
size_t pool_alloc<threads, inst>::heap_size = 0;


template <bool threads, int inst>
typename pool_alloc<threads, inst>::object * volatile pool_alloc<threads, inst>::free_list[NFREELISTS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


#ifdef LIBEXTCPP_THREADS
template <bool threads, int inst>
pthread_mutex_t pool_alloc<threads, inst>::chunk_allocator_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


LIBEXTCPP_DEPRECATED typedef pool_alloc<0, 0> alloc;


template <class T>
class allocator {
private:
    typedef pool_alloc<0, 0>     pool;
public:
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;
    typedef allocator allocator_type;
    template <class Up> struct rebind {
        typedef allocator<Up> other;
    };
    allocator() {}
    allocator(const allocator&) {}
    template <class T1>
    allocator(const allocator<T1>&) {}
    ~allocator() {}
    LIBEXTCPP_DEPRECATED pointer address(reference value) {
        return &value;
    }
    LIBEXTCPP_DEPRECATED const_pointer address(const_reference value) const {
        return &value;
    }
    pointer allocate(size_type n) {
        return n != 0 ? static_cast<pointer>(pool::allocate(n * sizeof(T))) : nullptr;
    }
    void deallocate(pointer p, size_type n) {
        pool::deallocate(p, n);
    }
    size_type max_size() const {
        return size_t(~0) / sizeof(T);
    }
};


template <class T>
class new_allocator {
public:
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;
    typedef new_allocator   allocator_type;
    template <class Up> struct rebind {
        typedef allocator<Up> other;
    };
    new_allocator() {}
    new_allocator(const new_allocator&) {}
    template <class T1>
    new_allocator(const new_allocator<T1>&) {}
    ~new_allocator() {}
    LIBEXTCPP_DEPRECATED pointer address(reference value) { return &value;}
    LIBEXTCPP_DEPRECATED const_pointer address(const_reference value) const { return &value;}
    pointer allocate(size_type n, const void* = nullptr) {
        return n != 0 ? static_cast<pointer>(::operator new(n * sizeof(T))) : nullptr;
    }
    void deallocate(pointer p, size_type n) { ::operator delete(p);}
    size_type max_size() const { return size_t(-1) / sizeof(T);}
};


template <class T>
class malloc_allocator {
public:
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;
    typedef malloc_allocator   allocator_type;
    template <class Up> struct rebind {
        typedef allocator<Up> other;
    };
    malloc_allocator() {}
    malloc_allocator(const malloc_allocator&) {}
    template <class T1>
    malloc_allocator(const malloc_allocator<T1>&) {}
    ~malloc_allocator() {}
    LIBEXTCPP_DEPRECATED pointer address(reference value) { return &value;}
    LIBEXTCPP_DEPRECATED const_pointer address(const_reference value) const { return &value;}
    pointer allocate(size_type n) {
        return n != 0 ? static_cast<pointer>(malloc(n * sizeof(T))) : nullptr;
    }
    void deallocate(pointer p, size_type n) { free(p);}
    size_type max_size() const { return size_t(-1) / sizeof(T);}
};


template <class Tp>
struct aligned_membuf {
    struct Tp2 { Tp t; };
    alignas(__alignof__(Tp2::t)) unsigned char storage[sizeof(Tp)];
    aligned_membuf() = default;
    aligned_membuf(std::nullptr_t) { }
    void* address() noexcept {
        return static_cast<void*>(&storage);
    }
    const void* address() const noexcept {
        return static_cast<const void*>(&storage);
    }
    Tp* pointer() noexcept {
        return static_cast<Tp*>(address());
    }
    const Tp* pointer() const noexcept {
        return static_cast<const Tp*>(address());
    }
};


LIBEXTCPP_END_NAMESPACE
#endif /* memory_h */
