//  
//  bin_search_tree.h
//  libextc++
//
//  Created by 李志航 on 2021/3/19.
//

#ifndef libextcpp_bin_search_tree_h
#define libextcpp_bin_search_tree_h
#include <extc++/pb_ds/bin_search_tree/bin_search_tree_utility.h>
#include <extc++/stack.h>
#include <extc++/algobase.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


struct bin_search_tree_tag {};


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
class bin_search_tree : public Node_And_It_Traits::node_update {
private:
    typedef Node_And_It_Traits                               tree_traits;
    typedef rebind_traits<Alloc, typename tree_traits::node> node_alloc_traits;
    typedef bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc> this_type;
    typedef typename node_alloc_traits::allocator_type       node_allocator;
protected:
    typedef mapped_traits<Key, Data, Alloc>                  types_traits;
    typedef typename node_alloc_traits::value_type           node;
    typedef typename node_alloc_traits::pointer              node_pointer;
public:
    typedef Alloc                                            allocator_type;
    typedef Compare                                          cmp_fn;
    typedef bin_search_tree_tag                              container_category;
    typedef typename tree_traits::node_update                node_update;
protected:
    typedef tree_policy_traits<node_update>                  policy_traits;
    typedef typename policy_traits::is_update_executable     is_update_executable;
    typedef typename policy_traits::is_size_maintainable     is_size_maintainable;
public:
    typedef typename node_alloc_traits::size_type            size_type;
    typedef typename node_alloc_traits::difference_type      difference_type;
    
    typedef typename types_traits::key_type                  key_type;
    typedef typename types_traits::key_reference             key_reference;
    typedef typename types_traits::key_const_reference       key_const_reference;
    typedef typename types_traits::key_pointer               key_pointer;
    typedef typename types_traits::key_const_pointer         key_const_pointer;
    
    typedef typename types_traits::mapped_type               mapped_type;
    typedef typename types_traits::mapped_reference          mapped_reference;
    typedef typename types_traits::mapped_const_reference    mapped_const_reference;
    typedef typename types_traits::mapped_pointer            mapped_pointer;
    typedef typename types_traits::mapped_const_pointer      mapped_const_pointer;
    
    typedef typename types_traits::value_type                value_type;
    typedef typename types_traits::reference                 reference;
    typedef typename types_traits::const_reference           const_reference;
    typedef typename types_traits::pointer                   pointer;
    typedef typename types_traits::const_pointer             const_pointer;
    
