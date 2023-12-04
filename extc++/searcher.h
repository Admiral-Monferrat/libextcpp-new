//
//  searcher.h
//  libextc++
//
//  Created by 李志航 on 2022/9/7.
//

#ifndef libextcpp_searcher_h
#define libextcpp_searcher_h
#include <extc++/iterator.h>
#include <extc++/functional.h>
#include <extc++/pair.h>
#include <extc++/algorithm.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class PatternIterator, class BinaryPredicate = ext::ops::unary_equal<>>
class default_searcher {
private:
    PatternIterator pattern_begin, pattern_end;
    BinaryPredicate predicate;
public:
    default_searcher(PatternIterator first, PatternIterator last, BinaryPredicate pred = BinaryPredicate()): pattern_begin(first), pattern_end(last), predicate(pred) {}
    template <class CorpusIterator>
    ext::pair<CorpusIterator, CorpusIterator>
    operator()(CorpusIterator corpus_begin, CorpusIterator corpus_end) const {
        CorpusIterator result_begin = ext::default_search(corpus_begin, corpus_end, pattern_begin, pattern_end, predicate);
        CorpusIterator result_end = result_begin == corpus_end ? corpus_end : ext::next(result_begin, ext::distance(pattern_begin, pattern_end));
        return ext::make_pair(result_begin, result_end);
    }
};


template <class PatternIterator, class BinaryPredicate = ops::unary_equal<>>
LIBEXTCPP_FORCE_INLINE default_searcher<PatternIterator, BinaryPredicate>
make_default_searcher(PatternIterator first, PatternIterator last, BinaryPredicate pred = ops::unary_equal<>()) {
    return ext::default_searcher<PatternIterator, BinaryPredicate>(first, last, pred);
}


template <class PatternIterator, class BinaryPredicate = ext::ops::unary_equal<>>
class knuth_morris_pratt_searcher {
private:
    typedef typename ext::iterator_traits<PatternIterator>::difference_type difference_type;
private:
    difference_type* next;
    const difference_type pattern_length;
    PatternIterator pattern_begin, pattern_end;
    BinaryPredicate predicate;
public:
    knuth_morris_pratt_searcher(PatternIterator first, PatternIterator last, BinaryPredicate func = BinaryPredicate()): pattern_begin(first), pattern_end(last), pattern_length(ext::distance(first, last)) {
        construct_skip_table(pattern_begin, pattern_end);
    }
    ~knuth_morris_pratt_searcher() {
        ext::deallocate(next);
    }
private:
    void construct_skip_table(PatternIterator first, PatternIterator last) {
        next = ext::allocate(pattern_length + 1, distance_type(first));
        const difference_type length = ext::distance(first, last);
        difference_type scan;
        next[0] = -1;
        for (difference_type index = 1; index <= length; ++index) {
            scan = next[index - 1];
            while (scan >= 0) {
                if (predicate(first[scan], first[index - 1]))
                    break;
                scan = next[scan];
            }
            next[index] = scan + 1;
        }
    }
    template <class CorpusIterator>
    ext::pair<CorpusIterator, CorpusIterator>
    match(CorpusIterator corpus_begin, CorpusIterator corpus_end, difference_type corpus_length) const {
        difference_type match_start = 0;
        const difference_type last_match = corpus_length - pattern_length;
        difference_type index = 0;
        while (match_start <= last_match) {
            while (predicate(pattern_begin[index], corpus_begin[match_start + index]))
                if (++index == pattern_length)
                    return ext::make_pair(corpus_begin + match_start, corpus_begin + pattern_length);
            match_start += index - next[index];
            index = next[index] >= 0 ? next[index] : 0;
        }
        return ext::make_pair(corpus_end, corpus_end);
    }
public:
    template <class CorpusIterator>
    ext::pair<CorpusIterator, CorpusIterator>
    operator()(CorpusIterator corpus_begin, CorpusIterator corpus_end) const {
        if (corpus_begin == corpus_end)
            return ext::make_pair(corpus_end, corpus_end);
        if (pattern_begin == pattern_end)
            return ext::make_pair(corpus_begin, corpus_begin);
        const difference_type corpus_length = ext::distance(corpus_begin, corpus_end);
        if (corpus_length < pattern_length)
            return ext::make_pair(corpus_end, corpus_end);
        return match(corpus_begin, corpus_end, corpus_length);
    }
};


template <class PatternIterator, class BinaryPredicate = ops::unary_equal<>>
LIBEXTCPP_FORCE_INLINE knuth_morris_pratt_searcher<PatternIterator, BinaryPredicate>
make_knuth_morris_pratt_searcher(PatternIterator first, PatternIterator last, BinaryPredicate pred = ops::unary_equal<>()) {
    return ext::knuth_morris_pratt_searcher<PatternIterator, BinaryPredicate>(first, last, pred);
}


template <class CorpusIterator, class Searcher>
LIBEXTCPP_FORCE_INLINE CorpusIterator
search(CorpusIterator first, CorpusIterator last, const Searcher& searcher) {
    return searcher(first, last).first;
}


LIBEXTCPP_END_NAMESPACE
#endif /* searcher_h */
