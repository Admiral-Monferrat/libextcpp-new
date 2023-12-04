//
//  pairing_heap.h
//  test on basic algorithms
//
//  Created by 李志航 on 2022/7/9.
//

#ifndef pairing_heap_h
#define pairing_heap_h
#include "left_child_next_sibling_heap.h"
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct pairing_heap_tag {};


template <class T, class Compare, class Alloc>
class pairing_heap: public left_child_next_sibling_heap<T, Compare, ext::null_type, Alloc> {
protected:
    typedef pairing_heap<T, Compare, Alloc>                                 this_type;
    typedef left_child_next_sibling_heap<T, Compare, ext::null_type, Alloc>  base_type;
    typedef typename base_type::node_pointer                                node_pointer;
public:
    typedef typename base_type::value_type                                  value_type;
    typedef typename base_type::pointer                                     pointer;
    typedef typename base_type::const_pointer                               const_pointer;
    typedef typename base_type::reference                                   reference;
    typedef typename base_type::const_reference                             const_reference;
    typedef typename base_type::difference_type                             difference_type;
    typedef typename base_type::size_type                                   size_type;
    typedef typename base_type::iterator                                    iterator;
    typedef typename base_type::allocator_type                              allocator_type;
protected:
    Compare comp = base_type::comp;
public:
    inline iterator push(const_reference value) {
        node_pointer pot = base_type::create_node(value);
        push_aux(pot);
        ++base_type::node_count;
        return iterator(pot);
    }
    const_reference top() {
        return base_type::root->value;
    }
    void erase(iterator position) {
        remove_node(position.p_pot);
        --base_type::node_count;
        base_type::destory_node(position.p_pot);
    }
    void pop() {
        node_pointer new_root = join_node_children(base_type::p_root);
        if (new_root != nullptr)
            new_root->prev_or_parent = nullptr;
        base_type::destory_node(base_type::p_root);
        base_type::root = new_root;
        --base_type::node_count;
    }
    template <class Predicate>
    size_type erase_if(Predicate pred) {
        if (base_type::empty()) return 0;
        base_type::to_linked_list();
        node_pointer output = base_type::prune(pred);
        size_type ersd = 0;
        while (output != nullptr) {
            ++ersd;
            node_pointer next = output->next_sibling;
            base_type::destory_node(output);
            output = next;
        }
        node_pointer cur = base_type::root;
        base_type::root = nullptr;
        while (cur != nullptr) {
            node_pointer next = cur->next_sibling;
            cur->left = cur->next_sibling = cur->prev_or_parent = nullptr;
            push_aux(cur);
            cur = next;
        }
        return ersd;
    }
    inline void join(this_type& other) {
        if (other.root == nullptr) return;
        if (base_type::root == nullptr)
            base_type::root = other.root;
        else if (comp(base_type::root->value, other.root->value)) {
            base_type::attach_as_child(base_type::root, other.root);
            base_type::root = other.root;
        } else
            base_type::attach_as_child(other.root, base_type::root);
        base_type::node_count += other.node_count;
        other.root = nullptr;
        other.node_count = 0;
    }
    template <class Predicate>
    inline void split(Predicate pred, this_type& other) {
        other.clear();
        if (base_type::empty()) return;
        base_type::to_linked_list();
        node_pointer output = base_type::prune(pred);
        while (output != nullptr) {
            --base_type::node_count;
            ++other.node_count;
            node_pointer next = output->next_sibling;
            output->left = output->next_sibling = output->prev_or_parent = nullptr;
            other.push_aux(output);
            output = next;
        }
        node_pointer cur = base_type::root;
        base_type::root = nullptr;
        while (cur != nullptr) {
            node_pointer next = cur->next_sibling;
            cur->left = cur->next_sibling = cur->prev_or_parent = nullptr;
            push_aux(cur);
            cur = next;
        }
    }
protected:
    inline void push_aux(node_pointer pot) {
        pot->left = nullptr;
        if (base_type::root == nullptr) {
            pot->next_sibling = pot->prev_or_parent = nullptr;
            base_type::root = pot;
        } else if (comp(base_type::p_root->value, pot->value)) {
            pot->next_sibling = pot->prev_or_parent = nullptr;
            base_type::attach_as_child(base_type::root, pot);
            base_type::root = pot;
        } else
            base_type::attach_as_child(pot, base_type::root);
    }
    node_pointer forward_join(node_pointer pot, node_pointer next) {
        if (comp(pot->value, next->value)) {
            next->prev_or_parent = pot->prev_or_parent;
            base_type::attach_as_child(pot, next);
            return next->next_sibling == nullptr ? next : next->next_sibling;
        }
        if (next->next_sibling != nullptr) {
            next->next_sibling->prev_or_parent = pot;
            pot->next_sibling = next->next_sibling;
            base_type::attach_as_child(next, pot);
            return pot->next_sibling;
        }
        pot->next_sibling = nullptr;
        base_type::attach_as_child(next, pot);
        return pot;
    }
    node_pointer backward_join(node_pointer pot, node_pointer next) {
        if (comp(pot->value, next->value)) {
            next->prev_or_parent = pot->prev_or_parent;
            base_type::attach_as_child(pot, next);
            return next;
        }
        pot->next_sibling = nullptr;
        base_type::attach_as_child(next, pot);
        return pot;
    }
    node_pointer join_node_children(node_pointer pot) {
        node_pointer ret = pot->left;
        if (ret == nullptr) return nullptr;
        while (ret->next_sibling != nullptr)
            ret = forward_join(ret, ret->next_sibling);
        while (ret->prev_or_parent != pot)
            ret = backward_join(ret->prev_or_parent, ret);
        return ret;
    }
    void remove_node(node_pointer pot) {
        node_pointer new_child = join_node_children(pot);
        if (pot == base_type::root) {
            if (new_child != nullptr)
                new_child->prev_or_parent = nullptr;
            base_type::root = new_child;
            return;
        }
        if (pot->prev_or_parent->left == pot) {
            if (new_child != nullptr) {
                new_child->prev_or_parent = pot->prev_or_parent;
                new_child->next_sibling = pot->next_sibling;
                if (new_child->next_sibling != nullptr)
                    new_child->next_sibling->prev_or_parent = new_child;
                pot->prev_or_parent->left = new_child;
                return;
            }
            pot->prev_or_parent->left = pot->next_sibling;
            if (pot->next_sibling != nullptr)
                pot->next_sibling->prev_or_parent = new_child;
            return;
        }
        if (new_child != nullptr) {
            new_child->prev_or_parent = pot->prev_or_parent;
            new_child->next_sibling = pot->next_sibling;
            if (new_child->next_sibling != nullptr)
                new_child->next_sibling->prev_or_parent = new_child;
            new_child->prev_or_parent->next_sibling = new_child;
            return;
        }
        pot->prev_or_parent->next_sibling = pot->next_sibling;
        if (pot->next_sibling != nullptr)
            pot->next_sibling->prev_or_parent = pot->prev_or_parent;
    }
};


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* pairing_heap_h */
