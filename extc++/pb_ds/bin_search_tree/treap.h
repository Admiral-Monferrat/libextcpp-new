//
//  treap.h
//  libextc++
//
//  Created by 李志航 on 2022/5/19.
//

#ifndef libextcpp_treap_h
#define libextcpp_treap_h
#include <extc++/pb_ds/bin_search_tree/bin_search_tree.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct treap_tag: public bin_search_tree_tag {};


struct treap_node { size_t seed; };


PB_DS_CREATE_TREE_NODE_MAPPING(treap_tag, treap_node);


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
class treap : public bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> {
protected:
    typedef treap<Key, Data, Compare, Node_And_It_Traits, Alloc> this_type;
    typedef bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> base_type;
    typedef typename base_type::node                   node;
    typedef typename base_type::node_pointer           node_pointer;
public:
    typedef typename base_type::value_type             value_type;
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    
    typedef typename base_type::key_type               key_type;
    typedef typename base_type::key_pointer            key_pointer;
    typedef typename base_type::key_const_pointer      key_const_pointer;
    typedef typename base_type::key_reference          key_reference;
    typedef typename base_type::key_const_reference    key_const_reference;
    
    typedef typename base_type::mapped_type            mapped_type;
    typedef typename base_type::mapped_pointer         mapped_pointer;
    typedef typename base_type::mapped_const_pointer   mapped_const_pointer;
    typedef typename base_type::mapped_reference       mapped_reference;
    typedef typename base_type::mapped_const_reference mapped_const_reference;
    
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::allocator_type         allocator_type;
    typedef treap_tag                                  container_category;
public:
    treap() { base_type::initialize(); base_type::head->seed = 0; }
    treap(const this_type& other) : base_type((const base_type&)other) {}
    treap(this_type&& other) : base_type((base_type&&)other) {}
public:
    inline void swap(this_type& other) { base_type::swap(other); }
protected:
    void erase_node(node_pointer x) override {
        while (x->left != nullptr && x->right != nullptr) {
            if (x->left->seed > x->right->seed)
                base_type::rotate_left(x);
            else if (x->right->seed > x->left->seed)
                base_type::rotate_right(x);
        }
        node_pointer y = x->left != nullptr ? x->left : x->right;
        if (x->parent == base_type::head)
            base_type::head->parent = y;
        else if (x->parent->left == x)
            x->parent->left = y;
        else if (x->parent->right == x)
            x->parent->right = y;
        if (y != nullptr)
            y->parent = x->parent;
        node_pointer new_y_parent = x->parent;
        base_type::update_min_max_for_erased_node(x);
        base_type::drop_node(x);
        node_pointer z = (y == nullptr) ? new_y_parent : y;
        base_type::update_to_top(z);
    }
    iterator insert_at_node(node_pointer x, node_pointer y, const_reference value) override {
        node_pointer pot = base_type::insert_leaf_new(x, y, value);
        int n = rand();
        pot->seed = n != 0 ? n : n + 1;
        while (pot != base_type::root() && pot->seed < pot->parent->seed)
            base_type::rotate_parent(pot);
//        base_type::initialize_min_max();
        return iterator(pot);
    }
public:
    inline void join(this_type& other) {
        if (base_type::join_prep(other)) {
            node_pointer temp_root = base_type::allocate_node();
            node_pointer this_root = base_type::root();
            node_pointer other_root = other.root();
            temp_root->left = this_root;
            this_root->parent = temp_root;
            temp_root->right = other_root;
            other_root->parent = temp_root;
            base_type::head->parent = temp_root;
            temp_root->parent = base_type::head;
            base_type::initialize_min_max();
            erase_node(temp_root);
            base_type::join_finish(other);
        }
    }
    inline void split(key_const_reference key, this_type& other) {
        if (base_type::split_prep(key, other)) {
            iterator it = base_type::insert_equal(key);
            node_pointer pot = base_type::extract(it);
            while (pot->parent != base_type::head)
                base_type::rotate_parent(pot);
            node_pointer x = pot->left;
            node_pointer y = pot->right;
            base_type::head->parent = x;
            x->parent = base_type::head;
            other.head->parent = y;
            y->parent = other.head;
            base_type::drop_node(pot);
            base_type::split_finish(other);
        }
    }
};


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* treap_h */
