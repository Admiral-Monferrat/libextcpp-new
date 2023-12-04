//
//  type_traits.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/1/12.
//

#ifndef libextcpp_tree_type_traits_h
#define libextcpp_tree_type_traits_h
#include <extc++/config.h>
#include <type_traits>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Tp>
Tp&& declval_base(int);


template <class Tp>
Tp declval_base(long);


template <class Tp>
decltype(ext::declval_base<Tp>(0)) declval() noexcept;


struct null_type {};


template <class T, T val>
struct integral_constant {
    constexpr static T value = val;
    typedef T value_type;
    typedef integral_constant type;
    constexpr value_type operator()() const noexcept { return value; }
};


template <class Tp>
struct type_constant {
    typedef Tp type;
};


template <class T, T val>
constexpr T integral_constant<T, val>::value;


typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;


template <bool b>
struct bool_constant : public integral_constant<bool, b> {};


template <class>
struct void_t { typedef void type; };


template <bool b, class If, class Then>
struct conditional { typedef If type; };


template <class If, class Then>
struct conditional<false, If, Then> { typedef Then type; };


template <class Tp>
struct type_identity { using type = Tp; };


template <class...>
struct Or {};


template <>
struct Or<> : public false_type {};


template <class B1>
struct Or<B1> : public B1 {};


template <class B1, class B2>
struct Or<B1, B2> : public conditional<B1::value, B1, B2>::type {};


template <class B1, class B2, class B3, class... Bn>
struct Or<B1, B2, B3, Bn...> : public conditional<B1::value, B1, Or<B2, B3, Bn...>>::type {};


template <class...>
struct And {};


template <>
struct And<> : public true_type {};


template <class B1>
struct And<B1> : public B1 {};


template <class B1, class B2>
struct And<B1, B2> : public conditional<B1::value, B2, B1>::type {};


template <class B1, class B2, class B3, class... Bn>
struct And<B1, B2, B3, Bn...> : public conditional<B1::value, And<B2, B3, Bn...>, B1>::type {};


template <class Bp>
struct Not : public integral_constant<bool, !bool(Bp::value)> {};


template <class... Bn>
struct conjunction : And<Bn...> {};


template <class... Bn>
struct disjunction : Or<Bn...> {};


template <class Pp>
struct negation : Not<Pp> {};


template <class Tp>
struct is_const: public false_type {};


template <class Tp>
struct is_const<Tp const>: public true_type {};


template <class Tp> struct is_function:
public integral_constant <
    bool,
    !is_const<const Tp>::value
> {};


template <class Tp>
struct remove_volatile { typedef Tp type; };


template <class Tp>
struct remove_volatile<volatile Tp> { typedef Tp type; };


template <class Tp>
struct remove_const { typedef Tp type; };


template <class Tp>
struct remove_const<const Tp> { typedef Tp type; };


template <class Tp>
struct remove_cv { typedef typename remove_volatile<typename remove_const<Tp>::type>::type type; };


template <class Tp>
struct add_const { typedef const Tp type; };


template <class Tp>
struct add_pointer { typedef Tp* type; };


//template <class Tp>
//struct select_first_argument {};
//
//
//template <template <class, class...> class Template, class FirstArg, class ...Args>
//struct select_first_argument<Template<FirstArg, Args...>> {
//    typedef FirstArg type;
//};


template <class Tp>
struct is_template : public false_type {};


template <template <class...> class Template, class ...Args>
struct is_template<Template<Args...>> : public true_type {};


template <class ...Args>
struct packed_args {};


template <class Tp, short n>
struct select_nth_argument {
    static_assert(is_template<Tp>::value, "ext::select_nth_argument<> requires 1st template parameter must be a template!");
};


template <template <class...> class Template, class FirstArg, short n, class ...Args>
struct select_nth_argument<Template<FirstArg, Args...>, n> {
    typedef typename select_nth_argument<packed_args<Args...>, n - 1>::type type;
};


template <template <class...> class Template, class FirstArg, class ...Args>
struct select_nth_argument<Template<FirstArg, Args...>, 0> {
    typedef FirstArg type;
};


template <class Tp>
struct select_first_argument {
    typedef typename select_nth_argument<Tp, 0>::type type;
};


template <class Tp, class Up>
struct replace_first_argument {
    static_assert(is_template<Tp>::value, "ext::replace_first_argument<> requires 1st template parameter must be a template!");
};


