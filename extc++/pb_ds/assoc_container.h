//
//  assoc_container.h
//  libextc++
//
//  Created by 李志航 on 2022/5/11.
//

#ifndef libextcpp_assoc_container_h
#define libextcpp_assoc_container_h
#include <extc++/pb_ds/bin_search_tree/container_base_dispatch.h>
#include <extc++/pb_ds/bin_search_tree/tree_policy.h>
#include <extc++/functional.h>
#include <extc++/memory.h>
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
class tree;


template <class Key,
          class Data,
          class Compare = ext::less<Key>,
          template <class Node_Const_Iterator,
                    class Node_Iterator,
                    class Cmp_Fn,
                    class Allocator>
          class Node_Update = tree_order_statistics_node_update,
          class Tag = rb_tree_tag,
          class Alloc = ext::allocator<char>>
class tree: public select_tree_base<Key, Data, Compare, Node_Update, Tag, Alloc>::type {
private:
    typedef tree<Key, Data, Compare, Node_Update, Tag, Alloc> this_type;
    typedef typename select_tree_base<Key, Data, Compare, Node_Update, Tag, Alloc>::type base_type;
public:
    tree() { }
    tree(const Compare& comp): base_type(comp) {}
    tree(const this_type& other) : base_type((const base_type&)other) { }
    tree(this_type&& other) noexcept : base_type((base_type&&)other) { }
    virtual ~tree() { }
public:
    
    inline void swap(this_type& other) {
        base_type::swap(other);
    }
    
    this_type& operator=(const this_type& other) {
        if (&other != this) {
            this_type tmp(other);
            swap(tmp);
        }
        return *this;
    }
    
    this_type& operator=(this_type&& other) {
        if (&other != this) {
            this_type tmp(ext::move(other));
            swap(tmp);
        }
        return *this;
    }
};


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
inline void swap(tree<Key, Data, Compare, Node_Update, Tag, Alloc>& x,
                 tree<Key, Data, Compare, Node_Update, Tag, Alloc>& y) {
    x.swap(y);
}


LIBEXTCPP_END_NAMESPACE /* namespace pb_ds */
LIBEXTCPP_END_NAMESPACE /* namespace ext */
#endif /* assoc_container_h */
