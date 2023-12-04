//
//  functional.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/1/14.
//

#ifndef libextcpp_functional_h
#define libextcpp_functional_h
#include <extc++/config.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Argument, class Result>
struct unary_function {
    typedef Argument    argument_type;
    typedef Result      result_type;
};


template <class First, class Second, class Result>
struct binary_function {
    typedef First     first_argument_type;
    typedef Second    second_argument_type;
    typedef Result    result_type;
};


template <class T = void>
struct greater : binary_function<T, T, bool>{
    bool operator()(const T &a, const T &b) const { return a > b; }
};


template <>
struct greater<void> {
    template <class T1, class T2>
    bool operator()(const T1& a, const T2& b) const { return a > b;}
};


template <class T = void>
struct less : binary_function<T, T, bool> {
    bool operator()(const T& a, const T& b) const { return a < b;}
};


template <>
struct less<void> {
    template <class T1, class T2>
    bool operator()(const T1& a, const T2& b) const { return a < b;}
};


template <class Function>
struct binder1st: public unary_function <
    typename Function::second_argument_type,
    typename Function::result_type
> {
protected:
    Function function;
    typename Function::first_argument_type value;
public:
    binder1st(const Function& func, const typename Function::first_argument_type operand): function(func), value(operand) {}
    typename Function::result_type operator()(typename Function::second_argument_type& x) const {
        return function(value, x);
    }
    typename Function::result_type operator()(const typename Function::second_argument_type& x) const {
        return function(value, x);
    }
};


template <class Function>
struct binder2nd: public unary_function <
    typename Function::first_argument_type,
    typename Function::result_type
> {
protected:
    Function function;
    typename Function::second_argument_type value;
public:
    binder2nd(const Function& func, const typename Function::second_argument_type& operand): function(func), value(operand) {}
    typename Function::result_type operator()(typename Function::second_argument_type& x) const {
        return function(x, value);
    }
    typename Function::result_type operator()(const typename Function::second_argument_type& x) const {
        return function(x, value);
    }
};


template <class Predicate>
struct unary_negate: public unary_function <
    typename Predicate::argument_type,
    bool
> {
private:
    Predicate predicate;
public:
    explicit unary_negate(const Predicate& func): predicate(func) {}
    bool operator()(const typename Predicate::argument_type& x) const {
        return !predicate(x);
    }
};


template <class Predicate>
struct binary_negate: public binary_function <
    typename Predicate::first_argument_type,
    typename Predicate::second_argument_type,
    bool
> {
private:
    Predicate predicate;
public:
    explicit binary_negate(const Predicate& func): predicate(func) {}
    bool operator()(const typename Predicate::first_argument_type& x, const typename Predicate::second_argument_type& y) const {
        return !predicate(x, y);
    }
};


template <class Predicate>
struct invert: public binary_function <
    typename Predicate::first_argument_type,
    typename Predicate::second_argument_type,
    bool
> {
private:
    Predicate predicate;
public:
    explicit invert(const Predicate& func): predicate(func) {}
    bool operator()(const typename Predicate::first_argument_type& x, const typename Predicate::second_argument_type& y) const {
        return predicate(y, x);
    }
};


template <class Tp>
struct identity {
    Tp& operator()(const Tp& x) const { return x;}
};


template <class T>
struct multiplies : binary_function<T, T, T> {
    T operator()(const T& a, const T& b) const { return a * b;}
};


LIBEXTCPP_END_NAMESPACE
#endif /* functional_h */
