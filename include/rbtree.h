#pragma once
#include "./maxsize_vector.h"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <queue>
#include <type_traits>


namespace RBTreeAlgorithmImpl {
template <typename T, typename NODE, typename KEY, bool complain=false>
struct treeop_traits {
    static NODE& node;
    static const NODE& const_node;
    static const KEY& const_key;

    template<typename U>
    static uint8_t  test_getLeft(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getLeft(node))>,bool> = true>
    static uint16_t test_getLeft(int);

    template<typename U>
    static uint8_t  test_setLeft(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setLeft(node, node))>,bool> = true>
    static uint16_t test_setLeft(int);

    template<typename U>
    static uint8_t  test_getRight(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getRight(node))>,bool> = true>
    static uint16_t test_getRight(int);

    template<typename U>
    static uint8_t  test_setRight(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setRight(node, node))>,bool> = true>
    static uint16_t test_setRight(int);

    template<typename U>
    static uint8_t  test_getParent(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getParent(node))>,bool> = true>
    static uint16_t test_getParent(int);

    template<typename U>
    static uint8_t  test_setParent(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setParent(node, node))>,bool> = true>
    static uint16_t test_setParent(int);

    template<typename U>
    static uint8_t  test_isBlack(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(static_cast<const U*>(nullptr)->isBlack(node))>,bool> = true>
    static uint16_t test_isBlack(int);

    template<typename U>
    static uint8_t  test_setBlack(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(static_cast<U*>(nullptr)->setBlack(node, *static_cast<bool*>(nullptr)))>,bool> = true>
    static uint16_t test_setBlack(int);

    template<typename U>
    static uint8_t  test_isNullNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(static_cast<const U*>(nullptr)->isNullNode(node))>,bool> = true>
    static uint16_t test_isNullNode(int);

    template<typename U>
    static uint8_t  test_getNullNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<NODE,decltype(static_cast<const U*>(nullptr)->getNullNode())>,bool> = true>
    static uint16_t test_getNullNode(int);

    template<typename U>
    static uint8_t  test_getKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<KEY,decltype(static_cast<const U*>(nullptr)->getKey(node))>,bool> = true>
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

    static constexpr bool has_getLeft          = sizeof(test_getLeft<T>(1))          == sizeof(uint16_t);
    static constexpr bool has_setLeft          = sizeof(test_setLeft<T>(1))          == sizeof(uint16_t);
    static constexpr bool has_getRight         = sizeof(test_getRight<T>(1))         == sizeof(uint16_t);
    static constexpr bool has_setRight         = sizeof(test_setRight<T>(1))         == sizeof(uint16_t);
    static constexpr bool has_getParent        = sizeof(test_getParent<T>(1))        == sizeof(uint16_t); // optional
    static constexpr bool has_setParent        = sizeof(test_setParent<T>(1))        == sizeof(uint16_t); // optional
    static constexpr bool has_isBlack          = sizeof(test_isBlack<T>(1))          == sizeof(uint16_t);
    static constexpr bool has_setBlack         = sizeof(test_setBlack<T>(1))         == sizeof(uint16_t);
    static constexpr bool has_isNullNode       = sizeof(test_isNullNode<T>(1))       == sizeof(uint16_t);
    static constexpr bool has_getNullNode      = sizeof(test_getNullNode<T>(1))      == sizeof(uint16_t);
    static constexpr bool has_getKey           = sizeof(test_getKey<T>(1))           == sizeof(uint16_t);
    static constexpr bool has_keyCompareLess   = sizeof(test_keyCompareLess<T>(1))   == sizeof(uint16_t);
    static constexpr bool has_keyCompareEqual  = sizeof(test_keyCompareEqual<T>(1))  == sizeof(uint16_t);
    static constexpr bool has_nodeCompareEqual = sizeof(test_nodeCompareEqual<T>(1)) == sizeof(uint16_t);

    static_assert(!complain || !std::is_reference_v<NODE>,       "NODE should not be a reference");
    static_assert(!complain || !std::is_const_v<NODE>,           "NODE should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<NODE>, "NODE should be copy assignable");
    static_assert(!complain || !std::is_reference_v<KEY>,        "KEY should not be a reference");
    static_assert(!complain || !std::is_const_v<KEY>,            "KEY should not be const-qualified");
    static_assert(!complain ||  has_getLeft,                     "should implement 'NODE getLeft(NODE) const;'");
    static_assert(!complain ||  has_setLeft,                     "should implement 'voie setLeft(NODE, NODE);'");
    static_assert(!complain ||  has_getRight,                    "should implement 'NODE getRight(NODE) const;'");
    static_assert(!complain ||  has_setRight,                    "should implement 'voie setRight(NODE, NODE);'");
    static_assert(!complain ||  has_isBlack,                     "should implement 'bool isBlack(NODE) const;'");
    static_assert(!complain ||  has_setBlack,                    "should implement 'voie setBlack(NODE, bool);'");
    static_assert(!complain ||  has_isNullNode,                  "should implement 'bool isNullNode(NODE) const;'");
    static_assert(!complain ||  has_getNullNode,                 "should implement 'NODE getNullNode() const;'");
    static_assert(!complain ||  has_getKey,                      "should implement 'KYE getKey(NODE) const;'");
    static_assert(!complain ||  has_keyCompareLess,              "should implement 'bool keyCompareLess(const KEY&, const KEY&) const;'");
    static_assert(!complain ||  has_nodeCompareEqual,            "should implement 'bool nodeCompareEqual(NODE, NODE) const;'");

    static constexpr bool value = !std::is_reference_v<NODE> && !std::is_const_v<NODE> && std::is_copy_assignable_v<NODE> &&
                                  !std::is_reference_v<KEY>  && !std::is_const_v<KEY> &&
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
        if constexpr (treeop_traits<T,NODE,KEY>::has_getParent) {
            return m_ops.getParent(node);
        }
        return node;
    }
    inline void setParent(NODE node, NODE n)  {
        if constexpr (treeop_traits<T,NODE,KEY>::has_setParent) {
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

private:
    using traits = treeop_traits<T,NODE,KEY>;
    T m_ops;
};

template<typename T, typename NODE, typename KEY,
         bool multikey = false, size_t static_vector_size = 128,
         std::enable_if_t<treeop_traits<T,NODE,KEY,true>::value,bool> = true>
class RBTreeAlgorithm {
public:
    using traits = treeop_traits<T,NODE,KEY>;
    static constexpr bool parents_ops = traits::has_getParent && traits::has_setParent;
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

    inline bool insertNode(NODE& root, NODE node) {
        if (m_ops.isNullNode(root)) {
            root = node;
            m_ops.setBlack(node, true);
            m_ops.setLeft(node, m_ops.getNullNode());
            m_ops.setRight(node, m_ops.getNullNode());
            if constexpr (parents_ops) {
                m_ops.setParent(node, m_ops.getNullNode());
            }
            return true;
        }

        auto cn = root;
        NodePath nodepath = this->InitPath<NodePath>();
        m_ops.setBlack(node, false);
        for(;;) {
            if constexpr (!multikey) {
                if (m_ops.keyCompareEqual(m_ops.getKey(node), m_ops.getKey(cn))) {
                    return false;
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

        if (m_ops.isBlack(cn)) {
            return true;
        }

        this->NodePathPush(nodepath, node);
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
                    node = this->GetNodeAncestor(nodepath, 0);
                } else {
                    node = p;
                    this->NodePathPop(nodepath);
                }
                assert(m_ops.nodeCompareEqual(m_ops.getLeft(pp), node));
                nodeRightRotate(nodepath);
                node = this->GetNodeAncestor(nodepath, 0);
                m_ops.setBlack(m_ops.getLeft(node), true);
            } else {
                assert(m_ops.nodeCompareEqual(m_ops.getRight(pp), p));

                if (!m_ops.nodeCompareEqual(m_ops.getRight(p), node)) {
                    assert(m_ops.nodeCompareEqual(m_ops.getLeft(p), node));
                    this->nodeRightRotate(nodepath);
                    node = this->GetNodeAncestor(nodepath, 0);
                } else {
                    node = p;
                    this->NodePathPop(nodepath);
                }
                assert(m_ops.nodeCompareEqual(m_ops.getRight(pp), node));
                nodeLeftRotate(nodepath);
                node = this->GetNodeAncestor(nodepath, 0);
                m_ops.setBlack(m_ops.getRight(node), true);
            }

            if (m_ops.nodeCompareEqual(this->GetNodeAncestor(nodepath, 1), m_ops.getNullNode())) {
                root = node;
                m_ops.setBlack(node, true);
            }
        }

        return true;
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

private:
    RbtreeOpWrapper<T,NODE,KEY> m_ops;
};
}
