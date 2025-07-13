#pragma once
#include <assert.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "./ldc_utils.h"

namespace ldc {
template <typename _Container, typename _IterBase, typename _Key,
          typename _Value, typename KVPair>
struct ContainerWrapper {
public:
    static constexpr auto ref_accessor = _Container::ref_accessor;

    inline explicit ContainerWrapper(_Container&& container)
        : m_container(std::move(container)) {}

    inline _IterBase k_insert(KVPair val) {
        return m_container.insert(std::move(val));
    }

    inline _IterBase k_find(const _Key& key) { return m_container.find(key); }
    inline _IterBase k_lower_bound(const _Key& key) {
        return m_container.lower_bound(key);
    }
    inline _IterBase k_upper_bound(const _Key& key) {
        return m_container.upper_bound(key);
    }
    inline bool k_exists(const _IterBase& iter) const {
        return m_container.exists(iter);
    }

    inline _IterBase k_begin() { return m_container.begin(); }
    inline _IterBase k_end() { return m_container.end(); }

    inline void k_forward(_IterBase& path) { m_container.forward(path); }
    inline void k_backward(_IterBase& path) { m_container.backward(path); }
    inline KVPair k_deleteIter(_IterBase iter) {
        return m_container.deleteIter(iter);
    }
    inline KVPair k_getHolder(_IterBase iter) const {
        return const_cast<_Container&>(m_container).getHolder(iter);
    }
    inline KVPair& k_getHolderRef(_IterBase iter) {
        return m_container.getHolderRef(iter);
    }
    inline _Key k_getHolderKey(_IterBase iter) const {
        return nonconst().m_container.getHolderKey(iter);
    }

    inline int k_compareHolderPath(const _IterBase& it1,
                                   const _IterBase& it2) const {
        if constexpr (LDC_CLASS_MEMBER_TEST_VALUE(_Container,
                                                  compareHolderPath)) {
            return m_container.compareHolderPath(it1, it2);
        } else if constexpr (LDC_CLASS_MEMBER_TEST_VALUE(_Container,
                                                         keyCompareLess)) {
            if (!this->k_exists(it1)) {
                if (this->k_exists(it2)) {
                    return 1;
                }
                return 0;
            } else if (!this->k_exists(it2)) {
                return -1;
            }

            if (it1 == it2) return 0;

            const auto k1 = this->k_getHolderKey(it1);
            const auto k2 = this->k_getHolderKey(it2);
            if (m_container.keyCompareLess(k1, k2)) {
                return -1;
            } else if (m_container.keyCompareLess(k2, k1)) {
                return 1;
            }

            auto m1 = it1, m2 = it2;
            for (; this->k_exists(it1) && this->k_exists(it2);
                 nonconst().k_forward(m1), nonconst().k_forward(m2)) {
                if (m1 == it2) {
                    return -1;
                } else if (m2 == it1) {
                    return 1;
                }
            }
            return this->k_exists(m1) ? -1 : 1;
        } else {
            static_assert(
                LDC_CLASS_MEMBER_TEST_VALUE(_Container, compareHolderPath) ||
                    LDC_CLASS_MEMBER_TEST_VALUE(_Container, keyCompareLess),
                "unable to implement compareHolderPath");
            return 0;
        }
    }

    template <typename T>
    inline void k_setHolderValue(_IterBase iter, T val) {
        return m_container.setHolderValue(iter, val);
    }

    inline size_t k_size() const { return m_container.size(); }
    inline void k_clear() { m_container.clear(); }

private:
    _Container m_container;
    inline ContainerWrapper& nonconst() const {
        return const_cast<ContainerWrapper&>(*this);
    }
    LDC_CLASS_MEMBER_TEST(const, compareHolderPath,
                          int(const _IterBase&, const _IterBase&));
    LDC_CLASS_MEMBER_TEST(const, keyCompareLess,
                          bool(const _Key&, const _Key&));
};

template <bool reverse, bool const_iterator, typename _Container,
          typename _IterBase, typename _Key, typename _Value, typename KVPair,
          typename IterCategory>
class GnContainerIterator {
public:
    using iterator_category = IterCategory;
    using value_type = KVPair;
    using difference_type = long;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference =
        typename std::conditional<const_iterator, const value_type&,
                                  value_type&>::type;
    using const_reference = const value_type&;

