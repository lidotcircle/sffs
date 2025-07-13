#pragma once
#include <iterator>
#include <memory>
#include <type_traits>

#include "./btree.h"
#include "./gn_container.h"

namespace ldc {
namespace BTree_container_impl {

template <typename _Key>
using default_compare_t = std::less<_Key>;

template <typename _Key, typename _Value>
using kvpair_t = std::conditional_t<std::is_same_v<_Value, void>, _Key,
                                    std::pair<const _Key, _Value>>;

template <typename _Key, typename _Value>
using default_allocator_t = std::allocator<kvpair_t<_Key, _Value>>;

#define BTree_BContainer                                                   \
    BTreeBasicContainerImpl::BTreeInMemory<_Key, _Value, _Compare, _Alloc, \
                                           order, parent, multi>

template <typename _Key, typename _Value, bool multi, bool parent, size_t order,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, _Value>>
class BTree_generic_container
    : public GnContainer<BTree_BContainer, typename BTree_BContainer::ITERATOR,
                         _Key, _Value, kvpair_t<_Key, _Value>,
                         std::forward_iterator_tag> {
public:
    using base_t =
        GnContainer<BTree_BContainer, typename BTree_BContainer::ITERATOR, _Key,
                    _Value, kvpair_t<_Key, _Value>, std::forward_iterator_tag>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    BTree_generic_container()
        : base_t(BTree_BContainer(_Compare(), _Alloc())) {}

    explicit BTree_generic_container(const _Compare& cmp,
                                     const _Alloc& _alloc = _Alloc())
        : base_t(BTree_BContainer(cmp, _alloc)) {}
    explicit BTree_generic_container(const _Alloc& _alloc)
        : base_t(BTree_BContainer(_Compare(), _alloc)) {}
};

template <typename _Key, bool multi,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, void>, size_t order = 4,
          bool parent = false>
class BTree_generic_set
    : public BTree_generic_container<_Key, void, multi, parent, order, _Compare,
                                     _Alloc> {
private:
    using base_t = BTree_generic_container<_Key, void, multi, parent, order,
                                           _Compare, _Alloc>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    BTree_generic_set() = default;
    template <typename... Args>
    // NOLINTNEXTLINE
    BTree_generic_set(Args&&... args) : base_t(std::forward<Args>(args)...) {}
};

template <typename _Key, typename _Value, bool multi,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, _Value>, size_t order = 8,
          bool parent = false>
class BTree_generic_map
    : public BTree_generic_container<_Key, _Value, multi, parent, order,
                                     _Compare, _Alloc> {
private:
    using base_t = BTree_generic_container<_Key, _Value, multi, parent, order,
                                           _Compare, _Alloc>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_const_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    BTree_generic_map() = default;
    template <typename... Args>
    // NOLINTNEXTLINE
    BTree_generic_map(Args&&... args) : base_t(std::forward<Args>(args)...) {}
};
};  // namespace BTree_container_impl

template <typename _Key, typename _Value,
          typename _Compare = BTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              BTree_container_impl::default_allocator_t<_Key, _Value>>
using btmap = BTree_container_impl::BTree_generic_map<_Key, _Value, false,
                                                      _Compare, _Alloc>;

template <typename _Key, typename _Value,
          typename _Compare = BTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              BTree_container_impl::default_allocator_t<_Key, _Value>>
using btmultimap = BTree_container_impl::BTree_generic_map<_Key, _Value, true,
                                                           _Compare, _Alloc>;

template <typename _Key,
          typename _Compare = BTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              BTree_container_impl::default_allocator_t<_Key, void>>
using btset =
    BTree_container_impl::BTree_generic_set<_Key, false, _Compare, _Alloc>;

template <typename _Key,
          typename _Compare = BTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              BTree_container_impl::default_allocator_t<_Key, void>>
using btmultiset =
    BTree_container_impl::BTree_generic_set<_Key, true, _Compare, _Alloc>;
};  // namespace ldc
