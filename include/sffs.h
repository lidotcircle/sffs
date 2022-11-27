#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <ios>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include <type_traits>
#include <array>
#include <list>
#include <queue>


namespace SFFS {
using addr_t  = size_t;

class BaseException: public std::exception {};
class RuntimeError:  public BaseException {};
class OutOfRange:    public BaseException {};
class OutOfSpace:    public BaseException {};
class BadFormat:     public BaseException {};
class FileCorrupt:   public BaseException {};
class SectorTooHuge: public BaseException {};
class AlreadyExists: public BaseException {};

namespace Impl {
template <typename T>
struct device_traits {
    static addr_t v1;
    static void* v2;
    static size_t v3;
    static const void* v4;

    template<typename U>
    static uint8_t  test_read(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(static_cast<const U*>(nullptr)->read(v1,v2,v3))>,bool> = true>
    static uint16_t test_read(int);

    template<typename U>
    static uint8_t  test_write(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(static_cast<U*>(nullptr)->write(v1,v4,v3))>,bool> = true>
    static uint16_t test_write(int);

    template<typename U>
    static uint8_t  test_flush(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,  decltype(static_cast<U*>(nullptr)->flush())>,bool> = true>
    static uint16_t test_flush(int);

    template<typename U>
    static uint8_t  test_noflush(...);
    template<typename U,std::enable_if_t<!std::is_same_v<void,decltype(&U::flush)>,bool> = true>
    static uint16_t test_noflush(int);

    template<typename U>
    static uint8_t  test_max_size(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(static_cast<const U*>(nullptr)->max_size())>,bool> = true>
    static uint16_t test_max_size(int);

    static constexpr bool has_read     = sizeof(test_read<T>(1))     == sizeof(uint16_t);
    static constexpr bool has_write    = sizeof(test_write<T>(1))    == sizeof(uint16_t);
    static constexpr bool has_flush    = sizeof(test_flush<T>(1))    == sizeof(uint16_t);
    static constexpr bool has_max_size = sizeof(test_max_size<T>(1)) == sizeof(uint16_t);
    static constexpr bool no_flush     = sizeof(test_noflush<T>(1))  == sizeof(uint8_t );
};

template <typename T>
struct is_block_device {
    using traits = device_traits<T>;

    static constexpr bool value = traits::has_read && traits::has_write && traits::has_max_size && (traits::has_flush || traits::no_flush);
};
}

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
    template<typename U,std::enable_if_t<std::is_same_v<const KEY&,decltype(static_cast<const U*>(nullptr)->getKey(node))>,bool> = true>
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
    static_assert(!complain ||  has_getKey,                      "should implement 'KYE& getKey(NODE) const;'");
    static_assert(!complain ||  has_keyCompareLess,              "should implement 'bool keyCompareLess(const KEY&, const KEY&) const;'");
    static_assert(!complain ||  has_keyCompareEqual,             "should implement 'bool keyCompareEqual(const KEY&, const KEY&) const;'");
    static_assert(!complain ||  has_nodeCompareEqual,            "should implement 'bool nodeCompareEqual(NODE, NODE) const;'");

    static constexpr bool value = !std::is_reference_v<NODE> && !std::is_const_v<NODE> && std::is_copy_assignable_v<NODE> &&
                                  !std::is_reference_v<KEY>  && !std::is_const_v<KEY> &&
                                  has_getLeft    && has_setLeft  &&
                                  has_getRight   && has_setRight &&
                                  has_isBlack    && has_setBlack &&
                                  has_isNullNode && has_getNullNode &&
                                  has_getKey && has_keyCompareLess && has_keyCompareEqual &&
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

    bool isBlack(NODE node) const { return m_ops.isBlack(node); }
    bool isBlackOrNULL(NODE node) const { return m_ops.isNullNode(node) || m_ops.isBlack(node); }
    void setBlack(NODE node, bool black) { m_ops.setBlack(node, black); }

    bool isNullNode(NODE node) const { return m_ops.isNullNode(node); }
    NODE getNullNode() const { return m_ops.getNullNode(); }

    const KEY& getKey(NODE n) const { return m_ops.getKey(n); }

    bool keyCompareLess(const KEY& lhs, const KEY& rhs) const { return m_ops.keyCompareLess(lhs, rhs); }
    bool keyCompareLess(NODE lhs, NODE rhs) const { return m_ops.keyCompareLess(m_ops.getKey(lhs), m_ops.getKey(rhs)); }

    bool keyCompareEqual(const KEY& lhs, const KEY& rhs) const {
        return m_ops.keyCompareEqual(lhs, rhs);
    }

    bool nodeCompareEqual(NODE lhs, NODE rhs) const {
        if constexpr (treeop_traits<T,NODE,KEY>::has_nodeCompareEqual) {
            return m_ops.nodeCompareEqual(lhs, rhs);
        } else {
            return lhs == rhs;
        }
    }

private:
    T m_ops;
};

template<typename T, typename NODE, typename KEY,
         std::enable_if_t<treeop_traits<T,NODE,KEY,true>::value,bool> = true>
class RBTreeAlgorithm {
private:
    using traits = treeop_traits<T,NODE,KEY>;
    static constexpr bool parents_ops = traits::has_getParent && traits::has_setParent;
    using NodePath = std::conditional_t<parents_ops,NODE,std::vector<NODE>>;

    template<typename N,std::enable_if_t<std::is_same_v<N,NODE>,bool> = true>
    inline NodePath InitPath() const {
        return m_ops.getNullNode();
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,NODE>,bool> = true>
    inline NodePath InitPath() const {
        return std::vector<NODE>();
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
        assert(np.size() > 0);
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
            if (m_ops.keyCompareEqual(m_ops.getKey(node), m_ops.getKey(cn))) {
                return false;
            }

            if constexpr (parents_ops) {
                nodepath = cn;
            } else {
                nodepath.push_back(cn);
            }

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

        if constexpr (parents_ops) {
            nodepath = node;
        } else {
            nodepath.push_back(node);
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
                    node = this->GetNodeAncestor(nodepath, 0);
                } else {
                    node = p;
                    if constexpr (!parents_ops) {
                        nodepath.pop_back();
                    } else {
                        nodepath = node;
                    }
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
                    if constexpr (!parents_ops) {
                        nodepath.pop_back();
                    } else {
                        nodepath = node;
                    }
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
            if constexpr (parents_ops) {
                successor = nn2;
            } else {
                successor.push_back(nn2);
            }
            for (;!m_ops.isNullNode(m_ops.getLeft(nn2));) {
                nn2 = m_ops.getLeft(nn2);
                if constexpr (parents_ops) {
                    successor = nn2;
                } else {
                    successor.push_back(nn2);
                }
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
        if constexpr (parents_ops) {
            extra_parent_path = m_ops.getParent(extra_parent_path);
        } else {
            extra_parent_path.pop_back();
        }
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

                    if constexpr (parents_ops) {
                        extra_parent_path = sibling;
                    } else {
                        extra_parent_path.push_back(sibling);
                    }
                    this->nodeLeftRotate(extra_parent_path);
                    const auto siblingx = this->GetNodeAncestor(extra_parent_path, 0);
                    assert(m_ops.nodeCompareEqual(sibling, siblingx));
                    m_ops.setBlack(sibling, true);
                    m_ops.setBlack(extra_parent, false);

                    extra_parent = m_ops.getLeft(sibling);
                    sibling = m_ops.getRight(extra_parent);
                    if constexpr (parents_ops) {
                        extra_parent_path = extra_parent;
                    } else {
                        extra_parent_path.push_back(extra_parent);
                    }
                }

                assert(m_ops.isBlack(sibling));
                if (m_ops.isBlackOrNULL(m_ops.getLeft(sibling)) && m_ops.isBlackOrNULL(m_ops.getRight(sibling))) {
                    m_ops.setBlack(sibling, false);
                    extra_black = extra_parent;
                    extra_parent = this->GetNodeAncestor(extra_parent_path, 1);
                    if constexpr (parents_ops) {
                        extra_parent_path = extra_parent;
                    } else {
                        extra_parent_path.pop_back();
                    }
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

                    if constexpr (parents_ops) {
                        extra_parent_path = sibling;
                    } else {
                        extra_parent_path.push_back(sibling);
                    }
                    this->nodeRightRotate(extra_parent_path);
                    const auto siblingx = this->GetNodeAncestor(extra_parent_path, 0);
                    assert(m_ops.nodeCompareEqual(sibling, siblingx));
                    m_ops.setBlack(sibling, true);
                    m_ops.setBlack(extra_parent, false);

                    extra_parent = m_ops.getRight(sibling);
                    sibling = m_ops.getLeft(extra_parent);
                    if constexpr (parents_ops) {
                        extra_parent_path = extra_parent;
                    } else {
                        extra_parent_path.push_back(extra_parent);
                    }
                }

                assert(m_ops.isBlack(sibling));
                if (m_ops.isBlackOrNULL(m_ops.getRight(sibling)) && m_ops.isBlackOrNULL(m_ops.getLeft(sibling))) {
                    m_ops.setBlack(sibling, false);
                    extra_black = extra_parent;
                    extra_parent = this->GetNodeAncestor(extra_parent_path, 1);
                    if constexpr (parents_ops) {
                        extra_parent_path = extra_parent;
                    } else {
                        extra_parent_path.pop_back();
                    }
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

    inline std::optional<NodePath> findNode(NODE root, KEY key) const {
        NodePath ans = this->InitPath<NodePath>();
        auto cn = root;

        for (;!m_ops.isNullNode(cn);) {
            if constexpr (!parents_ops) {
                ans.push_back(cn);
            }

            if (m_ops.keyCompareEqual(key, m_ops.getKey(cn))) {
                if constexpr (parents_ops) {
                    ans = cn;
                }

                return ans;
            }

            if (m_ops.keyCompareLess(key, m_ops.getKey(cn))) {
                cn = m_ops.getLeft(cn);
            } else {
                cn = m_ops.getRight(cn);
            }
        }

        return std::nullopt;
    }

    inline NODE getNode(NodePath& path) const {
        return this->GetNodeAncestor(path, 0);
    }

private:
    RbtreeOpWrapper<T,NODE,KEY> m_ops;
};
}

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class BlockView {
public:
    inline size_t read(addr_t addr, void* buf, size_t n) const {
        if (addr < m_offset || addr + n > m_offset + m_size) {
            throw OutOfRange();
        }

        return this->m_block.read(addr + m_offset, buf, n);
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        if (addr < m_offset || addr + n > m_offset + m_size) {
            throw OutOfRange();
        }

        return this->m_block.write(addr + m_offset, buf, n);
    }
    inline size_t max_size() const {
        return this->m_size;
    }
    inline void flush() {
        if constexpr (Impl::device_traits<T>::has_flush) {
            this->m_block.flush();
        }
    }

    inline BlockView(T& block, addr_t offset, size_t size): m_block(block), m_size(size), m_offset(offset)
    {
        if (m_offset + size > m_block.max_size()) {
            throw OutOfRange();
        }
    }

private:
    T& m_block;
    const size_t m_size;
    const addr_t m_offset;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class BlockDeviceExt {
private:
    T m_block;

    class ConstByteAccessor {
    protected:
        const BlockDeviceExt& m_wrapper;
        addr_t m_index;

    public:
        inline ConstByteAccessor(const BlockDeviceExt& wrapper, addr_t addr):
            m_wrapper(wrapper), m_index(addr) { }

        inline explicit operator char() const {
            return m_wrapper.get<char>(m_index);
        }
    };

    class ByteAccessor: public ConstByteAccessor {
    public:
        inline ByteAccessor(BlockDeviceExt& wrapper, addr_t addr):
            ConstByteAccessor(wrapper, addr) { }

        inline ByteAccessor& operator=(char byte) & {
            const_cast<BlockDeviceExt&>(this->m_wrapper).set<char>(this->m_index, byte);
            return *this;
        }

        inline ByteAccessor& operator=(char byte) && {
            const_cast<BlockDeviceExt&>(this->m_wrapper).set<char>(this->m_index, byte);
            return *this;
        }
    };


public:
    explicit inline BlockDeviceExt(T&& block): m_block(std::move(block)) {
    }

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        return this->m_block.read(addr, buf, n);
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        return this->m_block.write(addr, buf, n);
    }
    inline size_t max_size() const {
        return this->m_block.max_size();
    }
    inline void flush() {
        if constexpr (Impl::device_traits<T>::has_flush) {
            this->m_block.flush();
        }
    }

    // assume everything is little-endian FIXME
    template<typename Int, std::enable_if_t<std::is_integral<Int>::value,bool> = true>
    inline Int get(addr_t addr) const {
        Int ans;
        this->read(addr, &ans, sizeof(Int));
        return ans;
    }

    template<typename Int, std::enable_if_t<std::is_integral<Int>::value,bool> = true>
    inline void set(addr_t addr, Int val) {
        this->write(addr, &val, sizeof(Int));
    }

    inline ByteAccessor operator[](size_t idx) {
        return ByteAccessor(*this, idx);
    }

    inline ConstByteAccessor operator[](size_t idx) const {
        return ConstByteAccessor(*this, idx);
    }
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class BlockDeviceRefWrapper {
public:
    explicit inline BlockDeviceRefWrapper(T& block): m_block(block) {}

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        return this->m_block.read(addr, buf, n);
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        return this->m_block.write(addr, buf, n);
    }
    inline size_t max_size() const {
        return this->m_block.max_size();
    }
    inline void flush() {
        if constexpr (Impl::device_traits<T>::has_flush) {
            this->m_block.flush();
        }
    }

private:
    T& m_block;
};

// TODO
class CacheBlock {
public:
    explicit CacheBlock(size_t size):
        m_cache(new char[size]), m_size(size)
    {
    }

    ~CacheBlock() {
        delete[] this->m_cache;
        this->m_cache = nullptr;
    }

    size_t read (addr_t addr, void* buf, size_t n) const;
    size_t write(addr_t addr, const void* buf, size_t n);
    size_t size() const;

private:
    char*  m_cache;
    size_t m_size;
};

// TODO
template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class CachedBlockDevice {
public:
    CachedBlockDevice(T block, size_t sec_size, size_t max_sec, std::vector<addr_t> pinned);

    inline size_t read (addr_t addr, void* buf, size_t n) const {
        return 0;
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        return 0;
    }
    inline void   flush() {
    }
    inline size_t max_size() const {
        return m_internal.max_size();
    }

private:
    T m_internal;
    std::set<addr_t>            m_dirty;
    std::list<addr_t>           m_lru;
    std::set<addr_t>            m_cached;
    std::set<addr_t>            m_pinned;
    std::map<addr_t,CacheBlock> m_caches;
};

class StatInfo {
};

class OpenFileNode {
private:
    bool     m_valid;
    size_t   m_filesize;
    uint32_t m_secId;
    size_t   m_refcount;
};

class FileEntry {
private:
    std::shared_ptr<OpenFileNode> m_node;
    size_t                        m_offset;
    std::ios_base::openmode       m_mode;
};

class DirectoryEntry {
private:
    uint32_t m_entryid;
    size_t   m_version;
};

class DirectoryEntryIterator {
public:
    DirectoryEntryIterator(DirectoryEntry& entry, size_t version, std::vector<uint32_t> entryids);

private:
    DirectoryEntry& m_entry;
    std::vector<uint32_t> m_topdown_entryids;
    size_t m_version;
};

class FSPath {
private:
    std::vector<std::string> m_components;
};

using file_t = std::shared_ptr<FileEntry>;
using dir_t  = std::shared_ptr<DirectoryEntry>;

enum class ErrorCode {
    noerror = 0,
};

enum class AllocTableEntry {
    NOT_USED      = -1,
    END_OF_CHAIN  = -2,
    SAT_USED      = -3,
    MSAT_USED     = -4,
    NotApplicable = -5,
    MaxRegSector  = -6,
};
inline bool is_reg_entry(uint32_t id) {
    return id != static_cast<uint32_t>(AllocTableEntry::NOT_USED) &&
           id != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) &&
           id != static_cast<uint32_t>(AllocTableEntry::SAT_USED) &&
           id != static_cast<uint32_t>(AllocTableEntry::MSAT_USED) &&
           id != static_cast<uint32_t>(AllocTableEntry::NotApplicable) &&
           id != static_cast<uint32_t>(AllocTableEntry::MaxRegSector);
}

#define CompoundFileSignature { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 }

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class CompoundFileHeaderAccessor {
public:
    inline explicit CompoundFileHeaderAccessor(T block_ref): m_block(std::move(block_ref)) {
        if (m_block.max_size() < 512) {
            throw BadFormat();
        }

        unsigned char signature_buf[8] = { 0 };
        const unsigned char cf_signature[8] = CompoundFileSignature;
        if (m_block.read(0, signature_buf, sizeof(signature_buf)) != sizeof(signature_buf)) {
            throw RuntimeError();
        }
        for (size_t i=0;i<sizeof(signature_buf);i++) {
            if (signature_buf[i] != cf_signature[i]) {
                throw BadFormat();
            }
        }

        if (this->version() != 3 && this->version() != 4) {
            throw BadFormat();
        }

        if (this->version() != 3 && this->version() != 4) {
            throw BadFormat();
        }

        if (!this->isLittleEndian()) {
            throw BadFormat();
        }

        const auto s  = m_block.template get<uint16_t>(30);
        if (s > 25) {
            throw SectorTooHuge();
        }
        m_sizeOfSector = 1 << s;

        const auto sm = m_block.template get<uint16_t>(30);
        if (sm > 25) {
            throw SectorTooHuge();
        }
        if (sm >= s) {
            throw BadFormat();
        }
        m_sizeOfShortSector = 1 << sm;
    }

    inline uint16_t version() const {
        return m_block.template get<uint16_t>(26);
    }

    inline uint16_t revision() const {
        return m_block.template get<uint16_t>(24);
    }

    inline bool isLittleEndian() const {
        return m_block.template get<uint8_t>(28) == 0xFE && m_block.template get<uint8_t>(29) == 0xFF;
    }

    inline size_t sizeOfSector() const {
        return m_sizeOfSector;
    }

    inline size_t sizeOfShortSector() const {
        return m_sizeOfShortSector;
    }

    inline uint32_t getNumsSectorforSAT() const {
        return m_block.template get<uint32_t>(44);
    }

    inline void setNumsSectorforSAT(uint32_t val) const {
        m_block.template set<uint32_t>(44, val);
    }

    inline uint32_t getDirStreamSectorId() const {
        return m_block.template get<uint32_t>(48);
    }

    inline void setDirStreamSectorId(uint32_t val) const {
        m_block.template set<uint32_t>(48, val);
    }

    inline uint32_t getMinSizeOfStandardStream() const {
        return m_block.template get<uint32_t>(56);
    }

    inline void setMinSizeOfStandardStream(uint32_t val) const {
        m_block.template set<uint32_t>(56, val);
    }

    inline uint32_t getFirstSectorIdOfSSAT() const {
        return m_block.template get<uint32_t>(60);
    }

    inline void setFirstSectorIdOfSSAT(uint32_t val) const {
        m_block.template set<uint32_t>(60, val);
    }

    inline uint32_t getNumsSectorforSSAT() const {
        return m_block.template get<uint32_t>(64);
    }

    inline void setNumsSectorforSSAT(uint32_t val) const {
        m_block.template set<uint32_t>(64, val);
    }

    inline uint32_t getFirstSectorIdOfMSAT() const {
        return m_block.template get<uint32_t>(68);
    }

    inline void setFirstSectorIdOfMSAT(uint32_t val) const {
        m_block.template set<uint32_t>(68, val);
    }

    inline uint32_t getNumsSectorforMSAT() const {
        return m_block.template get<uint32_t>(72);
    }

    inline void setNumsSectorforMSAT(uint32_t val) const {
        m_block.template set<uint32_t>(72, val);
    }

    inline uint32_t getHeaderMSAT(uint32_t index) const {
        assert(index < 109);
        return m_block.template get<uint32_t>(76 + index * 4);
    }

    inline void setHeaderMSAT(uint32_t index, uint32_t val) const {
        assert(index < 109);
        m_block.template set<uint32_t>(76 + index * 4, val);
    }

    inline constexpr size_t headerSize() const {
        return 512;
    }

    inline static CompoundFileHeaderAccessor format(T& block, uint16_t majorVersion, uint16_t sectorShift, uint16_t shortSectorShift) {
        return CompoundFileHeaderAccessor(block, majorVersion, sectorShift, shortSectorShift);
    }

private:
    inline explicit CompoundFileHeaderAccessor(T& block, uint16_t majorVersion, uint16_t sectorShift, uint16_t shortSectorShift): m_block(block) {
        if (m_block.max_size() < 512) {
            throw BadFormat();
        }

        const char buf[512] = { 0 };
        if (m_block.write(0, buf, sizeof(buf)) != sizeof(buf)) {
            throw RuntimeError();
        }

        const unsigned char signature_buf[8] = CompoundFileSignature;
        if (m_block.write(0, signature_buf, sizeof(signature_buf)) != sizeof(signature_buf)) {
            throw RuntimeError();
        }

        if (sectorShift < shortSectorShift) {
            throw BadFormat();
        }

        if (sectorShift > 25 || shortSectorShift > 25) {
            throw BadFormat();
        }

        m_block.template set<uint16_t>(24, 0x3E);
        m_block.template set<uint16_t>(26, majorVersion);
        m_block.template set<uint8_t>(28, 0xFE);
        m_block.template set<uint8_t>(29, 0xFF);
        m_block.template set<uint16_t>(30, sectorShift);
        m_block.template set<uint16_t>(32, shortSectorShift);
        m_sizeOfSector = 1 << sectorShift;
        m_sizeOfShortSector = 1 << shortSectorShift;

        this->setDirStreamSectorId  (AllocTableEntry::END_OF_CHAIN);
        this->setFirstSectorIdOfSSAT(AllocTableEntry::END_OF_CHAIN);
        this->setFirstSectorIdOfMSAT(AllocTableEntry::END_OF_CHAIN);

        for (size_t i=0;i<109;i++) {
            this->setHeaderMSAT(i, AllocTableEntry::NOT_USED);
        }
    }

    BlockDeviceExt<T> m_block;
    size_t m_sizeOfSector, m_sizeOfShortSector;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class MasterSectorAllocationTable {
public:
    inline MasterSectorAllocationTable(CompoundFileHeaderAccessor<T>& header, T block_ref):
        m_header(header), m_block(std::move(block_ref)), m_lastSecId(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)),
        m_caches(header.getNumsSectorforMSAT() * this->entriesPerBlock() + 109, static_cast<uint32_t>(AllocTableEntry::NOT_USED))
    {
        for (size_t i=0;i<109;i++) {
            this->m_caches[i] = header.getHeaderMSAT(i);
        }

        const size_t numsOfMSATSec = header.getNumsSectorforMSAT();
        if (numsOfMSATSec > 0) {
            auto secId = header.getFirstSectorIdOfMSAT();

            size_t count = 0;
            for (;secId != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);count++){
                if (!is_reg_entry(secId)) {
                    throw FileCorrupt();
                }
                m_lastSecId = secId;

                for (size_t i=0;i<this->entriesPerBlock();i++) {
                    auto val = m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + i * sizeof(uint32_t));
                    this->m_caches[109 + count * this->entriesPerBlock() + i] = val;
                }

                secId = m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + this->entriesPerBlock() * sizeof(uint32_t));
            }

            if (count != numsOfMSATSec) {
                throw FileCorrupt();
            }
        }
    }

    inline std::optional<size_t> firstFreeEntry() const {
        for (size_t i=0;i<m_caches.size();i++) {
            if (m_caches[i] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                return i;
            }
        }

        return std::nullopt;
    }

    inline void UseEntry(size_t index, uint32_t secId) {
        assert(this->m_caches.size() > index);
        assert(this->m_caches[index] == static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        this->m_caches[index] = secId;
        if (index < 109) {
            this->m_header.setHeaderMSAT(index, secId);
        } else {
            index -= 109;
            auto secId = this->m_header.getFirstSectorIdOfMSAT();
            if (is_reg_entry(secId)) {
                throw FileCorrupt();
            }

            while (index >= this->entriesPerBlock()) {
                secId = m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + this->entriesPerBlock() * sizeof(uint32_t));
                index -= this->entriesPerBlock();

                if (is_reg_entry(secId)) {
                    throw FileCorrupt();
                }
            }

            m_block.template set<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + index * sizeof(uint32_t));
        }
    }

