//
//  algobase.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/1/16.
//

#ifndef libextcpp_algobase_h
#define libextcpp_algobase_h
#include <extc++/numeric_traits.h>
#include <extc++/functional.h>
#include <extc++/iterator.h>
#include <extc++/tempbuf.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Tp>
LIBEXTCPP_FORCE_INLINE void swap(Tp& x, Tp& y) {
    Tp t(ext::move(x));
    x = ext::move(y);
    y = ext::move(t);
}


template <class Ip>
LIBEXTCPP_FORCE_INLINE void iter_swap(Ip i, Ip j) {
    ext::swap(*i, *j);
}


template <class Tp, class Compare>
LIBEXTCPP_FORCE_INLINE const Tp& max(const Tp& a, const Tp& b, Compare comp) {
    return comp(a, b) ? b : a;
}


template <class Tp>
LIBEXTCPP_FORCE_INLINE const Tp& max(const Tp& a, const Tp& b) {
    return ext::max(a, b, less<Tp>());
}


template <class Tp, class Compare>
LIBEXTCPP_FORCE_INLINE const Tp& min(const Tp& a, const Tp& b, Compare comp) {
    return comp(a, b) ? a : b;
}


template <class Tp, class Compare = less<Tp>>
LIBEXTCPP_FORCE_INLINE const Tp& min(const Tp& a, const Tp& b) {
    return ext::min(a, b, less<Tp>());
}


template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    for (; n > 0; (void) (--n), ++first) *first = value;
    return first;
}


template <class ForwardIterator, class Tp>
typename enable_if<!is_scalar<Tp>::value>::type
fill(ForwardIterator first, ForwardIterator last, const Tp& value) {
    for (; first != last; ++first)
        *first = value;
}


template <class ForwardIterator, class Tp>
typename enable_if<is_scalar<Tp>::value>::type
fill(ForwardIterator first, ForwardIterator last, const Tp& value) {
    const Tp tmp = value;
    for (; first != last; ++first)
        *first = tmp;
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE typename enable_if<is_byte<Tp>::value>::type
fill(ForwardIterator first, ForwardIterator last, const Tp& value) {
    const size_t length = last - first;
    const Tp tmp = value;
    if (length > 0)
        memset(first, static_cast<unsigned char>(tmp), length);
}


template <class Size>
LIBEXTCPP_FORCE_INLINE unsigned char*
fill_n(unsigned char* first, Size n, const unsigned char& c) {
    fill(first, first + n, c);
    return first + n;
}


template <class Size>
LIBEXTCPP_FORCE_INLINE signed char* fill_n(signed char* first, Size n, const signed char& c) {
    fill(first, first + n, c);
    return first + n;
}


template <class Size>
LIBEXTCPP_FORCE_INLINE char* fill_n(char* first, Size n, const char& c) {
    fill(first, first + n, c);
    return first + n;
}


template <class Tp>
struct is_nonvolatile_trivially_copyable : public is_trivially_copyable<Tp> {};


template <class Tp>
struct is_nonvolatile_trivially_copyable<volatile Tp> : public false_type {};


template <class InputIterator, class OutputIterator>
struct is_memcpyable : public false_type {};


template <class Tp>
struct is_memcpyable<Tp*, Tp*> : public is_nonvolatile_trivially_copyable<Tp> {};


template <class Tp>
struct is_memcpyable<Tp*, const Tp*> : public is_nonvolatile_trivially_copyable<Tp> {};


template <bool Move, bool Simple, class Category>
struct copy_move {
    template <class InputIterator, class OutputIterator>
    static OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        for (; first != last; ++first, ++result)
            *result = *first;
        return result;
    }
};


template <class Category>
struct copy_move<true, false, Category> {
    template <class InputIterator, class OutputIterator>
    static OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        for (; first != last; ++first, ++result)
            *result = ext::move(*first);
        return result;
    }
};


template <>
struct copy_move<false, false, random_access_iterator_tag> {
    template <class RandomAccessIterator, class OutputIterator>
    static OutputIterator copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result) {
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        for (difference_type n = last - first; n > 0; --n) {
            *result = *first;
            ++first;
            ++result;
        }
        return result;
    }
};


template <>
struct copy_move<true, false, random_access_iterator_tag> {
    template <class RandomAccessIterator, class OutputIterator>
    static OutputIterator copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result) {
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        for (difference_type n = last - first; n > 0; --n) {
            *result = ext::move(*first);
            ++first;
            ++result;
        }
        return result;
    }
};


template <bool Move>
struct copy_move<Move, true, random_access_iterator_tag> {
    template <class Tp>
    static Tp* copy(const Tp* first, const Tp* last, Tp* result) {
        const ptrdiff_t length = last - first;
        if (length != 0)
            memmove(result, first, sizeof(Tp) * length);
        return result + length;
    }
};


template <bool Move, class InputIterator, class OutputIterator, class Category>
LIBEXTCPP_FORCE_INLINE OutputIterator
copy_move_dispatch(InputIterator first, InputIterator last, OutputIterator result, Category cat) {
    return copy_move <
        Move,
        is_memcpyable <
            InputIterator,
            OutputIterator
        >::value,
        Category
    >::copy(first, last, result);
}


