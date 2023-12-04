//
//  list.h
//  libextc++
//
//  Created by 李志航 on 2022/9/5.
//

#ifndef libextcpp_list_h
#define libextcpp_list_h
#include <extc++/iterator.h>
#include <extc++/memory.h>
#include <extc++/algobase.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class T>
struct list_node {
    list_node<T>* prev;
    list_node<T>* next;
    aligned_membuf<T> storage;
    list_node() {}
    inline T* valptr() {
        return storage.pointer();
    }
    inline const T* valptr() const {
        return storage.pointer();
    }
};


template <class T, class Ref, class Ptr>
class list_iterator {
public:
    typedef list_iterator<T, T&, T*>      iterator;
    typedef list_iterator<T, Ref, Ptr>    this_type;
    typedef bidirectional_iterator_tag    iterator_category;
    typedef T                             value_type;
    typedef Ptr                           pointer;
    typedef Ref                           reference;
    typedef size_t                        size_type;
    typedef ptrdiff_t                     difference_type;
    typedef list_node<T>*                 node_pointer;
private:
    template <class, class> friend class  list;
    node_pointer node;
public:
    list_iterator(node_pointer x) : node(x) {}
    list_iterator(const iterator& it) : node(it.node) {}
    list_iterator() {}
public:
    bool operator==(const iterator& it) const { return node == it.node;}
    bool operator!=(const iterator& it) const { return node != it.node;}
    reference operator*() const { return *(node->valptr());}
    pointer operator->() const { return &(operator*());}
    this_type& operator++() { node = (node_pointer)((*node).next); return *this;}
    this_type operator++(int) { this_type tmp = *this; ++*this; return tmp;}
    this_type& operator--() { node = (node_pointer)((*node).prev); return *this;}
    this_type operator--(int) { this_type tmp = *this; --*this; return tmp;}
};


