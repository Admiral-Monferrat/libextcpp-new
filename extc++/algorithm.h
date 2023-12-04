//
//  algorithm.h
//  GLIBCXX
//
//  Created by 李志航 on 2021/1/12.
//


//force-inline policy: if a function contains virtually one function call, then
//it's marked by LIBEXTCPP_FORCE_INLINE

#ifndef libextcpp_algorithm_h
#define libextcpp_algorithm_h
#include <extc++/predefined_ops.h>
#include <extc++/algobase.h>
#include <extc++/array.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


enum algorithm_settings
{
    
    inplace_stable_sort_minimal_length = 15,

    sort_minimal_length = 12,

    stable_sort_chunk_size = 7

};


template <class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator&& generator) {
    for (; first != last; ++first)
        *first = generator();
}


template <class RandomAccessIterator, class Predicate, class Distance>
RandomAccessIterator find_if_loop_unrolled(RandomAccessIterator first, RandomAccessIterator last, Predicate predicate, Distance trip_count) {
    for (; trip_count > 0; --trip_count) {
        if (predicate(*first))
            return first;
        ++first;
        if (predicate(*first))
            return first;
        ++first;
        if (predicate(*first))
            return first;
        ++first;
        if (predicate(*first))
            return first;
        ++first;
        switch (last - first) {
            case 3:
                if (predicate(*first))
                    return first;
                ++first;
            case 2:
                if (predicate(*first))
                    return first;
                ++first;
            case 1:
                if (predicate(*first))
                    return first;
                ++first;
            case 0:
            default:
                return last;
        }
    }
    return last;
}


template <class RandomAccessIterator, class Predicate>
LIBEXTCPP_FORCE_INLINE RandomAccessIterator
find_if_switch(RandomAccessIterator first, RandomAccessIterator last, Predicate predicate, random_access_iterator_tag) {
    return ext::find_if_loop_unrolled(first, last, predicate, (last - first) >> 2);
}


template <class InputIterator, class Predicate>
InputIterator find_if_switch(InputIterator first, InputIterator last, Predicate predicate, input_iterator_tag) {
    while (first != last && !predicate(*first))
        ++first;
    return first;
}


template <class InputIterator, class Predicate>
LIBEXTCPP_FORCE_INLINE InputIterator
find_if(InputIterator first, InputIterator last, Predicate predicate) {
    return ext::find_if_switch(first, last, predicate, iterator_category(first));
}


template <class InputIterator, class Predicate>
LIBEXTCPP_FORCE_INLINE InputIterator
find_if_not(InputIterator first, InputIterator last, Predicate predicate) {
    return ext::find_if(first, last, ops::negate(predicate));
}


template <class InputIterator, class Predicate, class Distance>
InputIterator find_if_not_n(InputIterator first, Distance& n, Predicate predicate) {
    for (; n; --n, (void) ++first)
        if (!predicate(*first))
            break;
    return first;
}


template <class InputIterator, class Tp>
LIBEXTCPP_FORCE_INLINE InputIterator find(InputIterator first, InputIterator last, const Tp& value) {
    return ext::find_if(first, last, ops::binder2nd(ops::equal_to(), value));
}


template <class CorpusIterator, class PatternIterator, class BinaryPredicate>
CorpusIterator default_search(CorpusIterator corpus_begin, CorpusIterator corpus_end, PatternIterator pattern_begin, PatternIterator pattern_end, BinaryPredicate predicate) {
    if (corpus_begin == corpus_end || pattern_begin == pattern_end)
        return corpus_begin;
    PatternIterator pattern_scan(pattern_begin);
    if (++pattern_scan == pattern_end)
        return ext::find_if(corpus_begin, corpus_end, ops::binder2nd(predicate, *pattern_begin));
    CorpusIterator current = corpus_begin;
    while (true) {
        corpus_begin = ext::find_if(corpus_begin, corpus_end, ops::binder2nd(predicate, *pattern_begin));
        if (corpus_begin == corpus_end)
            return corpus_end;
        PatternIterator scan = pattern_scan;
        current = corpus_begin;
        if (++current == corpus_end)
            return corpus_end;
        while (predicate(*current, *scan)) {
            if (++scan == pattern_end)
                return corpus_begin;
            if (++current == corpus_end)
                return corpus_end;
            ++corpus_begin;
        }
    }
    return corpus_begin;
}


template <class CorpusIterator, class PatternIterator, class BinaryPredicate>
LIBEXTCPP_FORCE_INLINE CorpusIterator
search(CorpusIterator corpus_begin, CorpusIterator corpus_end, PatternIterator pattern_begin, PatternIterator pattern_end, BinaryPredicate predicate) {
    return ext::default_search(corpus_begin, corpus_end, pattern_begin, pattern_end, predicate);
}


template <class CorpusIterator, class PatternIterator>
LIBEXTCPP_FORCE_INLINE CorpusIterator
search(CorpusIterator corpus_begin, CorpusIterator corpus_end, PatternIterator pattern_begin, PatternIterator pattern_end) {
    return ext::default_search(corpus_begin, corpus_end, pattern_begin, pattern_end, ops::equal_to());
}


template <class ForwardIterator, class Predicate, class Integer, class Distance>
ForwardIterator search_n_switch(ForwardIterator first, ForwardIterator last, Predicate predicate, const Integer& count, Distance*, forward_iterator_tag) {
    first = ext::find_if(first, last, predicate);
    while (first != last) {
        Distance n = count;
        ForwardIterator i = first;
        ++i;
        while (i != last && n != 1 && predicate(*i)) {
            ++i;
            --n;
        }
        if (n == 1)
            return first;
        if (i == last)
            return last;
        first = ext::find_if(++i, last, predicate);
    }
    return last;
}


template <class RandomAccessIterator, class Predicate, class Integer, class Distance>
RandomAccessIterator search_n_switch(RandomAccessIterator first, RandomAccessIterator last, Predicate predicate, const Integer& count, Distance*, random_access_iterator_tag) {
    Distance tail_size = last - first;
    Distance remainder = count;
    while (remainder <= tail_size) {
        first += remainder;
        tail_size -= remainder;
        RandomAccessIterator back_track = first;
        while (predicate(*--back_track)) {
            --remainder;
            if (remainder == 0)
                return first - count;
        }
        remainder = count + 1 - (first - back_track);
    }
    return last;
}


template <class ForwardIterator, class Predicate, class Integer>
LIBEXTCPP_FORCE_INLINE ForwardIterator
search_n(ForwardIterator first, ForwardIterator last, Predicate predicate, const Integer& count) {
    return ext::search_n_switch(first, last, predicate, count, distance_type(first), iterator_category(first));
}


template <class CorpusIterator, class PatternIterator, class BinaryPredicate>
CorpusIterator find_end_switch(CorpusIterator corpus_begin,
                               CorpusIterator corpus_end,
                               PatternIterator pattern_begin,
                               PatternIterator pattern_end,
                               BinaryPredicate predicate,
                               forward_iterator_tag, forward_iterator_tag) {
    if (pattern_begin == pattern_end)
        return corpus_end;
    CorpusIterator result = corpus_end;
    while (true) {
        CorpusIterator new_result = ext::search(corpus_begin, corpus_end, pattern_begin, pattern_end, predicate);
        if (new_result == corpus_end)
            return result;
        else {
            result = new_result;
            corpus_begin = new_result;
            ++corpus_begin;
        }
    }
}