    using ContainerWrapper_t =
        ContainerWrapper<_Container, _IterBase, _Key, _Value, KVPair>;
    using const_t = std::conditional<
        const_iterator, int,
        GnContainerIterator<reverse, true, _Container, _IterBase, _Key, _Value,
                            KVPair, IterCategory>>;

protected:
    ContainerWrapper_t* m_container;
    _IterBase m_iter;
    ContainerWrapper_t& container() { return *m_container; }
    const ContainerWrapper_t& container() const { return *m_container; }

public:
    GnContainerIterator(_IterBase iter, ContainerWrapper_t& cnt)
        : m_container(&cnt), m_iter(iter) {}

    explicit operator const_t() const {
        if constexpr (const_iterator) {
            return 0;
        } else {
            return const_t(m_iter, *this->m_container);
        }
    }

    const _IterBase& iter() const { return this->m_iter; }

    explicit operator bool() const { return container().k_exists(m_iter); }

    GnContainerIterator& operator++() {
        if constexpr (reverse) {
            if (m_iter == container().k_begin()) {
                m_iter = container().k_end();
            } else {
                container().k_backward(m_iter);
            }
        } else {
            assert(container().k_exists(m_iter));
            container().k_forward(m_iter);
        }
        return *this;
    }

    GnContainerIterator operator++(int) {
        auto ans = *this;
        this->operator++();
        return ans;
    }

    GnContainerIterator& operator--() {
        if constexpr (reverse) {
            if (container().k_exists(m_iter)) {
                container().k_forward(m_iter);
            } else {
                m_iter = container().k_begin();
            }
        } else {
            container().k_backward(m_iter);
        }
        return *this;
    }

    GnContainerIterator operator--(int) {
        auto ans = *this;
        this->operator--();
        return ans;
    }

    bool operator==(const GnContainerIterator& oth) const {
        return container().k_compareHolderPath(m_iter, oth.m_iter) == 0;
    }

    bool operator!=(const GnContainerIterator& oth) const {
        return !this->operator==(oth);
    }

    bool operator<(const GnContainerIterator& oth) const {
        return container().k_compareHolderPath(m_iter, oth.m_iter) < 0;
    }

    bool operator>(const GnContainerIterator& oth) const {
        return container().k_compareHolderPath(m_iter, oth.m_iter) > 0;
    }

    inline bool operator<=(const GnContainerIterator& oth) const {
        return oth.operator>(*this);
    }

    inline bool operator>=(const GnContainerIterator& oth) const {
        return oth.operator<(*this);
    }

    value_type get() const { return container().k_getHolder(m_iter); }