template <class T, class Alloc = allocator<char>>
class list {
protected:
    typedef list_node<T>                                     node;
    typedef rebind_traits<Alloc, node>                       node_alloc_traits;
    typedef typename node_alloc_traits::allocator_type       node_allocator;
public:
    typedef list<T, Alloc>                                   this_type;
    typedef typename node_alloc_traits::pointer              node_pointer;
    typedef T                                                value_type;
    typedef value_type*                                      pointer;
    typedef const value_type*                                const_pointer;
    typedef value_type&                                      reference;
    typedef const value_type&                                const_reference;
    typedef size_t                                           size_type;
    typedef ptrdiff_t                                        difference_type;
    typedef list_iterator<T, reference, pointer>             iterator;
    typedef list_iterator<T, const_reference, const_pointer> const_iterator;
    typedef reverse_bidirectional_iterator<iterator, value_type, reference, difference_type> reverse_iterator;
    typedef reverse_bidirectional_iterator<const_iterator, value_type, const_reference, difference_type> const_reverse_iterator;
protected:
    inline node_pointer allocate_node() {
        return node_alloc_traits::allocate(allocator, 1);
    }
    inline void deallocate_node(node_pointer p) {
        node_alloc_traits::deallocate(allocator, p, 1);
    }
    template <class ...Args>
    inline node_pointer construct_node(Args&& ...args) {
        node_pointer p = allocate_node();
        try {
            node_alloc_traits::construct(allocator, p->valptr(), ext::forward<Args>(args)...);
        } catch (...) {
            node_alloc_traits::destory(allocator, p);
            deallocate_node(p);
        }
        return p;
    }
    inline void drop_node(node_pointer p) {
        node_alloc_traits::destory(allocator, p->valptr());
        deallocate_node(p);
    }
public:
    list() { initialize();}
    list(size_type n) { while (n--) push_back(value_type());}
    list(size_type n, const_reference value) { while (n--) push_back(value);}
    list(const this_type& other) {
        initialize();
        for (node_pointer i = other.guard->next; i != other.guard; i = i->next)
            push_back(i->value_field);
    }
    list(this_type&& other) {
        guard = other.guard;
        other.empty_initialize();
    }
    bool operator<(const this_type& other) {
        return lexicographical_compare(begin(), end(), other.cbegin(), other.cend());
    }
    bool operator>=(const this_type& other) { return !(*this < other); }
    this_type& operator=(this_type&& other) {
        if (&other != this) {
            clear();
            drop_node(guard);
            guard = other.guard;
            other.empty_initialize();
        }
        return *this;
    }
    this_type& operator=(const this_type& other) {
        if (&other != this) {
            clear();
            for(iterator it = other.cbegin(); it != other.cend(); ++it)
                push_back(*it);
        }
        return *this;
    }
    ~list() { clear(); drop_node(guard);}
public:
    iterator begin() { return iterator(guard->next);}
    const_iterator cbegin() const { return const_iterator(guard->next);}
    iterator end() { return iterator(guard);}
    const_iterator cend() const { return const_iterator(guard);}
    reverse_iterator rbegin() { return reverse_iterator(end());}
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end());}
    reverse_iterator rend() { return reverse_iterator(begin());}
    const_reverse_iterator crend() const { return const_reverse_iterator(begin());}
    size_type size() const { return distance(begin(), end());}
    size_type max_size() const { return size_type(-1);}
    bool empty() const { return guard->next == guard;}
    reference at(iterator position) { return *position;}
    reference front() { return *begin();}
    const_reference front() const { return *cbegin();}
    reference back() { return *(--end());}
    const_reference back() const { return *(--cend());}
    reference at(size_type index) {
        iterator it = begin();
        advance(it, index);
        return *it;
    }
    iterator insert(iterator position, const_reference value) {
        node_pointer tmp = construct_node(value);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (node_pointer(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        return iterator(tmp);
    }
    void push_front(const_reference value) { insert(begin(), value);}
    void push_back(const_reference value) { insert(end(), value);}
    iterator erase(iterator position) {
        node_pointer next_node = position.node->next;
        node_pointer prev_node = position.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        drop_node(position.node);
        return iterator(next_node);
    }
    void pop_front() { erase(begin());}
    void pop_back() { erase(--end());}
    void clear() {
        node_pointer cur = guard->next;
        while (cur != guard) {
            node_pointer tmp = cur;
            cur = cur->next;
            drop_node(tmp);
        }
        guard->next = guard;
        guard->prev = guard;
    }
    void swap(this_type& lis) { ext::swap(guard, lis.guard);}
    void append(const this_type& other) {
        guard->prev->next = other.guard->next;
        other.guard->next->prev = guard->prev;
        other.guard->prev->next = guard;
        guard->prev = other.guard->prev;
        other.guard->next = other.guard;
        other.guard->prev = other.guard;
        other.node_count = 0;
    }
    size_type remove(const_reference value) {
        size_type count = 0;
        iterator first = begin();
        iterator last = end();
        if (first != last) {
            while (first != last) {
                iterator next = first;
                ++next;
                if (*first == value)
                    erase(first), ++count;
                first = next;
            }
        }
        return count;
    }
    template <class Predicate>
    size_type remove(Predicate pred) {
        size_type count = 0;
        iterator first = begin();
        iterator last = end();
        if (first != last) {
            while (first != last) {
                iterator next = first;
                ++next;
                if (pred(*first))
                    erase(first), ++count;
                first = next;
            }
        }
        return count;
    }
    void unique() {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;
        iterator next = first;
        while (++next != last) {
            if (*first == *next) erase(next);
            else first = next;
            next = first;
        }
    }
    void splice(iterator position, this_type& lis) {
        if (!lis.empty()) transfer(lis.begin(), lis.end(), position);
    }
    void splice(iterator position, this_type&, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(i, j, position);
    }
    void splice(iterator position, this_type&, iterator first, iterator last) {
        if (first != last)
            transfer(first, last, position);
    }
    template <class Compare = less<T>>
    void merge(this_type& lis, Compare comp = less<T>()) {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = lis.begin();
        iterator last2 = lis.end();
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                iterator next = first2;
                transfer(first2, ++next, first1);
                first2 = next;
            } else ++first1;
        }
        if (first2 != last2) transfer(first2, last2, last1);
    }
    void reverse() {
        if (guard->next == guard || node_pointer(guard->next)->next == guard) return;
        iterator first = begin();
        ++first;
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(old, first, begin());
        }
    }
    template <class Compare = less<T>>
    void sort(Compare comp = less<T>()) {
        if (guard->next == guard || node_pointer(guard->next)->next == guard) return;
        list<T, Alloc> carry;
        list<T, Alloc> counter[64];
        int fill = 0;
        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry, comp);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i = 1; i < fill; i++)
            counter[i].merge(counter[i - 1]);
        swap(counter[fill - 1]);
    }
protected:
    void initialize() {
        guard = allocate_node();
        guard->next = guard;
        guard->prev = guard;
    }
    void transfer(iterator first, iterator last, iterator position) {
        if (position != last) {
            (*(node_pointer((*last.node).prev))).next = position.node;
            (*(node_pointer((*first.node).prev))).next = last.node;
            (*(node_pointer((*position.node).prev))).next = first.node;
            node_pointer tmp = node_pointer((*position.node).prev);
            (*position.node).prev = (*last.node).prev;
            (*last.node).prev = (*first.node).prev;
            (*first.node).prev = tmp;
        }
    }
protected:
    node_pointer guard;
    static node_allocator allocator;
};


template <class Tp, class Alloc>
typename list<Tp, Alloc>::node_allocator list<Tp, Alloc>::allocator;


LIBEXTCPP_END_NAMESPACE
#endif /* list_h */
