//
//  typelist.h
//  libextc++
//
//  Created by 李志航 on 2021/10/5.
//

#ifndef typelist_h
#define typelist_h
#include <extc++/config.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class ...Args>
struct typelist { };


template <class Head, class ...Args>
struct typelist<Head, Args...> { };


template <class TypeList, size_t index>
struct at { };


template <class Head, class ...Args>
struct at<typelist<Head, Args...>, 0> {
    typedef Head type;
};


template <class Head, size_t index, class ...Args>
struct at<typelist<Head, Args...>, index> {
    typedef typename at<typelist<Args...>, index - 1>::type type;
};


template <class TypeList, class Tp>
struct find_type { };


template <class TypeList, class Tp>
struct push_back { };


template <class Tp, class ...Args>
struct push_back<typelist<Args...>, Tp> {
    typedef typelist<Args..., Tp> type;
};


template <class TypeList, class Tp>
struct push_front { };


template <class Tp, class ...Args>
struct push_front<typelist<Args...>, Tp> {
    typedef typelist<Tp, Args...> type;
};


template <class TypeList>
struct pop_front { };


template <class Head, class ...Args>
struct pop_front<typelist<Head, Args...>> {
    typedef typelist<Args...> type;
};


template <class TypeList1, class TypeList2>
struct concat { };


template <class ...Args1, class ...Args2>
struct concat<typelist<Args1...>, typelist<Args2...>> {
    typedef typelist<Args1..., Args2...> type;
};


template <class TypeList>
struct length { };


template <class ...Args>
struct length<typelist<Args...>> {
    constexpr static size_t value = sizeof...(Args);
};


template <class TypeList>
struct empty {
    constexpr static bool value = false;
};


template <>
struct empty<typelist<>> {
    constexpr static bool value = true;
};


LIBEXTCPP_END_NAMESPACE
#endif /* typelist_h */
