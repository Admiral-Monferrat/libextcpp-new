//
//  config.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/1/11.
//

#ifndef libextcpp_config_h
#define libextcpp_config_h
#include <iostream>
#include <stdio.h>
#include <utility>
#include <stdlib.h>


using std::cout;
using std::cin;
using std::endl;


#if __WORDSIZE == 64
typedef unsigned long size_t;
typedef long ptrdiff_t;
#else
typedef unsigned int size_t;
typedef int ptrdiff_t;
#endif


#if __WORDSIZE == 64
#   define LIBEXTCPP_ADDRESS_MASK 4
#else
#   define LIBEXTCPP_ADDRESS_MASK 3
#endif


#if __WORDSIZE == 64
#   define LIBEXTCPP_MAX_LOG2 64
#else
#   define LIBEXTCPP_MAX_LOG2 32
#endif


#define LIBEXTCPP_ADDRESS_BITS(x) ((x) & (~LIBEXTCPP_ADDRESS_MASK))


#define LIBEXTCPP_SET_BIT(x, i) \
    do { \
        (x) |= (1 << i); \
    } while (false)


#define LIBEXTCPP_CLEAR_BIT(x, i) \
    do { \
        (x) &= (~(1 << i)); \
    } while (false)


#define LIBEXTCPP_BEGIN_NAMESPACE(str) namespace str {
#define LIBEXTCPP_END_NAMESPACE }
#define LIBEXTCPP_THREADS


#ifndef LIBEXTCPP_VERSION
#   if __cplusplus <= 201103L
#       define LIBEXTCPP_VERSION 11
#   elif __cplusplus <= 201402L
#       define LIBEXTCPP_VERSION 14
#   elif __cplusplus <= 201703L
#       define LIBEXTCPP_VERSION 17
#   elif __cplusplus <= 202002L
#       define LIBEXTCPP_VERSION 20
#   else
#       define LIBEXTCPP_VERSION 23
#   endif
#endif


#define CPP11 11
#define CPP14 14
#define CPP17 17
#define CPP20 20
#define CPP23 23


#if !defined(LIBEXTCPP_DISABLE_DEPRECATION_WARNINGS)
#   if __has_attribute(deprecated)
#       define LIBEXTCPP_DEPRECATED __attribute__((deprecated))
#   elif LIBEXTCPP_STD_VER > CPP14
#       define LIBEXTCPP_DEPRECATED [[deprecated]]
#   else
#       define LIBEXTCPP_DEPRECATED
#   endif
#else
#   define LIBEXTCPP_DEPRECATED
#endif


#if defined(__DEPRECATED)
#   define LIBEXTCPP_DEPRECATED_SUGGEST(ALT) __attribute__ ((__deprecated__ ("use '" ALT "' instead")))
#else
#   define LIBEXTCPP_DEPRECATED_SUGGEST(ALT)
#endif


#if LIBEXTCPP_VERSION > CPP11
#   define LIBEXTCPP_NOEXCEPT noexcept
#else
#   define LIBEXTCPP_NOEXCEPT throw()
#endif


#define LIBEXTCPP_RETURN_IF(condition, ret) if (condition) return ret
#define LIBEXTCPP_RETURN_IF_VOID(condition) if (condition) return


//LIBEXTCPP's force-inline policy:
//If a function contains virtually one function call or can be done in O(1) time,
//then these functions can be marked with LIBEXTCPP_FORCE_INLIINE
//Otherwise, we don't suggest using this macro.
//Note: some compiler may not support this macro.
#if !defined(LIBEXTCPP_FORCE_INLINE)
#  if defined(_MSC_VER)
#    define LIBEXTCPP_FORCE_INLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
#    define LIBEXTCPP_FORCE_INLINE inline __attribute__ ((__always_inline__))
#  else
#    define LIBEXTCPP_FORCE_INLINE inline
#  endif
#endif


//constantly using this macro might cause increament of runtime.
#if !defined(LIBEXTCPP_AVOID_INLINE)
#  if defined(_MSC_VER)
#    define LIBEXTCPP_AVOID_INLINE __declspec(noinline)
#  elif defined(__GNUC__) && __GNUC__ > 3
#    define LIBEXTCPP_AVOID_INLINE inline __attribute__ ((noinline))
#  else
#    define LIBEXTCPP_AVOID_INLINE
#  endif
#endif


#define LIBEXTCPP_STACK_SIZE_MAX_LOG2 (sizeof(void*) << 3)


LIBEXTCPP_BEGIN_NAMESPACE(ext)


typedef unsigned int uint32_t;


typedef int int32_t;


typedef unsigned long long uint64_t;


typedef long long int64_t;


LIBEXTCPP_END_NAMESPACE

#endif /* config_h */
