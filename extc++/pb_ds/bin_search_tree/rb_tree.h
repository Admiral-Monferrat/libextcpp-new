//
//  rb_tree.h
//  libextc++
//
//  Created by 李志航 on 2021/5/20.
//

#ifndef libextcpp_rb_tree_h
#define libextcpp_rb_tree_h
#include <extc++/pb_ds/bin_search_tree/bin_search_tree.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct rb_tree_tag: public bin_search_tree_tag {};


enum rb_tree_color_type { red, black };


struct rb_tree_node { rb_tree_color_type color; };


PB_DS_CREATE_TREE_NODE_MAPPING(rb_tree_tag, rb_tree_node);


#define is_effectively_black(x) ((x) == nullptr || (x)->color == black)


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
class rb_tree : public bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> {
protected:
    typedef bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> base_type;
    typedef rb_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> this_type;
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
    typedef rb_tree_tag                                container_category;
public:
    rb_tree() { base_type::initialize(); base_type::head->color = red; }
    rb_tree(const Compare& comp): base_type(comp) {}
    rb_tree(const this_type& other) : base_type((const base_type&)other) {}
    rb_tree(this_type&& other) noexcept : base_type((base_type&&)other) {}
public:
    inline void swap(this_type& other) { base_type::swap(other); }
protected:
    void insert_fixup(node_pointer x) {
        x->color = red;
        while (x != base_type::root() && x->parent->color == red) {
            if (x->parent == x->parent->parent->left) {
                node_pointer y = x->parent->parent->right;
                if (y != nullptr && y->color == red) {
                    x->parent->color = black;
                    y->color = black;
                    x->parent->parent->color = red;
                    x = x->parent->parent;
                } else {
                    if (x == x->parent->right) {
                        x = x->parent;
                        base_type::rotate_left(x);
                    }
                    x->parent->color = black;
                    x->parent->parent->color = red;
                    base_type::rotate_right(x->parent->parent);
                }
            } else {
                node_pointer y = x->parent->parent->left;
                if (y != nullptr && y->color == red) {
                    x->parent->color = black;
                    y->color = black;
                    x->parent->parent->color = red;
                    x = x->parent->parent;
                } else {
                    if (x == x->parent->left) {
                        x = x->parent;
                        base_type::rotate_right(x);
                    }
                    x->parent->color = black;
                    x->parent->parent->color = red;
                    base_type::rotate_left(x->parent->parent);
                }
            }
        }
        base_type::update_to_top(x);
        base_type::root()->color = black;
    }
    void remove_fixup(node_pointer x, node_pointer new_x_parent) {
        while (x != base_type::root() && is_effectively_black(x)) {
            if (x == new_x_parent->left) {
                node_pointer w = new_x_parent->right;
                if (w->color == red) {
                    w->color = black;
                    new_x_parent->color = red;
                    base_type::rotate_left(new_x_parent);
                    w = new_x_parent->right;
                }
                if (is_effectively_black(w->left) && is_effectively_black(w->right)) {
                    w->color = red;
                    x = new_x_parent;
                    new_x_parent = new_x_parent->parent;
                } else {
                    if (is_effectively_black(w->right)) {
                        if (w->left != nullptr)
                            w->left->color = black;
                        w->color = red;
                        base_type::rotate_right(w);
                        w = new_x_parent->right;
                    }
                    w->color = new_x_parent->color;
                    new_x_parent->color = black;
                    if (w->right != nullptr)
                        w->right->color = black;
                    base_type::rotate_left(new_x_parent);
                    base_type::update_to_top(new_x_parent);
                    break;
                }
            } else {
                node_pointer w = new_x_parent->left;
                if (w->color == red) {
                    w->color = black;
                    new_x_parent->color = red;
                    base_type::rotate_right(new_x_parent);
                    w = new_x_parent->left;
                }
                if (is_effectively_black(w->left) && is_effectively_black(w->right)) {
                    w->color = red;
                    x = new_x_parent;
                    new_x_parent = new_x_parent->parent;
                } else {
                    if (is_effectively_black(w->left)) {
                        if (w->right != nullptr)
                            w->right->color = black;
                        w->color = red;
                        base_type::rotate_left(w);
                        w = new_x_parent->left;
                    }
                    w->color = new_x_parent->color;
                    new_x_parent->color = black;
                    if (w->left != nullptr)
                        w->left->color = black;
                    base_type::rotate_right(new_x_parent);
                    base_type::update_to_top(new_x_parent);
                    break;
                }
            }
        }
        if (x != nullptr)
            x->color = black;
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
            ext::swap(y->color, z->color);
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
        if (y->color != red)
            remove_fixup(x, new_x_parent);
        return y;
    }
    iterator insert_at_node(node_pointer x, node_pointer y, const_reference value) override {
        node_pointer pot = base_type::insert_leaf_new(x, y, value);
        insert_fixup(pot);
        return iterator(pot);
    }
    void erase_node(node_pointer x) override {
        node_pointer y = remove_at_node(x);
        base_type::drop_node(y);
    }
    node_pointer extract_min() {
        node_pointer x = base_type::leftmost();
        remove_at_node(x);
        return x;
    }
    size_type black_height(node_pointer x) {
        size_type height = 1;
        while (x != nullptr) {
            if (x->color == black)
                ++height;
            x = x->left;
        }
        return height;
    }
    ext::pair<node_pointer, node_pointer> find_join_pos_left(node_pointer x, size_type left_height, size_type right_height) {
        if (base_type::root() == nullptr)
            return ext::make_pair(nullptr, base_type::head);
        node_pointer y = base_type::head;
        while (right_height > left_height) {
            if (x->color == black)
                --right_height;
            y = x;
            x = x->left;
        }
        if (!is_effectively_black(x)) {
            y = x;
            x = x->left;
        }
        return ext::make_pair(x, y);
    }
    ext::pair<node_pointer, node_pointer> find_join_pos_right(node_pointer x, size_type left_height, size_type right_height) {
        if (base_type::root() == nullptr)
            return ext::make_pair(nullptr, base_type::head);
        node_pointer y = base_type::head;
        while (left_height > right_height) {
            if (x->color == black)
                --left_height;
            y = x;
            x = x->right;
        }
        if (!is_effectively_black(x)) {
            y = x;
            x = x->right;
        }
        return ext::make_pair(x, y);
    }
    void join_at_node(node_pointer x, node_pointer other_root) {
        if (other_root != nullptr)
            other_root->color = black;
        const size_type this_h = black_height(base_type::root());
        const size_type other_h = black_height(other_root);
        node_pointer new_x_left;
        node_pointer new_x_right;
        ext::pair<node_pointer, node_pointer> join_pos;
        const bool right_join = this_h >= other_h;
        if (right_join) {
            join_pos = find_join_pos_right(base_type::root(), this_h, other_h);
            new_x_left = join_pos.first;
            new_x_right = other_root;
        } else {
            new_x_left = base_type::root();
            base_type::root() = other_root;
            if (other_root != nullptr)
                other_root->parent = base_type::head;
            join_pos = find_join_pos_left(base_type::root(), this_h, other_h);
            new_x_right = join_pos.first;
        }
        node_pointer new_x_parent = join_pos.second;
        if (new_x_parent == base_type::head) {
            base_type::head->parent = x;
            x->parent = base_type::head;
        } else {
            x->parent = new_x_parent;
            if (right_join)
                x->parent->right = x;
            else x->parent->left = x;
        }
        x->left = new_x_left;
        if (new_x_left != nullptr)
            new_x_left->parent = x;
        x->right = new_x_right;
        if (new_x_right != nullptr)
            new_x_right->parent = x;
        x->color = red;
        base_type::initialize_min_max();
        base_type::update_to_top(x);
        insert_fixup(x);
    }
    void split_at_node(node_pointer x, this_type& other) {
        node_pointer x_left = x->left;
        node_pointer x_right = x->right;
        node_pointer x_parent = x->parent;
        if (x_parent == base_type::head) {
            base_type::head->parent = x_left;
            if (x_left != nullptr) {
                x_left->parent = base_type::head;
                x_left->color = black;
            }
        } else {
            if (x_parent->left == x)
                x_parent->left = x_left;
            else x_parent->right = x_left;
            if (x_left != nullptr)
                x_left->parent = x_parent;
            base_type::update_to_top(x_parent);
            if (x->color == black)
                remove_fixup(x_left, x_parent);
        }
        base_type::initialize_min_max();
        other.join_trees(x, x_right);
    }
public:
    void join(this_type& other) {
        if (base_type::join_prep(other)) {
            const node_pointer x = other.extract_min();
            join_at_node(x, other.root());
            base_type::join_finish(other);
        }
    }
    void split(key_const_reference key, this_type& other) {
        if (base_type::split_prep(key, other)) {
            iterator it = base_type::upper_bound(key);
            node_pointer pot = base_type::extract(it);
            do {
                node_pointer next = pot->parent;
                if (base_type::key_compare(key, base_type::key(base_type::value(pot))))
                    split_at_node(pot, other);
                pot = next;
            } while (pot != base_type::head);
            base_type::split_finish(other);
        }
    }
};


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* rb_tree_h */
