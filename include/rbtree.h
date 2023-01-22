#pragma once
#include "./maxsize_vector.h"
#include "./ldc_utils.h"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <queue>
#include <type_traits>
#include <functional>
#include <memory>


namespace ldc::RBTreeAlgorithmImpl {
template <typename _T, typename _Node, typename _Key, bool complain=false>
struct treeop_traits {
#define TREEOP_FUNC_TEST_OPTIONAL(A, B, C, D) LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(A, B, C, D)
#define TREEOP_FUNC_TEST_REQUIRED(A, B, F, P) \
    LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(A, B, F, P); \
    static_assert(!complain || has_##F, "should implement '" #F " " #P " " #B "';")

    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getLeft,          _Node(_Node));
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getRight,         _Node(_Node));
    TREEOP_FUNC_TEST_OPTIONAL(_T, const&, getParent,        _Node(_Node));
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       setLeft,          void (_Node,        _Node));
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       setRight,         void (_Node,        _Node));
    TREEOP_FUNC_TEST_OPTIONAL(_T, ,       setParent,        void (_Node,        _Node));
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, isBlack,          bool (_Node));
    TREEOP_FUNC_TEST_REQUIRED(_T, ,       setBlack,         void (_Node,        bool));
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, isNullNode,       bool (_Node));
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getNullNode,      _Node());
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, getKey,           _Key (_Node&));
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, keyCompareLess,   bool (const _Key&,  const _Key&));
    TREEOP_FUNC_TEST_OPTIONAL(_T, const&, keyCompareEqual,  bool (const _Key&,  const _Key&));
    TREEOP_FUNC_TEST_REQUIRED(_T, const&, nodeCompareEqual, bool (const _Node&, const _Node&));

#undef TREEOP_FUNC_TEST_REQUIRED
#undef TREEOP_FUNC_TEST_OPTIONAL

    static_assert(!complain || !std::is_reference_v<_Node>,       "NODE should not be a reference");
    static_assert(!complain || !std::is_const_v<_Node>,           "NODE should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<_Node>, "NODE should be copy assignable");
    static_assert(!complain || !std::is_reference_v<_Key>,        "KEY should not be a reference");
    static_assert(!complain || !std::is_const_v<_Key>,            "KEY should not be const-qualified");

    static constexpr bool value = !std::is_reference_v<_Node> && !std::is_const_v<_Node> && std::is_copy_assignable_v<_Node> &&
                                  !std::is_reference_v<_Key>  && !std::is_const_v<_Key> &&
                                  has_getLeft    && has_setLeft  &&
                                  has_getRight   && has_setRight &&
                                  has_isBlack    && has_setBlack &&
                                  has_isNullNode && has_getNullNode &&
                                  has_getKey && has_keyCompareLess &&
                                  has_nodeCompareEqual;
};

template<typename T, typename NODE, typename KEY,
         std::enable_if_t<treeop_traits<T,NODE,KEY>::value,bool> = true>
struct RbtreeOpWrapper {
    inline explicit RbtreeOpWrapper(T treeop): m_ops (treeop) {}

    inline NODE getLeft(NODE node) const  { return m_ops.getLeft(node); }
    inline NODE getRight(NODE node) const { return m_ops.getRight(node); }

    inline void setLeft(NODE node, NODE n)  { m_ops.setLeft(node, n); }
    inline void setRight(NODE node, NODE n) { m_ops.setRight(node, n); }

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

    inline bool isBlack(NODE node) const { return m_ops.isBlack(node); }
    inline bool isBlackOrNULL(NODE node) const { return m_ops.isNullNode(node) || m_ops.isBlack(node); }
    inline void setBlack(NODE node, bool black) { m_ops.setBlack(node, black); }

    inline bool isNullNode(NODE node) const { return m_ops.isNullNode(node); }
    inline NODE getNullNode() const { return m_ops.getNullNode(); }

    inline KEY getKey(NODE n) const { return m_ops.getKey(n); }

    inline bool keyCompareLess(const KEY& lhs, const KEY& rhs) const { return m_ops.keyCompareLess(lhs, rhs); }
    inline bool keyCompareLess(NODE lhs, NODE rhs) const { return m_ops.keyCompareLess(m_ops.getKey(lhs), m_ops.getKey(rhs)); }

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

    inline T& ops() { return m_ops; }
    inline const T& ops() const { return m_ops; }

private:
    using traits = treeop_traits<T,NODE,KEY>;
    T m_ops;
};

template<typename T, typename NODE, typename KEY,
         bool multikey = false, bool ParentsOps = true, size_t static_vector_size = 128,
         std::enable_if_t<treeop_traits<T,NODE,KEY,true>::value,bool> = true>
