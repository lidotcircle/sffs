#pragma once
#include "./maxsize_vector.h"
#include "./ldc_utils.h"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>
#include <queue>
#include <functional>
#include <type_traits>


namespace ldc::BTreeAlgorithmImpl {
template <typename _T, typename _Node, typename _Holder, typename _Key, typename _Value, bool complain=false>
struct treeop_traits {
#define TREEOP_FUNC_TEST_OPTIONAL(...) LDC_MEMBER_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR_AUTONAME(__VA_ARGS__)
#define TREEOP_FUNC_TEST_REQUIRED(A, B, F, R, ...) \
    LDC_MEMBER_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR_AUTONAME(A, B, F, R, __VA_ARGS__); \
    static_assert(!complain || has_##F, "should implement '" #R " " #F "(" #__VA_ARGS__ ") " #B "';")

    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNthChild,         _Node,    _Node, size_t);
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       setNthChild,         void,     _Node, size_t, _Node);
    TREEOP_FUNC_TEST_OPTIONAL(_T, ,       clearNthChild,       void,     _Node, size_t);
    TREEOP_FUNC_TEST_OPTIONAL(_T, const&, getParent,           _Node,    _Node);
    TREEOP_FUNC_TEST_OPTIONAL(_T, ,       setParent,           void,     _Node, _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNthHolder,        _Holder,  _Node, size_t);
    TREEOP_FUNC_TEST_OPTIONAL(_T, ,       getNthHolderRef,     _Holder&, _Node, size_t);
    TREEOP_FUNC_TEST_OPTIONAL(_T, ,       setNthHolderValue,   void,     _Node&, size_t, _Value);
    TREEOP_FUNC_TEST_OPTIONAL(_T, ,       setHolderValue,      void,     _Holder&, _Value);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNthKey,           _Key,     _Node, size_t);
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       extractNthHolder,    _Holder,  _Node&, size_t);
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       setNthHolder,        void,     _Node&, size_t, _Holder&&);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getOrder,            size_t);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNumberOfChildren, size_t,   _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNumberOfKeys,     size_t,   _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, isNullNode,          bool,     _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNullNode,         _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       createEmptyNode,     _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       releaseEmptyNode,    void,     _Node);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getKey,              _Key,     _Holder);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, keyCompareLess,      bool,     const _Key&, const _Key&);
    TREEOP_FUNC_TEST_OPTIONAL(_T, const&, keyCompareEqual,     bool,     const _Key&, const _Key&);
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, nodeCompareEqual,    bool,     const _Node&, const _Node&);

