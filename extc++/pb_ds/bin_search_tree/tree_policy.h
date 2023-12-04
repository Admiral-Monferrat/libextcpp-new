//
//  tree_policy.h
//  libextc++
//
//  Created by 李志航 on 2022/8/7.
//

#ifndef libextcpp_tree_policy_h
#define libextcpp_tree_policy_h
#include <extc++/iterator.h>
#include <extc++/allocator_traits.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class Node_Const_Iterator, class Node_Iterator, class Alloc>
struct branch_policy {
protected:
    typedef typename Node_Iterator::value_type                      iterator;
    typedef typename ext::iterator_traits<iterator>::value_type     value_type;
    typedef typename value_type::first_type                         key_type;
    
    typedef typename ext::remove_const<value_type>::type            rcvalue_type;
    typedef typename ext::remove_const<key_type>::type              rckey_type;
    
    typedef ext::rebind_traits<Alloc, rcvalue_type>                 value_rebind;
    typedef ext::rebind_traits<Alloc, rckey_type>                   key_rebind;
    
    typedef typename value_rebind::reference                        reference;
    typedef typename value_rebind::const_reference                  const_reference;
    typedef typename value_rebind::const_pointer                    const_pointer;
    typedef typename key_rebind::const_reference                    key_const_reference;
protected:
    
    static inline key_const_reference extract_key(const_reference value) {
        return value.first;
    }
    
    virtual ~branch_policy() { }
};


template <class Node_Const_Iterator, class Alloc>
struct branch_policy<Node_Const_Iterator, Node_Const_Iterator, Alloc> {
protected:
    typedef typename Node_Const_Iterator::value_type                iterator;
    typedef typename ext::iterator_traits<iterator>::value_type     value_type;
    typedef value_type                                              key_type;
    
    typedef typename ext::remove_const<value_type>::type            rcvalue_type;
    typedef typename ext::remove_const<key_type>::type              rckey_type;
    
    typedef ext::rebind_traits<Alloc, rcvalue_type>                 value_rebind;
    typedef ext::rebind_traits<Alloc, rckey_type>                   key_rebind;
    
    typedef typename value_rebind::reference                        reference;
    typedef typename value_rebind::const_reference                  const_reference;
    typedef typename value_rebind::const_pointer                    const_pointer;
    typedef typename key_rebind::const_reference                    key_const_reference;
protected:
    
    static inline key_const_reference extract_key(const_reference value) {
        return value;
    }
    
    virtual ~branch_policy() { }
};


template <class Node_Const_Iterator, class Node_Iterator, class Compare, class Alloc>
class null_node_update : public branch_policy<Node_Const_Iterator, Node_Iterator, Alloc> { };


template <class Node_Const_Iterator, class Node_Iterator, class Compare, class Alloc>
class tree_order_statistics_node_update:
public branch_policy<Node_Const_Iterator, Node_Iterator, Alloc> {
private:
    typedef branch_policy<Node_Const_Iterator, Node_Iterator, Alloc>   base_type;
    typedef tree_order_statistics_node_update<Node_Const_Iterator, Node_Iterator, Compare, Alloc> this_type;
    typedef allocator_traits<Alloc>                                 alloc_traits;
public:
    typedef Compare                                                 cmp_fn;
    typedef Alloc                                                   allocator_type;
    typedef typename alloc_traits::size_type                        size_type;
    typedef size_type                                               metadata_type;
    
    typedef typename base_type::key_type                            key_type;
    typedef typename base_type::key_const_reference                 key_const_reference;
    typedef typename base_type::const_reference                     const_reference;
    typedef typename base_type::const_pointer                       const_pointer;
    
    typedef ext::true_type                                          is_update_executable;
    typedef ext::true_type                                          is_size_maintainable;
    
    typedef Node_Const_Iterator                                     node_const_iterator;
    typedef Node_Iterator                                           node_iterator;
    typedef typename node_const_iterator::value_type                const_iterator;
    typedef typename node_iterator::value_type                      iterator;
private:
    typedef ext::rebind_traits<Alloc, metadata_type>                metadata_rebind;
    typedef typename metadata_rebind::reference                     metadata_reference;
    typedef typename metadata_rebind::const_reference               metadata_const_reference;
protected:
    virtual iterator end() = 0;
    virtual node_iterator node_begin() = 0;
    virtual node_iterator node_end() = 0;
    virtual iterator lower_bound(key_const_reference key) = 0;
    virtual iterator find(key_const_reference key) = 0;
    virtual size_type max_size() = 0;
public:
    iterator next(iterator p, size_type n) {
        node_iterator pot = node_iterator(p);
        while (n > 0) {
            if (n <= size_of(pot.right())) {
                pot = pot.right();
                while (n <= size_of(pot.left()))
                    pot = pot.left();
                n -= (size_of(pot.left()) + 1);
            } else {
                pot = pot.parent();
                n -= (size_of(pot.right()) + 1);
            }
        }
        return *pot;
    }
    iterator at(size_type order) {
        node_iterator pot = node_begin();
        while (pot != node_end()) {
            const size_type o = size_of(pot.left());
            if (order == o)
                return *pot;
            else if (order < o)
                pot = pot.left();
            else {
                order -= (o + 1);
                pot = pot.right();
            }
        }
        return end();
    }
    size_type order_of(iterator p) {
        node_iterator pot = node_iterator(p);
        size_type order = size_of(pot.left()) + 1;
        while (pot != node_begin()) {
            if (pot == pot.parent().right())
                order += (size_of(pot.parent().left()) + 1);
            pot = pot.parent();
        }
        return order - 1;
    }
    inline size_type order_of(key_const_reference key) {
        iterator i = lower_bound(key);
        return i == end() ? max_size() : order_of(i);
    }
protected:
    inline size_type size_of(node_iterator pot) {
        return pot == node_end() ? 0 : pot.get_metadata();
    }
    inline size_type count_node(node_iterator pot) {
        if (pot == node_end()) return 0;
        this->operator()(pot);
        return pot.get_metadata();
    }
    inline void operator()(node_iterator pot) {
        const size_type left_size = size_of(pot.left());
        const size_type right_size = size_of(pot.right());
        const_cast<metadata_reference>(pot.get_metadata()) = left_size + right_size + 1;
    }
    virtual ~tree_order_statistics_node_update() { }
};


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* tree_policy_h */
