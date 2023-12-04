//
//  node_storage_utility.h
//  libextc++
//
//  Created by 李志航 on 2021/3/15.
//

#ifndef libextcpp_node_storage_utility_h
#define libextcpp_node_storage_utility_h
#include <extc++/pair.h>
#include <extc++/allocator_traits.h>
LIBEXTCPP_BEGIN_NAMESPACE(ext)
LIBEXTCPP_BEGIN_NAMESPACE(pb_ds)


template <class Key, class Data>
struct select_value_type {
    typedef std::pair<const Key, Data> type;
};


template <class Key>
struct select_value_type<Key, ext::null_type> {
    typedef const Key type;
};


template <class Key, class Data, class Alloc>
struct mapped_traits {
public:
    typedef ext::allocator_traits<Alloc>                        alloc_traits;
    typedef typename alloc_traits::size_type                    size_type;
    typedef typename select_value_type<Key, Data>::type         value_type;
    typedef Key                                                 key_type;
    typedef Data                                                mapped_type;
private:
    typedef ext::rebind_traits<Alloc, value_type>               value_rebind;
    typedef ext::rebind_traits<Alloc, key_type>                 key_rebind;
    typedef ext::rebind_traits<Alloc, mapped_type>              metadata_rebind;
public:
    typedef typename metadata_rebind::pointer                   mapped_pointer;
    typedef typename metadata_rebind::const_pointer             mapped_const_pointer;
    typedef typename metadata_rebind::reference                 mapped_reference;
    typedef typename metadata_rebind::const_reference           mapped_const_reference;
    
    typedef typename value_rebind::pointer                      pointer;
    typedef typename value_rebind::const_pointer                const_pointer;
    typedef typename value_rebind::reference                    reference;
    typedef typename value_rebind::const_reference              const_reference;
    
    typedef typename key_rebind::pointer                        key_pointer;
    typedef typename key_rebind::const_pointer                  key_const_pointer;
    typedef typename key_rebind::reference                      key_reference;
    typedef typename key_rebind::const_reference                key_const_reference;
};


LIBEXTCPP_END_NAMESPACE
LIBEXTCPP_END_NAMESPACE
#endif /* node_storage_utility_h */
