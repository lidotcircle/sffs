#pragma once
#include "./gn_container.h"
#include "./bptree.h"
#include <memory>
#include <type_traits>
#include <iterator>


namespace bptree_container_impl {

template<typename _Key>
using default_compare_t = std::less<_Key>;

template<typename _Key, typename _Value>
using kvpair_t = std::conditional_t<std::is_same_v<_Value,void>,_Key,std::pair<const _Key,_Value>>;

template<typename _Key, typename _Value>
using default_allocator_t = std::allocator<kvpair_t<_Key,_Value>>;

struct default_config {
    static constexpr size_t interiorOrder  = 4;
    static constexpr size_t leafOrder      = 8;
    static constexpr bool   allowEmptyLeaf = false;
    static constexpr bool   prevLeaf       = true;
    static constexpr bool   parent         = false;
};


#define BContainer BPTreeBasicContainerImpl::BPTREE<_Key,_Value,ContainerConfig::interiorOrder,ContainerConfig::leafOrder, \
                                             Compare,Alloc, ContainerConfig::allowEmptyLeaf,ContainerConfig::parent, \
                                             ContainerConfig::prevLeaf, multi>

template<typename _Key, typename _Value, bool multi,
         typename Compare = default_compare_t<_Key>,
         typename Alloc = default_allocator_t<_Key,_Value>,
         typename ContainerConfig = default_config>
class bptree_generic_container: public GnContainer<BContainer,typename BContainer::ITERATOR,_Key,_Value,kvpair_t<_Key,_Value>,std::forward_iterator_tag> {
public:
    using base_t = GnContainer<BContainer,typename BContainer::ITERATOR,_Key,_Value,kvpair_t<_Key,_Value>,std::forward_iterator_tag>;

public:
    using iterator               = typename base_t::iterator;
    using const_iterator         = typename base_t::const_iterator;
    using reverse_iterator       = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    bptree_generic_container(): base_t(BContainer(Compare(), Alloc())) {}

    explicit bptree_generic_container(const Compare& cmp, const Alloc& alloc = Alloc()): base_t(BContainer(cmp, alloc)) { }
    explicit bptree_generic_container(const Alloc& alloc): base_t(BContainer(Compare(), alloc)) { }
};


template<typename _Key, bool multi,
         typename Compare = default_compare_t<_Key>,
         typename Alloc = default_allocator_t<_Key,void>,
         typename ContainerConfig = default_config>
class bptree_generic_set: public bptree_generic_container<_Key,void,multi,Compare,Alloc> {
private:
    using base_t = bptree_generic_container<_Key,void,multi,Compare,Alloc>;

public:
    using iterator               = typename base_t::iterator;
    using const_iterator         = typename base_t::const_iterator;
    using reverse_iterator       = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    bptree_generic_set() = default;
    template<typename ...Args>
    // NOLINTNEXTLINE
    bptree_generic_set(Args&& ...args): base_t(std::forward<Args>(args)...) {}
};

template<typename _Key, typename _Value, bool multi,
         typename Compare = default_compare_t<_Key>,
         typename Alloc = default_allocator_t<_Key,_Value>,
         typename ContainerConfig = default_config>
class bptree_generic_map: public bptree_generic_container<_Key,_Value,multi,Compare,Alloc> {
private:
    using base_t = bptree_generic_container<_Key,_Value,multi,Compare,Alloc>;

public:
    using iterator               = typename base_t::iterator;
    using const_iterator         = typename base_t::const_iterator;
    using reverse_iterator       = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    bptree_generic_map() = default;
    template<typename ...Args>
    // NOLINTNEXTLINE
    bptree_generic_map(Args&& ...args): base_t(std::forward<Args>(args)...) {}
};
};


template<typename _Key, typename _Value,
         typename Compare         = bptree_container_impl::default_compare_t<_Key>,
         typename Alloc           = bptree_container_impl::default_allocator_t<_Key,_Value>,
         typename ContainerConfig = bptree_container_impl::default_config>
using bptmap = bptree_container_impl::bptree_generic_map<_Key,_Value,false,Compare,Alloc,ContainerConfig>;

template<typename _Key, typename _Value,
         typename Compare         = bptree_container_impl::default_compare_t<_Key>,
         typename Alloc           = bptree_container_impl::default_allocator_t<_Key,_Value>,
         typename ContainerConfig = bptree_container_impl::default_config>
using bptmultimap = bptree_container_impl::bptree_generic_map<_Key,_Value,true,Compare,Alloc,ContainerConfig>;


template<typename _Key,
         typename Compare         = bptree_container_impl::default_compare_t<_Key>,
         typename Alloc           = bptree_container_impl::default_allocator_t<_Key,void>,
         typename ContainerConfig = bptree_container_impl::default_config>
using bptset = bptree_container_impl::bptree_generic_set<_Key,false,Compare,Alloc,ContainerConfig>;

template<typename _Key,
         typename Compare         = bptree_container_impl::default_compare_t<_Key>,
         typename Alloc           = bptree_container_impl::default_allocator_t<_Key,void>,
         typename ContainerConfig = bptree_container_impl::default_config>
using bptmultiset = bptree_container_impl::bptree_generic_set<_Key,true,Compare,Alloc,ContainerConfig>;