    inline void expand(uint32_t secId) {
        if (m_lastSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)) {
            m_header.setFirstSectorIdOfMSAT(secId);
        } else {
            m_block.template set<uint32_t>(
                    m_header.headerSize() + m_header.sizeOfSector() * m_lastSecId + this->entriesPerBlock() * sizeof(uint32_t),
                    secId);
        }

        for (size_t i=0;i<this->entriesPerBlock();i++) {
            m_block.template set<uint32_t>(
                    m_header.headerSize() + m_header.sizeOfSector() * secId + i * sizeof(uint32_t),
                    static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        }
        m_block.template set<uint32_t>(
                m_header.headerSize() + m_header.sizeOfSector() * secId + this->entriesPerBlock() * sizeof(uint32_t),
                static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));

        m_lastSecId = secId;
        m_caches.resize(m_caches.size() + this->entriesPerBlock(), AllocTableEntry::NOT_USED);
        m_header.setNumsSectorforMSAT(m_header.getNumsSectorforMSAT() + 1);
    }

    inline uint32_t get(size_t idx) const {
        assert(idx < this->m_caches.size());
        return m_caches[idx];
    }

    inline size_t size() const {
        return m_caches.size();
    }

private:
    inline uint32_t entriesPerBlock() const {
        return m_header.sizeOfSector() / sizeof(uint32_t) - 1;
    }

