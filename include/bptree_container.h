#pragma once
#include <iterator>
#include <memory>
#include <type_traits>

#include "./bptree.h"
#include "./gn_container.h"

namespace ldc {
namespace bptree_container_impl {

template <typename _Key>
using default_compare_t = std::less<_Key>;

template <typename _Key, typename _Value>
using kvpair_t = std::conditional_t<std::is_same_v<_Value, void>, _Key,
                                    std::pair<const _Key, _Value>>;

template <typename _Key, typename _Value>
using default_allocator_t = std::allocator<kvpair_t<_Key, _Value>>;

struct default_config {
    static constexpr size_t interiorOrder = 4;
    static constexpr size_t leafOrder = 8;
    static constexpr bool allowEmptyLeaf = false;
    static constexpr bool prevLeaf = true;
    static constexpr bool parent = false;
};

#define BContainer                                                  \
    BPTreeBasicContainerImpl::BPTreeInMemory<                       \
        _Key, _Value, _ContainerConfig::interiorOrder,              \
        _ContainerConfig::leafOrder, _Compare, _Alloc,              \
        _ContainerConfig::allowEmptyLeaf, _ContainerConfig::parent, \
        _ContainerConfig::prevLeaf, multi>

template <typename _Key, typename _Value, bool multi,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, _Value>,
          typename _ContainerConfig = default_config>
class bptree_generic_container
    : public GnContainer<BContainer, typename BContainer::ITERATOR, _Key,
                         _Value, kvpair_t<_Key, _Value>,
                         std::forward_iterator_tag> {
public:
    using base_t =
        GnContainer<BContainer, typename BContainer::ITERATOR, _Key, _Value,
                    kvpair_t<_Key, _Value>, std::forward_iterator_tag>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    bptree_generic_container() : base_t(BContainer(_Compare(), _Alloc())) {}

    explicit bptree_generic_container(const _Compare& cmp,
                                      const _Alloc& _alloc = _Alloc())
        : base_t(BContainer(cmp, _alloc)) {}
    explicit bptree_generic_container(const _Alloc& _alloc)
        : base_t(BContainer(_Compare(), _alloc)) {}
};

template <typename _Key, bool multi,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, void>,
          typename _ContainerConfig = default_config>
class bptree_generic_set
    : public bptree_generic_container<_Key, void, multi, _Compare, _Alloc> {
private:
    using base_t =
        bptree_generic_container<_Key, void, multi, _Compare, _Alloc>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    bptree_generic_set() = default;
    template <typename... Args>
    // NOLINTNEXTLINE
    bptree_generic_set(Args&&... args) : base_t(std::forward<Args>(args)...) {}
};

template <typename _Key, typename _Value, bool multi,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, _Value>,
          typename _ContainerConfig = default_config>
class bptree_generic_map
    : public bptree_generic_container<_Key, _Value, multi, _Compare, _Alloc> {
private:
    using base_t =
        bptree_generic_container<_Key, _Value, multi, _Compare, _Alloc>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    bptree_generic_map() = default;
    template <typename... Args>
    // NOLINTNEXTLINE
    bptree_generic_map(Args&&... args) : base_t(std::forward<Args>(args)...) {}
};
};  // namespace bptree_container_impl

template <typename _Key, typename _Value,
          typename _Compare = bptree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              bptree_container_impl::default_allocator_t<_Key, _Value>,
          typename _ContainerConfig = bptree_container_impl::default_config>
using bptmap =
    bptree_container_impl::bptree_generic_map<_Key, _Value, false, _Compare,
                                              _Alloc, _ContainerConfig>;

template <typename _Key, typename _Value,
          typename _Compare = bptree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              bptree_container_impl::default_allocator_t<_Key, _Value>,
          typename _ContainerConfig = bptree_container_impl::default_config>
using bptmultimap =
    bptree_container_impl::bptree_generic_map<_Key, _Value, true, _Compare,
                                              _Alloc, _ContainerConfig>;

template <typename _Key,
          typename _Compare = bptree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              bptree_container_impl::default_allocator_t<_Key, void>,
          typename _ContainerConfig = bptree_container_impl::default_config>
using bptset =
    bptree_container_impl::bptree_generic_set<_Key, false, _Compare, _Alloc,
                                              _ContainerConfig>;

template <typename _Key,
          typename _Compare = bptree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              bptree_container_impl::default_allocator_t<_Key, void>,
          typename _ContainerConfig = bptree_container_impl::default_config>
using bptmultiset =
    bptree_container_impl::bptree_generic_set<_Key, true, _Compare, _Alloc,
                                              _ContainerConfig>;
};  // namespace ldc