template <class CorpusIterator, class PatternIterator, class BinaryPredicate>
CorpusIterator find_end_switch(CorpusIterator corpus_begin,
                               CorpusIterator corpus_end,
                               PatternIterator pattern_begin,
                               PatternIterator pattern_end,
                               BinaryPredicate predicate,
                               bidirectional_iterator_tag, bidirectional_iterator_tag) {
    reverse_iterator<CorpusIterator> reverse_result;
    reverse_result = ext::search(ext::make_reverse_iterator(corpus_end),
                                 ext::make_reverse_iterator(corpus_begin),
                                 ext::make_reverse_iterator(pattern_end),
                                 ext::make_reverse_iterator(pattern_begin),
                                 predicate);
    if (reverse_result != ext::make_reverse_iterator(corpus_begin))
        return ext::next(reverse_result.base(), -ext::distance(pattern_begin, pattern_end));
    else return corpus_end;
}


template <class CorpusIterator, class PatternIterator, class BinaryPredicate>
LIBEXTCPP_FORCE_INLINE CorpusIterator
find_end(CorpusIterator corpus_begin, CorpusIterator corpus_end, PatternIterator pattern_begin, PatternIterator pattern_end, BinaryPredicate predicate) {
    return ext::find_end_switch(corpus_begin, corpus_end, pattern_begin, pattern_end, predicate, iterator_category(corpus_begin), iterator_category(pattern_begin));
}


template <class ForwardIterator, class Predicate>
typename iterator_traits<ForwardIterator>::difference_type
count_if(ForwardIterator first, ForwardIterator last, Predicate predicate) {
    typename ext::iterator_traits<ForwardIterator>::difference_type n = 0;
    for (; first != last; ++first)
        if (predicate(*first))
            ++n;
    return n;
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE typename iterator_traits<ForwardIterator>::difference_type
count(ForwardIterator first, ForwardIterator last, const Tp& value) {
    return ext::count_if(first, last, ops::binder2nd(ops::equal_to(), value));
}


template <class ForwardIterator, class Compare>
std::pair<ForwardIterator, ForwardIterator>
minmax_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    ForwardIterator next = ext::next(first);
    if (first == last || next == last)
        return std::make_pair(first, first);
    ForwardIterator min, max;
    if (comp(*next, *first)) {
        min = next;
        max = first;
    } else {
        min = first;
        max = next;
    }
    first = next;
    ++first;
    while (first != last) {
        next = first;
        if (++next == last) {
            if (comp(*first, *min))
                min = first;
            else if (!comp(*first, *max))
                max = first;
            break;
        }
        if (comp(*next, *first)) {
            if (comp(*next, *min))
                min = next;
            if (!comp(*first, *max))
                max = first;
        } else {
            if (comp(*first, *min))
                min = first;
            if (!comp(*next, *max))
                max = next;
        }
        first = next;
        ++first;
    }
    return std::make_pair(min, max);
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE std::pair<ForwardIterator, ForwardIterator>
minmax_element(ForwardIterator first, ForwardIterator last) {
    return ext::minmax_element(first, last, ops::less());
}


template <class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate predicate) {
    first = ext::find_if(first, last, predicate);
    if (first == last)
        return first;
    ForwardIterator result = first;
    ++first;
    for (; first != last; ++first) {
        if (!predicate(*first)) {
            *result = ext::move(*first);
            ++result;
        }
    }
    return result;
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
remove(ForwardIterator first, ForwardIterator last, const Tp& value) {
    return ext::remove_if(first, last, ops::binder2nd(ops::equal_to(), value));
}


template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate) {
    if (first == last)
        return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (predicate(first, next))
            return first;
        first = next;
    }
    return last;
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
adjacent_find(ForwardIterator first, ForwardIterator last, const Tp& value) {
    return ext::adjacent_find(first, last, ops::binder2nd(ops::equal_to(), value));
}


template <class ForwardIterator, class BinaryPredicate>
ForwardIterator unique(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate) {
    first = ext::adjacent_find(first, last, predicate);
    if (first == last)
        return last;
    ForwardIterator result = first;
    ++first;
    while (++first != last)
        if (!predicate(*result, *first))
            *++result = ext::move(*first);
    return ++result;
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE ForwardIterator
unique(ForwardIterator first, ForwardIterator last) {
    return ext::unique(first, last, ops::equal_to());
}


template <class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    ForwardIterator result = first;
    if (first != last)
        while (++first != last)
            if (comp(*first, *result))
                result = first;
    return result;
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
    return ext::min_element(first, last, ops::less());
}


template <class ForwardIterator, class Compare>
LIBEXTCPP_FORCE_INLINE ForwardIterator
max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    return ext::min_element(first, last, ops::invert(comp));
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
    return ext::max_element(first, last, ops::less());
}


