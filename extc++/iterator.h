//
//  iterator.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/1/11.
//

#ifndef libextcpp_iterator_h
#define libextcpp_iterator_h
#include <extc++/type_traits.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;
//using std::contiguous_iterator_tag;


struct trivial_iterator_tag {};


typedef void trivial_iterator_difference_type;


template <class Category, class Tp, class Distance = ptrdiff_t, class Pointer = Tp*, class Reference = Tp&>
class iterator {
public:
    typedef Category                             iterator_category;
    typedef Tp                                   value_type;
    typedef Distance                             difference_type;
    typedef Pointer                              pointer;
    typedef Reference                            reference;
};


template <class I, bool>
struct iterator_traits_base;


template <class Ip>
struct iterator_traits_base<Ip, true> {
    typedef typename Ip::value_type        value_type;
    typedef typename Ip::pointer           pointer;
    typedef typename Ip::reference         reference;
    typedef typename Ip::iterator_category iterator_category;
    typedef typename Ip::difference_type   difference_type;
};


template <class Ip>
struct iterator_traits_base<Ip, false> {};


template <class Ip, bool B>
struct iterator_traits_impl;


template <class Ip>
struct iterator_traits_impl<Ip, true>:
public iterator_traits_base <
    Ip,
    is_convertible <typename Ip::iterator_category, input_iterator_tag>::value ||
    is_convertible <typename Ip::iterator_category, output_iterator_tag>::value
> {};


template <class Ip>
struct iterator_traits_impl<Ip, false> {};


template <class Ip>
struct iterator_traits:
public iterator_traits_impl <
    Ip,
    LIBEXTCPP_HAS_TYPEDEF(Ip, iterator_category)
> {};


template <class Tp>
struct iterator_traits<Tp*> {
    typedef Tp                          value_type;
    typedef Tp*                         pointer;
    typedef Tp&                         reference;
    typedef random_access_iterator_tag  iterator_category;
    typedef ptrdiff_t                   difference_type;
};


template <class Tp>
struct iterator_traits<const Tp*> {
    typedef Tp                          value_type;
    typedef Tp*                         pointer;
    typedef Tp&                         reference;
    typedef random_access_iterator_tag  iterator_category;
    typedef ptrdiff_t                   difference_type;
};


template <class Tp, class Up, bool = LIBEXTCPP_HAS_TYPEDEF(iterator_traits<Tp>, iterator_category)>
struct has_iterator_category_convertible_to:
public integral_constant <
    bool,
    is_convertible<typename iterator_traits<Tp>::iterator_category, Up>::value
> {};


template <class Tp, class Up>
struct has_iterator_category_convertible_to<Tp, Up, false>: public false_type {};


template <class Tp, class Up, bool = LIBEXTCPP_HAS_TYPEDEF(iterator_traits<Tp>, iterator_category)>
struct has_iterator_category_equal_to:
public integral_constant <
    bool,
    is_same<typename iterator_traits<Tp>::iterator_category, Up>::value
> {};


template <class Tp, class Up>
struct has_iterator_category_equal_to<Tp, Up, false>: public false_type {};


template <class I>
struct is_cpp20_output_iterator:
public integral_constant <
    bool,
    has_iterator_category_convertible_to<I, output_iterator_tag>::value
> {};


template <class I>
struct is_output_iterator:
public integral_constant <
    bool,
    has_iterator_category_equal_to<I, output_iterator_tag>::value
> {};


template <class I>
struct is_cpp20_input_iterator:
public integral_constant <
    bool,
    has_iterator_category_convertible_to<I, input_iterator_tag>::value
> {};


template <class I>
struct is_input_iterator:
public integral_constant <
    bool,
    has_iterator_category_equal_to<I, input_iterator_tag>::value
> {};


template <class I>
struct is_cpp20_forward_iterator:
public integral_constant <
    bool,
    has_iterator_category_convertible_to<I, forward_iterator_tag>::value
> {};


template <class I>
struct is_forward_iterator:
public integral_constant <
    bool,
    has_iterator_category_equal_to<I, forward_iterator_tag>::value
> {};


template <class I>
struct is_cpp20_bidirectional_iterator:
public integral_constant <
    bool,
    has_iterator_category_convertible_to<I, bidirectional_iterator_tag>::value
> {};


