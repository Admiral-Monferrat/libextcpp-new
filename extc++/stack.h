//
//  stack.h
//  libextc++
//
//  Created by 李志航 on 2022/9/5.
//

#ifndef libextcpp_tree_stack_h
#define libextcpp_tree_stack_h
#include <extc++/list.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class T, class Sequence = list<T>>
class stack
{
public:
    typedef typename Sequence::value_type      value_type;
    typedef typename Sequence::size_type       size_type;
    typedef typename Sequence::reference       reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    typedef stack<T, Sequence>                 this_type;
    Sequence c;
public:
    this_type& operator=(const this_type& other) { c = other.c; return *this; }
    this_type& operator=(this_type&& other) { c = move(other.c) ; return *this; }
    bool empty() const { return c.empty();}
    size_type size() const { return c.size();}
    reference top() { return c.back();}
    const_reference top() const { return c.back();}
    void push(const_reference value) { c.push_back(value);}
    void pop() { c.pop_back();}
    void swap(stack<T>& x) { c.swap(x.c);}
};


LIBEXTCPP_END_NAMESPACE
#endif /* stack_h */