template <class InputIterator, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
copy(InputIterator first, InputIterator last, OutputIterator result) {
    return ext::copy_move_dispatch<false>(first, last, result, iterator_category(first));
}


template <class InputIterator, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
move(InputIterator first, InputIterator last, OutputIterator result) {
    return ext::copy_move_dispatch<true>(first, last, result, iterator_category(first));
}


template <bool Move, bool Simple, class Category>
struct copy_move_backward {
    template <class InputIterator, class OutputIterator>
    static OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        while (last != first)
            *--result = *--last;
        return result;
    }
};


template <class Category>
struct copy_move_backward<true, false, Category> {
    template <class InputIterator, class OutputIterator>
    static OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        while (last != first)
            *--result = ext::move(*--last);
        return result;
    }
};


template <>
struct copy_move_backward<false, false, random_access_iterator_tag> {
    template <class RandomAccessIterator, class OutputIterator>
    static OutputIterator copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result) {
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        for (difference_type n = last - first; n > 0; --n)
            *--result = *--first;
        return result;
    }
};


template <>
struct copy_move_backward<true, false, random_access_iterator_tag> {
    template <class RandomAccessIterator, class OutputIterator>
    static OutputIterator copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result) {
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        for (difference_type n = last - first; n > 0; --n)
            *--result = ext::move(*--last);
        return result;
    }
};


template <bool Move>
struct copy_move_backward<Move, true, random_access_iterator_tag> {
    template <class Tp>
    static Tp* copy(const Tp* first, const Tp* last, Tp* result) {
        const ptrdiff_t length = last - first;
        if (length != 0)
            memmove(result - length, first, sizeof(Tp) * length);
        return result - length;
    }
};


template <bool Move, class InputIterator, class OutputIterator, class Category>
LIBEXTCPP_FORCE_INLINE OutputIterator
copy_move_backward_dispatch(InputIterator first, InputIterator last, OutputIterator result, Category cat) {
    return copy_move_backward <
        Move,
        is_memcpyable <
            InputIterator,
            OutputIterator
        >::value,
        Category
    >::copy(first, last, result);
}


template <class InputIterator, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
copy_backward(InputIterator first, InputIterator last, OutputIterator result) {
    return ext::copy_move_backward_dispatch<false>(first, last, result, iterator_category(first));
}


template <class InputIterator, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
move_backward(InputIterator first, InputIterator last, OutputIterator result) {
    return ext::copy_move_backward_dispatch<true>(first, last, result, iterator_category(first));
}


template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2) {
    for (; first1 != last1; ++first1, ++first2)
        ext::iter_swap(first1, first2);
    return first2;
}


template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result) {
    while (first != last) {
        --last;
        *result = *last;
        ++result;
    }
    return result;
}


template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_move(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result) {
    while (first != last) {
        --last;
        *result = ext::move(*last);
        ++result;
    }
    return result;
}


template <class ForwardIterator, class OutputIterator, class Integer>
OutputIterator reverse_move_n(ForwardIterator first, const Integer& count, OutputIterator result) {
    while (count > 0) {
        *result = ext::move(*first);
        ++first;
        --result;
        --count;
    }
    return result;
}


template <class ForwardIterator, class Distance, class Pointer>
void reverse_adaptive_resize(ForwardIterator first, Distance length, Pointer buffer, Distance buffer_size) {
    if (length > 1) {
        ForwardIterator last = first;
        if (length <= buffer_size) {
            ext::reverse_move_n(first, length, buffer + length);
            ext::move(buffer, buffer + length, first);
        } else {
            Distance half = (length >> 1);
            ext::advance(last, half);
            ForwardIterator first2 = last;
            if ((length & 1) != 0)
                ++first2;
            ext::swap_ranges(first, last, first2);
            ext::reverse_adaptive_resize(first, half, buffer, buffer_size);
            ext::reverse_adaptive_resize(first2, half, buffer, buffer_size);
        }
    }
}


template <class ForwardIterator, class Distance, class Tp>
void reverse_adaptive(ForwardIterator first, ForwardIterator last, Distance length, Tp*) {
    temporary_buffer<ForwardIterator, Tp> buffer(first, length);
    ext::reverse_adaptive_resize(first, length, buffer.begin(), buffer.size());
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE void
reverse_switch(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    ext::reverse_adaptive(first, last, ext::distance(first, last), value_type(first));
}


template <class BidirectionalIterator>
void reverse_switch(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag) {
    while (true) {
        if (first == last || first == --last)
            return;
        else {
            ext::iter_swap(first, last);
            ++first;
        }
    }
}


template <class RandomAccessIterator>
void reverse_switch(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    if (first == last--)
        return;
    while (first < last) {
        ext::iter_swap(first, last);
        ++first;
        --last;
    }
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE void reverse(ForwardIterator first, ForwardIterator last) {
    ext::reverse_switch(first, last, iterator_category(first));
}


LIBEXTCPP_END_NAMESPACE
#endif /* algobase_h */
