//
//  allocator_traits.h
//  libextc++
//
//  Created by 李志航 on 2022/1/17.
//

#ifndef libextcpp_allocator_traits_h
#define libextcpp_allocator_traits_h
#include <extc++/memory.h>
#include <extc++/pointer_traits.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


LIBEXTCPP_BEGIN_NAMESPACE(detail)


template <class Alloc, bool = LIBEXTCPP_HAS_TYPEDEF(Alloc, value_type)>
struct allocator_traits_value_type {
    typedef typename Alloc::value_type type;
};


template <class Alloc>
struct allocator_traits_value_type<Alloc, false> {
    typedef typename select_first_argument<Alloc>::type type;
};


template <class Alloc, class Size, class = void>
struct allocator_has_allocate : public false_type {};


template <class Alloc, class Size>
struct allocator_has_allocate<Alloc, Size, decltype(
    (void)ext::declval<Alloc>().allocate(ext::declval<Size>())
)>: public true_type {};


template <class Alloc, class Pointer, class Size, class = void>
struct allocator_has_deallocate : public false_type {};


template <class Alloc, class Pointer, class Size>
struct allocator_has_deallocate<Alloc, Pointer, Size, decltype(
    (void)ext::declval<Alloc>().deallocate(ext::declval<Pointer>(), ext::declval<Size>())
)>: public true_type {};


template <class, class Alloc, class ...Args>
struct allocator_has_construct_base : public false_type {};


template <class Alloc, class ...Args>
struct allocator_has_construct_base<decltype(
    (void)ext::declval<Alloc>().construct(ext::declval<Args>()...)
), Alloc, Args...>: public true_type {};


template <class Alloc, class ...Args>
struct allocator_has_construct : public allocator_has_construct_base<void, Alloc, Args...> {};


template <class Alloc, class Pointer, class = void>
struct allocator_has_destory : public false_type {};


template <class Alloc, class Pointer>
struct allocator_has_destory<Alloc, Pointer, decltype(
    (void)ext::declval<Alloc>().destory(ext::declval<Pointer>())
)>: public true_type {};


template <class Alloc, class = void>
struct allocator_has_max_size : public false_type {};


template <class Alloc>
struct allocator_has_max_size<Alloc, decltype(
    (void)ext::declval<Alloc>().max_size()
)>: public true_type {};


template <class Tp, class Alloc, class RawAlloc = typename remove_reference<Alloc>::type, bool = LIBEXTCPP_HAS_TYPEDEF(RawAlloc, pointer)>
struct allocator_traits_pointer {
    typedef typename RawAlloc::pointer type;
};


template <class Tp, class Alloc, class RawAlloc>
struct allocator_traits_pointer<Tp, Alloc, RawAlloc, false> {
    typedef Tp* type;
};


template <class Alloc, class Size, class Tp, bool = allocator_has_allocate<Alloc, Size>::value>
struct select_allocate_return_type {
    typedef decltype(ext::declval<Alloc>().allocate(ext::declval<Size>())) type;
};


template <class Alloc, class Size, class Tp>
struct select_allocate_return_type<Alloc, Size, Tp, false> {
    typedef decltype(ext::allocate(ext::declval<Size>(), ext::declval<Tp*>())) type;
};


template <class Alloc, class Size, class Tp>
struct allocator_has_compatible_pointer:
public is_same <
    typename allocator_traits_pointer<Tp, Alloc>::type,
    typename select_allocate_return_type<Alloc, Size, Tp>::type
> {};


template <class Tp, class Pointer, class Alloc, bool = LIBEXTCPP_HAS_TYPEDEF(Alloc, const_pointer)>
struct allocator_traits_const_pointer {
    typedef typename Alloc::const_pointer type;
};


template <class Tp, class Pointer, class Alloc>
struct allocator_traits_const_pointer<Tp, Pointer, Alloc, false> {
    typedef typename pointer_traits<Pointer>::template rebind<const Tp>::other type;
};


template <class Pointer, class Alloc, bool = LIBEXTCPP_HAS_TYPEDEF(Alloc, void_pointer)>
struct allocator_traits_void_pointer {
    typedef typename Alloc::void_pointer type;
};


template <class Pointer, class Alloc>
struct allocator_traits_void_pointer<Pointer, Alloc, false> {
    typedef typename pointer_traits<Pointer>::template rebind<void>::other type;
};


template <class Pointer, class Alloc, bool = LIBEXTCPP_HAS_TYPEDEF(Alloc, void_pointer)>
struct allocator_traits_const_void_pointer {
    typedef typename Alloc::const_void_pointer type;
};


template <class Pointer, class Alloc>
struct allocator_traits_const_void_pointer<Pointer, Alloc, false> {
    typedef typename pointer_traits<Pointer>::template rebind<const void>::other type;
};


template <class Pointer, class Alloc, bool = LIBEXTCPP_HAS_TYPEDEF(Alloc, difference_type)>
struct allocator_traits_difference_type {
    typedef typename Alloc::difference_type type;
};


template <class Pointer, class Alloc>
struct allocator_traits_difference_type<Pointer, Alloc, false> {
    typedef typename pointer_traits<Pointer>::difference_type type;
};


template <class Distance, class Alloc, bool = LIBEXTCPP_HAS_TYPEDEF(Alloc, size_type)>
struct allocator_traits_size_type {
    typedef typename Alloc::size_type type;
};


template <class Distance, class Alloc>
struct allocator_traits_size_type<Distance, Alloc, false> {
    typedef typename std::make_unsigned<Distance>::type type;
};


