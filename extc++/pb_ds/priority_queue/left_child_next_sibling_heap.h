//
//  left_child_next_sibling_heap.h
//  test on basic algorithms
//
//  Created by 李志航 on 2022/7/9.
//

#ifndef left_child_next_sibling_heap_h
#define left_child_next_sibling_heap_h
#include "../../type_traits.h"
#include "../../algobase.h"
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class T, class Compare, class Node_Metadata, class Alloc>
class left_child_next_sibling_heap;


template <class T, class Metadata>
struct left_child_next_sibling_heap_node {
private:
    typedef left_child_next_sibling_heap_node<T, Metadata> this_type;
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef Metadata metadata_type;
    typedef this_type* node_pointer;
    value_type value;
    metadata_type metadata;
    node_pointer left;
    node_pointer next_sibling;
    node_pointer prev_or_parent;
};


template <class T>
struct left_child_next_sibling_heap_node<T, ext::null_type> {
private:
    typedef left_child_next_sibling_heap_node<T, null_type> this_type;
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef this_type* node_pointer;
    value_type value;
    node_pointer left;
    node_pointer next_sibling;
    node_pointer prev_or_parent;
};


#define GLIBCXX_HEAP_BASE_IT_NAME \
    left_child_next_sibling_heap_iterator<Val, Ref, Ptr, Node>


template <class Val, class Ref, class Ptr, class Node>
class left_child_next_sibling_heap_iterator {
public:
    typedef output_iterator_tag       iterator_category;
    typedef ptrdiff_t                 difference_type;
    typedef size_t                    size_type;
    typedef Val                       value_type;
    typedef Ptr                       pointer;
    typedef Ref                       reference;
    typedef GLIBCXX_HEAP_BASE_IT_NAME iterator;
private:
    typedef Node node;
    typedef node* node_pointer;
    template <class, class, class, class> friend class left_child_next_sibling_heap;
    template <class, class, class, class> friend class left_child_next_sibling_heap_iterator;
private:
    node_pointer p_pot;
public:
    left_child_next_sibling_heap_iterator() : p_pot(nullptr) {}
    left_child_next_sibling_heap_iterator(node_pointer p_nd) : p_pot(p_nd) {}
    left_child_next_sibling_heap_iterator(const iterator& it) : p_pot(it.p_pot) {}
public:
    reference operator*() { return p_pot->value; }
    pointer operator->() { return &(operator*()); }
    bool operator==(const iterator& it) { return p_pot == it.p_pot; }
    bool operator!=(const iterator& it) { return p_pot != it.p_pot; }
};


#define GLIBCXX_HEAP_BASE_NAME \
    left_child_next_sibling_heap<T, Compare, Node_Metadata, Alloc>
#define GLIBCXX_HEAP_BASE_NODE \
    left_child_next_sibling_heap_node<T, Node_Metadata>
#define GLIBCXX_HEAP_BASE_IT \
    left_child_next_sibling_heap_iterator<value_type, const_reference, const_pointer, node>