    void set(const std::conditional_t<std::is_same_v<_Value, void>,
                                      GnContainerIterator, _Value>& val) const {
        return container().k_setHolderValue(m_iter, val);
    }
};

template <bool reverse, bool const_iterator, typename _Container,
          typename _IterBase, typename _Key, typename _Value, typename KVPair,
          typename IterCategory>
class GnContainerIteratorMem
    : public GnContainerIterator<reverse, const_iterator, _Container, _IterBase,
                                 _Key, _Value, KVPair, IterCategory> {
private:
    using const_t = std::conditional_t<
        const_iterator, int,
        GnContainerIteratorMem<reverse, true, _Container, _IterBase, _Key,
                               _Value, KVPair, IterCategory>>;

public:
    using base_t =
        GnContainerIterator<reverse, const_iterator, _Container, _IterBase,
                            _Key, _Value, KVPair, IterCategory>;
    using ContainerWrapper_t = typename base_t::ContainerWrapper_t;

    using iterator_category = typename base_t::iterator_category;
    using value_type = typename base_t::value_type;
    using difference_type = typename base_t::difference_type;
    using pointer = typename base_t::pointer;
    using const_pointer = typename base_t::const_pointer;
    using reference = typename base_t::reference;
    using const_reference = typename base_t::const_reference;

    inline GnContainerIteratorMem(_IterBase iter, ContainerWrapper_t& cnt)
        : base_t(iter, cnt) {}

    explicit operator const_t() const {
        if constexpr (const_iterator) {
            return 0;
        } else {
            return const_t(this->m_iter, *this->m_container);
        }
    }

    using _P = std::conditional_t<const_iterator, const_pointer, pointer>;
    using _R = std::conditional_t<const_iterator, const_reference, reference>;
    _P operator->() const {
        return &const_cast<GnContainerIteratorMem*>(this)
                    ->container()
                    .k_getHolderRef(this->m_iter);
    }
    _R operator*() const {
        return const_cast<GnContainerIteratorMem*>(this)
            ->container()
            .k_getHolderRef(this->m_iter);
    }

    GnContainerIteratorMem& operator++() {
        base_t::operator++();
        return *this;
    }

    GnContainerIteratorMem operator++(int) {
        auto ans = *this;
        this->operator++();
        return ans;
    }

    GnContainerIteratorMem& operator--() {
        base_t::operator--();
        return *this;
    }

    GnContainerIteratorMem operator--(int) {
        auto ans = *this;
        this->operator++();
        return ans;
    }
};

template <bool ref_accessor, bool reverse, bool const_iterator,
          typename _Container, typename _IterBase, typename _Key,
          typename _Value, typename KVPair, typename IterCategory>
using GnContainerIteratorEx = std::conditional_t<
    ref_accessor,
    GnContainerIteratorMem<reverse, const_iterator, _Container, _IterBase, _Key,
                           _Value, KVPair, IterCategory>,
    GnContainerIterator<reverse, const_iterator, _Container, _IterBase, _Key,
                        _Value, KVPair, IterCategory>>;

template <typename _Container, typename _IterBase, typename _Key,
          typename _Value, typename KVPair, typename IterCategory>
class GnContainer {
private:
    using ContainerWrapper_t =
        ContainerWrapper<_Container, _IterBase, _Key, _Value, KVPair>;
    ContainerWrapper_t m_container;
    static constexpr auto ref_accessor = ContainerWrapper_t::ref_accessor;
    inline auto& nonconst() const {
        return const_cast<GnContainer*>(this)->m_container;
    }

    struct DUMMY {};
    using XValueType =
        std::conditional_t<!std::is_same_v<_Value, void> && ref_accessor,
                           _Value, DUMMY>;

public:
    using iterator =
        GnContainerIteratorEx<ref_accessor, false, false, _Container, _IterBase,
                              _Key, _Value, KVPair, IterCategory>;
    using const_iterator =
        GnContainerIteratorEx<ref_accessor, false, true, _Container, _IterBase,
                              _Key, _Value, KVPair, IterCategory>;
    using reverse_iterator =
        GnContainerIteratorEx<ref_accessor, true, false, _Container, _IterBase,
                              _Key, _Value, KVPair, IterCategory>;
    using reverse_const_iterator =
        GnContainerIteratorEx<ref_accessor, true, true, _Container, _IterBase,
                              _Key, _Value, KVPair, IterCategory>;

    explicit GnContainer(_Container&& container)
        : m_container(std::move(container)) {}

    bool operator==(const GnContainer& oth) const {
        if (this->size() != oth.size()) return false;
        for (auto b1 = this->begin(), b2 = oth.begin(); b1 != this->end();
             b1++, b2++) {
            if (b1.get() != b2.get()) return false;
        }
        return true;
    }

    bool operator!=(const GnContainer& oth) const {
        return !this->operator==(oth);
    }

    template <typename Key,
              std::enable_if_t<std::is_same_v<Key, _Key> && ref_accessor,
                               bool> = true>
    XValueType& at(const Key& key) {
        if constexpr (std::is_same_v<XValueType, DUMMY>) {
            return std::declval<DUMMY>();
        } else {
            const auto it = this->find(key);
            if (it == end()) {
                throw std::out_of_range("key not found");
            }
            return it->second;
        }
    }