    std::vector<uint32_t>          m_caches;
    uint32_t                       m_lastSecId;
    CompoundFileHeaderAccessor<T>& m_header;
    BlockDeviceExt<T>              m_block;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class SectorAllocationTable {
public:
    inline SectorAllocationTable(CompoundFileHeaderAccessor<T>& header, T block_ref):
        m_block(std::move(block_ref)),
        m_header(header),
        m_msat(header, block_ref),
        m_lru_sectorIdBase(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)),
        m_sat_free_count(m_msat.size(), 0)
    {
        size_t kspa = 0;
        for (size_t i=0;i<m_sat_free_count.size();i++) {
            std::vector<uint32_t> cache;
            size_t nfree = 0;
            this->readNthSATSecIntoVec(i, cache, nfree);
            m_sat_free_count[i] = nfree;
            if (nfree > kspa) {
                m_lru_sectorCaches = std::move(cache);
                m_lru_sectorIdBase = i * this->entriesPerBlock();
                kspa = nfree;
            }
        }
    }

    inline uint32_t allocateNextSector(uint32_t preSecId) {
        if (preSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)) {
            return this->allocateSector();
        }
        const auto secId = this->allocateSector();
        this->setEntry(preSecId, secId);
        return secId;
    }

    inline void freeInterSector(uint32_t preSecId, uint32_t secId) {
        if (preSecId == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
            const auto next = this->getEntry(secId);
            assert(next == static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        } else {
            assert(is_reg_entry(secId));
            assert(this->getEntry(preSecId) == secId);
            const auto next = this->getEntry(secId);
            assert(is_reg_entry(next) || next == static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            this->setEntry(preSecId, next);
        }
        this->setEntry(secId, static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        const auto idx = secId / this->entriesPerBlock();
        assert(idx < m_sat_free_count.size());
        m_sat_free_count[idx]++;
    }

    inline std::optional<uint32_t> getNextSector(uint32_t secId) const {
        auto ans = this->getEntry(secId);

        if (!is_reg_entry(ans)) {
            assert(ans == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
            return std::nullopt;
        }

        return ans;
    }

    inline size_t size() const {
        return m_header.getNumsSectorforSAT() * this->entriesPerBlock();
    }

private:
    inline uint32_t entriesPerBlock() const {
        return m_header.sizeOfSector() / sizeof(uint32_t);
    }

    inline uint32_t getEntry(uint32_t secId) const {
        assert(secId < m_msat.size() * this->entriesPerBlock());
        const auto tbl_idx = secId / this->entriesPerBlock();
        const auto tbl_sec = m_msat.get(tbl_idx);
        const auto sec_idx = secId % this->entriesPerBlock();

        if (!m_lru_sectorCaches.empty() && m_lru_sectorIdBase == tbl_idx * this->entriesPerBlock()) {
            assert(m_lru_sectorCaches.size() > sec_idx);
            return m_lru_sectorCaches[sec_idx];
        }

        return m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * tbl_sec + sec_idx * sizeof(uint32_t));
    }

    inline void setEntry(uint32_t secId, uint32_t val) {
        assert(secId < m_msat.size() * this->entriesPerBlock());
        const auto tbl_idx = secId / this->entriesPerBlock();
        const auto tbl_sec = m_msat.get(tbl_idx);
        const auto sec_idx = secId % this->entriesPerBlock();

        if (!m_lru_sectorCaches.empty() && m_lru_sectorIdBase == tbl_idx * this->entriesPerBlock()) {
            assert(m_lru_sectorCaches.size() > sec_idx);
            m_lru_sectorCaches[sec_idx] = val;
        }

        m_block.template set<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * tbl_sec + sec_idx * sizeof(uint32_t), val);
    }

    inline std::optional<size_t> readNthSATSecIntoVec(size_t nth, std::vector<uint32_t>& cache, size_t& nfree) {
        assert(cache.empty());
        assert(nfree == 0);

        const auto sec = m_msat.get(nth);
        uint32_t buf[4096] = { 0 };
        const size_t n = this->entriesPerBlock() * sizeof(uint32_t);
        std::optional<size_t> u = std::nullopt;
        size_t offset = 0;
        while (n > offset) {
            const auto k = std::min(n - offset, sizeof(buf));
            m_block.read(m_header.headerSize() + m_header.sizeOfSector() * sec + offset, buf, k);
            offset += k;

            for (size_t j=0;j<(k/sizeof(uint32_t));j++) {
                if (buf[j] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                    nfree++;
                    u = cache.size();
                }
                cache.push_back(buf[j]);
            }
        }
        return u;
    }

    inline uint32_t allocateSector() {
        for (size_t i=0;i<m_lru_sectorCaches.size();i++) {
            if (m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                const auto ans = m_lru_sectorIdBase + i;
                this->setEntry(ans, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                assert(m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                
                this->check_limit(ans);
                const auto idx = ans / this->entriesPerBlock();
                assert(m_sat_free_count.size() > idx);
                assert(m_sat_free_count[idx] > 0);
                m_sat_free_count[idx]--;
                if (m_sat_free_count[idx] == 0) {
                    m_lru_sectorCaches.clear();
                }
                return ans;
            }
        }

        for (size_t _i=m_sat_free_count.size();_i>0;_i--) {
            const auto i = _i - 1;
            if (m_sat_free_count[i] > 0) {
                this->m_lru_sectorCaches.clear();

                size_t nfree = 0;
                const auto u_opt = this->readNthSATSecIntoVec(i, m_lru_sectorCaches, nfree);
                assert (u_opt.has_value());
                const auto u = u_opt.value();

                m_lru_sectorIdBase = i * this->entriesPerBlock();;
                const auto ans = m_lru_sectorIdBase + u;
                this->setEntry(ans, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                assert(m_lru_sectorCaches[u] == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));

                m_sat_free_count[i]--;
                if (m_sat_free_count[i] == 0) {
                    m_lru_sectorCaches.clear();
                }

                this->check_limit(ans);
                return ans;
            }
        }

        // allocate new SAT sector
        auto fsat = m_msat.firstFreeEntry();
        if (!fsat.has_value()) {
            const auto msatSecId = m_msat.size() * this->entriesPerBlock();
            this->check_limit(msatSecId + 1);
            m_msat.expand(msatSecId);

            fsat = m_msat.firstFreeEntry();
            assert(fsat.has_value());
            m_msat.UseEntry(fsat.value(), msatSecId + 1);
            m_lru_sectorCaches.resize(this->entriesPerBlock(), static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            m_lru_sectorIdBase = msatSecId;

            this->setEntry(msatSecId,     static_cast<uint32_t>(AllocTableEntry::MSAT_USED));
            this->setEntry(msatSecId + 1, static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            assert(m_lru_sectorCaches[0] == static_cast<uint32_t>(AllocTableEntry::MSAT_USED));
            assert(m_lru_sectorCaches[1] == static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            this->m_sat_free_count.push_back(this->entriesPerBlock() - 2);
        } else {
            const auto satSecId = m_msat.size() * this->entriesPerBlock();
            m_msat.UseEntry(fsat.value(), satSecId);
            m_lru_sectorCaches.resize(this->entriesPerBlock(), static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            m_lru_sectorIdBase = satSecId;

            this->setEntry(satSecId, static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            assert(m_lru_sectorCaches[0] == static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            this->m_sat_free_count.push_back(this->entriesPerBlock() - 1);
        }

        for (size_t i=0;i<m_lru_sectorCaches.size();i++) {
            if (m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                const auto ans = m_lru_sectorIdBase + i;
                this->setEntry(ans, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                assert(m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
               
                this->check_limit(ans);

                const auto idx = ans / this->entriesPerBlock();
                assert(m_sat_free_count.size() > idx);
                assert(m_sat_free_count[idx] > 1);
                return ans;
            }
        }

        throw RuntimeError();
    }

    inline void check_limit(uint32_t secId) {
        if (m_header.sizeOfSector() * (secId + 1) + m_header.sizeOfSector() > m_block.max_size()) {
            throw OutOfSpace();
        }
    }

    BlockDeviceExt<T>               m_block;
    CompoundFileHeaderAccessor<T>&  m_header;
    MasterSectorAllocationTable<T>  m_msat;

    // keep least-recently-used allocated/free sector
    uint32_t                        m_lru_sectorIdBase;
    std::vector<uint32_t>           m_lru_sectorCaches;
    std::vector<size_t>             m_sat_free_count;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class SectorChainStream {
public:
    inline SectorChainStream(CompoundFileHeaderAccessor<T>& header, SectorAllocationTable<T>& sat, T block_ref, uint32_t headSecId):
        m_header(header),
        m_sat(sat),
        m_block(block_ref),
        m_headSecId(headSecId)
    {
        assert(is_reg_entry(m_headSecId) || m_headSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
    }

    inline uint32_t getHeadSectorID() const { return m_headSecId; }

    inline void deleteStream() {
        if (m_headSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)) {
            return;
        }

        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        m_secIdChainCache.pop_back();
        for (auto secId: m_secIdChainCache) {
            assert(is_reg_entry(secId));
            m_sat.freeInterSector(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN), secId);
        }
        m_secIdChainCache.clear();
        this->m_headSecId = static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
    }

    inline void deleteLastSector() {
        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        assert(this->m_secIdChainCache().size() > 1);

        const auto prev = m_secIdChainCache.size() > 2 
                             ? m_secIdChainCache[m_secIdChainCache.size() - 3] 
                             : static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
        m_sat.freeInterSector(prev, m_secIdChainCache[m_secIdChainCache.size() - 2]);
        m_secIdChainCache.erase(m_secIdChainCache.end() - 2);
        if (m_secIdChainCache.size() == 1) {
        this->m_headSecId = static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
        }
    }

    inline addr_t appendSector() {
        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        if (m_secIdChainCache.size() > 1) {
            m_secIdChainCache.pop_back();
            const auto id = this->m_sat.allocateNextSector(m_secIdChainCache.back());
            m_secIdChainCache.push_back(id);
            m_secIdChainCache.push_back(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        } else {
            const auto id = this->m_sat.allocateNextSector(m_secIdChainCache.back());
            m_secIdChainCache.insert(m_secIdChainCache.begin(), id);
            m_headSecId = id;
        }

        return static_cast<addr_t>(this->size() - this->sectorSize());
    }

    inline size_t size() const {
        return this->prepareCacheUntil(std::numeric_limits<size_t>::max());
    }

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        const auto tlt = prepareCacheUntil(static_cast<size_t>(n + addr));
        if (tlt < static_cast<size_t>(n + addr)) {
            while (this->size() < static_cast<size_t>(n + addr)) {
                this->appendSector();
            }
        }

        const auto ss = this->sectorSize();
        int nread = 0;
        while (nread < n) {
            const auto secIdx = (addr + nread) / ss;
            const auto offset = (addr + nread) % ss;
            const auto k = std::min(n - nread, ss - offset);
            assert(m_secIdChainCache.size() > secIdx + 1);
            const auto secId = m_secIdChainCache[secIdx];
            this->m_block.read(m_header.headerSize() + secId * ss + offset, (static_cast<char*>(buf) + nread), k);
            nread += k;
        }
    }

    inline size_t write(addr_t addr, const void* buf, size_t n) {
        const auto tlt = prepareCacheUntil(static_cast<size_t>(n + addr));
        if (tlt < static_cast<size_t>(n + addr)) {
            while (this->size() < static_cast<size_t>(n + addr)) {
                this->appendSector();
            }
        }

        const auto ss = this->sectorSize();
        int nwrited = 0;
        while (nwrited < n) {
            const auto secIdx = (addr + nwrited) / ss;
            const auto offset = (addr + nwrited) % ss;
            const auto k = std::min(n - nwrited, ss - offset);
            assert(m_secIdChainCache.size() > secIdx + 1);
            const auto secId = m_secIdChainCache[secIdx];
            this->m_block.write(m_header.headerSize() + secId * ss + offset, (static_cast<const char*>(buf) + nwrited), k);
            nwrited += k;
        }
    }

    inline void fillzeros(addr_t begin, addr_t end) {
        const char buf[4096] = {0};
        while (begin < end) {
            const char s = std::min(sizeof(buf), end - begin);
            this->write(begin, buf, s);
            begin += s;
        }
    }
 
    inline size_t max_size() const {
        return m_block.max_size();
    }

    SectorChainStream(const SectorChainStream&) = delete;
    SectorChainStream& operator=(const SectorChainStream&) = delete;

private:
    inline size_t sectorSize() const { return m_header.sizeOfSector(); }

    inline size_t prepareCacheUntil(size_t limit) const {
        if (m_secIdChainCache.empty()) {
            m_secIdChainCache.push_back(m_headSecId);
        }

        while (m_secIdChainCache.back() != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) &&
               m_secIdChainCache.size() * this->sectorSize() < limit)
        {
            auto next = m_sat.getNextSector(m_secIdChainCache.back());
            if (next.has_value()) {
                m_secIdChainCache.push_back(next.value());
            } else {
                m_secIdChainCache.push_back(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
            }
        }

        const auto n = m_secIdChainCache.back() == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) ? 
                           m_secIdChainCache.size() - 1: m_secIdChainCache.size();
        return n * this->sectorSize();
    }

    CompoundFileHeaderAccessor<T>& m_header;
    SectorAllocationTable<T>& m_sat;
    BlockDeviceExt<T> m_block;
    uint32_t m_headSecId;
    mutable std::vector<uint32_t> m_secIdChainCache;
};

#define COMPOUND_FILE_ENTRY_SIZE 128
template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class DirectoryTable {
public:
    enum class EntryType {
        Empty = 0,
        UserStorage = 1,
        UserStream = 2,
        LockBytes = 3,
        Property = 4,
        RootStorage = 5,
    };

    inline DirectoryTable(CompoundFileHeaderAccessor<T>& header, SectorAllocationTable<T>& sat, T block_ref):
        m_header(header),
        m_stream(header, sat, std::move(block_ref), header.getDirStreamSectorId())
    {
        if (m_stream.size() == 0) {
            const auto addr = m_stream.appendSector();
            m_stream.fillzeros(addr, static_cast<addr_t>(m_stream.size()));
            m_header.setDirStreamSectorId(m_stream.getHeadSectorID());
            m_free_entries = m_stream.size() / COMPOUND_FILE_ENTRY_SIZE - 1;
            const std::array<uint16_t, 32> RootEntry = { 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x45, 0x6E, 0x74, 0x72, 0x79, 0x00 };
            this->setName(0, RootEntry);
            this->setEntryType(0, EntryType::RootStorage);
            this->setBlack(0, true);
        }

        m_free_entries = m_stream.size() / COMPOUND_FILE_ENTRY_SIZE;
        m_usedentries.resize(m_free_entries, false);

        std::list<uint32_t> qnq = { 0 };
        std::set<uint32_t> seen = { 0 };
        const auto append = [&] (uint32_t entry) {
            if (entry == 0xFFFFFFFF) return;

            if (seen.find(entry) != seen.end()) {
                throw FileCorrupt();
            }
            seen.insert(entry);
        };

        while (!qnq.empty()) {
            const auto entry = qnq.front();
            qnq.pop_front();

            assert(m_free_entries > 0);
            assert(m_usedentries.size() > entry);
            m_free_entries--;
            m_usedentries[entry] = true;

            append(this->getLeftChild(entry));
            append(this->getRightChild(entry));

            const auto type = this->getEntryType(entry);
            if (type == EntryType::RootStorage || type == EntryType::UserStorage) {
                append(this->getSubdirectoryEntry(entry));
            }
        }
    }

    inline uint32_t createEntry(uint32_t parentDirEntryId, const std::array<uint16_t,32>& name, EntryType type) {
        if (m_free_entries == 0) {
            const auto addr = m_stream.appendSector();
            m_stream.fillzeros(addr, static_cast<addr_t>(m_stream.size()));
            const auto n = m_header.sizeOfSector() / COMPOUND_FILE_ENTRY_SIZE;
            for (size_t i=0;i<n;i++) m_usedentries.push_back(false);
            m_free_entries += n;
        }

        const auto ptype = this->getEntryType(parentDirEntryId);
        assert(ptype == EntryType::RootStorage || ptype == EntryType::UserStorage);

        size_t id = m_usedentries.size();
        for (;id<m_usedentries.size() && m_usedentries[id];id++);

        this->setName(id, name);
        this->setEntryType(id, type);
        m_usedentries[id] = true;
        m_free_entries--;

        if (!insertIntoDirectory(parentDirEntryId, id)) {
            m_free_entries++;
            m_usedentries[id] = false;
            throw AlreadyExists();
        }

        return id;
    }

    inline void deleteEntry(uint32_t parentDirEntryId, uint32_t deleteId) {
        // TODO
        assert(deleteId > 0);
    }

    inline void fillzeros(uint32_t entryid) {
        const char buf[COMPOUND_FILE_ENTRY_SIZE] = { 0 };
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE, buf, COMPOUND_FILE_ENTRY_SIZE);
    }

    inline std::array<uint16_t,32> getName(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<uint16_t,32> ans;
        const auto n = m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setName(uint32_t entryid, const std::array<uint16_t,32>& name){
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE, name.data(), sizeof(name));
        uint16_t len = 0;
        for (;len < 32 && name[len]!=0; len++);
        len = (len + 1)* 2;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 64, &len, sizeof(len));
    }

    inline uint16_t getNameBufferLength(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint16_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 64, &ans, sizeof(ans));
        return ans;
    }

    inline EntryType getEntryType(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint8_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 66, &ans, sizeof(ans));
        return static_cast<EntryType>(ans);
    }

    inline void setEntryType(uint32_t entryid, EntryType type) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint8_t t = type;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 66, &t, sizeof(t));
    }

    inline bool isBlack(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        bool ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 67, &ans, sizeof(ans));
        return ans;
    }

    inline void setBlack(uint32_t entryid, bool isBlack) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 66, &isBlack, sizeof(isBlack));
    }

    inline uint32_t getLeftChild(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 68, &ans, sizeof(ans));
        return ans;
    }

    inline void setLeftChild(uint32_t entryid, uint32_t childId) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 68, &childId, sizeof(childId));
    }

    inline uint32_t getRightChild(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 72, &ans, sizeof(ans));
        return ans;
    }

    inline void setRightChild(uint32_t entryid, uint32_t childId) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 72, &childId, sizeof(childId));
    }

    inline uint32_t getSubdirectoryEntry(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 76, &ans, sizeof(ans));
        return ans;
    }

    inline void setSubdirectoryEntry(uint32_t entryid, uint32_t dirid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 76, &dirid, sizeof(dirid));
    }

    inline std::array<char,16> getStreamUID(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<char,16> ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 80, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setStreamUID(uint32_t entryid, const std::array<char,16>& uid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 80, uid.data(), sizeof(uid));
    }

    inline uint32_t getUserFlags(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 96, &ans, sizeof(ans));
        return ans;
    }

    inline void setUserFlags(uint32_t entryid, uint32_t flags) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 96, &flags, sizeof(flags));
    }

    inline std::array<char,8> getCreatedTimestamp(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<char,8> ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 100, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setCreatedTimestamp(uint32_t entryid, const std::array<char,8>& tstamp) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 100, tstamp.data(), sizeof(tstamp));
    }

    inline std::array<char,8> getModifiedTimestamp(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<char,8> ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 108, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setModifiedTimestamp(uint32_t entryid, const std::array<char,8>& tstamp) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 108, tstamp.data(), sizeof(tstamp));
    }

    inline uint32_t getSectorID(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 116, &ans, sizeof(ans));
        return ans;
    }

    inline void setSectorID(uint32_t entryid, uint32_t secId) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 116, &secId, sizeof(secId));
    }

    inline uint32_t getSize(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 120, &ans, sizeof(ans));
        return ans;
    }

    inline void setSize(uint32_t entryid, uint32_t size) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 120, &size, sizeof(size));
    }