template <class RandomAccessIterator, class Distance, class Compare>
RandomAccessIterator is_heap_until_aux(RandomAccessIterator root, Distance length, Compare comp) {
    Distance parent = 0;
    for (Distance child = 1; child < length; ++child) {
        if (comp(*(root + parent), *(root + child)))
            return root + child;
        if ((child & 1) == 0)
            ++parent;
    }
    return root + length;
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE RandomAccessIterator
is_heap_until(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    return ext::is_heap_until_aux(first, last - first, comp);
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE RandomAccessIterator
is_heap_until(RandomAccessIterator first, RandomAccessIterator last) {
    return ext::is_heap_until_aux(first, last - first, ops::less());
}


//template <class RandomAccessIterator, class Distance, class Compare, class Tp>
//void shift_up(RandomAccessIterator root, RandomAccessIterator start, Distance length, Compare comp, Tp*) {
//    if (length > 1) {
//        length = (length - 2) >> 1;
//        RandomAccessIterator parent = root + length;
//        if (comp(*parent, *start)) {
//            Tp top(ext::move(*start));
//            do {
//                *parent = ext::move(*start);
//                start = parent;
//                if (!length) break;
//                length = (length - 2) >> 1;
//                parent = root + length;
//            } while (comp(*parent, top));
//            *start = ext::move(top);
//        }
//    }
//}


template <class RandomAccessIterator, class Distance, class Compare, class Tp>
void shift_up(RandomAccessIterator root, Distance start, Compare comp, Tp value) {
    Distance parent = (start - 1) >> 1;
    while (start > 0 && comp(*(root + parent), value)) {
        *(root + start) = ext::move(*(root + parent));
        start = parent;
        parent = (parent - 1) >> 1;
    }
    *(root + start) = ext::move(value);
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    ext::shift_up(first, last - first - 1, comp, ext::move(*(last - 1)));
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    ext::shift_up(first, last - 1, last - first - 1, ops::less(), value_type(first));
}


template <class RandomAccessIterator, class Distance, class Compare, class Tp>
void shift_down(RandomAccessIterator root, RandomAccessIterator start, Distance length, Compare comp, Tp* hint) {
    Distance index = start - root;
    if (length < 2 || index > ((length - 2) >> 1)) return;
    index = (index << 1) + 1;
    RandomAccessIterator child = root + index;
    if (index + 1 < length && comp(*child, *(child + 1))) {
        ++index;
        ++child;
    }
    if (comp(*child, *start)) return;
    Tp top(ext::move(*start));
    do {
        *start = ext::move(*child);
        start = child;
        if (index > ((length - 2) >> 1)) break;
        index = (index << 1) + 1;
        child = root + index;
        if (index + 1 < length && comp(*child, *(child + 1))) {
            ++index;
            ++child;
        }
    } while (!comp(*child, top));
    *start = ext::move(top);
}


template <class RandomAccessIterator, class Distance, class Compare>
void adjust_heap(RandomAccessIterator first, RandomAccessIterator last, Distance length, Compare comp) {
    if (length > 1) {
        ext::iter_swap(first, last - 1);
        ext::shift_down(first, first, length - 1, comp, value_type(first));
    }
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    ext::adjust_heap(first, last, last - first, comp);
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE void
pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    ext::adjust_heap(first, last, last - first, ops::less());
}


template <class RandomAccessIterator, class Distance, class Compare>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, Compare comp) {
    Distance length = last - first;
    if (length > 1)
        for (Distance start = ((length - 2) >> 1); start >= 0; --start)
            ext::shift_down(first, first + start, length, comp, value_type(first));
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    ext::make_heap_aux(first, last, distance_type(first), comp);
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    ext::make_heap_aux(first, last, distance_type(first), ops::less());
}


template <class RandomAccessIterator, class Distance, class Compare>
void sort_heap_loop(RandomAccessIterator first, RandomAccessIterator last, Distance*, Compare comp) {
    while (last > first)
        ext::pop_heap(first, last--, comp);
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    ext::sort_heap_loop(first, last, distance_type(first), comp);
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    ext::sort_heap(first, last, ops::less());
}


template <class Ip, class Compare>
LIBEXTCPP_FORCE_INLINE void
move_median_to(const Ip& pivot, const Ip& middle, const Ip& last, Ip& target, Compare comp) {
    if (comp(*pivot, *middle)) {
        if (comp(*middle, *last))
            ext::iter_swap(middle, target);
        else if (comp(*pivot, *last))
            ext::iter_swap(last, target);
        else
            ext::iter_swap(pivot, target);
    } else if (comp(*pivot, *last))
        ext::iter_swap(pivot, target);
    else
        ext::iter_swap(middle, target);
}


template <class Tp, class Compare>
LIBEXTCPP_FORCE_INLINE const Tp& median(const Tp& a, const Tp& b, const Tp& c, Compare comp) {
    if (comp(a, b)) {
        if (!comp(b, c))
            return b;
        else if (!comp(a, c))
            return c;
        else
            return a;
    } else {
        if (!comp(a, c))
            return a;
        else if (!comp(b, c))
            return c;
        else
            return b;
    }
}


template <class SizeType>
LIBEXTCPP_FORCE_INLINE SizeType log2(SizeType n) {
    SizeType k = 0;
    for (; n > 1; n >>= 1) ++k;
    return k;
}


//template <class EuclideanRingElement>
//EuclideanRingElement gcd(EuclideanRingElement m, EuclideanRingElement n) {
//    while (n != 0) {
//        EuclideanRingElement t = m % n;
//        m = n;
//        n = t;
//    }
//    return m;
//}


template <class UnsignedInteger>
UnsignedInteger gcd(UnsignedInteger x, UnsignedInteger y) {
    if (((x & (x - 1)) | (y & (y - 1))) == 0)
        return x < y ? x : y;
    else {
        UnsignedInteger z = 1;
        while ((!(x & 1)) & (!(y & 1))) {
            z = UnsignedInteger(z << 1);
            x = UnsignedInteger(x >> 1);
            y = UnsignedInteger(y >> 1);
        }
        while (x && y) {
            if (!(x & 1))
                x = UnsignedInteger(x >> 1);
            else if (!(y & 1))
                y = UnsignedInteger(y >> 1);
            else if (x >= y)
                x = UnsignedInteger((x - y) >> 1u);
            else y = UnsignedInteger((y - x) >> 1);
        }
        return UnsignedInteger(z * (x + y));
    }
}


template <class UnsignedInteger>
LIBEXTCPP_FORCE_INLINE UnsignedInteger lcm(UnsignedInteger x, UnsignedInteger y) {
    return x * y / ext::gcd(x, y);
}



template <class ForwardIterator, class Compare>
ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first != last) {
        ForwardIterator nxt = ext::next(first);
        for (; nxt != last; first = nxt, (void) ++nxt)
            if (comp(*nxt, *first))
                return nxt;
        return nxt;
    } else
        return last;
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last) {
    return ext::is_sorted_until(first, last, ops::less());
}


template <class ForwardIterator, class Compare>
LIBEXTCPP_FORCE_INLINE bool is_sorted(ForwardIterator first, ForwardIterator last, Compare comp) {
    return ext::is_sorted_until(first, last, comp) == last;
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE bool is_sorted(ForwardIterator first, ForwardIterator last) {
    return ext::is_sorted(first, last, ops::less());
}


template <class RandomAccessIterator, class Distance, class Compare>
void partial_sort_loop(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Distance* hint, Compare comp) {
    ext::make_heap(first, middle, comp);
    Distance length = middle - first;
    for (RandomAccessIterator it = middle; it != last; ++it) {
        if (comp(*it, *first)) {
            ext::iter_swap(first, it);
            ext::shift_down(first, first, length, comp, value_type(first));
        }
    }
    ext::sort_heap(first, middle, comp);
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Compare comp) {
    ext::partial_sort_loop(first, middle, last, distance_type(first), comp);
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE void
partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
    ext::partial_sort_loop(first, middle, last, distance_type(first), ops::less());
}


template <class BidirectionalIterator, class Compare, class Tp>
void unguarded_liner_insert(BidirectionalIterator start, Tp*, Compare comp) {
    Tp value = ext::move(*start);
    BidirectionalIterator next = ext::prev(start);
    while (comp(value, *next)) {
        *start = ext::move(*next);
        start = next;
        --next;
    }
    *start = ext::move(value);
}


template <class BidirectionalIterator, class Compare>
void unguarded_insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    for (BidirectionalIterator i = first; i != last; ++i)
        ext::unguarded_liner_insert(i, value_type(i), comp);
}


template <class BidirectionalIterator, class Compare, class Tp>
void insertion_sort_switch(BidirectionalIterator first, BidirectionalIterator last, Compare comp, Tp*, bidirectional_iterator_tag) {
    for (BidirectionalIterator i = ext::next(first); i != last; ++i) {
        if (comp(*i, *first)) {
            Tp tmp = ext::move(*i);
            ext::move_backward(first, i , ext::next(i));
            *first = ext::move(tmp);
        } else
            ext::unguarded_liner_insert(i, value_type(i), comp);
    }
}