template <class Alloc, class Up, bool = has_rebind_other<Alloc, Up>::value>
struct allocator_traits_rebind {
    typedef typename Alloc::template rebind<Up>::other type;
};


template <class Alloc, class Up>
struct allocator_traits_rebind<Alloc, Up, false> {
    typedef typename replace_first_argument<Alloc, Up>::type type;
};


LIBEXTCPP_END_NAMESPACE /* detail */


template <class Alloc>
struct allocator_traits_impl {
public:
    typedef Alloc allocator_type;
    typedef typename detail::allocator_traits_value_type <
        allocator_type
    >::type value_type;
    typedef typename detail::allocator_traits_pointer <
        value_type,
        allocator_type
    >::type pointer;
    typedef typename detail::allocator_traits_const_pointer <
        value_type,
        pointer,
        allocator_type
    >::type const_pointer;
    typedef typename detail::allocator_traits_void_pointer <
        pointer,
        allocator_type
    >::type void_pointer;
    typedef typename detail::allocator_traits_const_void_pointer <
        pointer,
        allocator_type
    >::type const_void_pointer;
    typedef typename detail::allocator_traits_difference_type <
        pointer,
        allocator_type
    >::type difference_type;
    typedef typename detail::allocator_traits_size_type <
        difference_type,
        allocator_type
    >::type size_type;
private:
    typedef allocator_traits<allocator_type> this_type;
private:
    static_assert(detail::allocator_has_allocate<allocator_type, size_type>::value == detail::allocator_has_deallocate<allocator_type, pointer, size_type>::value, "Using different memory allocation/deallocation models may cause undefined-behaviours.");
    static_assert(detail::allocator_has_compatible_pointer<allocator_type, size_type, value_type>::value, "decltype(Alloc::allocate()) and typename Alloc::pointer must be the same!");
public:
    template <class Up>
    struct rebind_alloc {
        typedef typename detail::allocator_traits_rebind<Alloc, Up>::type other;
    };
    
    template <class Up>
    struct rebind_traits {
        typedef allocator_traits<typename rebind_alloc<Up>::other> other;
    };
    
    template <class Allocator = Alloc>
    inline static typename enable_if <
        detail::allocator_has_allocate<Allocator, size_type>::value,
        pointer
    >::type allocate(allocator_type& alloc, size_type num_objects) {
        return alloc.allocate(num_objects);
    }
    
    template <class Allocator = Alloc>
    inline static typename enable_if <
        !detail::allocator_has_allocate<Allocator, size_type>::value,
        pointer
    >::type allocate(allocator_type& alloc, size_type num_objects) {
        return ext::allocate(num_objects, static_cast<pointer>(0));
    }
    
    template <class Allocator = Alloc>
    inline static typename enable_if <
        detail::allocator_has_deallocate<Allocator, pointer, size_type>::value
    >::type deallocate(allocator_type& alloc, pointer p, size_type num_objects) {
        alloc.deallocate(p, num_objects);
    }
    
    template <class Allocator = Alloc>
    inline static typename enable_if <
        !detail::allocator_has_deallocate<Allocator, pointer, size_type>::value
    >::type deallocate(allocator_type& alloc, pointer p, size_type num_objects) {
        ext::deallocate(p);
    }
    
    template <class Allocator = Alloc, class Up, class ...Args>
    inline static typename enable_if <
        detail::allocator_has_construct<Allocator, Up*, Args...>::value
    >::type construct(allocator_type& alloc, Up* p, Args&& ...args) {
        alloc.construct(p, args...);
    }
    
    template <class Allocator = Alloc, class Up, class ...Args>
    inline static typename enable_if <
        !detail::allocator_has_construct<Allocator, Up*, Args...>::value
    >::type construct(allocator_type& alloc, Up* p, Args&& ...args) {
        ext::construct_at(p, args...);
    }
    
    template <class Up, class Allocator = Alloc>
    inline static typename enable_if <
        detail::allocator_has_destory<Allocator, Up*>::value
    >::type destory(allocator_type& alloc, Up* p) {
        alloc.destory(p);
    }
    
    template <class Up, class Allocator = Alloc>
    inline static typename enable_if <
        !detail::allocator_has_destory<Allocator, Up*>::value
    >::type destory(allocator_type& alloc, Up* p) {
        ext::destory_at(p);
    }
    
    template <class Allocator = Alloc>
    inline static typename enable_if <
        detail::allocator_has_max_size<Allocator>::value,
        size_type
    >::type max_size(allocator_type& alloc) {
        return alloc.max_size();
    }
    
    template <class Allocator = Alloc>
    inline static typename enable_if <
        !detail::allocator_has_max_size<Allocator>::value,
        size_type
    >::type max_size(allocator_type& alloc) {
        return size_type(-1) / sizeof(value_type);
    }
};


template <class Alloc, bool IsTemplate>
struct allocator_traits_base;


template <class Alloc>
struct allocator_traits_base<Alloc, false> {
    static_assert(ext::is_template<Alloc>::value, "ext::allocator_traits<> requires its 1-st template parameter must be a template!");
};


template <class Alloc>
struct allocator_traits_base<Alloc, true>: public allocator_traits_impl<Alloc> {};


template <class Alloc>
struct allocator_traits: public allocator_traits_base<Alloc, is_template<Alloc>::value> {};


template <class Alloc, class Tp>
struct rebind_traits: public allocator_traits<Alloc>::template rebind_traits<Tp>::other {
    typedef Tp& reference;
    typedef const Tp& const_reference;
};


LIBEXTCPP_END_NAMESPACE /* namespace ext */
#endif /* allocator_traits_h */
