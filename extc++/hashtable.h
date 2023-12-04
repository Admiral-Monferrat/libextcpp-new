//
//  hashtable.h
//  libextc++
//
//  Created by 李志航 on 2022/9/14.
//

#ifndef libextcpp_hashtable_h
#define libextcpp_hashtable_h
#include "algobase.h"
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Value_Type>
struct hashtable_node {
    hashtable_node* next;
    ext::aligned_membuf<Value_Type> storage;
    
    Value_Type* valptr() {
        return storage.pointer();
    }
    
    const Value_Type* valptr() const {
        return storage.pointer();
    }
};


template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable;


template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable_iterator;


template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable_const_iterator;


template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable_iterator {
    typedef hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Alloc> table;
    typedef hashtable_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_const_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef hashtable_node<Value> node;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;
    
    node* current;
    table* table_pointer;
    
    hashtable_iterator(node* pot, table* tab) : current(pot), table_pointer(tab) {}
    hashtable_iterator() {}
    
    reference operator*() { return *(current->valptr()); }
    pointer operator->() { return &(operator*()); }
    iterator& operator++() {
        const node* previous = current;
        current = current->next;
        if (current == nullptr) {
            size_type bucket = table_pointer->bucket_num(*(previous->valptr()));
            while (current == nullptr && ++bucket < table_pointer->buckets.size())
                current = table_pointer->buckets[bucket];
        }
        return *this;
    }
    iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    bool operator==(const iterator& other) {
        return current == other.current;
    }
    bool operator!=(const iterator& other) {
        return current != other.current;
    }
};


template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
};


LIBEXTCPP_END_NAMESPACE

#endif /* hashtable_h */
