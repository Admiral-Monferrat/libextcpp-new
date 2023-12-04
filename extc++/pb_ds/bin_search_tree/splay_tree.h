//
//  splay_tree.h
//  libextc++
//
//  Created by 李志航 on 2022/5/20.
//

#ifndef libextcpp_splay_tree_h
#define libextcpp_splay_tree_h
#include <extc++/pb_ds/bin_search_tree/bin_search_tree.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct splay_tree_tag: public bin_search_tree_tag {};


PB_DS_CREATE_TREE_NODE_MAPPING(splay_tree_tag, ext::null_type);


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
class splay_tree : public bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> {
protected:
    typedef bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> base_type;
    typedef splay_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> this_type;
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
    typedef splay_tree_tag                             container_category;
public:
    splay_tree() { base_type::initialize(); }
    splay_tree(const this_type& other) : base_type((const base_type&)other) {}
    splay_tree(this_type&& other) : base_type((base_type&&)other) {}
public:
    inline void swap(this_type& other) { base_type::swap(other); }
protected:
    inline void splay_start(node_pointer x, node_pointer parent, node_pointer grandparent) {
        const bool grandparent_head = grandparent->parent == base_type::head;
        if (grandparent_head) {
            base_type::head->parent = base_type::head->parent;
            x->parent = base_type::head;
            return;
        }
        node_pointer greatgrandparent = grandparent->parent;
        x->parent = greatgrandparent;
        if (grandparent == greatgrandparent->left)
            greatgrandparent->left = x;
        else greatgrandparent->right = x;
    }
    inline void splay_finish(node_pointer x, node_pointer parent, node_pointer grandparent) {
        if (x->parent == base_type::head)
            base_type::head->parent = x;
        base_type::apply_update(grandparent);
        base_type::apply_update(parent);
        base_type::apply_update(x);
    }
    inline void splay_zig_zig_left(node_pointer x, node_pointer parent, node_pointer grandparent) {
        splay_start(x, parent, grandparent);
        node_pointer b = x->right;
        node_pointer c = parent->right;
        x->right = parent;
        parent->parent = x;
        parent->right = grandparent;
        grandparent->parent = parent;
        parent->left = b;
        if (b != nullptr)
            b->parent = parent;
        grandparent->left = c;
        if (c != nullptr)
            c->parent = grandparent;
        splay_finish(x, parent, grandparent);
    }
    inline void splay_zig_zig_right(node_pointer x, node_pointer parent, node_pointer grandparent) {
        splay_start(x, parent, grandparent);
        node_pointer b = x->left;
        node_pointer c = parent->left;
        x->left = parent;
        parent->parent = x;
        parent->left = grandparent;
        grandparent->parent = parent;
        parent->right = b;
        if (b != nullptr)
            b->parent = parent;
        grandparent->right = c;
        if (c != nullptr)
            c->parent = grandparent;
        splay_finish(x, parent, grandparent);
    }
    inline void splay_zig_zag_left(node_pointer x, node_pointer parent, node_pointer grandparent) {
        splay_start(x, parent, grandparent);
        node_pointer b = x->right;
        node_pointer c = x->left;
        x->right = parent;
        parent->parent = x;
        x->left = grandparent;
        grandparent->parent = x;
        parent->left = b;
        if (b != nullptr)
            b->parent = parent;
        grandparent->right = c;
        if (c != nullptr)
            c->parent = grandparent;
        splay_finish(x, parent, grandparent);
    }
    inline void splay_zig_zag_right(node_pointer x, node_pointer parent, node_pointer grandparent) {
        splay_start(x, parent, grandparent);
        node_pointer b = x->left;
        node_pointer c = x->right;
        x->left = parent;
        parent->parent = x;
        x->right = grandparent;
        grandparent->parent = x;
        parent->right = b;
        if (b != nullptr)
            b->parent = parent;
        grandparent->left = c;
        if (c != nullptr)
            c->parent = grandparent;
        splay_finish(x, parent, grandparent);
    }
    void splay(node_pointer x) {
        while (x->parent != base_type::head) {
            if (x->parent->parent == base_type::head)
                base_type::rotate_parent(x);
            else {
                const node_pointer parent = x->parent;
                const node_pointer grandparent = parent->parent;
                if (parent->left == x && grandparent->right == parent)
                    splay_zig_zag_left(x, parent, grandparent);
                else if (parent->right == x && grandparent->left == parent)
                    splay_zig_zag_right(x, parent, grandparent);
                else if (parent->left == x && grandparent->left == parent)
                    splay_zig_zig_left(x, parent, grandparent);
                else splay_zig_zig_right(x, parent, grandparent);
            }
        }
    }
    inline iterator insert_at_node(node_pointer x, node_pointer y, const_reference value) override {
        node_pointer pot = base_type::insert_leaf_new(x, y, value);
        splay(pot);
        base_type::initialize_min_max();
        return iterator(pot);
    }
    inline void erase_node(node_pointer x) override {
        splay(x);
        node_pointer x_left = x->left;
        node_pointer x_right = x->right;
        base_type::update_min_max_for_erase_node(x);
        base_type::drop_node(x);
        if (x_right == nullptr) {
            base_type::head->parent = x_left;
            if (x_left != nullptr)
                x_left->parent = base_type::head;
            return;
        }
        node_pointer target_right = base_type::minimum(x_right);
        x_right->parent = base_type::head;
        base_type::head->parent = x_right;
        splay(target_right);
        target_right->left = x_left;
        if (x_left != nullptr)
            x_left->parent = target_right;
        base_type::apply_update(target_right);
    }
public:
    inline void join(this_type& other) {
        if (base_type::join_prep(other)) {
            node_pointer target_right = other.minimum(other.head);
            other.splay(target_right);
            target_right->left = base_type::head->parent;
            target_right->left->parent = target_right;
            target_right->parent = base_type::head;
            base_type::apply_update(target_right);
            base_type::join_finish(other);
        }
    }
    void split(key_const_reference key, this_type& other) {
        if (base_type::split_prep(key, other)) {
            iterator it = base_type::upper_bound(key);
            node_pointer x = base_type::extract(it);
            splay(x);
            node_pointer new_root = x->left;
            base_type::head->parent = new_root;
            new_root->parent = base_type::head;
            other.head->parent = x;
            x->parent = other.head;
            x->left = nullptr;
            base_type::apply_update(x);
            base_type::split_finish(other);
        }
    }
};


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* splay_tree_h */
