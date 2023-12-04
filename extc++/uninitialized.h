//
//  uninitialized.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/1/18.
//

#ifndef libextcpp_tree_uninitialized_h
#define libextcpp_tree_uninitialized_h
#include <extc++/config.h>
#include <extc++/type_traits.h>
#include <extc++/algobase.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Alloc, class Category, bool Trivial>
struct uninitialized_copy_switch {
    template <class InputIterator, class ForwardIterator>
    static ForwardIterator copy(InputIterator first, InputIterator last, ForwardIterator result, Alloc& alloc) {
        typedef allocator_traits<Alloc> alloc_traits;
        ForwardIterator current = result;
        try {
            for (; first != last; ++first, ++current)
                alloc_traits::construct(alloc, ext::addressof(*current), *first);
            return current;
        } catch (...) {
            ext::destory(result, current, alloc);
            throw;
        }
    }
};


template <class Alloc, bool Trivial>
struct uninitialized_copy_switch<Alloc, random_access_iterator_tag, Trivial> {
    template <class RandomAccessIterator, class ForwardIterator>
    static ForwardIterator copy(RandomAccessIterator first, RandomAccessIterator last, ForwardIterator result, Alloc& alloc) {
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        typedef allocator_traits<Alloc> alloc_traits;
        ForwardIterator current = result;
        try {
            for (difference_type n = last - first; n > 0; --n) {
                alloc_traits::construct(alloc, ext::addressof(*current), *first);
                ++first;
                ++current;
            }
            return current;
        } catch (...) {
            ext::destory(result, current, alloc);
            throw;
        }
    }
};


template <class Alloc>
struct uninitialized_copy_switch<Alloc, random_access_iterator_tag, true> {
    template <class InputIterator, class ForwardIterator>
    static ForwardIterator copy(InputIterator first, InputIterator last, ForwardIterator result, Alloc& alloc) {
        return ext::copy(first, last, result);
    }
};


template <class InputIterator, class ForwardIterator, class Alloc>
LIBEXTCPP_FORCE_INLINE ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, Alloc& alloc) {
    return ext::uninitialized_copy_switch <
        Alloc,
        typename iterator_traits<InputIterator>::iterator_category,
        is_trivial <
            typename iterator_traits<InputIterator>::value_type
        >::value &&
        is_trivial <
            typename iterator_traits<ForwardIterator>::value_type
        >::value
    >::copy(first, last, result, alloc);
}


template <class InputIterator, class ForwardIterator>
LIBEXTCPP_FORCE_INLINE ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    allocator<typename iterator_traits<ForwardIterator>::value_type> alloc;
    return ext::uninitialized_copy(first, last, result, alloc);
}


template <class Alloc, class Category, bool Trivial>
struct uninitialized_fill_switch {
    template <class ForwardIterator, class Tp>
    static void fill(ForwardIterator first, ForwardIterator last, const Tp& value, Alloc& alloc) {
        typedef allocator_traits<Alloc> alloc_traits;
        ForwardIterator current = first;
        try {
            for (; first != last; ++first)
                alloc_traits::construct(alloc, ext::addressof(*current), value);
        } catch (...) {
            ext::destory(first, current, alloc);
            throw;
        }
    }
};


template <class Alloc, bool Trivial>
struct uninitialized_fill_switch<Alloc, random_access_iterator_tag, Trivial> {
    template <class RandomAccessIterator, class Tp>
    static void fill(RandomAccessIterator first, RandomAccessIterator last, const Tp& value, Alloc& alloc) {
        typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
        typedef allocator_traits<Alloc> alloc_traits;
        RandomAccessIterator current = first;
        try {
            for (difference_type n = last - first; n > 0; --n) {
                alloc_traits::construct(alloc, ext::addressof(*current), value);
                ++current;
            }
        } catch (...) {
            ext::destory(first, current, alloc);
            throw;
        }
    }
};


template <class Alloc>
struct uninitialized_fill_switch<Alloc, random_access_iterator_tag, true> {
    template <class RandomAccessIterator, class Tp>
    static void fill(RandomAccessIterator first, RandomAccessIterator last, const Tp& value, Alloc& alloc) {
        ext::fill(first, last, value);
    }
};


template <class ForwardIterator, class Tp, class Alloc>
LIBEXTCPP_FORCE_INLINE void
uninitialized_fill(ForwardIterator first, ForwardIterator last, const Tp& value, Alloc& alloc) {
    ext::uninitialized_fill_switch <
        Alloc,
        typename iterator_traits<ForwardIterator>::iterator_category,
        is_trivial <
            typename iterator_traits<ForwardIterator>::value_type
        >::value
    >::fill(first, last, value, alloc);
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE void
uninitialized_fill(ForwardIterator first, ForwardIterator last, const Tp& value) {
    allocator<Tp> alloc;
    ext::uninitialized_fill(first, last, value, alloc);
}


LIBEXTCPP_END_NAMESPACE
#endif /* uninitialized_h */
