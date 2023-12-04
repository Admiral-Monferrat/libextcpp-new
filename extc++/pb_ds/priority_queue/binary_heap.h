//
//  binary_heap.h
//  test on basic algorithms
//
//  Created by 李志航 on 2022/6/23.
//

#ifndef binary_heap_h
#define binary_heap_h
#include "../../config.h"
#include <queue>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct binary_heap_tag {};


template <class Tp, class Compare = ext::less<Tp>, class Alloc = ext::allocator<Tp>>
class binary_heap:
public std::priority_queue <
    Tp,
    std::vector <
        Tp,
        typename Alloc::template rebind<Tp>::other
    >,
    Compare
> {};
LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* binary_heap_h */