    typedef typename tree_traits::iterator                   iterator;
    typedef typename tree_traits::const_iterator             const_iterator;
    typedef typename tree_traits::reverse_iterator           reverse_iterator;
    typedef typename tree_traits::const_reverse_iterator     const_reverse_iterator;
private:
    typedef typename tree_traits::node_iterator              node_iterator;
    typedef typename tree_traits::node_const_iterator        node_const_iterator;
protected:
    static node_allocator allocator;
    cmp_fn key_compare;
    size_type node_count;
    node_pointer head;
protected:
    node_pointer allocate_node() {
        return node_alloc_traits::allocate(allocator, 1);
    }
    void deallocate_node(node_pointer p) {
        node_alloc_traits::deallocate(allocator, p, 1);
    }
    template <class ...Args>
    node_pointer construct_node(Args&& ...args) {
        node_pointer pot = allocate_node();
        try {
            node_alloc_traits::construct(allocator, pot->valptr(), ext::forward<Args>(args)...);
        } catch(...) {
            node_alloc_traits::destory(allocator, pot->valptr());
            deallocate_node(pot);
        }
        pot->type = internal;
        return pot;
    }
    void destory_node(node_pointer pot) {
        node_alloc_traits::destory(allocator, pot->valptr());
        pot->~node();
    }
    void drop_node(node_pointer p) {
        destory_node(p);
        deallocate_node(p);
    }
    node_pointer clone_node(node_pointer p) {
        node_pointer pot = construct_node(value(p));
        pot->type = p->type;
        pot->parent = nullptr;
        pot->left = nullptr;
        pot->right = nullptr;
        return pot;
    }
public:
    bin_search_tree() : key_compare() { initialize(); }
    explicit bin_search_tree(const Compare& comp) : key_compare(comp) { initialize(); }
    bin_search_tree(const this_type& other) {
        if (&other != this) {
            if (other.head->parent == nullptr)
                initialize();
            else {
                initialize();
                root() = copy(other.root(), head);
                leftmost() = minimum(root());
                rightmost() = maximum(root());
            }
            node_count = other.node_count;
        }
    }
    bin_search_tree(this_type&& other) noexcept {
        if (&other != this) {
            head = other.head;
            leftmost() = other.leftmost();
            rightmost() = other.rightmost();
            node_count = other.node_count;
            other.initialize();
        }
    }
    ~bin_search_tree() { clear(); deallocate_node(head); }
protected:
    inline node_pointer& root() const {
        return head->parent;
    }
    inline node_pointer& leftmost() {
        return head->left;
    }
    inline node_pointer& rightmost() {
        return head->right;
    }
    inline const_reference value(node_pointer pot) const {
        return *(pot->valptr());
    }
    inline key_const_reference key(node_pointer pot) const {
        return this->extract_key(value(pot));
    }
    inline key_const_reference key(const_reference value) const {
        return this->extract_key(value);
    }
    inline node_pointer extract(iterator it) {
        return it.pot;
    }
    virtual iterator insert_at_node(node_pointer x, node_pointer y, const_reference value) = 0;
    virtual void erase_node(node_pointer x) = 0;
public:
    inline allocator_type get_allocator() {
        return allocator_type();
    }
    inline iterator begin() {
        return iterator(leftmost());
    }
    inline iterator end() {
        return iterator(head);
    }
    inline reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    inline reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    inline bool empty() const {
        return node_count == 0;
    }
    inline size_type size() const {
        return node_count;
    }
    inline size_type max_size() {
        return node_alloc_traits::max_size(allocator);
    }
protected:
    inline node_allocator& get_node_allocator() {
        return allocator;
    }
    inline cmp_fn& get_comparator() {
        return key_compare;
    }
    inline node_iterator node_begin() {
        return node_iterator(root());
    }
    inline node_iterator node_end() {
        return node_iterator(nullptr);
    }
public:
    inline void swap(this_type& t) {
        ext::swap(head, t.head);
        ext::swap(node_count, t.node_count);
        ext::swap(key_compare, t.key_compare);
    }
    inline void clear() {
        if (node_count != 0) {
            remove(root());
            leftmost() = head;
            rightmost() = head;
            root() = nullptr;
            node_count = 0;
        }
    }
private:
    inline void apply_update_switch(node_pointer pot, ext::false_type) { }
    inline void apply_update_switch(node_pointer pot, ext::true_type) {
        node_update::operator()(node_iterator(pot));
    }
    inline void update_to_top_switch(node_pointer pot, ext::false_type) { }
    void update_to_top_switch(node_pointer pot, ext::true_type) {
        while (pot != head) {
            apply_update_switch(pot, is_update_executable());
            pot = pot->parent;
        }
    }
    size_type recursive_count(node_pointer p) {
        size_type ret = 0;
        if (p != nullptr)
            ret = recursive_count(p->left) + recursive_count(p->right) + 1;
        return ret;
    }
    inline size_type count_switch(node_pointer p, ext::false_type) {
        return recursive_count(p);
    }
    inline size_type count_switch(node_pointer p, ext::true_type) {
        return this->count_node(node_iterator(p));
    }
    template <class Function>
    Function for_each_recursive(node_pointer root, Function& function) {
        if (root != nullptr) {
            for_each_recursive(root->left, function);
            function(*(root->valptr()));
            for_each_recursive(root->right, function);
        }
        return function;
    }
protected:
    inline void apply_update(node_pointer pot) {
        apply_update_switch(pot, is_update_executable());
    }
    inline void update_to_top(node_pointer pot) {
        update_to_top_switch(pot, is_update_executable());
    }
    inline size_type count(node_pointer p) {
        return count_switch(p, is_size_maintainable());
    }
public:
    std::pair<iterator, bool> insert_unique(const_reference value) {
        node_pointer y = head;
        node_pointer x = root();
        bool comp = true;
        while (x != nullptr) {
            y = x;
            comp = key_compare(key(value), key(x));
            x = comp ? x->left : x->right;
        }
        iterator test = iterator(y);
        if (comp) {
            if (test == begin())
                return std::make_pair(insert_at_node(x, y, value), true);
            else --test;
        }
        if (key_compare(key(test.pot), key(value)))
            return std::make_pair(insert_at_node(x, y, value), true);
        return std::make_pair(test, false);
    }
    iterator insert_equal(const_reference value) {
        node_pointer y = head;
        node_pointer x = root();
        while (x != nullptr) {
            y = x;
            x = key_compare(key(value), key(x)) ? x->left : x->right;
        }
        return insert_at_node(x, y, value);
    }
    iterator insert_unique_at(iterator position, const_reference value) {
        if (position.pot == head->left) {
            if (size() > 0 && key_compare(key(value), key(position.pot)))
                return insert_at_node(position.pot, position.pot, value);
            else
                return insert_unique(value).first;
        } else if (position.pot == head) {
            if (key_compare(key(rightmost()), key(value)))
                return insert_at_node(nullptr, rightmost(), value);
            else
                return insert_unique(value).first;
        } else {
            iterator before = position;
            --before;
            if (key_compare(key(before.pot), key(value)) && key_compare(key(value), key(position.pot))) {
                if (before.pot->right == nullptr)
                    return insert_at_node(nullptr, before.pot, value);
                else
                    return insert_at_node(position.pot, position.pot, value);
            } else
                return insert_unique(value).first;
        }
    }
    iterator insert_equal_at(iterator position, const_reference value) {
        if (position.pot == head->left) {
            if (size() > 0 && !key_compare(key(position.pot), key(value)))
                return insert_at_node(position.pot, position.pot, value);
            else
                return insert_equal(value);
        } else if (position.pot == head) {
            if (!key_compare(key(value), key(rightmost())))
                return insert_at_node(nullptr, rightmost(), value);
            else
                return insert_equal(value);
        } else {
            iterator before = position;
            --before;
            if (!key_compare(key(value), key(before.pot)) && !key_compare(key(position.pot), key(value))) {
                if (before.pot->right == nullptr)
                    return insert_at_node(nullptr, before.pot, value);
                else
                    return insert_at_node(position.pot, position.pot, value);
            } else
                return insert_equal(value);
        }
    }
    void erase_at(iterator position) {
        if (position != end()) {
            erase_node(position.pot);
            --node_count;
        }
    }
    size_type erase_range(iterator first, iterator last) {
        size_type ersd = 0;
        while (first != last) {
            erase_at(first++);
            ++ersd;
        }
        return ersd;
    }
    size_type erase(key_const_reference key) {
        std::pair<iterator, iterator> range = equal_range(key);
        size_type ret = 0;
        if (range.first == begin() && range.second == end()) {
            ret = node_count;
            clear();
        } else
            ret = erase_range(range.first, range.second);
        return ret;
    }
    iterator find(key_const_reference key) {
        node_pointer y = head;
        node_pointer x = root();
        while (x != nullptr) {
            if (!key_compare(this->key(x), key))
                y = x, x = x->left;
            else x = x->right;
        }
        iterator j = iterator(y);
        return (j == end() || key_compare(key, this->key(j.pot))) ? end() : j;
    }
    size_type count(key_const_reference key) {
        std::pair<iterator, iterator> range = equal_range(key);
        size_type ret = 0;
        if (range.first == begin() && range.second == end())
            ret = node_count;
        else ret = ext::distance(range.first, range.second);
        return ret;
    }
    iterator lower_bound(key_const_reference key) {
        node_pointer y = head;
        node_pointer x = root();
        while (x != nullptr) {
            if (!key_compare(this->key(x), key))
                y = x, x = x->left;
            else x = x->right;
        }
        return iterator(y);
    }
    iterator upper_bound(key_const_reference key) {
        node_pointer y = head;
        node_pointer x = root();
        while (x != nullptr) {
            if (key_compare(key, this->key(x)))
                y = x, x = x->left;
            else x = x->right;
        }
        return iterator(y);
    }
    inline std::pair<iterator, iterator> equal_range(key_const_reference key) {
        return std::make_pair(lower_bound(key), upper_bound(key));
    }
    template <class Function>
    Function for_each(Function function) {
        return for_each_recursive(root(), function);
    }
protected:
    node_pointer insert_leaf_new(node_pointer x, node_pointer y, const_reference value) {
        node_pointer pot = construct_node(value);
        if (y == head || x != nullptr || key_compare(this->extract_key(value), key(y))) {
            y->left = pot;
            if (y == head)
                root() = pot;
            else if (leftmost() == y)
                leftmost() = pot;
        } else {
            y->right = pot;
            if (rightmost() == y)
                rightmost() = pot;
        }
        pot->parent = y;
        pot->left = nullptr;
        pot->right = nullptr;
        ++node_count;
        update_to_top(pot);
        return pot;
    }
    void remove(node_pointer x) {
        while (x != nullptr) {
            remove(x->right);
            node_pointer y = x->left;
            drop_node(x);
            x = y;
        }
    }
    node_pointer copy(node_pointer x, node_pointer p) {
        node_pointer top = clone_node(x);
        top->parent = p;
        try {
            if (x->right != nullptr)
                top->right = copy(x->right, top);
            p = top;
            x = x->left;
            while (x != nullptr) {
                node_pointer y = clone_node(x);
                p->left = y;
                y->parent = p;
                if (x->right != nullptr)
                    y->right = copy(x->right, y);
                p = y;
                x = x->left;
            }
        } catch(...) {
            remove(top);
        }
        return top;
    }
    node_pointer minimum(node_pointer x) {
        while (x->left != nullptr)
            x = x->left;
        return x;
    }
    node_pointer maximum(node_pointer x) {
        while (x->right != nullptr)
            x = x->right;
        return x;
    }
    void initialize() {
        head = allocate_node();
        head->type = header;
        root() = nullptr;
        leftmost() = head;
        rightmost() = head;
        node_count = 0;
    }
    void rotate_left(node_pointer x) {
        node_pointer y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;
        if (head->parent == x)
            head->parent = y;
        else if (x->parent->left == x)
            x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
        apply_update(x);
        apply_update(y);
    }
    void rotate_right(node_pointer x) {
        node_pointer y = x->left;
        x->left = y->right;
        if (y->right != nullptr)
            y->right->parent = x;
        y->parent = x->parent;
        if (head->parent == x)
            head->parent = y;
        else if (x->parent->left == x)
            x->parent->left = y;
        else x->parent->right = y;
        y->right = x;
        x->parent = y;
        apply_update(x);
        apply_update(y);
    }
    void rotate_parent(node_pointer x) {
        node_pointer x_parent = x->parent;
        (x == x_parent->left) ? rotate_right(x_parent) : rotate_left(x_parent);
    }
    void update_min_max_for_erased_node(node_pointer z) {
        if (node_count == 1) {
            head->left = head->right = head;
            return;
        }
        if (head->left == z) {
            iterator it(z);
            ++it;
            head->left = it.pot;
        } else if (head->right == z) {
            iterator it(z);
            --it;
            head->right = it.pot;
        }
    }
    void initialize_min_max() {
        if (head->parent == nullptr) {
            head->left = head->right = head;
            return;
        }
        leftmost() = minimum(root());
        rightmost() = maximum(root());
    }
    bool join_prep(this_type& other) {
        if (other.node_count == 0)
            return false;
        if (node_count == 0) {
            swap(other);
            return false;
        }
        const bool greater = key_compare(key(head->right), key(other.head->left));
        const bool lesser = key_compare(key(other.head->right), key(head->left));
        if (!greater && !lesser)
            throw;
        if (lesser)
            swap(other);
        node_count += other.node_count;
        return true;
    }
    void join_finish(this_type& other) {
        initialize_min_max();
        other.initialize();
    }
    bool split_prep(key_const_reference key, this_type& other) {
        other.clear();
        if (node_count == 0)
            return false;
        if (key_compare(key, this->extract_key(value(head->left)))) {
            swap(other);
            return false;
        }
        if (!key_compare(key, this->extract_key(value(head->right))))
            return false;
        if (node_count == 1) {
            swap(other);
            return false;
        }
        return true;
    }
    void split_finish(this_type& other) {
        other.initialize_min_max();
        other.node_count = other.count(other.root());
        node_count -= other.node_count;
        initialize_min_max();
    }
};


template <class Key,
          class Data,
          class Compare,
          class Node_And_It_Traits,
          class Alloc>
typename bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc>::node_allocator
bin_search_tree<Key, Data, Compare, Node_And_It_Traits, Alloc>::allocator;


LIBEXTCPP_END_NAMESPACE /* pb_ds */
LIBEXTCPP_END_NAMESPACE /* ext */
#endif /* bin_search_tree_h */