    template <typename Key,
              std::enable_if_t<std::is_same_v<Key, _Key> && ref_accessor,
                               bool> = true>
    XValueType& operator[](const Key& key) {
        if constexpr (std::is_same_v<XValueType, DUMMY>) {
            return std::declval<DUMMY>();
        } else {
            const auto it = this->find(key);
            if (it == end()) {
                if constexpr (std::is_integral_v<_Value>) {
                    const auto [it, _] =
                        this->insert(std::make_pair(key, _Value(0)));
                    return it->second;
                } else {
                    const auto [it, _] =
                        this->insert(std::make_pair(key, _Value()));
                    return it->second;
                }
            } else {
                return it->second;
            }
        }
    }

    template <typename Key,
              std::enable_if_t<std::is_same_v<Key, _Key>, bool> = true>
    const XValueType& at(const Key& key) const {
        if constexpr (std::is_same_v<XValueType, DUMMY>) {
            return std::declval<DUMMY>();
        } else {
            const auto it = this->find(key);
            if (it == end()) {
                throw std::out_of_range("key not found");
            }
            return it->second;
        }
    }

    inline iterator begin() {
        return iterator(m_container.k_begin(), nonconst());
    }
    inline iterator end() { return iterator(m_container.k_end(), nonconst()); }

    inline const_iterator begin() const {
        return const_iterator(nonconst().k_begin(), nonconst());
    }
    inline const_iterator end() const {
        return const_iterator(nonconst().k_end(), nonconst());
    }

    inline const_iterator cbegin() const {
        return const_iterator(nonconst().k_begin(), nonconst());
    }
    inline const_iterator cend() const {
        return const_iterator(nonconst().k_end(), nonconst());
    }

    inline reverse_iterator rbegin() {
        auto u = m_container.k_end();
        if (this->empty()) {
            return reverse_iterator(nonconst(), u);
        } else {
            m_container.k_backward(u);
            return reverse_iterator(nonconst(), u);
        }
    }
    inline reverse_iterator rend() {
        return reverse_iterator(m_container.k_end(), nonconst());
    }

    inline reverse_const_iterator rbegin() const {
        return reverse_const_iterator(const_cast<GnContainer*>(this)->rbegin());
    }
    inline reverse_const_iterator rend() const {
        return reverse_const_iterator(const_cast<GnContainer*>(this)->rend());
    }

    inline reverse_const_iterator crbegin() const {
        return reverse_const_iterator(const_cast<GnContainer*>(this)->rbegin());
    }
    inline reverse_const_iterator crend() const {
        return reverse_const_iterator(const_cast<GnContainer*>(this)->rend());
    }

    template <typename _K>
    iterator lower_bound(const _K& key) {
        auto lb = m_container.k_lower_bound(key);
        ;
        return iterator(lb, nonconst());
    }

    template <typename _K>
    const_iterator lower_bound(const _K& key) const {
        auto lb = nonconst().k_lower_bound(key);
        ;
        return const_iterator(lb, nonconst());
    }

    template <typename _K>
    iterator upper_bound(const _K& key) {
        auto ub = m_container.k_upper_bound(key);
        return iterator(ub, nonconst());
    }

    template <typename _K>
    const_iterator upper_bound(const _K& key) const {
        auto ub = nonconst().k_upper_bound(key);
        return const_iterator(ub, nonconst());
    }

    template <typename _K>
    iterator find(const _K& key) {
        auto node = m_container.k_find(key);
        return iterator(node, nonconst());
    }

    template <typename _K>
    const_iterator find(const _K& key) const {
        auto node = nonconst().k_find(key);
        return const_iterator(node, nonconst());
    }

    template <typename _K>
    std::pair<iterator, iterator> equal_range(const _K& key) {
        return std::make_pair(this->lower_bound(key), this->upper_bound(key));
    }

    template <typename _K>
    std::pair<const_iterator, const_iterator> equal_range(const _K& key) const {
        return std::make_pair(this->lower_bound(key), this->upper_bound(key));
    }

