//
//  predefined_ops.h
//  libextc++
//
//  Created by 李志航 on 2022/11/30.
//

#ifndef predefined_ops_h
#define predefined_ops_h
#include <extc++/config.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(ops)


template <class Pair>
struct select1st {
    const typename Pair::first_type& operator()(const Pair& x) const { return x.first;}
};


template <class Pair>
struct select2nd {
    const typename Pair::second_type& operator()(const Pair& x) const { return x.second;}
};


template <class Compare>
struct equal_by_comparasion: public binary_function <
    typename Compare::first_argument_type,
    typename Compare::second_argument_type,
    bool
> {
private:
    Compare comp;
private:
    typedef typename Compare::first_argument_type first_type;
    typedef typename Compare::second_argument_type second_type;
public:
    equal_by_comparasion(const Compare& cmp) : comp(cmp) {}
    bool operator()(first_type& x, second_type& y) const {
        return (!comp(x, y)) && (!comp(y, x));
    }
    bool operator()(const first_type& x, const second_type& y) const {
        return (!comp(x, y)) && (!comp(y, x));
    }
};


template <class T1, class T2>
struct binary_equal : binary_function<T1, T2, bool> {
    bool operator()(const T1& a, const T2& b) const { return a == b; }
};


template <class T = void>
struct unary_equal : binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a == b; }
};


template <>
struct unary_equal<void> {
    template <class T1, class T2>
    bool operator()(const T1& a, const T2& b) const { return a == b;}
};


template <class T1, class T2>
struct binary_less: public binary_function<T1, T2, bool> {
    bool operator()(const T1& a, const T2& b) const { return a < b;}
};


template <class Tp>
struct unary_less: public unary_function<Tp, bool> {
    bool operator()(const Tp& a, const Tp& b) const { return a < b;}
};


LIBEXTCPP_BEGIN_NAMESPACE(detail)


template <class BinaryPredicate, class Tp>
struct binder1st {
protected:
    BinaryPredicate predicate;
    Tp value;
public:
    binder1st(const BinaryPredicate& func, const Tp& operand): predicate(func), value(operand) {}
    template <class Up>
    bool operator()(Up& x) const {
        return bool(predicate(value, x));
    }
    template <class Up>
    bool operator()(const Up& x) const {
        return bool(predicate(value, x));
    }
};


template <class BinaryPredicate, class Tp>
struct binder2nd {
protected:
    BinaryPredicate predicate;
    Tp value;
public:
    binder2nd(const BinaryPredicate& func, const Tp& operand): predicate(func), value(operand) {}
    template <class Up>
    bool operator()(Up& x) const {
        return bool(predicate(x, value));
    }
    template <class Up>
    bool operator()(const Up& x) const {
        return bool(predicate(x, value));
    }
};


template <class Predicate>
struct negate {
protected:
    Predicate predicate;
public:
    negate(const Predicate& function): predicate(function) {}
    template <class ...Args>
    bool operator()(Args&& ...args) {
        return !bool(predicate(ext::forward<Args>(args)...));
    }
};


template <class BinaryPredicate>
struct invert {
protected:
    BinaryPredicate predicate;
public:
    invert(const BinaryPredicate& function) : predicate(function) {}
    template <class Tp, class Up>
    LIBEXTCPP_FORCE_INLINE bool operator()(Tp& x, Tp& y) {
        return bool(predicate(y, x));
    }
    template <class Tp, class Up>
    LIBEXTCPP_FORCE_INLINE bool operator()(const Tp& x, const Tp& y) const {
        return bool(predicate(y, x));
    }
};


struct less {
    template <class Tp, class Up>
    LIBEXTCPP_FORCE_INLINE bool operator()(const Tp& x, const Up& y) const {
        return x < y;
    }
};


struct equal_to {
    template <class Tp, class Up>
    LIBEXTCPP_FORCE_INLINE bool operator()(const Tp& x, const Up& y) const {
        return x == y;
    }
};


LIBEXTCPP_END_NAMESPACE


template <class Predicate>
LIBEXTCPP_FORCE_INLINE detail::negate<Predicate>
negate(const Predicate& predicate) {
    return detail::negate<Predicate>(predicate);
}


template <class BinaryPredicate, class Tp>
LIBEXTCPP_FORCE_INLINE detail::binder2nd<BinaryPredicate, Tp>
binder2nd(const BinaryPredicate& predicate, const Tp& value) {
    return detail::binder2nd<BinaryPredicate, Tp>(predicate, value);
}


template <class BinaryPredicate, class Tp>
LIBEXTCPP_FORCE_INLINE detail::binder1st<BinaryPredicate, Tp>
binder1st(const BinaryPredicate& predicate, const Tp& value) {
    return detail::binder1st<BinaryPredicate, Tp>(predicate, value);
}


template <class BinaryPredicate>
LIBEXTCPP_FORCE_INLINE detail::invert<BinaryPredicate>
invert(const BinaryPredicate& predicate) {
    return detail::invert<BinaryPredicate>(predicate);
}


LIBEXTCPP_FORCE_INLINE detail::less
less() {
    return detail::less();
}


LIBEXTCPP_FORCE_INLINE detail::equal_to
equal_to() {
    return detail::equal_to();
}


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* predefined_ops_h */