template <class ForwardIterator, class Compare, class Tp>
void insertion_sort_switch(ForwardIterator first, ForwardIterator last, Compare comp, Tp*, forward_iterator_tag) {
    
}


template <class BidirectionalIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    ext::insertion_sort_switch(first, last, comp, value_type(first), iterator_category(first));
}


template <class RandomAccessIterator, class Compare>
void final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (last - first > sort_minimal_length) {
        ext::insertion_sort(first, first + sort_minimal_length, comp);
        ext::unguarded_insertion_sort(first + sort_minimal_length, last, comp);
    } else
        ext::unguarded_insertion_sort(first, last, comp);
}


template <class RandomAccessIterator, class Compare>
RandomAccessIterator unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator pivot, Compare comp) {
    while (true) {
        while (comp(*first, *pivot)) ++first;
        --last;
        while (comp(*pivot, *last)) --last;
        if (!(first < last)) return first;
        ext::iter_swap(first, last);
        ++first;
    }
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE RandomAccessIterator
unguarded_partition_pivot(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    RandomAccessIterator middle = first + ((last - first) >> 1);
    ext::move_median_to(first + 1, middle, last - 1, first, comp);
    return ext::unguarded_partition(first + 1, last, first, comp);
}


template <class RandomAccessIterator, class Compare>
ext::pair<RandomAccessIterator, RandomAccessIterator>
partition_three_way(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator pivot, Compare comp) {
    RandomAccessIterator left = first, right = last;
    while (first < right) {
        if (comp(*first, *pivot))
            ext::iter_swap(first++, left++);
        else if (comp(*pivot, *first))
            ext::iter_swap(first, --right);
        else ++first;
    }
    return ext::make_pair(left, right);
}


template <class RandomAccessIterator, class Compare>
LIBEXTCPP_FORCE_INLINE ext::pair<RandomAccessIterator, RandomAccessIterator>
partition_pivot_three_way(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    RandomAccessIterator middle = first + ((last - first) >> 1);
    ext::move_median_to(first + 1, middle, last - 1, first, comp);
    return ext::partition_three_way(first + 1, last, first, comp);
}


template <class RandomAccessIterator, class SizeType>
struct introsort_data {
    
    RandomAccessIterator first;
    
    RandomAccessIterator last;
    
    SizeType depth_limit;
    
    introsort_data() {}
    
    introsort_data(RandomAccessIterator begin, RandomAccessIterator end, SizeType limit) : first(begin), last(end), depth_limit(limit) {}
    
};


template <class RandomAccessIterator, class SizeType, class Compare>
void introsort_loop(RandomAccessIterator first, RandomAccessIterator last, SizeType depth_limit, Compare comp) {
    typedef introsort_data<RandomAccessIterator, SizeType> SortData;
    typedef ext::pair<RandomAccessIterator, RandomAccessIterator> RangeType;
    ext::array<SortData, LIBEXTCPP_MAX_LOG2 << 1> aux;
    aux.push(SortData(first, last, depth_limit));
    while (!aux.empty()) {
        SortData parameters = aux.back();
        first = parameters.first;
        last = parameters.last;
        depth_limit = parameters.depth_limit;
        aux.pop();
        while (last - first > sort_minimal_length) {
            if (depth_limit == 0) {
                ext::partial_sort(first, last, last, comp);
                break;
            }
            --depth_limit;
            RangeType pivot_range = ext::partition_pivot_three_way(first, last, comp);
            RandomAccessIterator left = pivot_range.first, right = pivot_range.second;
            if (left - first < last - right) {
                aux.push(SortData(first, left, depth_limit));
                first = right;
            } else {
                aux.push(SortData(right, last, depth_limit));
                last = left;
            }
        }
    }
}


template <class RandomAccessIterator, class Compare>
void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first != last) {
        ext::introsort_loop(first, last, ext::log2(last - first) << 1, comp);
        ext::final_insertion_sort(first, last, comp);
    }
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE void sort(RandomAccessIterator first, RandomAccessIterator last) {
    ext::sort(first, last, ops::less());
}


template <class RandomAccessIterator, class Compare, class Distance, class Tp>
RandomAccessIterator lower_bound_switch(RandomAccessIterator first, RandomAccessIterator last, const Tp& value, Compare comp, Distance*, random_access_iterator_tag) {
    Distance length = last - first;
    while (length > 0) {
        Distance half = length >> 1;
        RandomAccessIterator middle = first + half;
        if (comp(*middle, value)) {
            first = middle + 1;
            length = length - half - 1;
        } else
            length = half;
    }
    return first;
}


template <class ForwardIterator, class Compare, class Distance, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
lower_bound_switch(ForwardIterator first, ForwardIterator last, const Tp& value, Compare comp, Distance*, forward_iterator_tag) {
    return ext::find_if_not(first, last, ops::binder2nd(comp, value));
}


template <class ForwardIterator, class Compare, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
lower_bound(ForwardIterator first, ForwardIterator last, const Tp& value, Compare comp) {
    return ext::lower_bound_switch(first, last, value, comp, distance_type(first), iterator_category(first));
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
lower_bound(ForwardIterator first, ForwardIterator last, const Tp& value) {
    return ext::lower_bound(first, last, value, ops::less());
}


template <class RandomAccessIterator, class Compare, class Distance, class Tp>
RandomAccessIterator upper_bound_switch(RandomAccessIterator first, RandomAccessIterator last, const Tp& value, Compare comp, Distance*, random_access_iterator_tag) {
    Distance length = last - first;
    while (length > 0) {
        Distance half = length >> 1;
        RandomAccessIterator middle = first + half;
        if (comp(value, *middle))
            length = half;
        else {
            first = middle + 1;
            length = length - half - 1;
        }
    }
    return first;
}


template <class ForwardIterator, class Compare, class Distance, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
upper_bound_switch(ForwardIterator first, ForwardIterator last, const Tp& value, Compare comp, Distance*, forward_iterator_tag) {
    return ext::find_if(first, last, ops::binder1st(comp, value));
}


template <class ForwardIterator, class Compare, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
upper_bound(ForwardIterator first, ForwardIterator last, const Tp& value, Compare comp) {
    return ext::upper_bound_switch(first, last, value, comp, distance_type(first), iterator_category(first));
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE ForwardIterator
upper_bound(ForwardIterator first, ForwardIterator last, const Tp& value) {
    return ext::upper_bound(first, last, value, ops::less());
}


template <class ForwardIterator, class Compare, class Distance, class Tp>
pair<ForwardIterator, ForwardIterator>
equal_range_switch(ForwardIterator first, ForwardIterator last, const Tp& value, Compare comp, Distance*, forward_iterator_tag) {
    ForwardIterator left = ext::find_if_not(first, last, ops::binder2nd(comp, value));
    ForwardIterator right = ext::find_if(first, last, ops::binder1st(comp, value));
    return ext::make_pair(left, right);
}


template <class RandomAccessIterator, class Compare, class Distance, class Tp>
pair<RandomAccessIterator, RandomAccessIterator>
equal_range_switch(RandomAccessIterator first, RandomAccessIterator last, const Tp& value, Compare comp, Distance*, random_access_iterator_tag) {
    Distance length = last - first;
    while (length > 0) {
        Distance half = length >> 1;
        RandomAccessIterator middle = first + half;
        if (comp(*middle, value)) {
            first = middle + 1;
            length = length - half - 1;
        } else if (comp(value, *middle))
            length = half;
        else {
            RandomAccessIterator left = ext::lower_bound(first, middle, value, comp);
            RandomAccessIterator right = ext::upper_bound(middle + 1, first + length, value, comp);
            return ext::make_pair(left, right);
        }
    }
    return ext::make_pair(first, first);
}


template <class ForwardIterator, class Compare, class Tp>
LIBEXTCPP_FORCE_INLINE pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const Tp& value, Compare comp) {
    return ext::equal_range_switch(first, last, value, comp, distance_type(first), iterator_category(first));
}


template <class ForwardIterator, class Tp>
LIBEXTCPP_FORCE_INLINE pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const Tp& value) {
    return ext::equal_range(first, last, value, ops::less());
}