class RBTreeAlgorithm {
public:
    using traits = treeop_traits<T,NODE,KEY>;
    static constexpr bool parents_ops = traits::has_getParent && traits::has_setParent && ParentsOps;
    using NodePath = std::conditional_t<parents_ops, NODE,
                         std::conditional_t<static_cast<bool>(static_vector_size > 0), maxsize_vector<NODE,static_vector_size>,std::vector<NODE>>>;

private:
    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline NodePath InitPath() const {
        return m_ops.getNullNode();
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline NodePath InitPath() const {
        return NodePath();
    }

    inline void NodePathPush(NodePath& path, NODE node) const {
        if constexpr (parents_ops) {
            path = node;
        } else {
            path.push_back(node);
        }
    }

    inline void NodePathPop(NodePath& path) const {
        if constexpr (parents_ops) {
            path = m_ops.getParent(path);
        } else {
            path.pop_back();
        }
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline NODE GetNodeAncestor(N node, size_t n) const {
        for (size_t i=0;i<n;i++) {
            assert(!m_ops.isNullNode(node));
            node = m_ops.getParent(node);
        }
        return node;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline NODE GetNodeAncestor(N np, size_t n) const {
        if (n == np.size()) return m_ops.getNullNode();
        assert(n < np.size());
        return np[np.size() - n - 1];
    }
 
    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline void SwapAncestorAndDescendant(N n1, N n2) {
        static_assert(parents_ops);
        assert(!m_ops.nodeCompareEqual(n1, n2));
        assert(!m_ops.nodeCompareEqual(m_ops.getParent(n1), n2));
        const auto n1_color = m_ops.isBlack(n1);
        const auto n2_color = m_ops.isBlack(n2);
        m_ops.setBlack(n1, n2_color);
        m_ops.setBlack(n2, n1_color);

        const auto beLeft = [&](N x1, N x2) {
            m_ops.setLeft(x1, x2);
            if (!m_ops.isNullNode(x2)) {
                m_ops.setParent(x2, x1);
            }
        };
        const auto beRight = [&](N x1, N x2) {
            m_ops.setRight(x1, x2);
            if (!m_ops.isNullNode(x2)) {
                m_ops.setParent(x2, x1);
            }
        };

        if (m_ops.nodeCompareEqual(m_ops.getParent(n2), n1)) {
            const auto p = m_ops.getParent(n1);
            const bool is_left_child = m_ops.nodeCompareEqual(m_ops.getLeft(n1), n2);
            const bool n1_is_left_child = !m_ops.isNullNode(p) ? m_ops.nodeCompareEqual(m_ops.getLeft(p), n1) : false;
            const auto n1_left = m_ops.getLeft(n1), n1_right = m_ops.getRight(n1);
            const auto n2_left = m_ops.getLeft(n2), n2_right = m_ops.getRight(n2);

            beLeft(n1, n2_left);
            beRight(n1, n2_right);

            if (is_left_child) {
                beLeft(n2, n1);
                beRight(n2, n1_right);
            } else {
                beRight(n2, n1);
                beLeft(n2, n1_left);
            }
            if (!m_ops.isNullNode(p)) {
                if (n1_is_left_child) {
                    beLeft(p, n2);
                } else {
                    beRight(p, n2);
                }
            } else {
                m_ops.setParent(n2, p);
            }
        } else {
            const auto n1_parent = m_ops.getParent(n1);
            const bool n1_is_left = !m_ops.isNullNode(n1_parent) ? m_ops.nodeCompareEqual(m_ops.getLeft(n1_parent), n1) : false;
            const auto n1_left = m_ops.getLeft(n1), n1_right = m_ops.getRight(n1);

            const auto n2_parent = m_ops.getParent(n2);
            const bool n2_is_left = !m_ops.isNullNode(n2_parent) ? m_ops.nodeCompareEqual(m_ops.getLeft(n2_parent), n2) : false;
            const auto n2_left = m_ops.getLeft(n2), n2_right = m_ops.getRight(n2);

            m_ops.setParent(n1, n2_parent);
            m_ops.setParent(n2, n1_parent);
            beLeft(n2, n1_left);
            beRight(n2, n1_right);
            beLeft(n1, n2_left);
            beRight(n1, n2_right);

            if (!m_ops.isNullNode(n1_parent)) {
                if (n1_is_left) {
                    m_ops.setLeft(n1_parent, n2);
                } else {
                    m_ops.setRight(n1_parent, n2);
                }
            }
            if (!m_ops.isNullNode(n2_parent)) {
                if (n2_is_left) {
                    m_ops.setLeft(n2_parent, n1);
                } else {
                    m_ops.setRight(n2_parent, n1);
                }
            }
        }
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline void SwapAncestorAndDescendant(N& ance, N& desc) {
        static_assert(!parents_ops);
        assert(ance.size() < desc.size());
        assert(ance.size() > 0);
        assert(desc.size() > 0);
        {
            auto n1 = this->GetNodeAncestor(ance, 0);
            auto n2 = this->GetNodeAncestor(desc, 0);
            const auto n1_color = m_ops.isBlack(n1);
            const auto n2_color = m_ops.isBlack(n2);
            m_ops.setBlack(n1, n2_color);
            m_ops.setBlack(n2, n1_color);
        }

        if (ance.size() + 1 == desc.size()) {
            const bool desc_is_left = m_ops.nodeCompareEqual(m_ops.getLeft(ance.back()), desc.back());
            assert(m_ops.nodeCompareEqual(m_ops.getLeft(ance.back()), desc.back()) ||
                   m_ops.nodeCompareEqual(m_ops.getRight(ance.back()), desc.back()));

            if (ance.size() > 1) {
                const auto pp = ance[ance.size() - 2];
                if (m_ops.nodeCompareEqual(m_ops.getLeft(pp), ance.back())) {
                    m_ops.setLeft(pp, desc.back());
                } else {
                    assert(m_ops.nodeCompareEqual(m_ops.getRight(pp), ance.back()));
                    m_ops.setRight(pp, desc.back());
                }
            }

            const auto desc_left  = m_ops.getLeft(desc.back());
            const auto desc_right = m_ops.getRight(desc.back());
            if (desc_is_left) {
                m_ops.setLeft(desc.back(), ance.back());
                m_ops.setRight(desc.back(), m_ops.getRight(ance.back()));
            } else {
                assert(m_ops.nodeCompareEqual(m_ops.getRight(ance.back()), desc.back()));
                m_ops.setRight(desc.back(), ance.back());
                m_ops.setLeft(desc.back(), m_ops.getLeft(ance.back()));
            }
            m_ops.setLeft(ance.back(), desc_left);
            m_ops.setRight(ance.back(), desc_right);
        } else {
            const auto n1_parent = ance.size() > 1 ? ance[ance.size() - 2] : m_ops.getNullNode();
            const auto n1_left   = m_ops.getLeft(ance.back());
            const auto n1_right  = m_ops.getRight(ance.back());

            assert(desc.size() > 2);
            const auto n2_parent = desc[desc.size() - 2];
            const auto n2_left   = m_ops.getLeft(desc.back());
            const auto n2_right  = m_ops.getRight(desc.back());

            if (!m_ops.isNullNode(n1_parent)) {
                if (m_ops.nodeCompareEqual(m_ops.getLeft(n1_parent), ance.back())) {
                    m_ops.setLeft(n1_parent, desc.back());
                } else {
                    assert(m_ops.nodeCompareEqual(m_ops.getRight(n1_parent), ance.back()));
                    m_ops.setRight(n1_parent, desc.back());
                }
            }

            if (m_ops.nodeCompareEqual(m_ops.getLeft(n2_parent), desc.back())) {
                m_ops.setLeft(n2_parent, ance.back());
            } else {
                assert(m_ops.nodeCompareEqual(m_ops.getRight(n2_parent), desc.back()));
                m_ops.setRight(n2_parent, ance.back());
            }

            m_ops.setLeft (ance.back(), n2_left);
            m_ops.setRight(ance.back(), n2_right);
            m_ops.setLeft (desc.back(), n1_left);
            m_ops.setRight(desc.back(), n1_right);
        }

        std::swap(ance.back(), desc.back());
        desc[ance.size() - 1] = ance.back();
    }

    inline void nodeLeftRotate(NodePath& np)
    {
        auto node = this->GetNodeAncestor(np, 0);
        auto p    = this->GetNodeAncestor(np, 1);
        auto pp   = this->GetNodeAncestor(np, 2);;
        assert(m_ops.nodeCompareEqual(m_ops.getRight(p), node));

        auto nq = m_ops.getLeft(node);
        m_ops.setLeft(node, p);
        m_ops.setRight(p, nq);
        if constexpr (parents_ops) {
            m_ops.setParent(p, node);
            if (!m_ops.isNullNode(nq)) {
                m_ops.setParent(nq, p);
            }
            m_ops.setParent(node, pp);
        } else {
            np.pop_back();
            np.pop_back();
            np.push_back(node);
        }

        if (!m_ops.isNullNode(pp)) {
            if (m_ops.nodeCompareEqual(m_ops.getLeft(pp), p)) {
                m_ops.setLeft(pp, node);
            } else {
                assert(m_ops.nodeCompareEqual(m_ops.getRight(pp), p));
                m_ops.setRight(pp, node);
            }
        }
    }

    inline void nodeRightRotate(NodePath& np)
    {
        auto node = this->GetNodeAncestor(np, 0);
        auto p    = this->GetNodeAncestor(np, 1);
        auto pp   = this->GetNodeAncestor(np, 2);;
        assert(m_ops.nodeCompareEqual(m_ops.getLeft(p), node));

        auto nq = m_ops.getRight(node);
        m_ops.setRight(node, p);
        m_ops.setLeft(p, nq);
        if constexpr (parents_ops) {
            m_ops.setParent(p, node);
            if (nq != nullptr) {
                m_ops.setParent(nq, p);
            }
            m_ops.setParent(node, pp);
        } else {
            np.pop_back();
            np.pop_back();
            np.push_back(node);
        }

        if (!m_ops.isNullNode(pp)) {
            if (m_ops.nodeCompareEqual(m_ops.getRight(pp), p)) {
                m_ops.setRight(pp, node);
            } else {
                assert(m_ops.nodeCompareEqual(m_ops.getLeft(pp), p));
                m_ops.setLeft(pp, node);
            }
        }
    }

    inline NodePath last(NODE root) const {
        NodePath ans = this->InitPath<NodePath>();
        if (m_ops.isNullNode(root)) return ans;

        for (auto node=root;!m_ops.isNullNode(node);node=m_ops.getRight(node)) {
            this->NodePathPush(ans, node);
        }

        return ans;
    }

public:
    inline explicit RBTreeAlgorithm(T treeop): m_ops(treeop) {}

    inline NodePath insertNode(NODE& root, NODE node) {
        auto ans = this->InitPath<NodePath>();
        if (m_ops.isNullNode(root)) {
            root = node;
            m_ops.setBlack(node, true);
            m_ops.setLeft(node, m_ops.getNullNode());
            m_ops.setRight(node, m_ops.getNullNode());
            if constexpr (parents_ops) {
                m_ops.setParent(node, m_ops.getNullNode());
            }
            this->NodePathPush(ans, root);
            return ans;
        }

        auto cn = root;
        NodePath nodepath = this->InitPath<NodePath>();
        m_ops.setBlack(node, false);
        for(;;) {
            if constexpr (!multikey) {
                if (m_ops.keyCompareEqual(m_ops.getKey(node), m_ops.getKey(cn))) {
                    return ans;
                }
            }

            this->NodePathPush(nodepath, cn);
            if (m_ops.keyCompareLess(node, cn)) {
                if (m_ops.isNullNode(m_ops.getLeft(cn))) {
                    m_ops.setLeft(cn, node);
                    if constexpr (parents_ops) {
                        m_ops.setParent(node, cn);
                    }
                    break;
                } else {
                    cn = m_ops.getLeft(cn);
                }
            } else {
                if (m_ops.isNullNode(m_ops.getRight(cn))) {
                    m_ops.setRight(cn, node);
                    if constexpr (parents_ops) {
                        m_ops.setParent(node, cn);
                    }
                    break;
                } else {
                    cn = m_ops.getRight(cn);
                }
            }
        }

        this->NodePathPush(nodepath, node);
        if (m_ops.isBlack(cn)) {
            return nodepath;
        }
        if constexpr (parents_ops) {
            ans = node;
        }

        for (auto p = this->GetNodeAncestor(nodepath, 1);
             !m_ops.isBlack(node) && p && !m_ops.isBlack(p);
             p = this->GetNodeAncestor(nodepath, 1))
        {
            auto pp = GetNodeAncestor(nodepath, 2);
            assert(!m_ops.isBlack(p));
            assert( m_ops.isBlack(pp));

            if (m_ops.nodeCompareEqual(m_ops.getLeft(pp), p)) {
                if (!m_ops.nodeCompareEqual(m_ops.getLeft(p), node)) {
                    assert(m_ops.nodeCompareEqual(m_ops.getRight(p), node));
                    this->nodeLeftRotate(nodepath);
                } else {
                    if constexpr (!parents_ops) {
                        ans.push_back(node);
                    }
                    node = p;
                    this->NodePathPop(nodepath);
                }
                assert(m_ops.nodeCompareEqual(m_ops.getLeft(pp), node));
                nodeRightRotate(nodepath);
                m_ops.setBlack(m_ops.getLeft(node), true);
            } else {
                assert(m_ops.nodeCompareEqual(m_ops.getRight(pp), p));

                if (!m_ops.nodeCompareEqual(m_ops.getRight(p), node)) {
                    assert(m_ops.nodeCompareEqual(m_ops.getLeft(p), node));
                    this->nodeRightRotate(nodepath);
                } else {
                    if constexpr (!parents_ops) {
                        ans.push_back(node);
                    }
                    node = p;
                    this->NodePathPop(nodepath);
                }
                assert(m_ops.nodeCompareEqual(m_ops.getRight(pp), node));
                nodeLeftRotate(nodepath);
                m_ops.setBlack(m_ops.getRight(node), true);
            }

            if (m_ops.nodeCompareEqual(this->GetNodeAncestor(nodepath, 1), m_ops.getNullNode())) {
                root = node;
                m_ops.setBlack(node, true);
            }
        }

        if constexpr (!parents_ops) {
            while (!ans.empty()) {
                nodepath.push_back(std::move(ans.back()));
                ans.pop_back();
            }
            ans = std::move(nodepath);
        }

        return ans;
    }

    void check_consistency(NODE root) const {
        if (m_ops.isNullNode(root)) return;

        std::queue<std::pair<NODE,size_t>> queue;
        queue.push(std::make_pair(root, 0));
        size_t black_depth = 0;
        bool is_end = false;

        for (;!queue.empty();queue.pop()) {
            auto front = queue.front();
            auto node = front.first;
            auto bdepth = front.second + (m_ops.isBlack(node) ? 1 : 0);
            if (is_end) {
                assert (black_depth >= bdepth);
            }
            black_depth = black_depth > bdepth ? black_depth : bdepth;

            if (!m_ops.isBlack(node)) {
                assert(m_ops.isBlackOrNULL(m_ops.getLeft(node)));
                assert(m_ops.isBlackOrNULL(m_ops.getRight(node)));
            }

            if (!m_ops.isNullNode(m_ops.getLeft(node))) {
                if constexpr (parents_ops) {
                    assert(m_ops.nodeCompareEqual(m_ops.getParent(m_ops.getLeft(node)), node));
                }
                assert(m_ops.keyCompareLess(m_ops.getKey(m_ops.getLeft(node)), m_ops.getKey(node)));
                queue.push(std::make_pair(m_ops.getLeft(node), bdepth));
            } else if (!m_ops.isNullNode(m_ops.getRight(node))) {
                if constexpr (parents_ops) {
                    assert(m_ops.nodeCompareEqual(m_ops.getParent(m_ops.getRight(node)), node));
                }
                assert(m_ops.keyCompareLess(m_ops.getKey(node), m_ops.getKey(m_ops.getRight(node))));
                queue.push(std::make_pair(m_ops.getRight(node), bdepth));
            }

            if (m_ops.isNullNode(m_ops.getLeft(node)) || m_ops.isNullNode(m_ops.getRight(node))) {
                is_end = true;
            }

            if (m_ops.isNullNode(m_ops.getLeft(node)) && m_ops.isNullNode(m_ops.getRight(node))) {
                assert(bdepth == black_depth);
            }
        }
    }

    inline NODE deleteNode(NODE& root, NodePath nodePath) {
        assert(!m_ops.isNullNode(root));
        NODE extra_black = m_ops.getNullNode();
        NODE extra_parent = m_ops.getNullNode();
        bool extra_is_left_child = true;
        auto node = this->GetNodeAncestor(nodePath, 0);

        if (!m_ops.isNullNode(m_ops.getLeft(node)) && !m_ops.isNullNode(m_ops.getRight(node)))
        {
            NodePath successor = nodePath;
            auto nn2 = m_ops.getRight(node);
            this->NodePathPush(successor, nn2);
            for (;!m_ops.isNullNode(m_ops.getLeft(nn2));) {
                nn2 = m_ops.getLeft(nn2);
                this->NodePathPush(successor, nn2);
            }

            if (m_ops.nodeCompareEqual(root, node)) {
                root = nn2;
            }

            this->SwapAncestorAndDescendant(nodePath, successor);
            if constexpr (!parents_ops) {
                std::swap(nodePath, successor);
            }
        }

        const auto beLeft = [&](NODE x1, NODE x2) {
            m_ops.setLeft(x1, x2);
            if constexpr (parents_ops) {
                if (!m_ops.isNullNode(x2)) {
                    m_ops.setParent(x2, x1);
                }
            }
        };
        const auto beRight = [&](NODE x1, NODE x2) {
            m_ops.setRight(x1, x2);
            if constexpr (parents_ops) {
                if (!m_ops.isNullNode(x2)) {
                    m_ops.setParent(x2, x1);
                }
            }
        };

        const auto node_parent = GetNodeAncestor(nodePath, 1);
        auto extra_parent_path = std::move(nodePath);
        this->NodePathPop(extra_parent_path);
        bool need_extra_black = m_ops.isBlack(node) && !m_ops.isNullNode(node_parent);
        if (m_ops.isNullNode(m_ops.getLeft(node))) {
            if (!m_ops.isNullNode(m_ops.getRight(node))) {
                assert(!m_ops.isBlack(m_ops.getRight(node)));
                need_extra_black = false;
                m_ops.setBlack(m_ops.getRight(node), true);
            }

            if (!m_ops.isNullNode(node_parent)) {
                if (m_ops.nodeCompareEqual(m_ops.getLeft(node_parent), node)) {
                    beLeft(node_parent, m_ops.getRight(node));
                } else {
                    assert(m_ops.nodeCompareEqual(m_ops.getRight(node_parent), node));
                    beRight(node_parent, m_ops.getRight(node));
                    extra_is_left_child = false;
                }
            } else {
                assert(m_ops.nodeCompareEqual(node, root));
                root = m_ops.getRight(node);
                if constexpr (parents_ops) {
                    if (!m_ops.isNullNode(root))
                        m_ops.setParent(root, m_ops.getNullNode());
                }
            }
            extra_parent = need_extra_black ? node_parent : m_ops.getNullNode();
            extra_black = need_extra_black ? m_ops.getRight(node) : m_ops.getNullNode();
        } else {
            assert(m_ops.isNullNode(m_ops.getRight(node)));;
            assert(!m_ops.isBlack(m_ops.getLeft(node)));
            assert(m_ops.isBlack(node));
            need_extra_black = false;
            m_ops.setBlack(m_ops.getLeft(node), true);

            if (!m_ops.isNullNode(node_parent)) {
                if (m_ops.nodeCompareEqual(m_ops.getLeft(node_parent), node)) {
                    beLeft(node_parent, m_ops.getLeft(node));
                } else {
                    assert(m_ops.nodeCompareEqual(m_ops.getRight(node_parent), node));
                    beRight(node_parent, m_ops.getLeft(node));
                }
            } else {
                assert(m_ops.nodeCompareEqual(node, root));
                root = m_ops.getLeft(node);
                if constexpr (parents_ops) {
                    if (!m_ops.isNullNode(root))
                        m_ops.setParent(root, m_ops.getNullNode());
                }
            }
        }

        m_ops.setLeft(node, m_ops.getNullNode());
        m_ops.setRight(node, m_ops.getNullNode());
        if constexpr (parents_ops) {
            m_ops.setParent(node, m_ops.getNullNode());
        }
        const auto ans = node;

        if (m_ops.isNullNode(root)) {
            return ans;
        }

        if constexpr (parents_ops) {
            assert(m_ops.isNullNode(m_ops.getParent(root)));
        }

        if (!need_extra_black) {
            return ans;
        }

        for (;m_ops.isBlackOrNULL(extra_black) && !m_ops.nodeCompareEqual(extra_black, root);)
        {
            assert(!m_ops.isNullNode(extra_parent));
            
            if (extra_is_left_child) {
                auto sibling = m_ops.getRight(extra_parent);
                assert(!m_ops.isNullNode(sibling));

                if (!m_ops.isBlack(sibling)) {
                    assert(m_ops.isBlack(extra_parent));

                    if (m_ops.isNullNode(this->GetNodeAncestor(extra_parent_path,1))) {
                        assert(m_ops.nodeCompareEqual(extra_parent, root));
                        root = sibling;
                    }

                    this->NodePathPush(extra_parent_path, sibling);
                    this->nodeLeftRotate(extra_parent_path);
                    const auto siblingx = this->GetNodeAncestor(extra_parent_path, 0);
                    assert(m_ops.nodeCompareEqual(sibling, siblingx));
                    m_ops.setBlack(sibling, true);
                    m_ops.setBlack(extra_parent, false);

                    extra_parent = m_ops.getLeft(sibling);
                    sibling = m_ops.getRight(extra_parent);
                    this->NodePathPush(extra_parent_path, extra_parent);
                }

                assert(m_ops.isBlack(sibling));
                if (m_ops.isBlackOrNULL(m_ops.getLeft(sibling)) && m_ops.isBlackOrNULL(m_ops.getRight(sibling))) {
                    m_ops.setBlack(sibling, false);
                    extra_black = extra_parent;
                    extra_parent = this->GetNodeAncestor(extra_parent_path, 1);
                    this->NodePathPop(extra_parent_path);
                    extra_is_left_child = !m_ops.isNullNode(extra_parent) ? m_ops.nodeCompareEqual(m_ops.getLeft(extra_parent), extra_black) : false;
                    if (m_ops.isNullNode(extra_parent)) {
                        m_ops.nodeCompareEqual(extra_black, root);
                    }
                } else {
                    if (m_ops.isBlackOrNULL(m_ops.getRight(sibling))) {
                        assert(!m_ops.isBlackOrNULL(m_ops.getLeft(sibling)));
                        auto sl = m_ops.getLeft(sibling);
                        auto sl_right = m_ops.getRight(sl);
                        beLeft(sibling, sl_right);
                        beRight(sl, sibling);
                        beRight(extra_parent, sl);

                        m_ops.setBlack(sl, true);
                        m_ops.setBlack(sibling, false);
                        sibling = sl;
                    }

                    const auto pp = this->GetNodeAncestor(extra_parent_path, 1);
                    bool pp_left = !m_ops.isNullNode(pp) && m_ops.nodeCompareEqual(m_ops.getLeft(pp), extra_parent);

                    auto extra_parent_color = m_ops.isBlack(extra_parent);

                    if (m_ops.isNullNode(pp)) {
                        assert(m_ops.nodeCompareEqual(extra_parent, root));
                        assert(m_ops.isBlack(extra_parent));
                        root = sibling;
                    }

                    assert(!m_ops.isBlackOrNULL(m_ops.getRight(sibling)));
                    auto sl = m_ops.getLeft(sibling), sr = m_ops.getRight(sibling);

                    beRight(extra_parent, sl);
                    beLeft(sibling, extra_parent);

                    m_ops.setBlack(sibling, extra_parent_color);
                    m_ops.setBlack(extra_parent, true);
                    m_ops.setBlack(sr, true);

                    if (!m_ops.isNullNode(pp)) {
                        if (pp_left) {
                            beLeft(pp, sibling);
                        } else {
                            beRight(pp, sibling);
                        }
                    } else {
                        if constexpr (parents_ops) {
                            m_ops.setParent(sibling, pp);
                        }
                    }
                    extra_black = root;
                    extra_parent = m_ops.getNullNode();
                    if constexpr (parents_ops) {
                        extra_parent_path = extra_parent;
                    } else {
                        extra_parent_path.clear();
                    }
                }
            } else {
                auto sibling = m_ops.getLeft(extra_parent);
                assert(!m_ops.isNullNode(sibling));

                if (!m_ops.isBlack(sibling)) {
                    assert(m_ops.isBlack(extra_parent));

                    if (m_ops.isNullNode(this->GetNodeAncestor(extra_parent_path,1))) {
                        assert(m_ops.nodeCompareEqual(extra_parent, root));
                        root = sibling;
                    }

                    this->NodePathPush(extra_parent_path, sibling);
                    this->nodeRightRotate(extra_parent_path);
                    const auto siblingx = this->GetNodeAncestor(extra_parent_path, 0);
                    assert(m_ops.nodeCompareEqual(sibling, siblingx));
                    m_ops.setBlack(sibling, true);
                    m_ops.setBlack(extra_parent, false);

                    extra_parent = m_ops.getRight(sibling);
                    sibling = m_ops.getLeft(extra_parent);
                    this->NodePathPush(extra_parent_path, extra_parent);
                }

                assert(m_ops.isBlack(sibling));
                if (m_ops.isBlackOrNULL(m_ops.getRight(sibling)) && m_ops.isBlackOrNULL(m_ops.getLeft(sibling))) {
                    m_ops.setBlack(sibling, false);
                    extra_black = extra_parent;
                    extra_parent = this->GetNodeAncestor(extra_parent_path, 1);
                    this->NodePathPop(extra_parent_path);
                    extra_is_left_child = !m_ops.isNullNode(extra_parent) ? m_ops.nodeCompareEqual(m_ops.getLeft(extra_parent), extra_black) : false;
                    if (m_ops.isNullNode(extra_parent)) {
                        m_ops.nodeCompareEqual(extra_black, root);
                    }
                } else {
                    if (m_ops.isBlackOrNULL(m_ops.getLeft(sibling))) {
                        assert(!m_ops.isBlackOrNULL(m_ops.getRight(sibling)));
                        auto sl = m_ops.getRight(sibling);
                        auto sl_Left = m_ops.getLeft(sl);
                        beRight(sibling, sl_Left);
                        beLeft(sl, sibling);
                        beLeft(extra_parent, sl);

                        m_ops.setBlack(sl, true);
                        m_ops.setBlack(sibling, false);
                        sibling = sl;
                    }

                    const auto pp = this->GetNodeAncestor(extra_parent_path, 1);
                    bool pp_right = !m_ops.isNullNode(pp) && m_ops.nodeCompareEqual(m_ops.getRight(pp), extra_parent);

                    auto extra_parent_color = m_ops.isBlack(extra_parent);

                    if (m_ops.isNullNode(pp)) {
                        assert(m_ops.nodeCompareEqual(extra_parent, root));
                        assert(m_ops.isBlack(extra_parent));
                        root = sibling;
                    }

                    assert(!m_ops.isBlackOrNULL(m_ops.getLeft(sibling)));
                    auto sr = m_ops.getRight(sibling), sl = m_ops.getLeft(sibling);

                    beLeft(extra_parent, sr);
                    beRight(sibling, extra_parent);

                    m_ops.setBlack(sibling, extra_parent_color);
                    m_ops.setBlack(extra_parent, true);
                    m_ops.setBlack(sl, true);

                    if (!m_ops.isNullNode(pp)) {
                        if (pp_right) {
                            beRight(pp, sibling);
                        } else {
                            beLeft(pp, sibling);
                        }
                    } else {
                        if constexpr (parents_ops) {
                            m_ops.setParent(sibling, pp);
                        }
                    }
                    extra_black = root;
                    extra_parent = m_ops.getNullNode();
                    if constexpr (parents_ops) {
                        extra_parent_path = extra_parent;
                    } else {
                        extra_parent_path.clear();
                    }
                }
            }
        }

        assert(!m_ops.isNullNode(extra_black));
        assert(!m_ops.isBlack(extra_black) || m_ops.nodeCompareEqual(extra_black, root));
        m_ops.setBlack(extra_black, true);
        return ans;
    }

    inline NodePath findNode(NODE root, KEY key) const {
        NodePath ans = this->InitPath<NodePath>();
        auto cn = root;

        for (;!m_ops.isNullNode(cn);) {
            this->NodePathPush(ans, cn);

            if (m_ops.keyCompareEqual(key, m_ops.getKey(cn)))
                return ans;

            if (m_ops.keyCompareLess(key, m_ops.getKey(cn))) {
                cn = m_ops.getLeft(cn);
            } else {
                cn = m_ops.getRight(cn);
            }
        }

        return this->InitPath<NodePath>();
    }

    inline NodePath lower_bound(NODE root, const KEY& val) {
        NodePath ans = this->InitPath<NodePath>();
        if (m_ops.isNullNode(root)) return ans;

        NodePath current_path = this->InitPath<NodePath>();
        for (auto node=root;!m_ops.isNullNode(node);) {
            this->NodePathPush(current_path, node);

            if (!m_ops.keyCompareLess(m_ops.getKey(node), val)) {
                ans = current_path;
                node = m_ops.getLeft(node);
            } else {
                node = m_ops.getRight(node);
            }
        }

        return ans;
    }

    inline NodePath upper_bound(NODE root, const KEY& val) {
        NodePath ans = this->InitPath<NodePath>();
        if (m_ops.isNullNode(root)) return ans;

        NodePath current_path = this->InitPath<NodePath>();
        for (auto node=root;!m_ops.isNullNode(node);) {
            this->NodePathPush(current_path, node);

            if (m_ops.keyCompareLess(val, m_ops.getKey(node))) {
                ans = current_path;
                node = m_ops.getLeft(node);
            } else {
                node = m_ops.getRight(node);
            }
        }

        return ans;
    }

    inline NodePath begin(NODE root) const {
        NodePath ans = this->InitPath<NodePath>();
        if (m_ops.isNullNode(root)) return ans;

        for (auto node=root;!m_ops.isNullNode(node);node=m_ops.getLeft(node)) {
            this->NodePathPush(ans, node);
        }

        return ans;
    }

    inline NodePath end(NODE root) {
        return this->InitPath<NodePath>();
    }

    inline NODE getNode(NodePath& path) const {
        return this->GetNodeAncestor(path, 0);
    }

    inline bool exists(const NodePath& path) const {
        if constexpr (parents_ops) {
            return !m_ops.isNullNode(path);
        } else {
            return !path.empty();
        }
    }

    inline void forward(NODE, NodePath& path) const {
        assert(this->exists(path));

        auto node = this->GetNodeAncestor(path, 0);
        if (!m_ops.isNullNode(m_ops.getRight(node))) {
            for (node = m_ops.getRight(node);!m_ops.isNullNode(node);node = m_ops.getLeft(node)) {
                this->NodePathPush(path, node);
            }
        } else {
            for (;!m_ops.isNullNode(node);node=this->GetNodeAncestor(path,0)) {
                if (!m_ops.isNullNode(this->GetNodeAncestor(path, 1)) &&
                    m_ops.nodeCompareEqual(m_ops.getLeft(this->GetNodeAncestor(path, 1)), node))
                {
                    this->NodePathPop(path);
                    break;
                }

                this->NodePathPop(path);
            }
        }
    }

    inline void backward(NODE root, NodePath& path) const {
        if (!this->exists(path)) {
            path = this->last(root);
            return;
        }

        auto node = this->GetNodeAncestor(path, 0);
        if (!m_ops.isNullNode(m_ops.getLeft(node))) {
            for (node = m_ops.getLeft(node);!m_ops.isNullNode(node);node = m_ops.getRight(node)) {
                this->NodePathPush(path, node);
            }
        } else {
            for (;!m_ops.isNullNode(node);node=this->GetNodeAncestor(path,0)) {
                if (!m_ops.isNullNode(this->GetNodeAncestor(path, 1)) &&
                    m_ops.nodeCompareEqual(m_ops.getRight(this->GetNodeAncestor(path, 1)), node))
                {
                    this->NodePathPop(path);
                    break;
                }

                this->NodePathPop(path);
            }
        }
    }

    inline void getNode(const NodePath& path) const {
        assert(this->exists(path));
        return this->GetNodeAncestor(path, 0);
    }

    inline bool keyCompareLess(const KEY& k1, const KEY& k2) const {
        return m_ops.keyCompareLess(k1, k2);
    }

    template<typename U, std::enable_if_t<std::is_same_v<NODE,decltype(std::declval<U&>()())>,bool> = true>
    inline NODE initWithAscSequence(size_t size, U iterFunc) {
        assert(size > 0);
        auto depth = 1;
        while ((1 << depth) - 1 < size) depth++;
        size_t m = 0;
        const std::function<NODE(size_t,size_t)> initNode = [&](size_t d, size_t s) {
            const auto n1 = (s - 1) / 2;
            const auto n2 = n1 + (s - 1) % 2;

            NODE left  = m_ops.getNullNode();
            if (n1 > 0) left  = initNode(d+1,n1);

            NODE ans = iterFunc();

            NODE right = m_ops.getNullNode();
            if (n2 > 0) right = initNode(d+1,n2);

            if (d == depth && depth > 1) {
                m_ops.setBlack(ans, false);
            } else {
                m_ops.setBlack(ans, true);
            }

            m_ops.setLeft(ans, left);
            m_ops.setRight(ans, right);
            if constexpr (parents_ops) {
                if (!m_ops.isNullNode(left)) m_ops.setParent(left, ans);
                if (!m_ops.isNullNode(right)) m_ops.setParent(right, ans);
            }
            return ans;
        };
        return initNode(1, size);
    }

    template<typename U>
    inline void releaseNode(NODE node, U rel) {
        NodePath ph = this->InitPath<NodePath>();
        this->NodePathPush(ph, node);
        for(;this->exists(ph);) {
            const auto n = this->GetNodeAncestor(ph, 0);
            if (!m_ops.isNullNode(m_ops.getLeft(n))) {
                this->NodePathPush(ph, m_ops.getLeft(n));
            } else if (!m_ops.isNullNode(m_ops.getRight(n))) {
                this->NodePathPush(ph, m_ops.getRight(n));
            } else {
                this->NodePathPop(ph);
                const auto p = this->GetNodeAncestor(ph, 0);
                if (!m_ops.isNullNode(p)) {
                    if (m_ops.nodeCompareEqual(n, m_ops.getLeft(p))) {
                        m_ops.setLeft(p, m_ops.getNullNode());
                    } else {
                        assert(m_ops.nodeCompareEqual(n, m_ops.getRight(p)));
                        m_ops.setRight(p, m_ops.getNullNode());
                    }
                }
                rel(n);
            }
        }
    }

protected:
    RbtreeOpWrapper<T,NODE,KEY> m_ops;
};
}


#include "./unarray.h"
namespace ldc::RBTreeBasicContainerImpl {
template<typename _Key, typename _Value, bool parentsOps>
struct TreeNode {
    using KVPair = std::conditional_t<std::is_same_v<_Value,void>,_Key,std::pair<const _Key,_Value>>;
    std::conditional_t<parentsOps,TreeNode*,dummy_struct> m_parent;

    TreeNode *m_left, *m_right;
    bool     m_black;
    KVPair   m_value;

    template<typename TK>
    inline explicit TreeNode(TK&& val):
        m_left(nullptr), m_right(nullptr), m_black(false), m_value(std::forward<TK>(val))
    {
        if constexpr (parentsOps) {
            m_parent = nullptr;
        }
    }

    ~TreeNode() = default;
};

template<typename _Key, typename _Value, typename _CmpLess, typename _Allocator,
         bool parentsOps>
struct TreeNodeOps {
    using TNODE  = TreeNode<_Key,_Value,parentsOps>;
    using KVPair = typename TNODE::KVPair;
    using _Node  = TNODE*;
    _CmpLess           m_cmp;

    explicit TreeNodeOps(const _CmpLess& cmp): m_cmp(cmp) {}

    _Node getLeft (_Node n) const { return n->m_left; }
    _Node getRight(_Node n) const { return n->m_right; }

    void setLeft (_Node n, _Node l) const { n->m_left = l; }
    void setRight(_Node n, _Node r) const { n->m_right = r; }

    bool isBlack(_Node n) const { return n->m_black; }
    void setBlack(_Node n, bool black) { n->m_black = black; }

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

    inline bool isNullNode(_Node node) const { return node == nullptr; }
    inline _Node getNullNode() const { return nullptr; }

    inline _Key getKey(_Node& n) const {
        if constexpr (std::is_same_v<_Value,void>) {
            return n->m_value;
        } else {
            return n->m_value.first;
        }
    }

    inline bool keyCompareLess(const _Key& lhs, const _Key& rhs) const { return m_cmp(lhs, rhs); }

    inline bool nodeCompareEqual(_Node lhs, _Node rhs) const { return lhs == rhs; }
};

template<typename _Key, typename _Value, typename _CmpLess, typename _Allocator,
         bool parentsOps, bool multikey>
using RBTreeAlgo = RBTreeAlgorithmImpl::RBTreeAlgorithm<
                         TreeNodeOps<_Key,_Value,_CmpLess,_Allocator,parentsOps>,
                         TreeNode<_Key,_Value,parentsOps>*,
                         _Key, multikey, parentsOps>;

template<typename _Key, typename _Value, typename _CmpLess, typename _Allocator,
         bool _parentsOps, bool _multikey>
struct RBTreeInMemory: protected RBTreeAlgo<_Key,_Value,_CmpLess,_Allocator,_parentsOps,_multikey> {
    using BASE      = RBTreeAlgo<_Key,_Value,_CmpLess,_Allocator,_parentsOps,_multikey>;
    using treeops_t = TreeNodeOps<_Key,_Value,_CmpLess,_Allocator,_parentsOps>;
    using TNODE     = typename treeops_t::TNODE;
    using _Node     = typename treeops_t::_Node;
    using KVPair    = typename treeops_t::KVPair;
    using ITERATOR  = typename BASE::NodePath;
    static constexpr auto  ref_accessor = true;


private:
    _Node  m_root;
    size_t m_size;
    using _storage_allocator = typename std::allocator_traits<_Allocator>::template rebind_alloc<TNODE>;
    _storage_allocator m_allocator;
    inline _Node createEmptyNode(KVPair&& pair) {
        auto ptr = m_allocator.allocate(1);
        return new (ptr) TNODE(std::move(pair));
    }
    inline void releaseEmptyNode(_Node&& node) {
        std::destroy_at(node);
        m_allocator.deallocate(node, 1);
    }

public:
    inline RBTreeInMemory(const _CmpLess& cmp, const _Allocator& alloc): 
        BASE(treeops_t(cmp)), m_allocator(alloc),
        m_root(nullptr), m_size(0) {}

    inline RBTreeInMemory(const RBTreeInMemory& _oth):
        BASE(treeops_t(_oth.m_ops.ops().m_cmp)), m_allocator(_oth.m_allocator),
        m_root(nullptr), m_size(0)
    {
        *this = _oth;
    }

    inline RBTreeInMemory(RBTreeInMemory&& _oth):
        BASE(std::move(_oth)), m_root(_oth.m_root), m_size(_oth.m_size)
    {
        _oth.m_size = 0;
        _oth.m_root = nullptr;
    }

    inline RBTreeInMemory& operator=(const RBTreeInMemory& _oth) {
        this->clear();
        if (_oth.size() > 0) {
            auto& oth = const_cast<RBTreeInMemory&>(_oth);
            auto b = oth.begin();
            const auto fn = [&]() {
                auto val = oth.getHolder(b);
                oth.forward(b);
                auto node = this->createEmptyNode(std::move(val));
                return node;
            };
            m_root = this->initWithAscSequence(oth.size(), fn);
            m_size = oth.size();
        }
        return *this;
    }

    inline RBTreeInMemory& operator=(RBTreeInMemory&& _oth) {
        this->clear();
        this->m_root = _oth.m_root;
        this->m_size = _oth.m_size;
        _oth.m_root = nullptr;
        _oth.m_size = 0;
        BASE::operator=(std::move(_oth));
        return *this;
    }

    inline ITERATOR insert(KVPair&& val) {
        auto node = this->createEmptyNode(std::move(val));
        auto ans = this->insertNode(m_root, node);
        if (this->exists(ans)) {
            m_size++;
        } else {
            this->releaseEmptyNode(std::move(node));
        }
        return ans;
    }

    inline ITERATOR find(const _Key& key) {
        if (this->m_root == nullptr) return this->end();

        return this->findNode(m_root, key);
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
        auto node = this->deleteNode(this->m_root, iter);
        const auto val = std::move(node->m_value);
        this->releaseEmptyNode(std::move(node));
        m_size--;
        return val;
    }

    using BASE::exists;
    using BASE::keyCompareLess;
    inline KVPair getHolder(ITERATOR iter) const {
        const auto node = this->getNode(iter);
        return node->m_value;
    }
    inline _Key getHolderKey(ITERATOR iter) const {
        const auto node = this->getNode(iter);
        if constexpr (std::is_same_v<_Value,void>) {
            return node->m_value;
        } else {
            return node->m_value.first;
        }
    }
    inline KVPair& getHolderRef(ITERATOR iter) {
        const auto node = this->getNode(iter);
        return node->m_value;
    }
    inline void setHolderValue(ITERATOR iter, std::conditional_t<std::is_same_v<_Value,void>,dummy_struct,_Value> val) {
        if constexpr (!std::is_same_v<_Value,void>) {
            const auto node = this->getNode(iter);
            node->m_value.second = val;
        } else {
            assert(false);
        }
    }

    inline size_t size() const { return m_size; }

    inline void clear() {
        if (m_root) {
            this->releaseNode(m_root, [&](_Node n) { this->releaseEmptyNode(std::move(n)); });
            m_root = nullptr;
        }
    }

    ~RBTreeInMemory() { this->clear(); }
};
}
