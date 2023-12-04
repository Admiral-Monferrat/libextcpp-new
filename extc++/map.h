//
//  map.h
//  GLIBCXX
//
//  Created by 李志航 on 2022/5/1.
//

#ifndef libextcpp_map_h
#define libextcpp_map_h
#include <extc++/pb_ds/assoc_container.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


using pb_ds::tree;
using pb_ds::null_node_update;
using pb_ds::rb_tree_tag;


template <class Key, class Data, class Compare = less<Key>, class Alloc = allocator<char>>
class map {
public:
    typedef Key                                       key_type;
    typedef Data                                      data_type;
    typedef Data                                      mapped_type;
    typedef std::pair<const key_type, data_type>      value_type;
    typedef mapped_type&                              mapped_reference;
    
    typedef Compare                                   key_compare;
    typedef map<Key, Data, Compare, Alloc>            this_type;
private:
    typedef tree<Key, Data, Compare, null_node_update, rb_tree_tag, Alloc> rep_type;
    rep_type base;
private:
    typedef rebind_traits<Alloc, key_type>            rebind_k;
    typedef rebind_traits<Alloc, value_type>          rebind_v;
public:
    typedef typename rebind_k::pointer                key_pointer;
    typedef typename rebind_k::const_pointer          key_const_pointer;
    typedef typename rebind_k::reference              key_reference;
    typedef typename rebind_k::const_reference        key_const_reference;
    
    typedef typename rebind_v::pointer                pointer;
    typedef typename rebind_v::const_pointer          const_pointer;
    typedef typename rebind_v::reference              reference;
    typedef typename rebind_v::const_reference        const_reference;
    
    typedef typename rep_type::iterator               iterator;
    typedef typename rep_type::const_iterator         const_iterator;
    typedef typename rep_type::reverse_iterator       reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    
    typedef typename rebind_v::size_type              size_type;
    typedef typename rebind_v::difference_type        difference_type;
    typedef typename rebind_v::allocator_type         allocator_type;
public:
    map(): base() {}
    map(const key_compare& comp): base(comp) {};
    allocator_type get_allocator() { return base.get_allocator();}
    iterator begin() { return base.begin();}
    iterator end() { return base.end();}
    reverse_iterator rbegin() { return base.rbegin();}
    reverse_iterator rend() const { return base.rend();}
    bool empty() const { return base.empty();}
    size_type size() const { return base.size();}
    size_type max_size() const { return base.max_size();}
    void clear() { base.clear();}
    mapped_reference operator[](key_const_reference k) {
        iterator it = lower_bound(k);
        value_type tmp = value_type(k, data_type());
        if (it == end() || key_compare()(k, (*it).first))
            it = insert_at(it, tmp);
        return (*it).second;
    }
    this_type& operator=(const this_type& other) { base = other.base; return *this; }
    this_type& operator=(this_type&& other) { base = ext::move(other.base); return *this; }
    std::pair<iterator, bool> insert(const_reference v) { return base.insert_unique(v);}
    iterator insert_at(iterator position, const_reference v) { return base.insert_unique_at(position, v);}
    void swap(this_type& other) { base.swap(other); }
    void erase(iterator position) { base.erase(position);}
    void erase(key_const_reference k) { base.erase(k);}
    iterator find(key_const_reference k) { return base.find(k);}
    size_type count(key_const_reference k) const { return base.find(k) == base.end() ? 0 : 1;}
    iterator lower_bound(key_const_reference k) { return base.lower_bound(k);}
    iterator upper_bound(key_const_reference k) { return base.upper_bound(k);}
    std::pair<iterator, iterator> equal_range(key_const_reference k) { return base.equal_range(k);}
};


template <class Key, class Data, class Compare = less<Key>, class Alloc = allocator<char>>
class multimap {
public:
    typedef Key                                       key_type;
    typedef Data                                      data_type;
    typedef Data                                      mapped_type;
    typedef pair<const key_type, data_type>           value_type;
    
    typedef Compare                                   key_compare;
    typedef multimap<Key, Data, Compare, Alloc>       this_type;
private:
    typedef tree<Key, Data, Compare, null_node_update, rb_tree_tag, Alloc> rep_type;
    rep_type base;
private:
    typedef rebind_traits<Alloc, key_type>            rebind_k;
    typedef rebind_traits<Alloc, value_type>          rebind_v;
public:
    typedef typename rebind_k::key_pointer            key_pointer;
    typedef typename rebind_k::key_const_pointer      key_const_pointer;
    typedef typename rebind_k::key_reference          key_reference;
    typedef typename rebind_k::key_const_reference    key_const_reference;
    
    typedef typename rebind_v::pointer                pointer;
    typedef typename rebind_v::const_pointer          const_pointer;
    typedef typename rebind_v::reference              reference;
    typedef typename rebind_v::const_reference        const_reference;
    
    typedef typename rep_type::iterator               iterator;
    typedef typename rep_type::const_iterator         const_iterator;
    typedef typename rep_type::reverse_iterator       reverse_iterator;
    typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
    
    typedef typename rebind_v::size_type              size_type;
    typedef typename rebind_v::difference_type        difference_type;
    typedef typename rebind_v::allocator_type         allocator_type;
public:
    multimap(): base() {}
    multimap(const key_compare& comp): base(comp) {}
    allocator_type get_allocator() { return base.get_allocator();}
    iterator begin() { return base.begin();}
    iterator end() { return base.end();}
    reverse_iterator rbegin() { return base.rbegin();}
    reverse_iterator rend() const { return base.rend();}
    bool empty() const { return base.empty();}
    size_type size() const { return base.size();}
    size_type max_size() const { return base.max_size();}
    void clear() { base.clear(); }
    this_type& operator=(const this_type& other) { base = other.base; return *this; }
    this_type& operator=(this_type&& other) { base = ext::move(other.base); return *this; }
    iterator insert(const_reference v) { return base.insert_equal(v);}
    iterator insert(iterator position, const_reference v) { return base.insert_equal_at(position, v);}
    void erase(iterator position) { base.erase_at(position);}
    void erase(iterator first, iterator last) { base.erase_range(first, last);}
    void swap(this_type& other) { base.swap(other); }
    size_type erase(key_const_reference k) { return base.erase(k);}
    iterator find(key_const_reference k) { return base.find(k);}
    size_type count(key_const_reference k) { return base.count(k);}
    iterator lower_bound(key_const_reference k) { return base.lower_bound(k);}
    iterator upper_bound(key_const_reference k) { return base.upper_bound(k);}
    std::pair<iterator, iterator> equal_range(key_const_reference k) { return base.equal_range(k);}
};


LIBEXTCPP_END_NAMESPACE
#endif /* map_h */
