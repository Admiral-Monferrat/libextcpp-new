//
//  bin_search_tree_utility.h
//  libextc++
//
//  Created by 李志航 on 2021/3/15.
//

#ifndef libextcpp_bin_search_tree_utility_h
#define libextcpp_bin_search_tree_utility_h
#include <extc++/memory.h>
#include <extc++/iterator.h>
#include <extc++/pb_ds/node_storage_utility.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
class bin_search_tree;


enum bin_search_tree_node_type { internal, header };


template <class Extra, class Value_Type, class Node_Metadata, class Alloc>
struct bin_search_tree_node: public Extra {
public:
    typedef Node_Metadata metadata_type;
    typedef bin_search_tree_node node;
private:
    typedef ext::rebind_traits<Alloc, node> node_rebind;
    typedef ext::rebind_traits<Alloc, metadata_type> metadata_rebind;
public:
    typedef typename node_rebind::pointer node_pointer;
    typedef typename metadata_rebind::reference metadata_reference;
    typedef typename metadata_rebind::const_reference metadata_const_reference;
public:
    inline metadata_const_reference get_metadata() const {
        return metadata;
    }
    inline metadata_reference get_metadata() {
        return metadata;
    }
    inline Value_Type* valptr() {
        return storage.pointer();
    }
    inline const Value_Type* valptr() const {
        return storage.pointer();
    }
    inline bool special() const {
        return type == header;
    }
    metadata_type metadata;
    node_pointer parent;
    node_pointer left;
    node_pointer right;
    bin_search_tree_node_type type;
    ext::aligned_membuf<Value_Type> storage;
};


template <class Extra, class Value_Type, class Alloc>
struct bin_search_tree_node<Extra, Value_Type, ext::null_type, Alloc>: public Extra {
public:
    typedef ext::null_type metadata_type;
    typedef bin_search_tree_node node;
private:
    typedef ext::rebind_traits<Alloc, node> node_rebind;
public:
    typedef typename node_rebind::pointer node_pointer;
public:
    inline Value_Type* valptr() {
        return storage.pointer();
    }
    inline const Value_Type* valptr() const {
        return storage.pointer();
    }
    inline bool special() const {
        return type == header;
    }
    node_pointer parent;
    node_pointer left;
    node_pointer right;
    bin_search_tree_node_type type;
    ext::aligned_membuf<Value_Type> storage;
};


template <class Node, class Iterator, class Alloc>
class bin_search_tree_node_iterator {
private:
    template <class, class, class, class, class> friend class bin_search_tree;
    typedef Node                             node;
    typedef ext::rebind_traits<Alloc, node>  node_rebind;
    typedef typename node_rebind::pointer    node_pointer;
    typedef bin_search_tree_node_iterator<Node, Iterator, Alloc> this_type;
public:
    typedef trivial_iterator_tag             iterator_category;
    typedef Iterator                         value_type;
    typedef Iterator                         reference;
    typedef Iterator                         const_reference;
    typedef trivial_iterator_difference_type difference_type;
    typedef typename node::metadata_type     metadata_type;
    typedef const metadata_type&             metadata_const_reference;
public:
    explicit bin_search_tree_node_iterator(node_pointer p = nullptr) : pot(p) {}
    bin_search_tree_node_iterator(const_reference p) : pot(p.pot) {};
    inline reference operator*() {
        return value_type(pot);
    }
    inline metadata_const_reference get_metadata() const {
        return pot->get_metadata();
    }
    inline this_type left() {
        return this_type(pot->left);
    }
    inline this_type right() {
        return this_type(pot->right);
    }
    inline this_type parent() {
        return this_type(pot->parent);
    }
    inline bool operator==(const this_type& other) {
        return pot == other.pot;
    }
    inline bool operator!=(const this_type& other) {
        return pot != other.pot;
    }
private:
    node_pointer pot;
};


template <class Node_Pointer, class Value, class Pointer, class Reference>
class bin_search_tree_iterator:
public iterator <
    bidirectional_iterator_tag,
    Value,
    ptrdiff_t,
    Pointer,
    Reference