template <template <class, class...> class Template, class FirstArg, class Replacement, class ...Args>
struct replace_first_argument<Template<FirstArg, Args...>, Replacement> {
    typedef Template<Replacement, Args...> type;
};


template <class Tp, size_t stage>
struct multistage_pointer:
public multistage_pointer <
    typename add_pointer<Tp>::type,
    stage - 1
> {};


template <class Tp>
struct multistage_pointer<Tp, 0> {
    typedef Tp type;
};


template <class T1, class T2>
struct is_same : public false_type {};


template <class T>
struct is_same<T, T> : public true_type {};


template <class Tp>
struct is_void : public is_same<typename remove_cv<Tp>::type, void> {};


template <class>
struct is_array : public false_type {};


template <class T>
struct is_array<T[]> : public true_type {};


template <class T, std::size_t Size>
struct is_array<T[Size]> : public true_type {};


template <class>
struct is_lvalue_reference : public false_type {};


template <class T>
struct is_lvalue_reference<T&> : public true_type {};


template <class>
struct is_rvalue_reference : public false_type {};


template <class T>
struct is_rvalue_reference<T&&> : public true_type {};


template <class T>
struct is_reference: public Or<is_lvalue_reference<T>, is_rvalue_reference<T>> {};


template <class Tp>
struct is_pointer_base: public false_type {};


template <class Tp>
struct is_pointer_base<Tp*>: public true_type {};


template <class Tp>
struct is_pointer: public is_pointer_base<typename remove_cv<Tp>::type> {};


template <class Tp>
struct is_referenceable {
private:
    struct two { char x, y; };
    template <class Up> static Up& test(int);
    template <class Up> static two test(...);
public:
    constexpr static bool value = !ext::is_same<decltype(test<Tp>(0)), two>::value;
};


template <class Tp, class Up, class = void>
struct is_assignable : public false_type {};


template <class Tp, class Up>
struct is_assignable<Tp, Up, decltype(
    (void)(ext::declval<Tp>() = ext::declval<Up>())
)> : public true_type {};


template <class Tp, bool = is_referenceable<Tp>::value>
struct add_lvalue_reference {
    typedef Tp type;
};


template <class Tp>
struct add_lvalue_reference<Tp, true> {
    typedef Tp& type;
};


template <class Tp, bool = is_referenceable<Tp>::value>
struct add_rvalue_reference {
    typedef Tp type;
};


template <class Tp>
struct add_rvalue_reference<Tp, true> {
    typedef Tp&& type;
};


template <class Tp>
struct is_copy_assignable:
public is_assignable <
    typename add_lvalue_reference<Tp>::type,
    typename add_lvalue_reference<typename add_const<Tp>::type>::type
> {};


template <class Tp>
struct is_move_assignable:
public is_assignable <
    typename add_lvalue_reference<Tp>::type,
    typename add_rvalue_reference<Tp>::type
> {};


template <class Tp, class... Args>
struct is_trivially_constructible:
public integral_constant <
    bool,
    std::is_trivially_constructible<Tp, Args...>::value
> {};


template <class Tp>
struct is_trivially_default_constructible:
public bool_constant <
    std::is_trivially_default_constructible<Tp>::value
> {};


template <class Tp>
struct is_union:
public bool_constant <
    std::is_union<Tp>::value
> {};


template <class Tp>
struct is_class_helper {
private:
    typedef struct { char a, b; } fail;
    typedef char success;
    template <class Up> static success test(int Tp::*);
    template <class Up> static fail test(...);
public:
    constexpr static bool value = (sizeof(test<Tp>(0)) == sizeof(success));
};


template <class Tp, bool = is_union<Tp>::value>
struct is_class:
public bool_constant <
    is_class_helper<Tp>::value && !is_union<Tp>::value
> {};


template <class Tp>
struct is_empty_aux_extended : private Tp { double x; };


struct is_empty_aux_raw { double x; };


template <class Tp, bool = std::is_class<Tp>::value>
struct is_empty:
public bool_constant <
    sizeof(is_empty_aux_extended<Tp>) == sizeof(is_empty_aux_raw)
> {};


template <class Tp>
struct is_empty<Tp, false> : public false_type {};


template <class Tp>
struct is_final:
public bool_constant <
    __is_final(Tp)