#undef TREEOP_FUNC_TEST_REQUIRED
#undef TREEOP_FUNC_TEST_OPTIONAL

    static_assert(!complain || !std::is_reference_v<_Node>,            "NODE should not be a reference");
    static_assert(!complain || !std::is_const_v<_Node>,                "NODE should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<_Node>,      "NODE should be copy assignable");
    static_assert(!complain || !std::is_reference_v<_Holder>,          "HOLDER should not be a reference");
    static_assert(!complain || !std::is_const_v<_Holder>,              "HOLDER should not be const-qualified");
    static_assert(!complain ||  std::is_move_constructible_v<_Holder>, "HOLDER should be copy assignable");
    static_assert(!complain || !std::is_reference_v<_Key>,             "KEY should not be a reference");
    static_assert(!complain || !std::is_const_v<_Key>,                 "KEY should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<_Key>,       "KEY should be copy assignable");
    static_assert(!complain ||  std::is_same_v<_Value,void> || (has_setNthHolderValue || (has_getNthHolderRef && has_setHolderValue)), 
                                                                      "should provide value accessor for non-void value");

    static constexpr bool value = !std::is_reference_v<_Node> && !std::is_const_v<_Node> &&
                                  !std::is_reference_v<_Holder>  && !std::is_const_v<_Holder> && std::is_copy_assignable_v<_Holder> &&
                                   std::is_move_constructible_v<_Holder> &&
                                  !std::is_reference_v<_Key>  && !std::is_const_v<_Key> && std::is_copy_assignable_v<_Key> &&
                                  has_getNthChild && has_setNthChild  &&
                                  std::is_same_v<_Value,void> || (has_setNthHolderValue || (has_getNthHolderRef && has_setHolderValue)) &&
                                  has_getNthHolder && has_extractNthHolder && has_setNthHolder  &&
                                  has_getOrder && has_getNumberOfChildren && has_getNumberOfKeys &&
                                  has_isNullNode && has_getNullNode && has_createEmptyNode &&
                                  has_getKey && has_keyCompareLess &&
                                  has_nodeCompareEqual;
};

template<typename T, typename NODE, typename HOLDER, typename KEY, typename VALUE,
         std::enable_if_t<treeop_traits<T,NODE,HOLDER,KEY,VALUE>::value,bool> = true>
struct BTreeOpWrapper {
    using traits = treeop_traits<T,NODE,HOLDER,KEY,VALUE>;

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

    inline HOLDER& getNthHolderRef(NODE node, size_t nth) {
        if constexpr (traits::has_getNthHolderRef) {
            return m_ops.getNthHolderRef(node, nth);
        } else {
            return std::declval<HOLDER&>();
        }
    }

    using HolderValue = std::conditional_t<std::is_same_v<VALUE,void>,dummy_struct,VALUE>;
    inline void setHolderValue(HOLDER& holder, HolderValue val) {
        if constexpr (traits::has_setHolderValue) {
            m_ops.setHolderValue(holder, val);
        } else {
            assert(false);
        }
    }

    inline void setNthHolderValue(NODE node, size_t nth, HolderValue val) {
        if constexpr (traits::has_setNthHolderValue) {
            return m_ops.setNthHolderValue(node, nth, val);
        } else if constexpr (traits::has_getNthHolderRef && traits::has_setHolderValue) {
            m_ops.setHolderValue(m_ops.getNthHolderRef(node, nth), val);
        } else if constexpr (std::is_same_v<VALUE,void>) {
        } else {
            static_assert(std::is_same_v<VALUE,void> ||traits::has_setNthHolderValue || (traits::has_getNthHolderRef && traits::has_setHolderValue),
                          "failed to implement setNthHolderValue");
        }
    }

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

    inline T& ops() { return m_ops; }
    inline const T& ops() const { return m_ops; }

private:
    T m_ops;
};

template<typename T, typename NODE, typename HOLDER, typename KEY, typename VALUE, bool enableParent=true,
         bool multikey = false, size_t static_vector_size = 32,
         std::enable_if_t<treeop_traits<T,NODE,HOLDER,KEY,VALUE,true>::value,bool> = true>
class BTreeAlgorithm {
public:
    using traits = treeop_traits<T,NODE,HOLDER,KEY,VALUE>;
    static constexpr bool parents_ops = traits::has_getParent && traits::has_getParent && enableParent;
    static constexpr bool ref_accessor = traits::has_getNthHolderRef;
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

    inline void setHolderValue(const HolderPath& path, std::conditional_t<std::is_same_v<VALUE,void>,dummy_struct,VALUE> value) {
        assert(this->exists(path));
        m_ops.setNthHolderValue(this->GetNodeAncestor(path.m_path, 0), path.m_index, value);
    }

    inline HOLDER& getHolderRef(const HolderPath& path) {
        assert(this->exists(path));
        return m_ops.getNthHolderRef(this->GetNodeAncestor(path.m_path, 0), path.m_index);
    }

    inline KEY getHolderKey(const HolderPath& path) {
        assert(this->exists(path));
        return m_ops.getKey(m_ops.getNthHolder(this->GetNodeAncestor(path.m_path, 0), path.m_index));
    }

    inline int compareHolderPath(const HolderPath& p1, const HolderPath& p2) const {
        if (!this->exists(p1)) {
            if (this->exists(p2)) {
                return 1;
            } else {
                return 0;
            }
        } else if (!this->exists(p2)) {
            return -1;
        }

        const auto node1 = this->GetNodeAncestor(p1.m_path,0);
        const auto node2 = this->GetNodeAncestor(p2.m_path,0);
        if (m_ops.nodeCompareEqual(node1, node2)) {
            if (p1.m_index == p2.m_index) {
                return 0;
            } else if (p1.m_index < p2.m_index) {
                return -1;
            } else {
                return 1;
            }
        }

        const auto& k1 = m_ops.getNthKey(node1, p1.m_index);
        const auto& k2 = m_ops.getNthKey(node2, p2.m_index);
        if (m_ops.keyCompareLess(k1, k2)) return -1;
        if (m_ops.keyCompareLess(k2, k1)) return 1;

        if constexpr (multikey) {
            auto pp1 = p1, pp2 = p2;
            for (;this->exists(pp1) && this->exists(pp2);
                  this->forward(pp1), this->forward(pp2))
            {
                const auto n1 = this->GetNodeAncestor(pp1.m_path, 0);
                const auto n2 = this->GetNodeAncestor(pp2.m_path, 0);
                if (m_ops.nodeCompareEqual(node1, n2)) {
                    return -1;
                } else if (m_ops.nodeCompareEqual(node2, n1)) {
                    return 1;
                }

                pp1.m_index = m_ops.getNumberOfKeys(n1) - 1;
                pp2.m_index = m_ops.getNumberOfKeys(n2) - 1;
            }

            return this->exists(pp1) ? -1 : 1;
        } else {
            assert(false);
            return 0;
        }
    }

    inline void releaseNode(NODE node) {
        if (!m_ops.isLeaf(node)) {
            const auto n = m_ops.getNumberOfChildren(node);
            for (size_t i=0;i<n;i++) {
                this->releaseNode(m_ops.getNthChild(node, i));
                m_ops.setNthChild(node, i, m_ops.getNullNode());
            }
        }
        m_ops.releaseEmptyNode(std::move(node));
    }

    template<typename U, std::enable_if_t<std::is_same_v<HOLDER,decltype(std::declval<U>()())>,bool> = true>
    inline NODE initWithAscSequence(size_t size, U iterFunc) {
        assert(size > 0);

        const auto t = m_ops.getOrder();
        size_t depth = 1;
        {
            const auto vn = 2 * t - 1;
            for (size_t n=vn,k=vn;n<size;k *= vn,n+=k,depth++);
        }
        const auto countBound = [&](size_t d) {
            size_t v1 = t;
            size_t v2 = 2*t;
            for (size_t i=1;i<d;i++, v1*=t, v2*=2*t) ;
            return std::make_pair(v1-1,v2-1);
        };
        const std::function<NODE(size_t,size_t)> initNode = [&](size_t d, size_t s) {
            auto nd = m_ops.createEmptyNode();

            if (d == 1) {
                assert(depth == 1 || (t-1 <= s && s <= 2*t-1));
                for (size_t i=0;i<s;i++) {
                    m_ops.setNthHolder(nd, i, iterFunc());
                }
                return nd;
            } else {
                const auto pp = countBound(d-1);
                const auto minx = d == depth ? 2 : t;
                const auto minchild = (s / (pp.second+1)) + 1;
                const auto nchild = std::max(minchild, minx);
                const auto mchild = std::min((s+1) / (pp.first+1), 2*t);
                const auto child_s = (s - nchild + 1) / nchild;
                const auto child_r = (s - nchild + 1) % nchild;
                for (size_t i=0;i<nchild;i++) {
                    const auto cs = (i < child_r) ? child_s +1 : child_s;
                    assert(pp.first <= cs && cs <= pp.second);
                    m_ops.setNthChild(nd, i, initNode(d-1,cs));
                    if (i+1 < nchild) {
                        m_ops.setNthHolder(nd, i, iterFunc());
                    }
                }
            }

            return nd;
        };

        return initNode(depth, size);
    }

    inline bool exists(const HolderPath& path) const {
        if constexpr (parents_ops) {
            return !m_ops.isNullNode(path.m_path);
        } else {
            return !path.m_path.empty();
        }
    }

public:
    BTreeOpWrapper<T,NODE,HOLDER,KEY,VALUE> m_ops;
};
}


