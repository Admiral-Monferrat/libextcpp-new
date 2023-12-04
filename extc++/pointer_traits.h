//
//  pointer_traits.h
//  libextc++
//
//  Created by 李志航 on 2022/9/2.
//

#ifndef libextcpp_pointer_traits_h
#define libextcpp_pointer_traits_h
#include <extc++/type_traits.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)


LIBEXTCPP_BEGIN_NAMESPACE(detail)


LIBEXTCPP_DEFINE_HAS_TYPEDEF(element_type);


template <class Pointer, bool = LIBEXTCPP_HAS_TYPEDEF(Pointer, element_type)>
struct pointer_traits_element_type {
    typedef typename Pointer::element_type type;
};


template <class Pointer>
struct pointer_traits_element_type<Pointer, false> {
    typedef typename select_first_argument<Pointer>::type type;
};


template <class Pointer, bool = LIBEXTCPP_HAS_TYPEDEF(Pointer, difference_type)>
struct pointer_traits_difference_type {
    typedef typename Pointer::difference_type type;
};


template <class Pointer>
struct pointer_traits_difference_type<Pointer, false> {
    typedef ptrdiff_t type;
};


template <class Tp, class Other, class = void>
struct has_rebind_other: public false_type {};


template <class Tp, class Other>
struct has_rebind_other<Tp, Other, typename void_t<
    typename Tp::template rebind<Other>::other
>::type>: public true_type {};


template <class Pointer, class Other, bool = has_rebind_other<Pointer, Other>::value>
struct pointer_traits_rebind {
    typedef typename Pointer::template rebind<Other>::other type;
};


template <class Pointer, class Other>
struct pointer_traits_rebind<Pointer, Other, false> {
    typedef typename replace_first_argument<Pointer, Other>::type type;
};


LIBEXTCPP_END_NAMESPACE //detail


template <class Pointer>
struct pointer_traits {
    typedef Pointer                                                        pointer;
    typedef typename detail::pointer_traits_element_type<pointer>::type    element_type;
    typedef typename detail::pointer_traits_difference_type<pointer>::type difference_type;
    
    template <class Up>
    struct rebind {
        typedef typename detail::pointer_traits_rebind<pointer, Up>::type other;
    };
};


template <class Tp>
struct pointer_traits<Tp*> {
    typedef Tp*            pointer;
    typedef Tp             element_type;
    typedef ptrdiff_t      difference_type;
    
    template <class Up>
    struct rebind {
        typedef Up* other;
    };
};


LIBEXTCPP_END_NAMESPACE //ext
#endif /* pointer_traits_h */
