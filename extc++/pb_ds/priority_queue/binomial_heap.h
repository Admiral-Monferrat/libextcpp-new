//
//  binomial_heap.h
//  test on basic algorithms
//
//  Created by 李志航 on 2022/7/10.
//

#ifndef binomial_heap_h
#define binomial_heap_h
#include "left_child_next_sibling_heap.h"
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class Tp, class Compare, class Alloc>
class binomial_heap:
public left_child_next_sibling_heap<Tp, Compare, typename Alloc::size_type, Alloc> {
private:
    typedef rebind_traits<Alloc, Tp>                    rebind_v;
    typedef left_child_next_sibling_heap<Tp, Compare, typename Alloc::size_type, Alloc> base_type;
protected:
    typedef typename base_type::node node;
    typedef typename base_type::node_pointer node_pointer;
public:
    typedef Tp                                          value_type;
    typedef Compare                                     cmp_fn;
    typedef Alloc                                       allocator_type;
    typedef typename Alloc::size_type                   size_type;
    typedef typename Alloc::difference_type             difference_type;
    
    typedef typename rebind_v::pointer                  pointer;
    typedef typename rebind_v::const_pointer            const_pointer;
    typedef typename rebind_v::reference                reference;
    typedef typename rebind_v::const_reference          const_reference;
    
    typedef typename base_type::iterator iterator;
protected:
    node_pointer max_pointer;
private:
    inline node_pointer fix(node_pointer pot) {
        while (pot->next_sibling != nullptr && pot->metadata == pot->next_sibling->metadata) {
            node_pointer next = pot->next_sibling;
            if (base_type::comp(pot->value, next->value)) {
                next->prev_or_parent = pot->prev_or_parent;
                if (pot->prev_or_parent != nullptr)
                    pot->prev_or_parent->next_sibling = next;
                base_type::attach_as_child(pot, next);
                ++next->metadata;
                pot = next;
            } else {
                pot->next_sibling = next->next_sibling;
                if (pot->next_sibling != nullptr)
                    pot->next_sibling = nullptr;
                base_type::attach_as_child(next, pot);
                ++pot->metadata;
            }
        }
        if (pot->next_sibling != nullptr)
            pot->next_sibling->prev_or_parent = pot;
        return pot;
    }
    inline void insert_node(node_pointer pot) {
        if (base_type::root == nullptr) {
            pot->next_sibling = nullptr;
            pot->prev_or_parent = nullptr;
            pot->left = nullptr;
            pot->metadata = 0;
            base_type::root = pot;
            return;
        }
        if (base_type::root->metadata > 0) {
            pot->prev_or_parent = pot->left = nullptr;
            pot->next_sibling = base_type::root;
            base_type::root->prev_or_parent = pot;
            base_type::root = pot;
            pot->metadata = 0;
            return;
        }
        if (base_type::comp(base_type::root->value, pot->value)) {
            pot->next_sibling = base_type::root->next_sibling;
            pot->prev_or_parent = nullptr;
            pot->metadata = 1;
            pot->left = base_type::root;
            base_type::root->prev_or_parent = pot;
            base_type::root->next_sibling = nullptr;
            base_type::root = pot;
        } else {
            pot->next_sibling = nullptr;
            pot->left = nullptr;
            pot->prev_or_parent = base_type::root;
            pot->metadata = 0;
            base_type::root->left = pot;
            base_type::root->metadata = 1;
        }
        base_type::root = fix(base_type::root);
    }
public:
    inline iterator push(const_reference value) {
        node_pointer pot = base_type::create_node(value);
        insert_node(pot);
        max_pointer = nullptr;
        return iterator(pot);
    }
    inline void modify(iterator pot, const_reference new_value) {
        
    }
    inline const_reference top() const {
        
    }
    inline void erase(iterator pot) {
        
    }
};


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* binomial_heap_h */
