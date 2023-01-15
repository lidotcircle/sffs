#pragma once
#include "./maxsize_vector.h"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>
#include <queue>
#include <type_traits>


namespace ldc::BTreeAlgorithmImpl {
template <typename T, typename NODE, typename HOLDER, typename KEY, bool complain=false>
struct treeop_traits {
    static NODE& node;
    static const NODE& const_node;
    static const KEY& const_key;
    static HOLDER& holder;
    static const HOLDER& const_holder;
    const size_t index;

    template<typename U>
    static uint8_t  test_getNthChild(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getNthChild(node,index))>,bool> = true>
    static uint16_t test_getNthChild(int);

    template<typename U>
    static uint8_t  test_setNthChild(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setNthChild(node, index, node))>,bool> = true>
    static uint16_t test_setNthChild(int);

    template<typename U>
    static uint8_t  test_clearNthChild(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->clearNthChild(node, index))>,bool> = true>
    static uint16_t test_clearNthChild(int);

    template<typename U>
    static uint8_t  test_getParent(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getParent(node))>,bool> = true>
    static uint16_t test_getParent(int);

    template<typename U>
    static uint8_t  test_setParent(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setParent(node, node))>,bool> = true>
    static uint16_t test_setParent(int);

    template<typename U>
    static uint8_t  test_getNthHolder(...);
    template<typename U,std::enable_if_t<std::is_same_v<HOLDER,decltype(static_cast<const U*>(nullptr)->getNthHolder(node,index))>,bool> = true>
    static uint16_t test_getNthHolder(int);

    template<typename U>
    static uint8_t  test_getNthKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<KEY,decltype(static_cast<const U*>(nullptr)->getNthKey(node,index))>,bool> = true>
    static uint16_t test_getNthKey(int);

    template<typename U>
    static uint8_t  test_extractNthHolder(...);
    template<typename U,std::enable_if_t<std::is_same_v<HOLDER,decltype(static_cast<U*>(nullptr)->extractNthHolder(node,index))>,bool> = true>
    static uint16_t test_extractNthHolder(int);

    template<typename U>
    static uint8_t  test_setNthHolder(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setNthHolder(node, index, std::move(holder)))>,bool> = true>
    static uint16_t test_setNthHolder(int);

    template<typename U>
    static uint8_t  test_getOrder(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(static_cast<const U*>(nullptr)->getOrder())>,bool> = true>
    static uint16_t test_getOrder(int);

    template<typename U>
    static uint8_t  test_getNumberOfChildren(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(static_cast<const U*>(nullptr)->getNumberOfChildren(node))>,bool> = true>
    static uint16_t test_getNumberOfChildren(int);

    template<typename U>
    static uint8_t  test_getNumberOfKeys(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(static_cast<const U*>(nullptr)->getNumberOfKeys(node))>,bool> = true>
    static uint16_t test_getNumberOfKeys(int);

    template<typename U>
    static uint8_t  test_isNullNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(static_cast<const U*>(nullptr)->isNullNode(node))>,bool> = true>
    static uint16_t test_isNullNode(int);

    template<typename U>
    static uint8_t  test_getNullNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getNullNode())>,bool> = true>
    static uint16_t test_getNullNode(int);

    template<typename U>
    static uint8_t  test_createEmptyNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<U*>(nullptr)->createEmptyNode())>,bool> = true>
    static uint16_t test_createEmptyNode(int);

    template<typename U>
    static uint8_t  test_releaseEmptyNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->releaseEmptyNode(std::move(node)))>,bool> = true>
    static uint16_t test_releaseEmptyNode(int);

    template<typename U>
    static uint8_t  test_getKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<KEY,decltype(static_cast<const U*>(nullptr)->getKey(const_holder))>,bool> = true>
    static uint16_t test_getKey(int);