    template <typename _K>
    inline size_t count(const _K& key) const {
        auto its = this->equal_range(key);
        return std::distance(its.first, its.second);
    }

    template <typename _K>
    inline bool contains(const _K& key) const {
        return this->find(key) != this->end();
    }

    inline size_t size() const { return m_container.k_size(); }
    inline bool empty() const { return this->size() == 0; }
    inline size_t max_size() const noexcept {
        return std::numeric_limits<size_t>::max();
    }

    std::pair<iterator, bool> insert(const KVPair& val) {
        auto iter = m_container.k_insert(val);
        return std::make_pair(iterator(iter, nonconst()),
                              m_container.k_exists(iter));
    }

    template <
        typename ValType,
        typename std::enable_if<std::is_constructible<KVPair, ValType&&>::value,
                                bool>::type = true>
    std::pair<iterator, bool> insert(ValType&& val) {
        auto iter = m_container.k_insert(std::move(val));
        return std::make_pair(iterator(iter, nonconst()),
                              m_container.k_exists(iter));
    }

    template <
        typename ValType,
        typename std::enable_if<std::is_constructible<KVPair, ValType>::value,
                                bool>::type = true>
    inline iterator insert(iterator hint, ValType&& val) {
        return this->insert(std::forward<ValType>(val)).first;
    }

    template <
        typename ValType,
        typename std::enable_if<std::is_constructible<KVPair, ValType>::value,
                                bool>::type = true>
    inline iterator insert(const_iterator hint, ValType&& val) {
        return this->insert(std::forward<ValType>(val)).first;
    }

    template <
        typename InputIt,
        typename std::enable_if<
            std::is_constructible<KVPair, typename std::iterator_traits<
                                              InputIt>::value_type>::value &&
                std::is_convertible<
                    typename std::iterator_traits<InputIt>::iterator_category,
                    std::forward_iterator_tag>::value,
            bool>::type = true>
    inline void insert(InputIt first, InputIt last) {
        for (; first != last; first++) this->insert(*first);
    }

    template <typename T,
              typename std::enable_if<std::is_constructible<KVPair, T>::value,
                                      bool>::type = true>
    inline void insert(std::initializer_list<T> list) {
        this->insert(list.begin(), list.end());
    }

    template <
        typename... Args,
        typename std::enable_if<std::is_constructible<KVPair, Args&&...>::value,
                                bool>::type = true>
    inline std::pair<iterator, bool> emplace(Args&&... args) {
        return this->insert(KVPair(std::forward<Args>(args)...));
    }

    template <
        typename... Args,
        typename std::enable_if<std::is_constructible<KVPair, Args&&...>::value,
                                bool>::type = true>
    iterator emplace_hint(const_iterator, Args&&... args) {
        return this->emplace(std::forward<Args>(args)...);
    }

    iterator erase(iterator pos) { return this->erase(const_iterator(pos)); }

    iterator erase(const_iterator pos) {
        const auto key = m_container.k_getHolderKey(pos.iter());
        size_t nth = 0;
        auto s = const_iterator(this->lower_bound(key));
        for (; s != pos; s++, nth++);
        m_container.k_deleteIter(pos.iter());
        auto s2 = this->lower_bound(key);
        for (; nth > 0; nth--, s2++);
        return s2;
    }

    iterator erase(iterator first, iterator last) {
        return this->erase(const_iterator(first), const_iterator(last));
    }

    iterator erase(const_iterator first, const_iterator last) {
        if (first > last) {
            throw std::logic_error("invalid range");
        }

        iterator ans = this->end();
        for (; first != this->cend() && first != last;) {
            ans = this->erase(first);
            first = const_iterator(ans);
        }
        return ans;
    }

    size_t erase(const _Key& key) {
        auto first = this->lower_bound(key);
        auto last = this->upper_bound(key);
        auto ans = std::distance(first, last);
        this->erase(first, last);
        return ans;
    }

    void clear() { m_container.k_clear(); }
};
};  // namespace ldc