template <class ForwardIterator>
ForwardIterator rotate_switch(ForwardIterator first, ForwardIterator middle, ForwardIterator last, forward_iterator_tag) {
    ForwardIterator i = middle;
    while (true) {
        ext::iter_swap(first, i);
        ++first;
        ++i;
        if (i == last)
            break;
        if (first == middle)
            middle = i;
    }
    ForwardIterator result = first;
    if (first != middle) {
        i = middle;
        while (true) {
            ext::iter_swap(first, i);
            ++first;
            ++i;
            if (i == last) {
                if (first == middle)
                    break;
                i = middle;
            } else if (first == middle)
                middle = i;
        }
    }
    return result;
}


template <class BidirectionalIterator>
BidirectionalIterator rotate_switch(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, bidirectional_iterator_tag) {
    ext::reverse(first, middle);
    ext::reverse(middle, last);
    while (first != middle && middle != last) {
        --last;
        ext::iter_swap(first, last);
        ++first;
    }
    if (first == middle) {
        ext::reverse(middle, last);
        return last;
    } else {
        ext::reverse(first, middle);
        return first;
    }
}


template <class RandomAccessIterator, class Distance, class Tp>
RandomAccessIterator rotate_gcd(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Distance*, Tp*) {
    Distance len1 = middle - first;
    Distance len2 = last - middle;
    if (len1 == len2) {
        ext::swap_ranges(first, middle, middle);
        return middle;
    }
    Distance count = ext::gcd(len1, len2);
    for (RandomAccessIterator i = first + count; i != first;) {
        --i;
        Tp tmp(ext::move(*i));
        RandomAccessIterator current = i;
        RandomAccessIterator next = current + len1;
        do {
            *current = ext::move(*next);
            current = next;
            Distance leftover = last - next;
            next = (len1 < leftover) ? next + len1 : first + (len1 - leftover);
        } while (next != i);
        *current = ext::move(tmp);
    }
    return first + len2;
}


template <class RandomAccessIterator>
LIBEXTCPP_FORCE_INLINE RandomAccessIterator
rotate_switch(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, random_access_iterator_tag) {
    return ext::rotate_gcd(first, middle, last, distance_type(first), value_type(first));
}


template <class ForwardIterator>
LIBEXTCPP_FORCE_INLINE ForwardIterator
rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
    if (middle == first)
        return last;
    if (middle == last)
        return first;
    return ext::rotate_switch(first, middle, last, iterator_category(first));
}



template <class BidirectionalIterator, class Pointer, class Distance>
BidirectionalIterator rotate_adaptive(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance length1, Distance length2, Pointer buffer, Distance buffer_size) {
    Pointer buffer_end;
    if (length1 > length2 && length2 <= buffer_size) {
        if (length2 > 0) {
            buffer_end = ext::move(middle, last, buffer);
            ext::move_backward(first, middle, last);
            return ext::move(buffer, buffer_end, first);
        } else
            return first;
    } else if (length1 <= buffer_size) {
        if (length1 > 0) {
            buffer_end = ext::move(first, middle, buffer);
            ext::move(middle, last, first);
            return ext::move_backward(buffer, buffer_end, last);
        } else
            return last;
    }
    return ext::rotate(first, middle, last);
}


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } else {
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    return ext::copy(first2, last2, ext::copy(first1, last1, result));
}


template <class InputIterator1, class InputIterator2, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    return ext::merge(first1, last1, first2, last2, result, ops::less());
}


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge_move_advance(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = ext::move(*first1);
            ++first1;
        } else {
            *result = ext::move(*first2);
            ++first2;
        }
        ++result;
    }
    return ext::move(first2, last2, ext::move(first1, last1, result));
}


template <class BidirectionalIterator1, class BidirectionalIterator2, class BidirectionalIterator3, class Compare>
void merge_move_backward(BidirectionalIterator1 first1, BidirectionalIterator1 last1, BidirectionalIterator2 first2, BidirectionalIterator2 last2, BidirectionalIterator3 result, Compare comp) {
    if (first1 == last1) {
        ext::move_backward(first2, last2, result);
        return;
    } else if (first2 == last2)
        return;
    --last1;
    --last2;
    while (true) {
        --result;
        if (comp(*last2, *last1)) {
            *result = ext::move(*last1);
            if (first1 == last1) {
                ext::move_backward(first2, ext::next(last2), result);
                return;
            }
            --last1;
        } else {
            *result = ext::move(*last2);
            if (first2 == last2)
                return;
            --last2;
        }
    }
}


template <class BidirectionalIterator, class Pointer, class Distance, class Compare>
void merge_adaptive(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance length1, Distance length2, Pointer buffer, Compare comp) {
    Pointer buffer_end;
    if (length1 <= length2) {
        buffer_end = ext::move(first, middle, buffer);
        ext::merge_move_advance(buffer, buffer_end, middle, last, first, comp);
    } else {
        buffer_end = ext::move(middle, last, buffer);
        ext::merge_move_backward(first, middle, buffer, buffer_end, last, comp);
    }
}


template <class BidirectionalIterator, class Distance>
struct recursive_merge_data {
    
    BidirectionalIterator first;
    
    BidirectionalIterator middle;
    
    BidirectionalIterator last;
    
    Distance length1;
    
    Distance length2;
    
    recursive_merge_data() {};
    
    recursive_merge_data(BidirectionalIterator l, BidirectionalIterator m, BidirectionalIterator r, Distance a, Distance b): first(l), middle(m), last(r), length1(a), length2(b) {}
    
};


#define LIBEXTCPP_FETCH_MERGE_PARAMETERS(p, f, m, l, l1, l2) \
do { \
    f = p.back().first; \
    m = p.back().middle; \
    l = p.back().last; \
    l1 = p.back().length1; \
    l2 = p.back().length2; \
    p.pop(); \
} while (false)