> {};


template <bool, class T = void>
struct enable_if {};


template <class T>
struct enable_if<true, T> { using type = T;};


template <class T>
struct is_null_pointer : public false_type {};


template <>
struct is_null_pointer<std::nullptr_t> : public true_type {};


template <class Tp, class Up, bool =
    Or<is_void<Tp>, is_function<Up>, is_array<Up>>::value
> struct is_convertible_helper {
    typedef typename is_void<Up>::type type;
};


template <class Tp, class Up>
struct is_convertible_helper<Tp, Up, false> {
    template <class U>
    static void test_aux(U) noexcept;
    template <class Tp1, class Up1, typename =
        decltype(test_aux<Up1>(ext::declval<Tp1>()))
    > static true_type test(int);
    template <class, class>
    static false_type test(...);
public:
    typedef decltype(test<Tp, Up>(0)) type;
};


template <class Tp, class Up>
struct is_convertible:
public is_convertible_helper<Tp, Up>::type {};


template <class Tp>
struct is_trivial:
public bool_constant <
    std::is_trivial<Tp>::value
> {};


template <class Tp>
struct is_trivially_copyable:
public bool_constant <
    std::is_trivially_copyable<Tp>::value
> {};


template <class Tp, class Arg>
struct is_trivially_assignable:
public bool_constant <
    std::is_trivially_assignable<Tp, Arg>::value
> {};


template <class Tp>
struct is_trivially_copy_assignable:
public is_trivially_assignable <
    Tp&,
    const Tp&
> {};


template <class Tp>
struct is_pod:
public bool_constant <
    std::is_pod<Tp>::value
> {};


template <class Tp>
struct is_standard_layout:
public bool_constant <
    std::is_standard_layout<Tp>::value
> {};


template <class Tp>
struct is_trivially_destructible:
public bool_constant <
    std::is_trivially_destructible<Tp>::value
> {};


template <bool, bool, class Tp, class ...Args>
struct is_nothrow_constructible_base;


template <class Tp, class ...Args>
struct is_nothrow_constructible_base<true, false, Tp, Args...>:
public integral_constant<bool, noexcept(Tp(declval<Args>()...))> {};


template <class Tp>
void implicit_conversion_to(Tp) noexcept { }


template <class T>
struct remove_reference { typedef T type;};


template <class T>
struct remove_reference<T&> { typedef T type;};


template <class T>
struct remove_reference<T&&> { typedef T type;};


template <class T>
inline typename remove_reference<T>::type move(T&& obj) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(obj);
}


template <class Tp>
constexpr Tp&& forward(typename remove_reference<Tp>::type& t) noexcept {
    return static_cast<Tp&&>(t);
}


template <class Tp>
constexpr Tp&& forward(typename remove_reference<Tp>::type&& t) noexcept {
    static_assert(!is_lvalue_reference<Tp>::value, "can not forward an rvalue as an lvalue");
    return static_cast<Tp&&>(t);
}


#define LIBEXTCPP_HAS_MEMBER(type, member) libextcpp_class_has_##member<type>::value


#define LIBEXTCPP_DEFINE_HAS_MEMBER(member)\
template <class Tp>\
struct libextcpp_class_has_##member { \
    template <class Up> static auto test(...)->int;\
    template <class Up> static auto test(decltype(&Up::member))->void;\
    constexpr static bool value = ext::is_void<decltype(test<Tp>(0))>::value;\
}


#define LIBEXTCPP_HAS_TYPEDEF(type, typevar) libextcpp_class_defined_##typevar<type>::value


#define LIBEXTCPP_DEFINE_HAS_TYPEDEF(typevar)\
template <class Tp>\
struct libextcpp_class_defined_##typevar {\
    struct two { char a, b; };\
    template <class Up> static two test(...);\
    template <class Up> static char test(typename ext::void_t<typename Up::typevar>::type* = 0);\
    constexpr static bool value = sizeof(test<Tp>(nullptr)) == 1;\
}


LIBEXTCPP_DEFINE_HAS_TYPEDEF(value_type);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(pointer);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(const_pointer);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(void_pointer);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(const_void_pointer);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(size_type);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(reference);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(const_reference);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(iterator_category);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(difference_type);


LIBEXTCPP_END_NAMESPACE
#endif /* type_traits_h */
