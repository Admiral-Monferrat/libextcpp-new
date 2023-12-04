//
//  array.h
//  恶臭代码测试
//
//  Created by 李志航 on 2022/1/18.
//

#ifndef array_h
#define array_h
#include <extc++/config.h>
#include <extc++/iterator.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


template <class Tp, size_t num_elements>
class array {
public:
    typedef Tp                                   value_type;
    typedef Tp*                                  pointer;
    typedef const Tp*                            const_pointer;
    typedef Tp&                                  reference;
    typedef const Tp&                            const_reference;
    typedef ptrdiff_t                            difference_type;
    typedef size_t                               size_type;
    typedef pointer                              iterator;
    typedef const_pointer                        const_iterator;
    typedef ext::reverse_iterator<iterator>      reverse_iterator;
    typedef ext::reverse_iterator<const_pointer> const_reverse_iterator;
private:
    Tp elements[num_elements];
    size_type length;
public:
    
    array() : length(0) {}
    
    pointer data() LIBEXTCPP_NOEXCEPT {
        return elements;
    }
    
    const_pointer data() const LIBEXTCPP_NOEXCEPT {
        return elements;
    }
    
    iterator begin() LIBEXTCPP_NOEXCEPT {
        return iterator(data());
    }
    
    const_iterator begin() const LIBEXTCPP_NOEXCEPT {
        return const_iterator(data());
    }
    
    iterator end() LIBEXTCPP_NOEXCEPT {
        return begin() + length;
    }
    
    const_iterator end() const LIBEXTCPP_NOEXCEPT {
        return begin() + length;
    }
    
    size_type size() const LIBEXTCPP_NOEXCEPT {
        return length;
    }
    
    reference front() LIBEXTCPP_NOEXCEPT {
        return (*this)[0];
    }
    
    const_reference front() const LIBEXTCPP_NOEXCEPT {
        return (*this)[0];
    }
    
    reference back() LIBEXTCPP_NOEXCEPT {
        return (*this)[length - 1];
    }
    
    const_reference back() const LIBEXTCPP_NOEXCEPT {
        return (*this)[length - 1];
    }
    
    bool empty() const LIBEXTCPP_NOEXCEPT {
        return length == 0;
    }
    
    bool full() const LIBEXTCPP_NOEXCEPT {
        return length == num_elements;
    }
    
    void push(const_reference value) {
        ext::construct_at(ext::addressof(elements[length]), value);
        ++length;
    }
    
    void pop() {
        --length;
        ext::destory_at(ext::addressof(elements[length]));
    }
    
    
    reference operator[](size_type i) LIBEXTCPP_NOEXCEPT {
        return elements[i];
    }
    
    const_reference operator[](size_type i) const LIBEXTCPP_NOEXCEPT {
        return elements[i];
    }
};


LIBEXTCPP_END_NAMESPACE


#endif /* array_h */