template <class BidirectionalIterator, class Pointer, class Distance, class Compare>
void merge_adaptive_resize(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance length1, Distance length2, Pointer buffer, Distance buffer_size, Compare comp) {
    typedef recursive_merge_data<BidirectionalIterator, Distance> MergeData;
    ext::array<MergeData, LIBEXTCPP_MAX_LOG2> aux;
    aux.push(MergeData(first, middle, last, length1, length2));
    while (!aux.empty()) {
        LIBEXTCPP_FETCH_MERGE_PARAMETERS(aux, first, middle, last, length1, length2);
        while (true) {
            if (length1 > buffer_size || length2 > buffer_size) {
                ext::merge_adaptive(first, middle, last, length1, length2, buffer, comp);
                break;
            }
            BidirectionalIterator first_cut = first;
            BidirectionalIterator second_cut = middle;
            BidirectionalIterator new_middle;
            Distance length11 = 0;
            Distance length22 = 0;
            if (length1 > length2) {
                length11 = length1 >> 1;
                ext::advance(first_cut, length11);
                second_cut = ext::lower_bound(middle, last, *first_cut, comp);
                length22 = ext::distance(middle, second_cut);
            } else {
                length22 = length2 >> 1;
                ext::advance(second_cut, length22);
                first_cut = ext::upper_bound(first, middle, *second_cut, comp);
                length11 = ext::distance(first, first_cut);
            }
            length1 -= length11;
            length2 -= length22;
            new_middle = ext::rotate_adaptive(first_cut, middle, second_cut, length1, length22, buffer, buffer_size);
            if (length1 + length2 < length11 + length22) {
                aux.push(MergeData(new_middle, second_cut, last, length1, length2));
                middle = first_cut;
                last = new_middle;
                length1 = length11;
                length2 = length22;
            } else {
                aux.push(MergeData(first, first_cut, new_middle, length11, length22));
                first = new_middle;
                middle = second_cut;
            }
        }
    }
}


template <class BidirectionalIterator, class Distance, class Compare>
void merge_without_buffer(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance length1, Distance length2, Compare comp) {
    typedef recursive_merge_data<BidirectionalIterator, Distance> MergeData;
    ext::array<MergeData, LIBEXTCPP_MAX_LOG2> aux;
    aux.push(MergeData(first, middle, last, length1, length2));
    while (!aux.empty()) {
        LIBEXTCPP_FETCH_MERGE_PARAMETERS(aux, first, middle, last, length1, length2);
        while (true) {
            if (length1 == 0 || length2 == 0)
                break;
            if (length1 + length2 == 2) {
                if (comp(*middle, *first))
                    ext::iter_swap(first, middle);
                break;
            }
            BidirectionalIterator first_cut = first;
            BidirectionalIterator second_cut = middle;
            BidirectionalIterator new_middle;
            Distance length11 = 0;
            Distance length22 = 0;
            if (length1 > length2) {
                length11 = length1 >> 1;
                ext::advance(first_cut, length11);
                second_cut = ext::lower_bound(middle, last, *first_cut, comp);
                length22 = ext::distance(middle, second_cut);
            } else {
                length22 = length2 >> 1;
                ext::advance(second_cut, length22);
                first_cut = ext::upper_bound(first, middle, *second_cut, comp);
                length11 = ext::distance(first, first_cut);
            }
            length1 -= length11;
            length2 -= length22;
            new_middle = ext::rotate(first_cut, middle, second_cut);
            if (length1 + length2 < length11 + length22) {
                aux.push(MergeData(new_middle, second_cut, last, length1, length2));
                middle = first_cut;
                last = new_middle;
                length1 = length11;
                length2 = length22;
            } else {
                aux.push(MergeData(first, first_cut, new_middle, length11, length22));
                first = new_middle;
                middle = second_cut;
            }
        }
    }
}


template <class BidirectionalIterator, class Compare, class Distance, class Tp>
void inplace_merge_switch(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Compare comp, Distance*, Tp*) {
    Distance length1 = ext::distance(first, middle);
    Distance length2 = ext::distance(middle, last);
    temporary_buffer<BidirectionalIterator, Tp> buffer(first, ext::min(length1, length2));
    if (buffer.requested_size() == buffer.size())
        ext::merge_adaptive(first, middle, last, length1, length2, buffer.begin(), comp);
    else if (buffer.size() <= 1)
        ext::merge_without_buffer(first, middle, last, length1, length2, comp);
    else
        ext::merge_adaptive_resize(first, middle, last, length1, length2, buffer.begin(), buffer.size(), comp);
}


template <class BidirectionalIterator, class Compare>
void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Compare comp) {
    if (!ext::is_sorted(first, last, comp))
        ext::inplace_merge_switch(first, middle, last, comp, distance_type(first), value_type(first));
}


template <class BidirectionalIterator>
LIBEXTCPP_FORCE_INLINE void
inplace_merge(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last) {
    ext::inplace_merge(first, middle, last, ops::less());
}


template <class BidirectionalIterator, class Distance, class Compare>
void chunk_insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Distance chunk_size, Distance length, Compare comp) {
    BidirectionalIterator chunk_start = first, chunk_end = first;
    if (length >= chunk_size)
        ext::advance(chunk_end, chunk_size);
    Distance leftover = length;
    while (leftover >= chunk_size) {
        ext::insertion_sort(chunk_start, chunk_end, comp);
        leftover -= chunk_size;
        chunk_start = chunk_end;
        if (leftover > chunk_size)
            ext::advance(chunk_end, chunk_size);
    }
    ext::insertion_sort(chunk_start, last, comp);
}


template <class BidirectionalIterator, class Distance>
void adjust_chunk_borders(BidirectionalIterator& begin, BidirectionalIterator& middle, BidirectionalIterator& end, Distance step_size, Distance leftover, bool first_adjust) {
    if (!first_adjust)
        begin = middle = end;
    if (leftover >= step_size) {
        ext::advance(middle, step_size);
        if (leftover >= (step_size << 1))
            end = ext::next(middle, step_size);
    }
}


template <class BidirectionalIterator, class Distance>
LIBEXTCPP_FORCE_INLINE void
initialize_chunk_borders(BidirectionalIterator& begin, BidirectionalIterator& middle, BidirectionalIterator& end, Distance step_size, Distance leftover) {
    adjust_chunk_borders(begin, middle, end, step_size, leftover, true);
}


template <class BidirectionalIterator, class Distance>
LIBEXTCPP_FORCE_INLINE void
advance_chunk_borders (BidirectionalIterator& begin, BidirectionalIterator& middle, BidirectionalIterator& end, Distance step_size, Distance leftover) {
    adjust_chunk_borders(begin, middle, end, step_size, leftover, false);
}


template <class BidirectionalIterator, class OutputIterator, class Distance, class Compare>
void merge_sort_step(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result, Distance step_size, Distance length, Compare comp) {
    Distance two_step = step_size << 1, leftover = length;
    BidirectionalIterator begin = first, middle = first, end = first;
    ext::initialize_chunk_borders(begin, middle, end, step_size, leftover);
    while (leftover >= two_step) {
        result = ext::merge_move_advance(begin, middle, middle, end, result, comp);
        leftover -= two_step;
        ext::advance_chunk_borders(begin, middle, end, step_size, leftover);
    }
    step_size = ext::min(leftover, step_size);
    ext::advance_chunk_borders(begin, middle, end, step_size, leftover);
    ext::merge_move_advance(begin, middle, middle, last, result, comp);
}


