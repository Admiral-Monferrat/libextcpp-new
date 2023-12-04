//
//  numeric_traits.h
//  Class & Object
//
//  Created by 李志航 on 2022/2/14.
//

#ifndef numeric_traits_h
#define numeric_traits_h
#include <extc++/type_traits.h>
#include <extc++/typelist.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Tp>
struct is_integral: public false_type {};


#define LIBEXTCPP_DEFINE_IS_INTEGRAL(T) \
template <> \
struct is_integral<T>: public true_type {}


LIBEXTCPP_DEFINE_IS_INTEGRAL(bool);
LIBEXTCPP_DEFINE_IS_INTEGRAL(char);
LIBEXTCPP_DEFINE_IS_INTEGRAL(signed char);
LIBEXTCPP_DEFINE_IS_INTEGRAL(unsigned char);
LIBEXTCPP_DEFINE_IS_INTEGRAL(wchar_t);
LIBEXTCPP_DEFINE_IS_INTEGRAL(char16_t);
LIBEXTCPP_DEFINE_IS_INTEGRAL(char32_t);
LIBEXTCPP_DEFINE_IS_INTEGRAL(short);
LIBEXTCPP_DEFINE_IS_INTEGRAL(unsigned short);
LIBEXTCPP_DEFINE_IS_INTEGRAL(int);
LIBEXTCPP_DEFINE_IS_INTEGRAL(unsigned int);
LIBEXTCPP_DEFINE_IS_INTEGRAL(long);
LIBEXTCPP_DEFINE_IS_INTEGRAL(unsigned long);
LIBEXTCPP_DEFINE_IS_INTEGRAL(long long);
LIBEXTCPP_DEFINE_IS_INTEGRAL(unsigned long long);


template <class Tp>
struct is_floating_point: public false_type {};


#define LIBEXTCPP_DEFINE_IS_FLOATING_POINT(T) \
template <> \
struct is_floating_point<T>: public true_type {}


LIBEXTCPP_DEFINE_IS_FLOATING_POINT(float);
LIBEXTCPP_DEFINE_IS_FLOATING_POINT(double);
LIBEXTCPP_DEFINE_IS_FLOATING_POINT(long double);


template <class Tp>
struct is_byte: public false_type {};


#define LIBEXTCPP_DEFINE_IS_BYTE(T) \
template <> \
struct is_byte<T>: public true_type {}


LIBEXTCPP_DEFINE_IS_BYTE(char);
LIBEXTCPP_DEFINE_IS_BYTE(signed char);
LIBEXTCPP_DEFINE_IS_BYTE(unsigned char);
#if LIBEXTCPP_VERSION > CPP17
LIBEXTCPP_DEFINE_IS_BYTE(std::byte);
#endif


template <class Tp>
struct is_arithmetic: public bool_constant <
    is_integral<Tp>::value || is_floating_point<Tp>::value
> {};


template <class Tp>
struct is_scalar: public bool_constant <
    is_arithmetic<Tp>::value || is_pointer<Tp>::value
> {};


typedef typelist <
    signed char,
    signed short,
    signed int,
    signed long,
    signed long long
> signed_types;


typedef typelist <
    unsigned char,
    unsigned short,
    unsigned int,
    unsigned long,
    unsigned long long
> unsigned_types;


template <class TypeList, class Tp, size_t Size, bool = Size <= sizeof(Tp)>
struct find_type_by_size;


template <template <class ...> class TypeList, class Head, class Tp, size_t Size, class ...Args>
struct find_type_by_size<TypeList<Head, Args...>, Tp, Size, true> {
    typedef Tp type;
};


template <template <class ...> class TypeList, class Head, class Tp, size_t Size, class ...Args>
struct find_type_by_size<TypeList<Head, Args...>, Tp, Size, false> {
    typedef typename find_type_by_size<TypeList<Args...>, Head, Size>::type type;
};


LIBEXTCPP_END_NAMESPACE


#endif /* numeric_traits_h */
