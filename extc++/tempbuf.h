//
//  tempbuf.h
//  Class & Object
//
//  Created by 李志航 on 2021/11/11.
//

#ifndef libextcpp_tempbuf_h
#define libextcpp_tempbuf_h
#include <extc++/memory.h>
#include <extc++/pair.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Tp>
ext::pair<Tp*, ptrdiff_t> get_temporary_buffer(ptrdiff_t length) LIBEXTCPP_NOEXCEPT {
    const ptrdiff_t max_size = std::numeric_limits<ptrdiff_t>::max() / sizeof(Tp);
    if (length > max_size)
        length = max_size;
    while (length > 0) {
        Tp* buffer = ext::allocate(length, static_cast<Tp*>(0));
        if (buffer != nullptr)
            return ext::pair<Tp*, ptrdiff_t>(buffer, length);
        length = length == 1 ? 0 : (length >> 1);
    }
    return ext::pair<Tp*, ptrdiff_t>(nullptr, 0);
}


template <class Tp>
LIBEXTCPP_FORCE_INLINE void return_temporary_buffer(Tp* p) {
    ext::deallocate(p);
}


template <bool Simple>
struct uninitialized_construct_buffer_switch {
    template <class Pointer, class ForwardIterator>
    static void initialize(Pointer first, Pointer last, ForwardIterator begin) {
        if (first != last) {
            Pointer current = first;
            try {
                ext::construct_at(ext::addressof(*first), ext::move(*begin));
                Pointer previous = current;
                ++current;
                for (; current != last; ++current, ++previous) {
                    ext::construct_at(ext::addressof(*current), ext::move(*previous));
                    *begin = ext::move(*previous);
                }
            } catch (...) {
                ext::destory(first, last);
                throw;
            }
        }
    }
};


template <>
struct uninitialized_construct_buffer_switch<true> {
    template <class Pointer, class ForwardIterator>
    static void initialize(Pointer, Pointer, ForwardIterator) {}
};


template <class Pointer, class ForwardIterator>
LIBEXTCPP_FORCE_INLINE void
uninitialized_construct_buffer(Pointer first, Pointer last, ForwardIterator begin) {
    ext::uninitialized_construct_buffer_switch <
        ext::is_trivially_constructible <
            typename ext::iterator_traits <
                ForwardIterator
            >::value_type
        >::value
    >::initialize(first, last, begin);
}


template <class ForwardIterator, class Tp>
class temporary_buffer {
public:
    typedef Tp value_type;
    typedef value_type* pointer;
    typedef pointer iterator;
    typedef ptrdiff_t size_type;
protected:
    size_type original_length;
    size_type length;
    pointer buffer;
public:
    
    temporary_buffer(ForwardIterator first, size_type request_length): original_length(request_length), length(0), buffer(nullptr) {
        ext::pair<pointer, size_type> ret(ext::get_temporary_buffer<value_type>(request_length));
        if (ret.first != nullptr) {
            try {
                ext::uninitialized_construct_buffer(ret.first, ret.first + ret.second, first);
                buffer = ret.first;
                length = ret.second;
            } catch (...) {
                ext::return_temporary_buffer(ret.first);
                throw;
            }
        }
    }
    
    ~temporary_buffer() {
        ext::destory(buffer, buffer + length);
        ext::return_temporary_buffer(buffer);
    }
    
    size_type size() const {
        return length;
    }
    
    size_type requested_size() const {
        return original_length;
    }
    
    iterator begin() {
        return buffer;
    }
    
    iterator end() {
        return buffer + length;
    }
};


LIBEXTCPP_END_NAMESPACE
#endif /* tempbuf_h */