> {
private:
    typedef Node_Pointer node_pointer;
    template <class, class, class, class, class> friend class bin_search_tree;
    template <class, class, class> friend class bin_search_tree_node_iterator;
public:
    typedef bin_search_tree_iterator<Node_Pointer, Value, Pointer, Reference> this_type;
    typedef Reference reference;
    typedef Pointer pointer;
private:
    void inc() {
        if (pot->right != nullptr) {
            pot = pot->right;
            while (pot->left != nullptr)
                pot = pot->left;
        } else {
            node_pointer y = pot->parent;
            while (pot == y->right) {
                pot = y;
                y = y->parent;
            }
            if (pot->right != y)
                pot = y;
        }
    }
    void dec() {
        if (pot->special() && pot->parent->parent == pot)
            pot = pot->right;
        else if (pot->left != nullptr) {
            pot = pot->left;
            while (pot->right != nullptr)
                pot = pot->right;
        } else {
            node_pointer y = pot->parent;
            while (pot == y->left) {
                pot = y;
                y = y->parent;
            }
            pot = y;
        }
    }
public:
    bin_search_tree_iterator() {}
    bin_search_tree_iterator(node_pointer p) : pot(p) {}
    bin_search_tree_iterator(const this_type& it) : pot(it.pot) {}
    inline this_type& operator++() {
        inc();
        return *this;
    }
    inline this_type operator++(int) {
        this_type ret = *this;
        ++*this;
        return ret;
    }
    inline this_type& operator--() {
        dec();
        return *this;
    }
    inline this_type operator--(int) {
        this_type ret = *this;
        --*this;
        return ret;
    }
    inline reference operator*() {
        return *(pot->valptr());
    }
    inline pointer operator->() {
        return &(operator*());
    }
    inline bool operator==(const this_type& it) {
        return pot == it.pot;
    }
    inline bool operator!=(const this_type& it) {
        return pot != it.pot;
    }
private:
    node_pointer pot;
};


#define PB_DS_CREATE_TREE_NODE_MAPPING(tag, node) \
template <class Key, \
          class Data, \
          class Compare, \
          template <class Node_Const_Iterator, \
                    class Node_Iterator, \
                    class Cmp_Fn, \
                    class Allocator> \
          class Node_Update, \
          class Alloc> \
struct tree_traits<Key, Data, Compare, Node_Update, tag, Alloc>: \
public bin_search_tree_traits < \
    Key, \
    Data, \
    Compare, \
    Node_Update, \
    node, \
    Alloc \
> {}


LIBEXTCPP_DEFINE_HAS_TYPEDEF(is_update_executable);
LIBEXTCPP_DEFINE_HAS_TYPEDEF(is_size_maintainable);


template <class Node_Update, bool = LIBEXTCPP_HAS_TYPEDEF(Node_Update, is_update_executable)>
struct tree_policy_is_update_executable:
public Node_Update::is_update_executable {};


template <class Node_Update>
struct tree_policy_is_update_executable<Node_Update, false>:
public ext::false_type {};


template <class Node_Update, bool = LIBEXTCPP_HAS_TYPEDEF(Node_Update, is_size_maintainable)>
struct tree_policy_is_size_maintainable_base:
public Node_Update::is_size_maintainable {};


template <class Node_Update>
struct tree_policy_is_size_maintainable_base<Node_Update, false>:
public ext::false_type {};


template <class Node_Update, bool = tree_policy_is_update_executable<Node_Update>::value>
struct tree_policy_is_size_maintainable:
public Node_Update::is_size_maintainable {};

template <class Node_Update>
struct tree_policy_is_size_maintainable<Node_Update, false>:
public ext::false_type {};