#include "./unarray.h"
namespace ldc::BTreeBasicContainerImpl {
template<typename _Key, typename _Value, size_t Order, bool parentsOps>
struct TreeNode {
    using KVPair = std::conditional_t<std::is_same_v<_Value,void>,_Key,std::pair<const _Key,_Value>>;
    std::conditional_t<parentsOps,TreeNode*,dummy_struct> m_parent;

    unarray<TreeNode*,2*Order> m_children;
    unarray<KVPair,2*Order - 1> m_holders;

    inline explicit TreeNode()
    {
        if constexpr (parentsOps) {
            m_parent = nullptr;
        }
    }

    ~TreeNode() = default;
};

template<typename _Key, typename _Value, typename _CmpLess, typename _Allocator,
         size_t Order, bool parentsOps>
struct TreeNodeOps {
    using TNODE  = TreeNode<_Key,_Value,Order,parentsOps>;
    using KVPair = typename TNODE::KVPair;
    using _Node  = TNODE*;
    using _storage_allocator = typename std::allocator_traits<_Allocator>::template rebind_alloc<TNODE>;
    _CmpLess           m_cmp;
    _storage_allocator m_allocator;

    TreeNodeOps(const _CmpLess& cmp, const _Allocator& allocator): m_cmp(cmp), m_allocator(allocator) {}

