//
//  container_base_dispatch.h
//  libextc++
//
//  Created by 李志航 on 2021/5/28.
//

#ifndef libextcpp_container_base_dispatch_h
#define libextcpp_container_base_dispatch_h
#include <extc++/pb_ds/bin_search_tree/rb_tree.h>
#include <extc++/pb_ds/bin_search_tree/splay_tree.h>
#include <extc++/pb_ds/bin_search_tree/treap.h>
#include <extc++/pb_ds/bin_search_tree/balanced_tree.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class Key,
          class Data,
          class Compare,
          template <class Node_Const_Iterator,
                    class Node_Iterator,
                    class Cmp_Fn,
                    class Allocator>
          class Node_Update,
          class Tag,
          class Alloc>
struct select_tree_base;


#define PB_DS_CREATE_TREE_BASE_SELECTOR_ENTRY(tag, name) \
template <class Key, \
          class Data, \
          class Compare, \
          template <class Node_Const_Iterator, \
                    class Node_Iterator, \
                    class Cmp_Fn, \
                    class Allocator> \
          class Node_Update, \
          class Alloc> \
struct select_tree_base<Key, Data, Compare, Node_Update, tag, Alloc> { \
private: \
    typedef tree_traits<Key, Data, Compare, Node_Update, tag, Alloc> traits_type; \
public: \
    typedef name<Key, Data, Compare, traits_type, Alloc> type; \
} \


PB_DS_CREATE_TREE_BASE_SELECTOR_ENTRY(treap_tag, treap);
PB_DS_CREATE_TREE_BASE_SELECTOR_ENTRY(rb_tree_tag, rb_tree);
PB_DS_CREATE_TREE_BASE_SELECTOR_ENTRY(splay_tree_tag, splay_tree);
PB_DS_CREATE_TREE_BASE_SELECTOR_ENTRY(balanced_tree_tag, balanced_tree);


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* container_base_dispatch_h */