template <class I>
struct is_bidirectional_iterator:
public integral_constant <
    bool,
    has_iterator_category_equal_to<I, bidirectional_iterator_tag>::value
> {};


template <class I>
struct is_cpp20_random_access_iterator:
public integral_constant <
    bool,
    has_iterator_category_convertible_to<I, random_access_iterator_tag>::value
> {};


template <class I>
struct is_random_access_iterator:
public integral_constant <
    bool,
    has_iterator_category_equal_to<I, random_access_iterator_tag>::value
> {};

/*
template <class I>
struct is_cpp20_contiguous_iterator:
public integral_constant <
    bool,
    has_iterator_category_convertible_to<I, contiguous_iterator_tag>::value
> {};*/


template <class Ip>
constexpr typename iterator_traits<Ip>::value_type* value_type(Ip&) {
    return static_cast<typename iterator_traits<Ip>::value_type*>(0);
}


template <class Ip>
constexpr typename iterator_traits<Ip>::difference_type* distance_type(Ip&) {
    return static_cast<typename iterator_traits<Ip>::difference_type*>(0);
}


template <class Ip>
constexpr typename iterator_traits<Ip>::iterator_category iterator_category(Ip&) {
    return typename iterator_traits<Ip>::iterator_category();
}


template <class ForwardIterator, class Distance>
inline Distance distance_switch(ForwardIterator first, ForwardIterator last, Distance*, forward_iterator_tag) {
    Distance n = 0;
    for (; first != last; ++first) ++n;
    return n;
}


template <class RandomAccessIterator, class Distance>
LIBEXTCPP_FORCE_INLINE Distance
distance_switch(RandomAccessIterator first, RandomAccessIterator last, Distance*, random_access_iterator_tag) {
    return last - first;
}


template <class InputIterator>
LIBEXTCPP_FORCE_INLINE typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return ext::distance_switch(first, last, distance_type(first), iterator_category(first));
}


template <class ForwardIterator, class Distance>
inline void advance_switch(ForwardIterator& position, Distance step, forward_iterator_tag) {
    if (step > 0)
        for (; step > 0; --step) ++position;
}


template <class BidirectionalIterator, class Distance>
inline void advance_switch(BidirectionalIterator& position, Distance step, bidirectional_iterator_tag) {
    if (step >= 0)
        for (; step > 0; --step) ++position;
    else
        for (; step < 0; ++step) --position;
}


template <class RandomAccessIterator, class Distance>
LIBEXTCPP_FORCE_INLINE void
advance_switch(RandomAccessIterator& position, Distance step, random_access_iterator_tag) {
    position += step;
}


template <class InputIterator, class Distance>
LIBEXTCPP_FORCE_INLINE void advance(InputIterator& position, Distance step) {
    ext::advance_switch(position, step, ext::iterator_category(position));
}


template <class InputIterator>
LIBEXTCPP_FORCE_INLINE InputIterator
next(InputIterator it, typename iterator_traits<InputIterator>::difference_type step = 1) {
    ext::advance(it, step);
    return it;
}


template <class BidirectionalIterator>
LIBEXTCPP_FORCE_INLINE BidirectionalIterator
prev(BidirectionalIterator it, typename iterator_traits<BidirectionalIterator>::difference_type step = 1) {
    ext::advance(it, -step);
    return it;
}


template <class Ip>
class reverse_iterator:
public iterator <
    typename iterator_traits<Ip>::iterator_category,
    typename iterator_traits<Ip>::value_type,
    typename iterator_traits<Ip>::difference_type,
    typename iterator_traits<Ip>::pointer,
    typename iterator_traits<Ip>::reference