    inline _Node getNthChild(_Node node, size_t nth) const { return node->m_children.at(nth); }

    inline void setNthChild(_Node node, size_t nth, _Node n)  {
        if (n == nullptr) {
            node->m_children.destroy(nth);
        } else {
            node->m_children.construct(nth, n);
        }
    }

    inline KVPair& getNthHolderRef(_Node node, size_t nth) const {
        return node->m_holders.at(nth);
    }

    inline void setHolderValue(KVPair& h, std::conditional_t<std::is_same_v<_Value,void>,dummy_struct,_Value> val) const {
        if constexpr (std::is_same_v<_Value,void>) {
        } else {
            h.second = val;
        }
    }

    inline _Key getNthKey(_Node node, size_t nth) const {
        if constexpr (std::is_same_v<_Value,void>) {
            return node->m_holders.at(nth);
        } else {
            return node->m_holders.at(nth).first;
        }
    }

    inline KVPair getNthHolder(_Node node, size_t nth) const {
        return node->m_holders.at(nth);
    }

    inline KVPair extractNthHolder(_Node node, size_t nth) {
        auto n = std::move(node->m_holders.at(nth));
        node->m_holders.destroy(nth);
        return n;
    }

    inline void setNthHolder(_Node node, size_t nth, KVPair&& holder) {
        node->m_holders.construct(nth, std::move(holder));
    }

    inline _Node getParent(_Node node) const  {
        if constexpr (parentsOps) {
            return node->m_parent;
        } else {
            return nullptr;
        }
    }
    inline void setParent(_Node node, _Node n)  {
        if constexpr (parentsOps) {
            node->m_parent = n;
        }
    }

    inline size_t getOrder() const { return Order; }

    inline size_t getNumberOfChildren(_Node node) const { return node->m_children.size(); }
    inline size_t getNumberOfKeys    (_Node node) const { return node->m_holders.size(); }

    inline bool isNullNode(_Node node) const { return node == nullptr; }
    inline _Node getNullNode() const { return nullptr; }
    inline _Node createEmptyNode() {
        auto ptr = m_allocator.allocate(1);
        return new (ptr) TNODE();
    }
    inline void releaseEmptyNode(_Node&& node) {
        std::destroy_n(node, 1);
        m_allocator.deallocate(node, 1);
    }

    inline _Key getKey(const KVPair& n) const {
        if constexpr (std::is_same_v<_Value,void>) {
            return n;
        } else {
            return n.first;
        }
    }

    inline bool keyCompareLess(const _Key& lhs, const _Key& rhs) const { return m_cmp(lhs, rhs); }