template <class BidirectionalIterator, class Pointer, class Compare, class Distance>
void merge_sort_with_buffer(BidirectionalIterator first, BidirectionalIterator last, Pointer buffer, Compare comp, Distance length) {
    Distance step_size = stable_sort_chunk_size;
    Pointer buffer_end = buffer + length;
    ext::chunk_insertion_sort(first, last, step_size, length, comp);
    while (step_size < length) {
        ext::merge_sort_step(first, last, buffer, step_size, length, comp);
        step_size <<= 1;
        ext::merge_sort_step(buffer, buffer_end, first, step_size, length, comp);
        step_size <<= 1;
    }
}


template <class BidirectionalIterator, class Compare, class Distance>
void inplace_stable_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp, Distance length) {
    Distance chunk_size = inplace_stable_sort_minimal_length;
    ext::chunk_insertion_sort(first, last, chunk_size, length, comp);
    for (Distance step_size = chunk_size; step_size < length; step_size <<= 1) {
        BidirectionalIterator begin = first, middle = first, end = first;
        Distance two_step = step_size << 1, leftover = length, next_step_size;
        while (leftover > step_size) {
            next_step_size = ext::min(step_size, leftover - step_size);
            middle = ext::next(begin, step_size);
            end = ext::next(middle, next_step_size);
            ext::merge_without_buffer(begin, middle, end, step_size, next_step_size, comp);
            begin = end;
            leftover -= two_step;
        }
    }
}


template <class BidirectionalIterator, class Pointer, class Distance, class Compare>
void stable_sort_adaptive(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance length1, Distance length2, Pointer buffer, Compare comp) {
    ext::merge_sort_with_buffer(first, middle, buffer, comp, length1);
    ext::merge_sort_with_buffer(middle, last, buffer, comp, length2);
    ext::merge_adaptive(first, middle, last, length1, length2, buffer, comp);
}


template <class BidirectionalIterator, class Pointer, class Distance, class Compare>
void buffered_chunk_stable_sort(BidirectionalIterator first, BidirectionalIterator last, Distance length, Pointer buffer, Distance buffer_size, Compare comp) {
    BidirectionalIterator begin = first, middle = first, end = first;
    Distance leftover = length, two_step = buffer_size << 1, next_chunk_size;
    while (leftover > buffer_size) {
        next_chunk_size = ext::min(buffer_size, leftover - buffer_size);
        middle = ext::next(begin, buffer_size);
        end = ext::next(middle, next_chunk_size);
        ext::stable_sort_adaptive(begin, middle, end, buffer_size, next_chunk_size, buffer, comp);
        begin = end;
        leftover -= two_step;
    }
}


template <class BidirectionalIterator, class Pointer, class Distance, class Compare>
void stable_sort_adaptive_resize(BidirectionalIterator first, BidirectionalIterator last, Distance length, Pointer buffer, Distance buffer_size, Compare comp) {
    Distance half = (length + 1) >> 1;
    BidirectionalIterator middle = ext::next(first, half);
    if (half > buffer_size) {
        ext::stable_sort_adaptive_resize(first, middle, half, buffer, buffer_size, comp);
        ext::stable_sort_adaptive_resize(middle, last, length - half, buffer, buffer_size, comp);
        ext::merge_adaptive_resize(first, middle, last, half, length - half, buffer, buffer_size, comp);
    } else
        ext::stable_sort_adaptive(first, middle, last, half, length - half, buffer, comp);
}


template <class BidirectionalIterator, class Compare, class Distance, class Tp>
void stable_sort_switch(BidirectionalIterator first, BidirectionalIterator last, Compare comp, Distance*, Tp*) {
    Distance length = ext::distance(first, last), half = (length + 1) >> 1;
    temporary_buffer<BidirectionalIterator, Tp> buffer(first, half);
    BidirectionalIterator middle = ext::next(first, buffer.size());
    if (buffer.requested_size() == buffer.size())
        ext::stable_sort_adaptive(first, middle, last, half, length - half, buffer.begin(), comp);
    else if (buffer.size() <= 1)
        ext::inplace_stable_sort(first, last, comp, length);
    else ext::stable_sort_adaptive_resize(first, last, length, buffer.begin(), buffer.size(), comp);
}


template <class BidirectionalIterator, class Compare>
LIBEXTCPP_FORCE_INLINE void
stable_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    if (first != last)
        ext::stable_sort_switch(first, last, comp, distance_type(first), value_type(first));
}


template <class BidirectionalIterator>
LIBEXTCPP_FORCE_INLINE void stable_sort(BidirectionalIterator first, BidirectionalIterator last) {
    ext::stable_sort(first, last, ops::less());
}


//template <class ForwardIterator, class Predicate>
//ForwardIterator partition_switch(ForwardIterator first, ForwardIterator last, Predicate predicate, forward_iterator_tag) {
//    if (first == last)
//        return first;
//    while (predicate(*first))
//        if (++first == last)
//            return first;
//    ForwardIterator i = first;
//    while (++i != last) {
//        if (predicate(*i)) {
//            ext::iter_swap(first, i);
//            ++first;
//        }
//    }
//    return first;
//}


template <class ForwardIterator, class Predicate>
ForwardIterator partition_switch(ForwardIterator first, ForwardIterator last, Predicate predicate, forward_iterator_tag) {
    ForwardIterator slow = ext::find_if_not(first, last, predicate);
    if (slow == last)
        return slow;
    for (ForwardIterator fast = ext::next(slow); fast != last; ++fast) {
        if (!predicate(*fast)) {
            ext::iter_swap(fast, slow);
            ++slow;
        }
    }
    return slow;
}


template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition_switch(BidirectionalIterator first, BidirectionalIterator last, Predicate predicate, bidirectional_iterator_tag) {
    while (true) {
        while (true) {
            if (first == last)
                return first;
            else if (predicate(*first))
                ++first;
            else break;
        }
        --last;
        while (true) {
            if (first == last)
                return first;
            else if (!predicate(*last))
                --last;
            else break;
        }
        ext::iter_swap(first, last);
        ++first;
    }
    return first;
}


template <class ForwardIterator, class Predicate>
LIBEXTCPP_FORCE_INLINE ForwardIterator
partition(ForwardIterator first, ForwardIterator last, Predicate predicate) {
    return ext::partition_switch(first, last, predicate, iterator_category(first));
}


template <class ForwardIterator, class Pointer, class Predicate>
ForwardIterator buffered_stable_partition(ForwardIterator first, ForwardIterator last, Pointer buffer, Predicate predicate) {
    ForwardIterator result1 = first;
    Pointer result2 = buffer;
    *result2 = ext::move(*first);
    ++result2;
    ++first;
    for (; first != last; ++first) {
        if (predicate(*first)) {
            *result1 = ext::move(*first);
            ++result1;
        } else {
            *result2 = ext::move(*first);
            ++result2;
        }
    }
    ext::move(buffer, result2, result1);
    return result1;
}


