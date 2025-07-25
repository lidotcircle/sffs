#pragma once
#include <iterator>
#include <memory>
#include <type_traits>

#include "./gn_container.h"
#include "./rbtree.h"

namespace ldc {
namespace RBTree_container_impl {

template <typename _Key>
using default_compare_t = std::less<_Key>;

template <typename _Key, typename _Value>
using kvpair_t = std::conditional_t<std::is_same_v<_Value, void>, _Key,
                                    std::pair<const _Key, _Value>>;

template <typename _Key, typename _Value>
using default_allocator_t = std::allocator<kvpair_t<_Key, _Value>>;

#define RBTree_BContainer                                                    \
    RBTreeBasicContainerImpl::RBTreeInMemory<_Key, _Value, _Compare, _Alloc, \
                                             parent, multi>

template <typename _Key, typename _Value, bool multi, bool parent,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, _Value>>
class RBTree_generic_container
    : public GnContainer<RBTree_BContainer,
                         typename RBTree_BContainer::ITERATOR, _Key, _Value,
                         kvpair_t<_Key, _Value>, std::forward_iterator_tag> {
public:
    using base_t =
        GnContainer<RBTree_BContainer, typename RBTree_BContainer::ITERATOR,
                    _Key, _Value, kvpair_t<_Key, _Value>,
                    std::forward_iterator_tag>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    RBTree_generic_container()
        : base_t(RBTree_BContainer(_Compare(), _Alloc())) {}

    explicit RBTree_generic_container(const _Compare& cmp,
                                      const _Alloc& _alloc = _Alloc())
        : base_t(RBTree_BContainer(cmp, _alloc)) {}
    explicit RBTree_generic_container(const _Alloc& _alloc)
        : base_t(RBTree_BContainer(_Compare(), _alloc)) {}
};

template <
    typename _Key, bool multi, typename _Compare = default_compare_t<_Key>,
    typename _Alloc = default_allocator_t<_Key, void>, bool parent = false>
class RBTree_generic_set
    : public RBTree_generic_container<_Key, void, multi, parent, _Compare,
                                      _Alloc> {
private:
    using base_t =
        RBTree_generic_container<_Key, void, multi, parent, _Compare, _Alloc>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    RBTree_generic_set() = default;
    template <typename... Args>
    // NOLINTNEXTLINE
    RBTree_generic_set(Args&&... args) : base_t(std::forward<Args>(args)...) {}
};

template <typename _Key, typename _Value, bool multi,
          typename _Compare = default_compare_t<_Key>,
          typename _Alloc = default_allocator_t<_Key, _Value>,
          bool parent = false>
class RBTree_generic_map
    : public RBTree_generic_container<_Key, _Value, multi, parent, _Compare,
                                      _Alloc> {
private:
    using base_t =
        RBTree_generic_container<_Key, _Value, multi, parent, _Compare, _Alloc>;

public:
    using iterator = typename base_t::iterator;
    using const_iterator = typename base_t::const_iterator;
    using reverse_iterator = typename base_t::reverse_iterator;
    using reverse_const_iterator = typename base_t::reverse_const_iterator;

    RBTree_generic_map() = default;
    template <typename... Args>
    // NOLINTNEXTLINE
    RBTree_generic_map(Args&&... args) : base_t(std::forward<Args>(args)...) {}
};
};  // namespace RBTree_container_impl

template <typename _Key, typename _Value,
          typename _Compare = RBTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              RBTree_container_impl::default_allocator_t<_Key, _Value>>
using rbtmap = RBTree_container_impl::RBTree_generic_map<_Key, _Value, false,
                                                         _Compare, _Alloc>;

template <typename _Key, typename _Value,
          typename _Compare = RBTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              RBTree_container_impl::default_allocator_t<_Key, _Value>>
using rbtmultimap =
    RBTree_container_impl::RBTree_generic_map<_Key, _Value, true, _Compare,
                                              _Alloc>;

template <typename _Key,
          typename _Compare = RBTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              RBTree_container_impl::default_allocator_t<_Key, void>>
using rbtset =
    RBTree_container_impl::RBTree_generic_set<_Key, false, _Compare, _Alloc>;

template <typename _Key,
          typename _Compare = RBTree_container_impl::default_compare_t<_Key>,
          typename _Alloc =
              RBTree_container_impl::default_allocator_t<_Key, void>>
using rbtmultiset =
    RBTree_container_impl::RBTree_generic_set<_Key, true, _Compare, _Alloc>;
};  // namespace ldc