    inline bool nodeCompareEqual(_Node lhs, _Node rhs) const { return lhs == rhs; }
};

template<typename _Key, typename _Value, typename _CmpLess, typename _Allocator,
         size_t Order, bool parentsOps, bool multikey>
using BTreeAlgo = BTreeAlgorithmImpl::BTreeAlgorithm<
                         TreeNodeOps<_Key,_Value,_CmpLess,_Allocator,Order,parentsOps>,
                         TreeNode<_Key,_Value,Order,parentsOps>*,
                         typename TreeNodeOps<_Key,_Value,_CmpLess,_Allocator,Order,parentsOps>::KVPair,
                         _Key, _Value, parentsOps, multikey>;

template<typename _Key, typename _Value, typename _CmpLess, typename _Allocator,
         size_t _Order, bool _parentsOps, bool _multikey>
struct BTreeInMemory: protected BTreeAlgo<_Key,_Value,_CmpLess,_Allocator,_Order,_parentsOps,_multikey> {
    using BASE      = BTreeAlgo<_Key,_Value,_CmpLess,_Allocator,_Order,_parentsOps,_multikey>;
    using treeops_t = TreeNodeOps<_Key,_Value,_CmpLess,_Allocator,_Order,_parentsOps>;
    using TNODE     = typename treeops_t::TNODE;
    using _Node     = typename treeops_t::_Node;
    using KVPair    = typename treeops_t::KVPair;
    using ITERATOR  = typename BASE::HolderPath;
    static constexpr auto  ref_accessor = BASE::ref_accessor;


private:
    _Node  m_root;
    size_t m_size;


public:
    inline BTreeInMemory(const _CmpLess& cmp, const _Allocator& alloc): 
        BASE(treeops_t(cmp, alloc)),
        m_root(nullptr), m_size(0) {}

    inline BTreeInMemory(const BTreeInMemory& _oth):
        BASE(treeops_t(_oth.m_ops.ops().m_cmp, _oth.m_ops.ops().m_allocator)),
        m_root(nullptr), m_size(0)
    {
        *this = _oth;
    }

    inline BTreeInMemory(BTreeInMemory&& _oth):
        BASE(std::move(_oth)), m_root(_oth.m_root), m_size(_oth.m_size)
    {
        _oth.m_size = 0;
        _oth.m_root = nullptr;
    }

    inline BTreeInMemory& operator=(const BTreeInMemory& _oth) {
        this->clear();
        if (_oth.size() > 0) {
            auto& oth = const_cast<BTreeInMemory&>(_oth);
            auto b = oth.begin();
            const auto fn = [&]() {
                const auto ans = oth.getHolder(b);
                oth.forward(b);
                return ans;
            };
            m_root = this->initWithAscSequence(oth.size(), fn);
            m_size = oth.size();
        }
        return *this;
    }

    inline BTreeInMemory& operator=(BTreeInMemory&& _oth) {
        this->clear();
        this->m_root = _oth.m_root;
        this->m_size = _oth.m_size;
        _oth.m_root = nullptr;
        _oth.m_size = 0;
        BASE::operator=(std::move(_oth));
        return *this;
    }

    inline ITERATOR insert(KVPair&& val) {
        auto ans = this->insertHolder(this->m_root, std::move(val));
        if (this->exists(ans)) m_size++;
        return ans;
    }

    inline ITERATOR find(const _Key& key) {
        if (this->m_root == nullptr) return this->end();

        return this->findKey(m_root, key);
    }

    inline ITERATOR lower_bound(const _Key& key) {
        if (this->m_root == nullptr) return this->end();

        return BASE::lower_bound(this->m_root, key);
    }

    inline ITERATOR upper_bound(const _Key& key) {
        if (this->m_root == nullptr) return this->end();

        return BASE::upper_bound(this->m_root, key);
    }

    inline ITERATOR begin() { return BASE::begin(this->m_root); }
    inline ITERATOR end()   { return BASE::end(this->m_root); }

    inline void forward(ITERATOR& path) { BASE::forward(this->m_root, path); }

    inline void backward(ITERATOR& path) { BASE::backward(this->m_root, path); }

    inline KVPair deleteIter(ITERATOR iter) {
        assert(this->m_root && this->exists(iter));
        const auto ans = this->deleteHolder(this->m_root, iter);
        m_size--;
        return ans;
    }

    using BASE::exists;
    using BASE::getHolder;
    using BASE::getHolderKey;
    using BASE::getHolderRef;
    using BASE::setHolderValue;
    using BASE::compareHolderPath;

    inline size_t size() const { return m_size; }

    inline void clear() {
        if (m_root) {
            this->releaseNode(m_root);
            m_root = nullptr;
            m_size = 0;
        }
    }

    ~BTreeInMemory() { this->clear(); }
};
}