template <class T, class Compare, class Node_Metadata, class Alloc>
class left_child_next_sibling_heap {
protected:
    typedef GLIBCXX_HEAP_BASE_NAME        Self;
    typedef GLIBCXX_HEAP_BASE_NODE        node;
    typedef node*                         node_pointer;
public:
    typedef T                             value_type;
    typedef value_type*                   pointer;
    typedef const value_type*             const_pointer;
    typedef value_type&                   reference;
    typedef const value_type&             const_reference;
    typedef ptrdiff_t                     difference_type;
    typedef size_t                        size_type;
    typedef typename Alloc::template rebind<node>::other node_allocator;
    typedef Alloc                         allocator_type;
    typedef GLIBCXX_HEAP_BASE_IT          iterator;
protected:
    node_allocator heap_node_allocator;
    node_pointer root;
    size_type node_count;
    Compare comp;
protected:
    node_pointer get_node() { return heap_node_allocator.allocate(1); }
    void put_node(node_pointer p) { heap_node_allocator.deallocate(p, 1); }
    template <class ...Args>
    node_pointer create_node(const Args& ...args) {
        node_pointer pot = get_node();
        construct_at(&pot->value, args...);
        return pot;
    }
    void destory_node(node_pointer pot) {
        destory_at(&pot->value);
        put_node(pot);
    }
    void clear_tree(node_pointer pot) {
        while (pot != nullptr) {
            clear_tree(pot->left);
            node_pointer next = pot->next_sibling;
            destory_node(pot);
            pot = next;
        }
    }
    node_pointer recursive_copy_node(node_pointer pot) {
        if (pot == nullptr) return nullptr;
        node_pointer ret = get_node();
        try {
            construct(&ret->value, pot->value);
        } catch(...) {
            put_node(ret);
            throw;
        }
        ret->left = ret->next_sibling = ret->prev_or_parent = nullptr;
        try {
            ret->left = recursive_copy_node(pot->left);
            ret->next_sibling = recursive_copy_node(pot->next_sibling);
        } catch(...) {
            clear_tree(ret);
            throw;
        }
        if (ret->left != nullptr)
            ret->left->prev_or_parent = ret;
        if (ret->next_sibling != nullptr)
            ret->next_sibling->prev_or_parent = pot->next_sibling->prev_or_parent == pot ? ret : nullptr;
        return ret;
    }
public:
    left_child_next_sibling_heap() : root(nullptr), node_count(0) {}
    left_child_next_sibling_heap(const Compare& __comp) : comp(__comp), root(nullptr), node_count(0) {}
    left_child_next_sibling_heap(const Self& other) : comp(other.comp), root(nullptr), node_count(0) {
        root = recursive_copy_node(other.p_root);
        node_count = other.node_count;
    }
    ~left_child_next_sibling_heap() { clear(); }
public:
    void value_swap(Self& other) {
        ext::swap(root, other.p_root);
        ext::swap(node_count, other.node_count);
    }
    void swap(Self& other) { value_swap(other); }
    void clear() { clear_tree(root); root = nullptr; node_count = 0; }
    bool empty() const { return node_count == 0;}
    iterator begin() {
        node_pointer pot = root;
        if (pot == nullptr)
            return iterator(nullptr);
        while (pot->left != nullptr)
            pot = pot->left;
        return iterator(pot);
    }
    iterator end() { return iterator(nullptr); }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }
protected:
    void attach_as_child(node_pointer pot, node_pointer new_parent) {
        pot->next_sibling = new_parent->left;
        if (new_parent->left != nullptr)
            new_parent->left->prev_or_parent = pot;
        pot->prev_or_parent = new_parent;
        new_parent->left = pot;
    }
    void swap_with_parent(node_pointer pot, node_pointer parent) {
        if (parent == root) root = pot;
        const bool node_direct_child = parent->left == pot;
        const bool parent_root = parent->prev_or_parent == nullptr;
        const bool parent_direct_child = !parent_root && parent->prev_or_parent->left == parent;
        ext::swap(parent->prev_or_parent, pot->prev_or_parent);
        ext::swap(parent->next_sibling, pot->next_sibling);
        ext::swap(parent->left, pot->left);
        ext::swap(parent->metadata, pot->metadata);
        if (pot->next_sibling != nullptr)
            pot->next_sibling->prev_or_parent = pot;
        if (parent->next_sibling != nullptr)
            parent->next_sibling->prev_or_parent = parent;
        if (parent->left != nullptr)
            parent->left->prev_or_parent = pot;
        if (parent_direct_child)
            pot->prev_or_parent->left = pot;
        else if (!parent_root)
            pot->prev_or_parent->next_sibling = pot;
        if (!node_direct_child) {
            pot->left->prev_or_parent = pot;
            parent->prev_or_parent->next_sibling = parent;
        } else {
            pot->left = parent;
            parent->prev_or_parent = pot;
        }
    }
    static node_pointer parent(node_pointer pot) {
        while (true) {
            node_pointer next = pot->prev_or_parent;
            if (next == nullptr || next->left == pot)
                return next;
            pot = next;
        }
    }
    void bubble_to_top(node_pointer pot) {
        node_pointer parent = parent(pot);
        while (parent != nullptr) {
            swap_with_parent(pot, parent);
            parent = parent(pot);
        }
    }
    void to_linked_list() {
        node_pointer cur = root;
        while (cur != nullptr) {
            if (cur->left != nullptr) {
                node_pointer p_child_next = cur->left->next_sibling;
                cur->left->next_sibling = cur->next_sibling;
                cur->next_sibling = cur->left;
                cur->left = p_child_next;
            } else
                cur = cur->next_sibling;
        }
    }
    template <class Predicate>
    node_pointer prune(Predicate pred) {
        node_pointer cur = root;
        root = nullptr;
        node_pointer output = nullptr;
        while (cur != nullptr) {
            node_pointer next = cur->next_sibling;
            if (pred(cur->value)) {
                cur->next_sibling = output;
                if (output != nullptr)
                    output->prev_or_parent = cur;
                output = cur;
            } else {
                cur->next_sibling = root;
                if (root != nullptr)
                    root->prev_or_parent = cur;
                root = cur;
            }
            cur = next;
        }
        return output;
    }
};


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* left_child_next_sibling_heap_h */