private:
    inline int compare(uint32_t lhs, uint32_t rhs) const {
        const auto n1 = this->getName(lhs);
        const auto n2 = this->getName(rhs);
        size_t l1 = 0, l2 = 0;
        for (;l1<n1.size()&&n1[l1]!=0;l1++);
        for (;l2<n2.size()&&n2[l2]!=0;l2++);

        if (l1 < l2) return -1;
        if (l1 > l2) return 1;

        for (size_t i=0;i<l1;i++) {
            if (n1[i] < n2[i]) {
                return -1;
            } else if (n1[i] > n2[i]) {
                return 1;
            }
        }

        return 0;
    }

    inline bool insertIntoDirectory(uint32_t parentDir, uint32_t newentry) {
        this->setBlack(newentry, false);
        const uint32_t root = this->getSubdirectoryEntry(parentDir);
        if (root == 0xFFFFFFFF) {
            this->setSubdirectoryEntry(parentDir, newentry);
            this->setBlack(newentry, true);
            return true;
        }
        std::vector<uint32_t> ancestor = { root };
    }

    CompoundFileHeaderAccessor<T>& m_header;
    SectorChainStream<T>           m_stream;
    std::vector<bool>              m_usedentries;
    size_t                         m_free_entries;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class FileSystem {
private:
    using DeviceType = BlockDeviceExt<T>;

public:
    explicit FileSystem(T&& block):
        m_block(std::move(block)),
        m_header(BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_sat(m_header, BlockDeviceRefWrapper<DeviceType>(m_block))
    {
    }

    static FileSystem format(T&& block) {
        // TODO
    }

    inline ErrorCode get_error() const
    {
        return this->m_errcode;
    }

    bool mkdir(const FSPath& dirname);
    bool rmdir(const FSPath& dirname);

    file_t open(const FSPath& filename, std::ios_base::openmode mode);
    bool   close(file_t file);

    dir_t  opendir(const FSPath& path);
    bool   closedir(dir_t dir);

    bool unlink(const FSPath& file);
    bool rename(file_t file, const std::string& newname);
    bool rename(dir_t  dir,  const std::string& newname);
    bool move(const FSPath& path, const FSPath& newpath);

    size_t read (file_t file, void* buf, size_t n) const;
    size_t write(file_t file, const void* buf, size_t n);

    void truncate(file_t file, size_t new_size);

    StatInfo stat(file_t file);
    StatInfo stat(dir_t  dir);

    size_t seek(file_t file, long offset, int origin);

private:
    DeviceType                                                    m_block;
    CompoundFileHeaderAccessor<BlockDeviceRefWrapper<DeviceType>> m_header;
    SectorAllocationTable<BlockDeviceRefWrapper<DeviceType>>      m_sat;

    ErrorCode m_errcode;

    std::map<uint32_t,std::shared_ptr<OpenFileNode>> m_entryid2node;
    std::map<uint32_t,dir_t> m_entryid2dir;
};

class MemorySpace {
public:
    inline explicit MemorySpace(size_t size): m_space(size, 0) { }

    inline size_t read (addr_t addr, void* buf, size_t n) const {
        if (addr + n > this->max_size()) {
            throw OutOfRange();
        }

        memcpy(buf, this->m_space.data(), n);
        return n;
    }

    inline size_t write(addr_t addr, const void* buf, size_t n) {
        if (addr + n > this->max_size()) {
            throw OutOfRange();
        }

        memcpy(this->m_space.data(), buf, n);
        return n;
    }

    inline size_t max_size() const {
        return this->m_space.size();
    }

private:
    std::vector<char> m_space;
};

}
