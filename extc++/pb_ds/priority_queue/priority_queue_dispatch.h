//
//  priority_queue_dispatch.h
//  test on basic algorithms
//
//  Created by 李志航 on 2022/6/23.
//

#ifndef priority_queue_dispatch_h
#define priority_queue_dispatch_h
#include "left_child_next_sibling_heap.h"
#include "binary_heap.h"
#include "pairing_heap.h"
#include "binomial_heap.h"
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class Tp, class Compare, class Alloc, class Tag>
struct select_heap_base;


template <class Tp, class Compare, class Alloc>
struct select_heap_base<Tp, Compare, Alloc, binary_heap_tag> {
    typedef binary_heap<Tp, Compare, Alloc> type;
};


template <class Tp, class Compare, class Alloc>
struct select_heap_base<Tp, Compare, Alloc, pairing_heap_tag> {
    typedef pairing_heap<Tp, Compare, Alloc> type;
};


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* priority_queue_dispatch_h */
