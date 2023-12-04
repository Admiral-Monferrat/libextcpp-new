//
//  balanced_tree.h
//  libextc++
//
//  Created by 李志航 on 2021/5/22.
//

#ifndef libextcpp_balanced_tree_h
#define libextcpp_balanced_tree_h
#include <extc++/pb_ds/bin_search_tree/bin_search_tree.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct balanced_tree_tag: public bin_search_tree_tag {};


struct balanced_tree_node { short height; };


PB_DS_CREATE_TREE_NODE_MAPPING(balanced_tree_tag, balanced_tree_node);


#define heightof(x) ((x != nullptr) ? (x->height) : (-1))


#define balance_factor(x) ((x == nullptr) ? 0 : (heightof(x->left) - heightof(x->right)))


#define update_height(x) (x->height = max(heightof(x->left), heightof(x->right)) + 1)


#define unbalanced(x) (balance_factor(x) <= -2 || balance_factor(x) >= 2)


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
class balanced_tree : public bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> {
private:
    typedef bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> base_type;
    typedef balanced_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> this_type;
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
    
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::allocator_type         allocator_type;
    typedef balanced_tree_tag                          container_category;
public:
    balanced_tree() { base_type::initialize(); }
protected:
    inline void rotate_left(node_pointer pot) {
        base_type::rotate_left(pot);
        update_height(pot);
        update_height(pot->parent);
    }
    inline void rotate_right(node_pointer pot) {
        base_type::rotate_right(pot);
        update_height(pot);
        update_height(pot->parent);
    }
    void rebalance(node_pointer pot, node_pointer parent) {
        while (parent != base_type::head) {
            update_height(parent);
            node_pointer grandparent = parent->parent;
            if (grandparent == base_type::head)
                grandparent = nullptr;
            if (unbalanced(grandparent)) {
                if (parent == grandparent->left) {
                    if (pot == grandparent->left->left)
                        rotate_right(grandparent);
                    else if (pot == grandparent->left->right) {
                        rotate_left(parent);
                        rotate_right(grandparent);
                    }
                } else {
                    if (pot == grandparent->right->right)
                        rotate_left(grandparent);
                    else if (pot == grandparent->right->left) {
                        rotate_right(parent);
                        rotate_left(grandparent);
                    }
                }
            }
            parent = parent->parent;
            pot = pot->parent;
        }
        base_type::update_to_top(pot);
    }
    node_pointer remove_at_node(node_pointer z) {
        node_pointer new_x_parent = nullptr;
        node_pointer x = nullptr;
        node_pointer y = z;
        if (y->left == nullptr)
            x = y->right;
        else {
            if (y->right == nullptr)
                x = y->left;
            else {
                y = y->right;
                while (y->left != nullptr)
                    y = y->left;
                x = y->right;
            }
        }
        if (y != z) {
            z->left->parent = y;
            y->left = z->left;
            if (y != z->right) {
                new_x_parent = y->parent;
                if (x != nullptr)
                    x->parent = y->parent;
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            } else
                new_x_parent = y;
            if (base_type::root() == z)
                base_type::root() = y;
            else if (z->parent->left == z)
                z->parent->left = y;
            else z->parent->right = y;
            y->parent = z->parent;
            y = z;
        } else {
            new_x_parent = y->parent;
            if (x != nullptr)
                x->parent = y->parent;
            if (base_type::root() == z)
                base_type::root() = x;
            else if (z->parent->left == z)
                z->parent->left = x;
            else z->parent->right = x;
            if (base_type::leftmost() == z) {
                if (z->right == nullptr)
                    base_type::leftmost() = z->parent;
                else base_type::leftmost() = base_type::minimum(x);
            }
            if (base_type::rightmost() == z) {
                if (z->left == nullptr)
                    base_type::rightmost() = z->parent;
                else base_type::rightmost() = base_type::maximum(x);
            }
        }
        base_type::update_to_top(new_x_parent);
        if (x == nullptr)
            x = new_x_parent;
        rebalance(x, x->parent);
        return y;
    }
    inline iterator insert_at_node(node_pointer x, node_pointer y, const_reference value) override {
        node_pointer z = base_type::insert_leaf_new(x, y, value);
        rebalance(z, y);
        return iterator(z);
    }
    void erase_node(node_pointer z) override {
        node_pointer y = remove_at_node(z);
        base_type::drop_node(y);
    }
};


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* balanced_tree_h */