template <class ForwardIterator, class Pointer, class Distance, class Predicate>
ForwardIterator stable_partition_adaptive(ForwardIterator first, ForwardIterator last, Predicate predicate, Distance length, Pointer buffer, Distance buffer_size) {
    if (length == 1)
        return first;
    if (length <= buffer_size)
        return ext::buffered_stable_partition(first, last, buffer, predicate);
    ForwardIterator middle = first;
    ext::advance(middle, length >> 1);
    ForwardIterator left_split = ext::stable_partition_adaptive(first, middle, predicate, length >> 1, buffer, buffer_size);
    Distance right_length = length - (length >> 1);
    ForwardIterator right_split = ext::find_if_not_n(middle, right_length, predicate);
    if (right_length > 0)
        right_split = ext::stable_partition_adaptive(right_split, last, predicate, right_length, buffer, buffer_size);
    return ext::rotate(left_split, middle, right_split);
}


template <class ForwardIterator, class Predicate, class Distance, class Tp>
ForwardIterator stable_partition_aux(ForwardIterator first, ForwardIterator last, Predicate predicate, Distance*, Tp*) {
    first = ext::find_if_not(first, last, predicate);
    if (first == last)
        return first;
    Distance length = ext::distance(first, last);
    temporary_buffer<ForwardIterator, Tp> buffer(first, length);
    return ext::stable_partition_adaptive(first, last, predicate, length, buffer.begin(), buffer.size());
}


template <class ForwardIterator, class Predicate>
LIBEXTCPP_FORCE_INLINE ForwardIterator
stable_partition(ForwardIterator first, ForwardIterator last, Predicate predicate) {
    return ext::stable_partition_aux(first, last, predicate, distance_type(first), value_type(first));
}


template <class RandomAccessIterator, class Predicate, class Distance>
RandomAccessIterator partition_point_switch(RandomAccessIterator first, RandomAccessIterator last, Predicate predicate, Distance*, random_access_iterator_tag) {
    Distance length = last - first;
    while (length > 0) {
        Distance half = length >> 1;
        RandomAccessIterator middle = first + half;
        if (predicate(*middle)) {
            first = middle + 1;
            length = length - half - 1;
        } else
            length = half;
    }
    return first;
}


template <class ForwardIterator, class Predicate, class Distance>
LIBEXTCPP_FORCE_INLINE ForwardIterator
partition_point_switch(ForwardIterator first, ForwardIterator last, Predicate predicate, Distance*, forward_iterator_tag) {
    return ext::find_if_not(first, last, predicate);
}


template <class ForwardIterator, class Predicate>
LIBEXTCPP_FORCE_INLINE ForwardIterator
partition_point(ForwardIterator first, ForwardIterator last, Predicate predicate) {
    return ext::partition_point_switch(first, last, predicate, distance_type(first), iterator_category(first));
}


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    return ext::copy(first2, last2, ext::copy(first1, last1, result));
}


template <class InputIterator1, class InputIterator2, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    return ext::set_union(first1, last1, first2, last2, result, ops::less());
}


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2))
            ++first1;
        else if (comp(*first2, *first1))
            ++first2;
        else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    return result;
}


template <class InputIterator1, class InputIterator2, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
set_intersection(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    return ext::set_intersection(first1, last1, first2, last2, result, ops::less());
}


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } else if (comp(*first2, *first1))
            ++first2;
        else {
            ++first1;
            ++first2;
        }
    }
    return ext::copy(first1, last1, result);
}


template <class InputIterator1, class InputIterator2, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    return ext::set_difference(first1, last1, first2, last2, result, ops::less());
}


template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
        ++result;
    }
    return ext::copy(first2, last2, ext::copy(first1, last1, result));
}


template <class InputIterator1, class InputIterator2, class OutputIterator>
LIBEXTCPP_FORCE_INLINE OutputIterator
set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    return ext::set_symmetric_difference(first1, last1, first2, last2, result, ops::less());
}


template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1))
            return false;
        if (!comp(*first1, *first2))
            ++first2;
        ++first1;
    }
    return first2 == last2;
}


template <class InputIterator1, class InputIterator2>
LIBEXTCPP_FORCE_INLINE bool
includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    return ext::includes(first1, last1, first2, last2, ops::less());
}


template <class BidirectionalIterator, class Compare>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    if (first == last || ext::next(first) == last)
        return false;
    BidirectionalIterator i = ext::prev(last);
    while (true) {
        BidirectionalIterator ii = i;
        --i;
        if (comp(*i, *ii)) {
            BidirectionalIterator j = last;
            while (!comp(*i, *--j)) {}
            ext::iter_swap(i, j);
            ext::reverse(ii, last);
            return true;
        }
        if (i == first) {
            ext::reverse(first, last);
            return false;
        }
    }
}


template <class BidirectionalIterator>
LIBEXTCPP_FORCE_INLINE bool
next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    return ext::next_permutation(first, last, ops::less());
}


template <class BidirectionalIterator, class Compare>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
    if (first == last || ext::next(first) == last)
        return false;
    BidirectionalIterator i = ext::prev(last);
    while (true) {
        BidirectionalIterator ii = i;
        --i;
        if (comp(*ii, *i)) {
            BidirectionalIterator j = last;
            while (!comp(*--j, *i)) {}
            ext::iter_swap(i, j);
            ext::reverse(ii, last);
            return true;
        }
        if (i == first) {
            ext::reverse(first, last);
            return false;
        }
    }
}


template <class BidirectionalIterator>
LIBEXTCPP_FORCE_INLINE bool
prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    return ext::prev_permutation(first, last, ops::less());
}


template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate predicate) {
    constexpr bool is_random_accessible1 = is_random_access_iterator<ForwardIterator1>::value;
    constexpr bool is_random_accessible2 = is_random_access_iterator<ForwardIterator2>::value;
    constexpr bool both_are_random_accessible = is_random_accessible1 && is_random_accessible2;
    if (both_are_random_accessible) {
        auto distance1 = ext::distance(first1, last1);
        auto distance2 = ext::distance(first2, last2);
        if (distance1 != distance2)
            return false;
    }
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        if (!predicate(*first1, *first2)) break;
    if (both_are_random_accessible) {
        if (first1 == last1)
            return true;
    } else {
        auto distance1 = ext::distance(first1, last1);
        auto distance2 = ext::distance(first2, last2);
        if (distance1 == 0 && distance2 == 0)
            return true;
        if (distance1 != distance2)
            return false;
    }
    for (ForwardIterator1 scan = first1; scan != last1; ++scan) {
        auto scan_finder = ops::binder2nd(predicate, *scan);
        if (scan != ext::find_if(first1, scan, scan_finder))
            continue;
        auto matches = ext::count_if(first2, last2, scan_finder);
        if (matches == 0 || ext::count_if(scan, last1, scan_finder) != matches)
            return false;
    }
    return true;
}


template <class ForwardIterator1, class ForwardIterator2>
LIBEXTCPP_FORCE_INLINE bool
is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2) {
    return ext::is_permutation(first1, last1, first2, last2, ops::equal_to());
}


LIBEXTCPP_END_NAMESPACE
#endif /* algorithm_h */