> {
protected:
    Ip current;
public:
    typedef typename iterator_traits<Ip>::iterator_category iterator_category;
    typedef typename iterator_traits<Ip>::value_type        value_type;
    typedef typename iterator_traits<Ip>::difference_type   difference_type;
    typedef typename iterator_traits<Ip>::pointer           pointer;
    typedef typename iterator_traits<Ip>::reference         reference;
    typedef Ip                                              iterator_type;
    typedef reverse_iterator<Ip>                            this_type;
public:
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type it):current(it) {}
    reverse_iterator(const this_type& it):current(it.current) {}
    template <class Iter>
    reverse_iterator(const reverse_iterator<Iter>& it):current(it.base()) {}
    iterator_type base() const { return current;}
    reference operator*() const { Ip tmp = current; return *--tmp;}
    pointer operator->() const { return &(operator*());}
    this_type& operator++() { --current; return *this;}
    this_type operator++(int) { this_type tmp = *this; --current; return tmp;}
    this_type& operator--() { ++current; return *this;}
    this_type operator--(int) { this_type tmp = *this; ++current; return tmp;}
    this_type operator+(difference_type n) { return this_type(current - n);}
    this_type& operator+=(difference_type n) { current -= n; return *this;}
    this_type operator-(difference_type n) { return this_type(current + n);}
    this_type& operator-=(difference_type n) { current += n; return *this;}
    bool operator==(const this_type& other) { return current == other.current; }
    bool operator!=(const this_type& other) { return current != other.current; }
    reference operator[](difference_type n) const { return *(*this + n);}
};


template <class Ip>
LIBEXTCPP_FORCE_INLINE reverse_iterator<Ip>
make_reverse_iterator(const Ip& it) {
    return reverse_iterator<Ip>(it);
}


template <class Ip, class T, class Ref, class Distance>
class reverse_bidirectional_iterator:
public iterator <
    typename iterator_traits<Ip>::iterator_category,
    typename iterator_traits<Ip>::value_type,
    typename iterator_traits<Ip>::difference_type,
    typename iterator_traits<Ip>::pointer,
    typename iterator_traits<Ip>::reference
>
{
private:
    typedef reverse_bidirectional_iterator<Ip, T, Ref, Distance>  this_type;
protected:
    Ip current;
public:
    typedef bidirectional_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef Distance                   difference_type;
    typedef T*                         pointer;
    typedef Ref                        reference;
    reverse_bidirectional_iterator() {}
    explicit reverse_bidirectional_iterator(Ip it):current(it) {}
    Ip base() const { return current; }
    reference operator*() const { Ip tmp = current; return *--tmp; }
    pointer operator->() const { return &(operator*()); }
    this_type& operator++() { --current; return *this; }
    this_type operator++(int) { this_type tmp = *this; --current; return tmp; }
    this_type& operator--() { ++current; return *this; }
    this_type operator--(int) { this_type tmp = *this; ++current; return tmp; }
    bool operator==(const this_type& other) { return current == other.current; }
    bool operator!=(const this_type& other) { return current != other.current; }
};


template <class Iterator>
class move_iterator {
private:
    typedef iterator_traits<Iterator> traits_type;
    typedef typename traits_type::reference base_reference;
    template <class>
    friend class move_iterator;
public:
    typedef Iterator iterator_type;
    typedef typename traits_type::iterator_category iterator_category;
    typedef typename traits_type::value_type value_type;
    typedef typename traits_type::difference_type difference_type;
    typedef typename conditional <
        is_reference<base_reference>::value,
        typename remove_reference<base_reference>::type&&,
        base_reference
    >::type reference;
    typedef Iterator pointer;
private:
    Iterator current;
public:
    move_iterator() : current() {}
    explicit move_iterator(iterator_type it) : current(ext::move(it.current)) {}
    template <class Ip>
    move_iterator(const move_iterator<Ip>& it) : current(it.current) {}
    template <class Ip>
    move_iterator& operator=(const move_iterator<Ip>& other) {
        current = other.current;
        return *this;
    }
};


template <class Container>
LIBEXTCPP_FORCE_INLINE auto begin(Container& cont) -> decltype(cont.begin()) {
    return cont.begin();
}


template <class Tp, size_t N>
LIBEXTCPP_FORCE_INLINE Tp* begin(Tp (&array)[N]) {
    return array;
}


template <class Container>
LIBEXTCPP_FORCE_INLINE auto end(Container& cont) -> decltype(cont.end()) {
    return cont.end();
}


template <class Tp, size_t N>
LIBEXTCPP_FORCE_INLINE Tp* end(Tp (&array)[N]) {
    return array + N;
}


template <class Ip>
LIBEXTCPP_FORCE_INLINE auto unwrap_iterator(Ip i) -> decltype(i.base()) {
    return i.base();
}


template <class Tp>
LIBEXTCPP_FORCE_INLINE Tp* unwrap_iterator(Tp* i) {
    return i;
}


LIBEXTCPP_END_NAMESPACE
#endif /* iterator_h */