template <class Node_Update>
struct tree_policy_traits {
private:
    typedef Node_Update node_update;
public:
    typedef tree_policy_is_update_executable<node_update> is_update_executable;
    typedef tree_policy_is_size_maintainable<node_update> is_size_maintainable;
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
struct tree_traits;


template <class Node_Update, bool Need_Node_Metadata>
struct node_metadata_selector;


template <class Node_Update>
struct node_metadata_selector<Node_Update, true> {
    typedef typename Node_Update::metadata_type type;
};


template <class Node_Update>
struct node_metadata_selector<Node_Update, false> {
    typedef ext::null_type type;
};

template <class Key, class Data, class Alloc>
class dumnode_const_iterator {
private:
    typedef mapped_traits<Key, Data, Alloc> traits_type;
    typedef typename traits_type::pointer const_iterator;
public:
    typedef const_iterator value_type;
    typedef const_iterator const_reference;
    typedef const_reference reference;
};


template <class Key,
          class Data,
          class Compare,
          template <class Node_Const_Iterator,
                    class Node_Iterator,
                    class Cmp_Fn,
                    class Allocator>
          class Node_Update,
          class Alloc>
struct select_node_metadata {
private:
    typedef dumnode_const_iterator<Key, Data, Alloc> iterator;
    typedef Node_Update<iterator, iterator, Compare, Alloc> node_update;
    typedef tree_policy_traits<node_update> policy_traits;
    typedef typename policy_traits::is_update_executable is_update_executable;
public:
    typedef typename node_metadata_selector <
        node_update,
        is_update_executable::value
    >::type type;
};


template <class Key,
          class Data,
          class Compare,
          template <class Node_Const_Iterator,
                    class Node_Iterator,
                    class Cmp_Fn,
                    class Allocator>
          class Node_Update,
          class Node,
          class Alloc>
struct bin_search_tree_traits {
private:
    typedef mapped_traits<Key, Data, Alloc> types_traits;
public:
    typedef bin_search_tree_node <
        Node,
        typename select_value_type<Key, Data>::type,
        typename select_node_metadata<Key, Data, Compare, Node_Update, Alloc>::type,
        Alloc
    > node;
private:
    typedef ext::rebind_traits<Alloc, node> node_alloc_traits;
public:
    typedef bin_search_tree_iterator <
        typename node_alloc_traits::pointer,
        typename types_traits::value_type,
        typename types_traits::pointer,
        typename types_traits::reference
    > iterator;
    typedef bin_search_tree_iterator <
        typename node_alloc_traits::pointer,
        typename types_traits::value_type,
        typename types_traits::const_pointer,
        typename types_traits::const_reference
    > const_iterator;
    typedef ext::reverse_bidirectional_iterator <
        iterator,
        typename types_traits::value_type,
        typename types_traits::reference,
        typename node_alloc_traits::difference_type
    > reverse_iterator;
    typedef ext::reverse_bidirectional_iterator <
        const_iterator,
        typename types_traits::value_type,
        typename types_traits::const_reference,
        typename node_alloc_traits::difference_type
    > const_reverse_iterator;
    typedef bin_search_tree_node_iterator <
        node,
        iterator,
        Alloc
    > node_iterator;
    typedef bin_search_tree_node_iterator <
        node,
        const_iterator,
        Alloc
    > node_const_iterator;
    typedef Node_Update <
        node_const_iterator,
        node_iterator,
        Compare,
        Alloc
    > node_update;
};


template <class Key,
          class Compare,
          template <class Node_Const_Iterator,
                    class Node_Iterator,
                    class Cmp_Fn,
                    class Allocator>
          class Node_Update,
          class Node,
          class Alloc>
struct bin_search_tree_traits<Key, ext::null_type, Compare, Node_Update, Node, Alloc> {
private:
    typedef mapped_traits<Key, ext::null_type, Alloc> types_traits;
public:
    typedef bin_search_tree_node <
        Node,
        typename select_value_type<Key, ext::null_type>::type,
        typename select_node_metadata<Key, ext::null_type, Compare, Node_Update, Alloc>::type,
        Alloc
    > node;
private:
    typedef ext::rebind_traits<Alloc, node> node_alloc_traits;
public:
    typedef bin_search_tree_iterator <
        typename node_alloc_traits::pointer,
        typename types_traits::value_type,
        typename types_traits::const_pointer,
        typename types_traits::const_reference
    > iterator;
    typedef iterator const_iterator;
    typedef ext::reverse_bidirectional_iterator <
        const_iterator,
        typename types_traits::value_type,
        typename types_traits::const_reference,
        typename node_alloc_traits::difference_type
    > reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;
    typedef bin_search_tree_node_iterator <
        node,
        const_iterator,
        Alloc
    > node_const_iterator;
    typedef node_const_iterator node_iterator;
    typedef Node_Update <
        node_const_iterator,
        node_iterator,
        Compare,
        Alloc
    > node_update;
};


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* bin_search_tree_utility_h */