    template<typename U>
    static uint8_t  test_keyCompareLess(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(static_cast<const U*>(nullptr)->keyCompareLess(const_key,const_key))>,bool> = true>
    static uint16_t test_keyCompareLess(int);

    template<typename U>
    static uint8_t  test_keyCompareEqual(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(static_cast<const U*>(nullptr)->keyCompareEqual(const_key,const_key))>,bool> = true>
    static uint16_t test_keyCompareEqual(int);

    template<typename U>
    static uint8_t  test_nodeCompareEqual(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(static_cast<const U*>(nullptr)->nodeCompareEqual(node,node))>,bool> = true>
    static uint16_t test_nodeCompareEqual(int);

    static constexpr bool has_getNthChild         = sizeof(test_getNthChild<T>(1))         == sizeof(uint16_t);
    static constexpr bool has_setNthChild         = sizeof(test_setNthChild<T>(1))         == sizeof(uint16_t);
    static constexpr bool has_clearNthChild       = sizeof(test_clearNthChild<T>(1))       == sizeof(uint16_t);
    static constexpr bool has_getParent           = sizeof(test_getParent<T>(1))           == sizeof(uint16_t); // optional
    static constexpr bool has_setParent           = sizeof(test_setParent<T>(1))           == sizeof(uint16_t); // optional
    static constexpr bool has_getNthHolder        = sizeof(test_getNthHolder<T>(1))        == sizeof(uint16_t);
    static constexpr bool has_getNthKey           = sizeof(test_getNthKey<T>(1))           == sizeof(uint16_t);
    static constexpr bool has_extractNthHolder    = sizeof(test_extractNthHolder<T>(1))    == sizeof(uint16_t);
    static constexpr bool has_setNthHolder        = sizeof(test_setNthHolder<T>(1))        == sizeof(uint16_t);
    static constexpr bool has_getOrder            = sizeof(test_getOrder<T>(1))            == sizeof(uint16_t);
    static constexpr bool has_getNumberOfChildren = sizeof(test_getNumberOfChildren<T>(1)) == sizeof(uint16_t);
    static constexpr bool has_getNumberOfKeys     = sizeof(test_getNumberOfKeys<T>(1))     == sizeof(uint16_t);
    static constexpr bool has_isNullNode          = sizeof(test_isNullNode<T>(1))          == sizeof(uint16_t);
    static constexpr bool has_getNullNode         = sizeof(test_getNullNode<T>(1))         == sizeof(uint16_t);
    static constexpr bool has_createEmptyNode     = sizeof(test_createEmptyNode<T>(1))     == sizeof(uint16_t);
    static constexpr bool has_releaseEmptyNode    = sizeof(test_releaseEmptyNode<T>(1))    == sizeof(uint16_t);
    static constexpr bool has_getKey              = sizeof(test_getKey<T>(1))              == sizeof(uint16_t);
    static constexpr bool has_keyCompareLess      = sizeof(test_keyCompareLess<T>(1))      == sizeof(uint16_t);
    static constexpr bool has_keyCompareEqual     = sizeof(test_keyCompareEqual<T>(1))     == sizeof(uint16_t);
    static constexpr bool has_nodeCompareEqual    = sizeof(test_nodeCompareEqual<T>(1))    == sizeof(uint16_t);

    static_assert(!complain || !std::is_reference_v<NODE>,            "NODE should not be a reference");
    static_assert(!complain || !std::is_const_v<NODE>,                "NODE should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<NODE>,      "NODE should be copy assignable");
    static_assert(!complain || !std::is_reference_v<HOLDER>,          "HOLDER should not be a reference");
    static_assert(!complain || !std::is_const_v<HOLDER>,              "HOLDER should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<HOLDER>,    "HOLDER should be copy assignable");
    static_assert(!complain ||  std::is_move_constructible_v<HOLDER>, "HOLDER should be copy assignable");
    static_assert(!complain || !std::is_reference_v<KEY>,             "KEY should not be a reference");
    static_assert(!complain || !std::is_const_v<KEY>,                 "KEY should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<KEY>,       "KEY should be copy assignable");
    static_assert(!complain ||  has_getNthChild,                      "should implement 'NODE getNthChild(NODE, size_t) const;'");
    static_assert(!complain ||  has_setNthChild,                      "should implement 'void setNthChild(NODE, size_t, NODE);'");
    static_assert(!complain ||  has_getNthHolder,                     "should implement 'HOLDER getNthHolder(NODE, size_t) const;'");
    static_assert(!complain ||  has_extractNthHolder,                 "should implement 'HOLDER extractNthHolder(NODE, size_t);'");
    static_assert(!complain ||  has_setNthHolder,                     "should implement 'void setNthHolder(NODE, size_t, HOLDER&& holder);'");
    static_assert(!complain ||  has_getOrder,                         "should implement 'size_t getOrder() const;'");
    static_assert(!complain ||  has_getNumberOfChildren,              "should implement 'size_t getNumberOfChildren(NODE);'");
    static_assert(!complain ||  has_getNumberOfKeys,                  "should implement 'size_t getNumberOfKeys(NODE);'");
    static_assert(!complain ||  has_isNullNode,                       "should implement 'bool isNullNode(NODE) const;'");
    static_assert(!complain ||  has_getNullNode,                      "should implement 'NODE getNullNode() const;'");
    static_assert(!complain ||  has_createEmptyNode,                  "should implement 'NODE createEmptyNode();'");
    static_assert(!complain ||  has_releaseEmptyNode,                 "should implement 'void releaseEmptyNode(NODE);'");
    static_assert(!complain ||  has_getKey,                           "should implement 'KEY getKey(const HOLDER&) const;'");
    static_assert(!complain ||  has_keyCompareLess,                   "should implement 'bool keyCompareLess(const KEY&, const KEY&) const;'");
    static_assert(!complain ||  has_nodeCompareEqual,                 "should implement 'bool nodeCompareEqual(NODE, NODE) const;'");

    static constexpr bool value = !std::is_reference_v<NODE> && !std::is_const_v<NODE> && std::is_copy_assignable_v<NODE> &&
                                  !std::is_reference_v<HOLDER>  && !std::is_const_v<HOLDER> && std::is_copy_assignable_v<HOLDER> &&
                                   std::is_move_constructible_v<HOLDER> &&
                                  !std::is_reference_v<KEY>  && !std::is_const_v<KEY> && std::is_copy_assignable_v<KEY> &&
                                  has_getNthChild && has_setNthChild  &&
                                  has_getNthHolder && has_extractNthHolder && has_setNthHolder  &&
                                  has_getOrder && has_getNumberOfChildren && has_getNumberOfKeys &&
                                  has_isNullNode && has_getNullNode && has_createEmptyNode &&
                                  has_getKey && has_keyCompareLess &&
                                  has_nodeCompareEqual;
};

template<typename T, typename NODE, typename HOLDER, typename KEY,
         std::enable_if_t<treeop_traits<T,NODE,HOLDER,KEY>::value,bool> = true>
struct BTreeOpWrapper {
    using traits = treeop_traits<T,NODE,HOLDER,KEY>;

    inline explicit BTreeOpWrapper(T treeop): m_ops (treeop) {}

    inline NODE   getNthChild (NODE node, size_t nth) const { return m_ops.getNthChild(node, nth); }
    inline HOLDER getNthHolder(NODE node, size_t nth) const { return m_ops.getNthHolder(node, nth); }
    inline KEY    getNthKey(NODE node, size_t nth) const {
        if constexpr (traits::has_getNthKey) {
            return m_ops.getNthKey(node, nth);
        } else {
            return this->getKey(m_ops.getNthHolder(node, nth));
        }
    }

    inline NODE getFirstChild(NODE node) const { return m_ops.getNthChild(node, 0); }
    inline NODE getLastChild (NODE node) const { return m_ops.getNthChild(node, m_ops.getNumberOfChildren(node)-1); }

    inline HOLDER getFirstHolder(NODE node) const { return m_ops.getNthHolder(node, 0); }
    inline HOLDER getLastHolder (NODE node) const { return m_ops.getNthHolder(node, m_ops.getNumberOfKeys(node)-1); }

    inline KEY getFirstKey(NODE node) const { return m_ops.getNthKey(node, 0); }
    inline KEY getLastKey (NODE node) const { return m_ops.getNthKey(node, m_ops.getNumberOfKeys(node)-1); }

    inline HOLDER extractNthHolder(NODE node, size_t nth) { return m_ops.extractNthHolder(node, nth); }

    inline void setNthChild (NODE node, size_t nth, NODE n)  { m_ops.setNthChild(node, nth, n); }
    inline void clearNthChild (NODE node, size_t nth)  {
        if constexpr (traits::has_clearNthChild) {
            m_ops.clearNthChild(node, nth);
        } else {
            m_ops.setNthChild(node, nth, m_ops.getNullNode());
        }
    }
    inline void setNthHolder(NODE node, size_t nth, HOLDER&& holder) { m_ops.setNthHolder(node, nth, std::move(holder)); }

    inline NODE getParent(NODE node) const  {
        if constexpr (traits::has_getParent) {
            return m_ops.getParent(node);
        }
        return node;
    }
    inline void setParent(NODE node, NODE n)  {
        if constexpr (traits::has_setParent) {
            return m_ops.setParent(node, n);
        }
    }

    inline size_t getOrder() const { return m_ops.getOrder(); }
    inline size_t getNumberOfChildren(NODE node) const { return m_ops.getNumberOfChildren(node); }
    inline size_t getNumberOfKeys(NODE node) const { return m_ops.getNumberOfKeys(node); }

    inline bool isLeaf(NODE node) const { return this->getNumberOfChildren(node) == 0; }
    inline bool isFull(NODE node) const { return this->getNumberOfKeys(node) == 2 * this->getOrder() - 1; }

    inline bool isNullNode(NODE node) const { return m_ops.isNullNode(node); }
    inline NODE getNullNode() const { return m_ops.getNullNode(); }
    inline NODE createEmptyNode() { return m_ops.createEmptyNode(); }
    inline bool isEmptyNode(NODE node) const { return !m_ops.isNullNode(node) && m_ops.getNumberOfChildren(node) == 0 && m_ops.getNumberOfKeys(node) == 0; }
    inline void releaseEmptyNode(NODE&& node) { return m_ops.releaseEmptyNode(std::move(node)); }

    inline KEY getKey(const HOLDER& n) const { return m_ops.getKey(n); }

    inline bool keyCompareLess(const KEY& lhs, const KEY& rhs) const { return m_ops.keyCompareLess(lhs, rhs); }

    inline bool keyCompareEqual(const KEY& lhs, const KEY& rhs) const {
        if constexpr (traits::has_keyCompareEqual) {
            return m_ops.keyCompareEqual(lhs, rhs);
        } else {
            return !this->keyCompareLess(lhs, rhs) && !this->keyCompareLess(rhs, lhs);
        }
    }

    inline bool nodeCompareEqual(NODE lhs, NODE rhs) const {
        if constexpr (traits::has_nodeCompareEqual) {
            return m_ops.nodeCompareEqual(lhs, rhs);
        } else {
            return lhs == rhs;
        }
    }

    inline size_t lower_bound(NODE node, const KEY& key) {
        size_t lower = 0;
        size_t upper = m_ops.getNumberOfKeys(node);

        while (lower < upper) {
            auto n = (upper + lower) / 2;
            if (!m_ops.keyCompareLess(m_ops.getNthKey(node, n), key)) {
                upper = n;
            } else {
                lower = n + 1;
            }
        }

        return upper;
    }

    inline size_t upper_bound(NODE node, const KEY& key) {
        size_t lower = 0;
        size_t upper = m_ops.getNumberOfKeys(node);

        while (lower < upper) {
            auto n = (upper + lower) / 2;
            if (m_ops.keyCompareLess(key, m_ops.getNthKey(node, n))) {
                upper = n;
            } else {
                lower = n + 1;
            }
        }

        return upper;
    }

private:
    T m_ops;
};

template<typename T, typename NODE, typename HOLDER, typename KEY, bool enableParent=true,
         bool multikey = false, size_t static_vector_size = 32,
         std::enable_if_t<treeop_traits<T,NODE,HOLDER,KEY,true>::value,bool> = true>
class BTreeAlgorithm {
public:
    using traits = treeop_traits<T,NODE,HOLDER,KEY>;
    static constexpr bool parents_ops = traits::has_getParent && traits::has_getParent && enableParent;
    using NodePath = std::conditional_t<parents_ops, NODE,
                         std::conditional_t<static_cast<bool>(static_vector_size > 0), maxsize_vector<std::pair<NODE,size_t>,static_vector_size>,std::vector<std::pair<NODE,size_t>>>>;

    class HolderPath {
        friend BTreeAlgorithm;
        NodePath m_path;
        size_t   m_index;

        HolderPath(NodePath&& path, size_t idx): m_path(std::move(path)), m_index(idx) { }
    };

private:
    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline NodePath InitPath() const {
        return m_ops.getNullNode();
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline NodePath InitPath() const {
        return NodePath();
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline void NodePathPush(NodePath& path, NODE n, size_t nth) const {
        path = n;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline void NodePathPush(NodePath& path, NODE n, size_t nth) const {
        path.push_back(std::make_pair(n, nth));
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline void NodePathPop(NodePath& path) const {
        path = m_ops.getParent(path);;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline void NodePathPop(NodePath& path) const {
        path.pop_back();
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline NODE GetNodeAncestor(N node, size_t n) const {
        for (size_t i=0;i<n;i++) {
            assert(!m_ops.isNullNode(node));
            node = m_ops.getParent(node);
        }
        return node;
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline size_t GetPathDepth(N node) const {
        size_t i = 0;
        for (;!m_ops.isNullNode(node);i++) {
            node = m_ops.getParent(node);
        }
        return i;
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline size_t GetNodeIndex(N node, size_t n) const {
        auto p = this->GetNodeAncestor(node, n+1);
        if (m_ops.isNullNode(p)) return 0;
        auto c = this->GetNodeAncestor(node, n);
        const auto s = m_ops.getNumberOfChildren(p);
        for (size_t i=0;i<s;i++) {
            if (m_ops.nodeCompareEqual(c, m_ops.getNthChild(p, i))) {
                return i;
            }
        }
        assert(false && "NOT FOUND");
        return 0;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline NODE GetNodeAncestor(N np, size_t n) const {
        if (n == np.size()) return m_ops.getNullNode();
        assert(n < np.size());
        return np[np.size() - n - 1].first;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline size_t GetPathDepth(N np) const {
        return np.size();
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline size_t GetNodeIndex(N np, size_t n) const {
        assert(n < np.size());
        return np[np.size() - n - 1].second;
    }

    inline void nodeShiftRight(NODE node, size_t index) {
        assert(!m_ops.isFull(node));
        assert(!m_ops.isLeaf(node));
        const auto hs = m_ops.getNumberOfKeys(node);
        assert(hs > 0);

        assert(m_ops.getNumberOfChildren(node) == hs+1);
        for (size_t i=hs+1;i>index;i--) {
            auto n = m_ops.getNthChild(node, i-1);
            m_ops.setNthChild(node, i, n);
            m_ops.clearNthChild(node, i - 1);
            if constexpr (parents_ops) {
                m_ops.setParent(n, node);
            }
        }
    }

    inline void holderShiftRight(NODE node, size_t index) {
        assert(!m_ops.isFull(node));
        const auto hs = m_ops.getNumberOfKeys(node);
        assert(hs > 0);

        for (size_t i=hs;i>index;i--) {
            auto h = m_ops.extractNthHolder(node, i-1);
            m_ops.setNthHolder(node, i, std::move(h));
        }
    }

    inline void shiftRight(NODE node, size_t index) {
        this->holderShiftRight(node, index);
        if (!m_ops.isLeaf(node)) {
            this->nodeShiftRight(node, index);
        }
    }

    inline void nodeShiftLeft(NODE node, size_t index) {
        assert(!m_ops.isNullNode(node));
        assert(!m_ops.isLeaf(node));
        const auto hs = m_ops.getNumberOfKeys(node);
        assert(hs < m_ops.getOrder() * 2 - 1);

        assert(m_ops.getNumberOfChildren(node) == hs+1);
        for (size_t i=index;i+1<=hs+1;i++) {
            auto n = m_ops.getNthChild(node, i+1);
            m_ops.setNthChild(node, i, n);
            m_ops.clearNthChild(node, i+1);
            if constexpr (parents_ops) {
                m_ops.setParent(n, node);
            }
        }
    }

    inline void holderShiftLeft(NODE node, size_t index) {
        assert(!m_ops.isNullNode(node));
        assert(m_ops.getNumberOfKeys(node) > 0);
        const auto hs = m_ops.getNumberOfKeys(node);
        assert(hs < m_ops.getOrder() * 2 - 1);

        for (size_t i=index;i+1<=hs;i++) {
            auto h = m_ops.extractNthHolder(node, i+1);
            m_ops.setNthHolder(node, i, std::move(h));
        }
    }

    inline void shiftLeft(NODE node, size_t index) {
        this->holderShiftLeft(node, index);
        if (!m_ops.isLeaf(node)) {
            this->nodeShiftLeft(node, index);
        }
    }

    inline std::pair<KEY,NODE> splitFullNode(NODE& root, NODE parent, size_t nodeIdx, NODE node) {
        assert(m_ops.isNullNode(parent) || !m_ops.isFull(parent));
        assert(m_ops.isFull(node));
        const auto t = m_ops.getOrder();
        const auto isLeaf = m_ops.isLeaf(node);
        auto newNode = m_ops.createEmptyNode();

        for (auto i=t;i<2*t-1;i++) {
            auto holder = m_ops.extractNthHolder(node, i);
            m_ops.setNthHolder(newNode, i-t, std::move(holder));
        }

        if (!isLeaf) {
            for (auto i=t;i<2*t;i++) {
                auto knode = m_ops.getNthChild(node, i);
                m_ops.setNthChild(newNode, i-t, knode);
                m_ops.clearNthChild(node, i);
                if constexpr (parents_ops) {
                    m_ops.setParent(knode, newNode);
                }
            }
        }

        auto middle_h = m_ops.extractNthHolder(node, t-1);
        auto middle_key = m_ops.getKey(middle_h);
        if (m_ops.isNullNode(parent)) {
            assert(m_ops.nodeCompareEqual(root, node));
            parent = root = m_ops.createEmptyNode();
            m_ops.setNthHolder(root, 0, std::move(middle_h));
            m_ops.setNthChild(root, 0, node);
            m_ops.setNthChild(root, 1, newNode);
            if constexpr (parents_ops) {
                m_ops.setParent(node, root);
                m_ops.setParent(newNode, root);
            }
        } else {
            this->holderShiftRight(parent, nodeIdx);
            this->nodeShiftRight(parent, nodeIdx + 1);
            m_ops.setNthHolder(parent, nodeIdx, std::move(middle_h));
            m_ops.setNthChild(parent, nodeIdx + 1, newNode);
            if constexpr (parents_ops) {
                m_ops.setParent(newNode, parent);
            }
        }

        if constexpr (parents_ops) {
            m_ops.setParent(newNode, parent);
        }
        return std::make_pair(middle_key,parent);
    }

    inline void mergeTwoNodes(NODE parent, size_t firstIdx) {
        assert(m_ops.getNumberOfChildren(parent) > firstIdx + 1);
        auto n1 = m_ops.getNthChild(parent, firstIdx);
        auto n2 = m_ops.getNthChild(parent, firstIdx + 1);
        const auto t = m_ops.getOrder();
        assert(!m_ops.isNullNode(n1) && m_ops.getNumberOfKeys(n1) == t - 1);
        assert(!m_ops.isNullNode(n2) && m_ops.getNumberOfKeys(n2) == t - 1);

        auto h = m_ops.extractNthHolder(parent, firstIdx);
        m_ops.setNthHolder(n1, t-1, std::move(h));

        for (size_t i=0;i<t-1;i++) {
            auto h = m_ops.extractNthHolder(n2, i);
            m_ops.setNthHolder(n1, i+t, std::move(h));
        }

        assert(m_ops.isLeaf(n1) == m_ops.isLeaf(n2));
        if (!m_ops.isLeaf(n1)) {
            for (size_t i=0;i<t;i++) {
                auto n = m_ops.getNthChild(n2, i);
                m_ops.setNthChild(n1, i+t, n);
                m_ops.clearNthChild(n2, i);
                if constexpr (parents_ops) {
                    m_ops.setParent(n , n1);
                }
            }
        }

        assert(m_ops.isEmptyNode(n2));
        m_ops.clearNthChild(parent, firstIdx + 1);
        m_ops.releaseEmptyNode(std::move(n2));

        if (m_ops.getNumberOfKeys(parent) > 0) {
            this->holderShiftLeft(parent, firstIdx);
        } else {
            // TODO assert parent
        }
        this->nodeShiftLeft(parent, firstIdx + 1);
    }

public:
    inline explicit BTreeAlgorithm(T ops): m_ops(ops) {}

    inline void check_consistency(NODE root) const {
        if (m_ops.isNullNode(root)) return;
        std::queue<std::pair<NODE,size_t>> queue;
        queue.push(std::make_pair(root, 1));
        size_t depth = 0;
        for (;!queue.empty();queue.pop()) {
            auto front = queue.front();
            auto node = front.first;

            if (m_ops.isLeaf(node)) {
                if (depth == 0) {
                    depth = front.second;
                }
                assert(depth == front.second);
            } else {
                assert(m_ops.getNumberOfKeys(node) + 1 == m_ops.getNumberOfChildren(node));
            }

            if (front.second != 1) {
                assert(m_ops.getNumberOfKeys(node) >= m_ops.getOrder() - 1);
            }

            const auto n1 = m_ops.getNumberOfKeys(node);
            for (size_t i=0;i<n1;i++) {
                auto h1 = m_ops.getNthHolder(node, i);
                if (i>0) {
                    auto h0 = m_ops.getNthHolder(node, i-1);
                    assert(m_ops.keyCompareLess(m_ops.getKey(h0), m_ops.getKey(h1)));
                }
            }
            const auto n2 = m_ops.getNumberOfChildren(node);
            for (size_t i=0;i<n2;i++) {
                auto nn = m_ops.getNthChild(node, i);
                assert(!m_ops.isNullNode(nn));
                queue.push(std::make_pair(nn, front.second + 1));
            }

            if (!m_ops.isLeaf(node)) {
                for (size_t i=0;i<n1;i++) {
                    auto h1 = m_ops.getNthHolder(node, i);
                    auto n1 = m_ops.getNthChild(node, i);
                    auto n2 = m_ops.getNthChild(node, i+1);
                    assert(!m_ops.isNullNode(n1));
                    assert(!m_ops.isNullNode(n2));
                    const auto& k1 = m_ops.getKey(m_ops.getLastHolder(n1));
                    const auto& k2 = m_ops.getKey(m_ops.getFirstHolder(n2));
                    assert(m_ops.keyCompareLess(k1, m_ops.getKey(h1)));
                    assert(m_ops.keyCompareLess(m_ops.getKey(h1), k2));
                }
            }
        }
    }

    inline HolderPath insertHolder(NODE& root, HOLDER&& holder) {
        HolderPath ans = { this->InitPath<NodePath>(), 0 };
        if (m_ops.isNullNode(root)) {
            root = m_ops.createEmptyNode();
            m_ops.setNthHolder(root, 0, std::move(holder));
            this->NodePathPush<NodePath>(ans.m_path, root, 0);
            return ans;
        }

        const auto t = m_ops.getOrder();
        const auto key = m_ops.getKey(holder);

        auto parentNode = m_ops.getNullNode();
        auto currentIdx = 0;
        auto current = root;

        while (true) {
            this->NodePathPush<NodePath>(ans.m_path, current, currentIdx);
            if (m_ops.isFull(current)) {
                this->NodePathPop<NodePath>(ans.m_path);
                auto result = this->splitFullNode(root, parentNode, currentIdx, current);
                if (!m_ops.keyCompareLess(key, result.first)) {
                    current = result.second;
                    currentIdx++;
                }

                if (m_ops.isNullNode(parentNode)) {
                    this->NodePathPush<NodePath>(ans.m_path, root, 0);
                }

                this->NodePathPush<NodePath>(ans.m_path, current, currentIdx);
            }

            currentIdx = m_ops.upper_bound(current, key);
            assert (currentIdx <= m_ops.getNumberOfKeys(current));

            if constexpr (!multikey) {
                if (currentIdx > 0 &&
                    m_ops.keyCompareEqual(key, m_ops.getNthKey(current, currentIdx-1)))
                {
                    return { this->InitPath<NodePath>(), 0 };
                }
            }

            if (m_ops.isLeaf(current)) {
                if (currentIdx < m_ops.getNumberOfKeys(current))
                    this->holderShiftRight(current, currentIdx);

                ans.m_index = currentIdx;
                m_ops.setNthHolder(current, currentIdx, std::move(holder));
                break;
            } else {
                parentNode = current;
                current = m_ops.getNthChild(parentNode, currentIdx);
            }
        }

        return ans;
    }

    inline HOLDER deleteHolder(NODE& root, HolderPath path) {
        assert(!m_ops.isNullNode(root));
        const auto t = m_ops.getOrder();
        auto node = this->GetNodeAncestor(path.m_path, 0);

        if (!m_ops.isLeaf(node)) {
            auto ndd = node;
            auto hdd = m_ops.extractNthHolder(node, path.m_index);
            auto idd = path.m_index;

            node = m_ops.getNthChild(node, path.m_index+1);
            this->NodePathPush<NodePath>(path.m_path, node, path.m_index+1);
            while (!m_ops.isLeaf(node)) {
                node = m_ops.getNthChild(node, 0);
                this->NodePathPush<NodePath>(path.m_path, node, 0);
            }
            path.m_index = 0;
            m_ops.setNthHolder(ndd, idd, m_ops.extractNthHolder(node, 0));
            m_ops.setNthHolder(node, 0, std::move(hdd));
        }

        const auto depth = this->GetPathDepth(path.m_path);
        assert(depth >= 1);
        if (depth == 1) {
            auto node = this->GetNodeAncestor(path.m_path, 0);
            assert(m_ops.nodeCompareEqual(node, root));
            const auto s = m_ops.getNumberOfKeys(node);
            assert(s > 0 && path.m_index < s);
            auto ans = m_ops.extractNthHolder(node, path.m_index);
            if (path.m_index + 1 < s) {
                this->holderShiftLeft(node, path.m_index);
            } else if (s == 1) {
                m_ops.releaseEmptyNode(std::move(node));
                root = m_ops.getNullNode();
            }
            return ans;
        }

        for (size_t i=depth;i>0;i--) {
            auto p = this->GetNodeAncestor(path.m_path, i);
            auto n = this->GetNodeAncestor(path.m_path, i-1);
            auto idx = this->GetNodeIndex(path.m_path, i-1);
            if (!m_ops.isNullNode(p)) {
                assert(m_ops.nodeCompareEqual(n, m_ops.getNthChild(p, idx)));
            }
            if (m_ops.nodeCompareEqual(n, root)) continue;

            assert(!m_ops.isNullNode(p));
            if (m_ops.getNumberOfKeys(n) <= t - 1) {
                assert(m_ops.getNumberOfKeys(n) == t - 1);

                if (idx > 0) {
                    auto sibling_prev = m_ops.getNthChild(p, idx-1);
                    if (m_ops.getNumberOfKeys(sibling_prev) > t - 1) {
                        this->shiftRight(n, 0);
                        const auto sib_prev_n = m_ops.getNumberOfKeys(sibling_prev);
                        auto ph = m_ops.extractNthHolder(sibling_prev, sib_prev_n-1);
                        auto parent_holder = m_ops.extractNthHolder(p, idx-1);
                        m_ops.setNthHolder(p, idx-1, std::move(ph));
                        if (!m_ops.isLeaf(n)) {
                            auto pn = m_ops.getNthChild(sibling_prev, sib_prev_n);
                            assert(!m_ops.isNullNode(pn));
                            m_ops.clearNthChild(sibling_prev, sib_prev_n);
                            m_ops.setNthChild(n, 0, pn);
                            if constexpr (parents_ops) {
                                m_ops.setParent(pn, n);
                            }
                        }
                        m_ops.setNthHolder(n, 0, std::move(parent_holder));

                        if constexpr (!parents_ops) {
                            if (i > 1) {
                                path.m_path[depth+1-i].second++;
                            }
                        }

                        if (i == 1) {
                            assert(m_ops.isLeaf(n));
                            path.m_index++;
                        }
                    } else {
                        assert(m_ops.getNumberOfKeys(sibling_prev) == t - 1);
                        this->mergeTwoNodes(p, idx-1);
                        if constexpr (parents_ops) {
                            n = m_ops.getNthChild(p, idx-1);
                            if (i == 1)
                                path.m_path = n;
                        } else {
                            auto mergedNode = m_ops.getNthChild(p, idx-1);
                            path.m_path[depth-i].first = n = mergedNode;
                            path.m_path[depth-i].second = idx = idx - 1;
                            if (i > 1) {
                                path.m_path[depth-i+1].second += t;
                            }
                        }

                        if (i == 1) {
                            assert(m_ops.isLeaf(n));
                            path.m_index += t;
                        }
                    }
                } else {
                    assert(idx + 1 < m_ops.getNumberOfChildren(p));
                    auto sibling_next = m_ops.getNthChild(p, idx+1);
                    if (m_ops.getNumberOfKeys(sibling_next) > t - 1) {
                        const auto sib_next_n = m_ops.getNumberOfKeys(sibling_next);
                        auto nh = m_ops.extractNthHolder(sibling_next, 0);
                        auto parent_holder = m_ops.extractNthHolder(p, idx);
                        m_ops.setNthHolder(p, idx, std::move(nh));
                        if (!m_ops.isLeaf(n)) {
                            auto nn = m_ops.getNthChild(sibling_next, 0);
                            m_ops.clearNthChild(sibling_next, 0);
                            m_ops.setNthChild(n, t, nn);
                            if constexpr (parents_ops) {
                                m_ops.setParent(nn, n);
                            }
                        }
                        this->shiftLeft(sibling_next, 0);
                        m_ops.setNthHolder(n, t-1, std::move(parent_holder));
                    } else {
                        assert(m_ops.getNumberOfKeys(sibling_next) == t - 1);
                        this->mergeTwoNodes(p, idx);
                    }
                }
            }
        }

        auto leafNode = this->GetNodeAncestor(path.m_path, 0);
        assert(m_ops.getNumberOfKeys(leafNode) > t - 1);
        assert(m_ops.isLeaf(leafNode));

        const bool shiftLeft = path.m_index + 1 < m_ops.getNumberOfKeys(leafNode);
        auto ans = m_ops.extractNthHolder(leafNode, path.m_index);
        if (shiftLeft) {
            this->shiftLeft(leafNode, path.m_index);
        }

        if (m_ops.getNumberOfKeys(root) == 0) {
            assert(m_ops.getNumberOfChildren(root) == 1);
            auto new_root = m_ops.getFirstChild(root);
            m_ops.clearNthChild(root, 0);
            m_ops.releaseEmptyNode(std::move(root));
            root = new_root;
            if constexpr (parents_ops) {
                m_ops.setParent(root, m_ops.getNullNode());
            }
        }

        return ans;
    }

    inline HolderPath findKey(NODE root, const KEY& key) {
        const auto invalid_idx = m_ops.getOrder() * 2;
        HolderPath path { this->InitPath<NodePath>(), invalid_idx };
        if (m_ops.isNullNode(root))
            return path;

        size_t node_index = 0;
        for (auto node=root;!m_ops.isNullNode(node);) {
            this->NodePathPush<NodePath>(path.m_path, node, node_index);

            auto kn = m_ops.upper_bound(node, key);
            if (kn > 0 && m_ops.keyCompareEqual(key, m_ops.getNthKey(node, kn-1))) {
                assert(kn < invalid_idx);
                path.m_index = kn - 1;
                break;
            }

            node = m_ops.getNthChild(node, kn);
            node_index = kn;
        }

        if (path.m_index == invalid_idx)
            path.m_path= this->InitPath<NodePath>();

        return path;
    }

    inline HolderPath lower_bound(NODE root, const KEY& key) {
        assert(!m_ops.isNullNode(root));
        auto ans = HolderPath(this->InitPath<NodePath>(), 0);
        auto current = HolderPath(this->InitPath<NodePath>(), 0);
        size_t idx = 0;
        for (auto node=root;;) {
            this->NodePathPush<NodePath>(current.m_path, node, idx);
            auto pos = m_ops.lower_bound(node, key);

            if (pos < m_ops.getNumberOfKeys(node)) {
                auto pos_key = m_ops.getNthKey(node, pos);
                if (!exists(ans) || !m_ops.keyCompareLess(pos_key, key)) {
                    current.m_index = pos;
                    ans = current;
                }
            }

            if (m_ops.isLeaf(node)) break;

            idx = pos;
            node = m_ops.getNthChild(node, pos);
        }
        return ans;
    }

    inline HolderPath upper_bound(NODE root, const KEY& key) {
        assert(!m_ops.isNullNode(root));
        auto ans = HolderPath(this->InitPath<NodePath>(), 0);
        auto current = HolderPath(this->InitPath<NodePath>(), 0);
        size_t idx = 0;
        for (auto node=root;;) {
            this->NodePathPush<NodePath>(current.m_path, node, idx);
            auto pos = m_ops.upper_bound(node, key);

            if (pos < m_ops.getNumberOfKeys(node)) {
                auto pos_key = m_ops.getNthKey(node, pos);
                if (!exists(ans) || m_ops.keyCompareLess(key, pos_key)) {
                    current.m_index = pos;
                    ans = current;
                }
            }

            if (m_ops.isLeaf(node)) break;

            idx = pos;
            node = m_ops.getNthChild(node, pos);
        }
        return ans;
    }

    inline HolderPath begin(NODE root) {
        auto ans = HolderPath(this->InitPath<NodePath>(), 0);
        if (m_ops.isNullNode(root)) return ans;
        for (auto node=root;;node=m_ops.getFirstChild(node)) {
            assert(!m_ops.isNullNode(node));
            if constexpr (parents_ops) {
                ans.m_path = node;
            } else {
                ans.m_path.push_back(std::make_pair(node,0));
            }

            if (m_ops.isLeaf(node)) break;
        }
        return ans;
    }

    inline HolderPath end(NODE) {
        return HolderPath(this->InitPath<NodePath>(), 0);
    }

    inline void forward(NODE root, HolderPath& path) {
        auto node = this->GetNodeAncestor(path.m_path, 0);
        if (m_ops.isLeaf(node)) {
            if ( path.m_index + 1 < m_ops.getNumberOfKeys(node)) {
                path.m_index++;
            } else {
                auto p = this->GetNodeAncestor(path.m_path, 1);
                auto u = this->GetNodeIndex(path.m_path, 0);
                this->NodePathPop<NodePath>(path.m_path);
                path.m_index = u;
                while (!m_ops.isNullNode(p) && u + 1 == m_ops.getNumberOfChildren(p)) {
                    node = this->GetNodeAncestor(path.m_path, 0);
                    u = this->GetNodeIndex(path.m_path, 0);
                    p = this->GetNodeAncestor(path.m_path, 1);
                    path.m_index = u;
                    this->NodePathPop<NodePath>(path.m_path);
                }
            }
        } else {
            auto index = path.m_index + 1;
            for (auto n =m_ops.getNthChild(node, index);;n=m_ops.getFirstChild(n)) {
                this->NodePathPush<NodePath>(path.m_path, n, index);
                index = 0;
                if (m_ops.isLeaf(n))
                    break;
            }
            path.m_index = 0;
        }
    }

    inline void backward(NODE root, HolderPath& path) {
        if (!this->exists(path)) {
            path = HolderPath(this->InitPath<NodePath>(), 0);
            size_t index = 0;
            for (auto node=root;;) {
                assert(!m_ops.isNullNode(node));
                if constexpr (parents_ops) {
                    path.m_path = node;
                } else {
                    path.m_path.push_back(std::make_pair(node,index));
                }
                if (m_ops.isLeaf(node)) {
                    path.m_index = m_ops.getNumberOfKeys(node) - 1;
                    break;
                } else {
                    auto s = m_ops.getNumberOfChildren(node);
                    node=m_ops.getNthChild(node, s-1);
                    index = s-1;
                }
            }
            return;
        }

        auto node = this->GetNodeAncestor(path.m_path, 0);
        if (m_ops.isLeaf(node)) {
            if ( path.m_index > 0) {
                path.m_index--;
            } else {
                auto p = this->GetNodeAncestor(path.m_path, 1);
                auto u = this->GetNodeIndex(path.m_path, 0);
                this->NodePathPop<NodePath>(path.m_path);
                path.m_index = u > 0 ? u - 1 : 0;
                while (!m_ops.isNullNode(p) && u == 0) {
                    node = this->GetNodeAncestor(path.m_path, 0);
                    u = this->GetNodeIndex(path.m_path, 0);
                    p = this->GetNodeAncestor(path.m_path, 1);
                    path.m_index = u > 0 ? u - 1 : 0;
                    this->NodePathPop<NodePath>(path.m_path);
                }
            }
        } else {
            auto index = path.m_index;
            for (auto n =m_ops.getNthChild(node, index);;n=m_ops.getLastChild(n)) {
                this->NodePathPush<NodePath>(path.m_path, n, index);
                if (m_ops.isLeaf(n)){
                    path.m_index = m_ops.getNumberOfKeys(n) - 1;
                    break;
                }
                index = m_ops.getNumberOfChildren(n) - 1;
            }
        }
    }

    inline HOLDER getHolder(const HolderPath& path) {
        assert(this->exists(path));
        return m_ops.getNthHolder(this->GetNodeAncestor(path.m_path, 0), path.m_index);
    }

    inline bool exists(const HolderPath& path) {
        if constexpr (parents_ops) {
            return !m_ops.isNullNode(path.m_path);
        } else {
            return !path.m_path.empty();
        }
    }

private:
    BTreeOpWrapper<T,NODE,HOLDER,KEY> m_ops;
};
}
