#pragma once
#include "./maxsize_vector.h"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <utility>
#include <queue>
#include <optional>
#include <type_traits>


namespace ldc::BPTreeAlgorithmImpl {
template <typename _T, typename _Node, typename _Holder, typename _Key, typename _Value, bool complain=false>
struct treeop_traits {
    template<typename U>
    static uint8_t  test_isLeaf(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(std::declval<const U&>().isLeaf(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_isLeaf(int);

    template<typename U>
    static uint8_t  test_allowEmptyLeaf(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(U::allowEmptyLeaf())>,bool> = true>
    static uint16_t test_allowEmptyLeaf(int);

    template<typename U>
    static uint8_t  test_getNthChild(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<const U&>().getNthChild(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_getNthChild(int);

    template<typename U>
    static uint8_t  test_setNthChild(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().setNthChild(std::declval<_Node&>(), std::declval<size_t>(), std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_setNthChild(int);

    template<typename U>
    static uint8_t  test_clearNthChild(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().clearNthChild(std::declval<_Node&>(), std::declval<size_t>()))>,bool> = true>
    static uint16_t test_clearNthChild(int);

    template<typename U>
    static uint8_t  test_getParent(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<const U&>().getParent(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_getParent(int);

    template<typename U>
    static uint8_t  test_setParent(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().setParent(std::declval<_Node&>(), std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_setParent(int);

    template<typename U>
    static uint8_t  test_leafGetNext(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<const U&>().leafGetNext(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_leafGetNext(int);

    template<typename U>
    static uint8_t  test_leafSetNext(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().leafSetNext(std::declval<_Node&>(), std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_leafSetNext(int);

    template<typename U>
    static uint8_t  test_leafGetPrev(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<const U&>().leafGetPrev(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_leafGetPrev(int);

    template<typename U>
    static uint8_t  test_leafSetPrev(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().leafSetPrev(std::declval<_Node&>(), std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_leafSetPrev(int);

    template<typename U>
    static uint8_t  test_getNthHolder(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Holder,decltype(std::declval<const U&>().getNthHolder(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_getNthHolder(int);

    template<typename U>
    static uint8_t  test_getNthHolderRef(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Holder&,decltype(std::declval<U&>().getNthHolderRef(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_getNthHolderRef(int);

    template<typename U>
    static uint8_t  test_setNthHolderValue(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().setNthHolderValue(std::declval<_Node&>(),std::declval<size_t>(),std::declval<_Value>()))>,bool> = true>
    static uint16_t test_setNthHolderValue(int);

    template<typename U>
    static uint8_t  test_setHolderValue(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().setHolderValue(std::declval<_Holder&>(),std::declval<_Value>()))>,bool> = true>
    static uint16_t test_setHolderValue(int);

    template<typename U>
    static uint8_t  test_leafGetNthKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Key,decltype(std::declval<const U&>().leafGetNthKey(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_leafGetNthKey(int);

    template<typename U>
    static uint8_t  test_interiorGetNthKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Key,decltype(std::declval<const U&>().interiorGetNthKey(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_interiorGetNthKey(int);

    template<typename U>
    static uint8_t  test_interiorSetNthKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().interiorSetNthKey(std::declval<_Node&>(),std::declval<size_t>(),std::declval<const _Key&>()))>,bool> = true>
    static uint16_t test_interiorSetNthKey(int);

    template<typename U>
    static uint8_t  test_interiorClearNthKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().interiorClearNthKey(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_interiorClearNthKey(int);

    template<typename U>
    static uint8_t  test_extractNthHolder(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Holder,decltype(std::declval<U&>().extractNthHolder(std::declval<_Node&>(),std::declval<size_t>()))>,bool> = true>
    static uint16_t test_extractNthHolder(int);

    template<typename U>
    static uint8_t  test_setNthHolder(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().setNthHolder(std::declval<_Node&>(), std::declval<size_t>(), std::move(std::declval<_Holder&>())))>,bool> = true>
    static uint16_t test_setNthHolder(int);

    template<typename U>
    static uint8_t  test_leafGetOrder(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(std::declval<const U&>().leafGetOrder())>,bool> = true>
    static uint16_t test_leafGetOrder(int);

    template<typename U>
    static uint8_t  test_interiorGetOrder(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(std::declval<const U&>().interiorGetOrder())>,bool> = true>
    static uint16_t test_interiorGetOrder(int);

    template<typename U>
    static uint8_t  test_getNumberOfChildren(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(std::declval<const U&>().getNumberOfChildren(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_getNumberOfChildren(int);

    template<typename U>
    static uint8_t  test_leafGetNumberOfKeys(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(std::declval<const U&>().leafGetNumberOfKeys(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_leafGetNumberOfKeys(int);

    template<typename U>
    static uint8_t  test_interiorGetNumberOfKeys(...);
    template<typename U,std::enable_if_t<std::is_same_v<size_t,decltype(std::declval<const U&>().interiorGetNumberOfKeys(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_interiorGetNumberOfKeys(int);

    template<typename U>
    static uint8_t  test_isNullNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(std::declval<const U&>().isNullNode(std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_isNullNode(int);

    template<typename U>
    static uint8_t  test_getNullNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<const U&>().getNullNode())>,bool> = true>
    static uint16_t test_getNullNode(int);

    template<typename U>
    static uint8_t  test_leafCreateEmptyNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<U&>().leafCreateEmptyNode())>,bool> = true>
    static uint16_t test_leafCreateEmptyNode(int);

    template<typename U>
    static uint8_t  test_interiorCreateEmptyNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Node,decltype(std::declval<U&>().interiorCreateEmptyNode())>,bool> = true>
    static uint16_t test_interiorCreateEmptyNode(int);

    template<typename U>
    static uint8_t  test_releaseEmptyNode(...);
    template<typename U,std::enable_if_t<std::is_same_v<void,decltype(std::declval<U&>().releaseEmptyNode(std::move(std::declval<_Node&>())))>,bool> = true>
    static uint16_t test_releaseEmptyNode(int);

    template<typename U>
    static uint8_t  test_getKey(...);
    template<typename U,std::enable_if_t<std::is_same_v<_Key,decltype(std::declval<const U&>().getKey(std::declval<const _Holder&>()))>,bool> = true>
    static uint16_t test_getKey(int);

    template<typename U>
    static uint8_t  test_keyCompareLess(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(std::declval<const U&>().keyCompareLess(std::declval<const _Key&>(),std::declval<const _Key&>()))>,bool> = true>
    static uint16_t test_keyCompareLess(int);

    template<typename U>
    static uint8_t  test_keyCompareEqual(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(std::declval<const U&>().keyCompareEqual(std::declval<const _Key&>(),std::declval<const _Key&>()))>,bool> = true>
    static uint16_t test_keyCompareEqual(int);

    template<typename U>
    static uint8_t  test_nodeCompareEqual(...);
    template<typename U,std::enable_if_t<std::is_same_v<bool,decltype(std::declval<const U&>().nodeCompareEqual(std::declval<_Node&>(),std::declval<_Node&>()))>,bool> = true>
    static uint16_t test_nodeCompareEqual(int);

    static constexpr bool has_isLeaf                  = sizeof(test_isLeaf<_T>(1))                  == sizeof(uint16_t);
    static constexpr bool has_allowEmptyLeaf          = sizeof(test_allowEmptyLeaf<_T>(1))          == sizeof(uint16_t);
    static constexpr bool has_getNthChild             = sizeof(test_getNthChild<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_setNthChild             = sizeof(test_setNthChild<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_clearNthChild           = sizeof(test_clearNthChild<_T>(1))           == sizeof(uint16_t);
    static constexpr bool has_getParent               = sizeof(test_getParent<_T>(1))               == sizeof(uint16_t); // optional
    static constexpr bool has_setParent               = sizeof(test_setParent<_T>(1))               == sizeof(uint16_t); // optional
    static constexpr bool has_leafGetNext             = sizeof(test_leafGetNext<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_leafSetNext             = sizeof(test_leafSetNext<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_leafGetPrev             = sizeof(test_leafGetPrev<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_leafSetPrev             = sizeof(test_leafSetPrev<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_getNthHolder            = sizeof(test_getNthHolder<_T>(1))            == sizeof(uint16_t);
    static constexpr bool has_getNthHolderRef         = sizeof(test_getNthHolderRef<_T>(1))         == sizeof(uint16_t);
    static constexpr bool has_setNthHolderValue       = sizeof(test_setNthHolderValue<_T>(1))       == sizeof(uint16_t);
    static constexpr bool has_setHolderValue          = sizeof(test_setHolderValue<_T>(1))          == sizeof(uint16_t);
    static constexpr bool has_leafGetNthKey           = sizeof(test_leafGetNthKey<_T>(1))           == sizeof(uint16_t);
    static constexpr bool has_interiorGetNthKey       = sizeof(test_interiorGetNthKey<_T>(1))       == sizeof(uint16_t);
    static constexpr bool has_interiorSetNthKey       = sizeof(test_interiorSetNthKey<_T>(1))       == sizeof(uint16_t);
    static constexpr bool has_interiorClearNthKey     = sizeof(test_interiorClearNthKey<_T>(1))     == sizeof(uint16_t);
    static constexpr bool has_extractNthHolder        = sizeof(test_extractNthHolder<_T>(1))        == sizeof(uint16_t);
    static constexpr bool has_setNthHolder            = sizeof(test_setNthHolder<_T>(1))            == sizeof(uint16_t);
    static constexpr bool has_leafGetOrder            = sizeof(test_leafGetOrder<_T>(1))            == sizeof(uint16_t);
    static constexpr bool has_getNumberOfChildren     = sizeof(test_getNumberOfChildren<_T>(1))     == sizeof(uint16_t);
    static constexpr bool has_interiorGetOrder        = sizeof(test_interiorGetOrder<_T>(1))        == sizeof(uint16_t);
    static constexpr bool has_leafGetNumberOfKeys     = sizeof(test_leafGetNumberOfKeys<_T>(1))     == sizeof(uint16_t);
    static constexpr bool has_interiorGetNumberOfKeys = sizeof(test_interiorGetNumberOfKeys<_T>(1)) == sizeof(uint16_t);
    static constexpr bool has_isNullNode              = sizeof(test_isNullNode<_T>(1))              == sizeof(uint16_t);
    static constexpr bool has_getNullNode             = sizeof(test_getNullNode<_T>(1))             == sizeof(uint16_t);
    static constexpr bool has_leafCreateEmptyNode     = sizeof(test_leafCreateEmptyNode<_T>(1))     == sizeof(uint16_t);
    static constexpr bool has_interiorCreateEmptyNode = sizeof(test_interiorCreateEmptyNode<_T>(1)) == sizeof(uint16_t);
    static constexpr bool has_releaseEmptyNode        = sizeof(test_releaseEmptyNode<_T>(1))        == sizeof(uint16_t);
    static constexpr bool has_getKey                  = sizeof(test_getKey<_T>(1))                  == sizeof(uint16_t);
    static constexpr bool has_keyCompareLess          = sizeof(test_keyCompareLess<_T>(1))          == sizeof(uint16_t);
    static constexpr bool has_keyCompareEqual         = sizeof(test_keyCompareEqual<_T>(1))         == sizeof(uint16_t);
    static constexpr bool has_nodeCompareEqual        = sizeof(test_nodeCompareEqual<_T>(1))        == sizeof(uint16_t);

    static_assert(!complain || !std::is_reference_v<_Node>,            "NODE should not be a reference");
    static_assert(!complain || !std::is_const_v<_Node>,                "NODE should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<_Node>,      "NODE should be copy assignable");
    static_assert(!complain || !std::is_reference_v<_Holder>,          "HOLDER should not be a reference");
    static_assert(!complain || !std::is_const_v<_Holder>,              "HOLDER should not be const-qualified");
    static_assert(!complain ||  std::is_move_constructible_v<_Holder>, "HOLDER should be copy assignable");
    static_assert(!complain || !std::is_reference_v<_Key>,             "KEY should not be a reference");
    static_assert(!complain || !std::is_const_v<_Key>,                 "KEY should not be const-qualified");
    static_assert(!complain ||  std::is_copy_assignable_v<_Key>,       "KEY should be copy assignable");
    static_assert(!complain ||  has_isLeaf,                           "should implement 'bool isLeaf(NODE) const;'");
    static_assert(!complain ||  has_getNthChild,                      "should implement 'NODE getNthChild(NODE, size_t) const;'");
    static_assert(!complain ||  has_setNthChild,                      "should implement 'void setNthChild(NODE, size_t, NODE);'");
    static_assert(!complain ||  has_getNthHolder || has_getNthHolderRef,
                                                                      "should implement 'HOLDER getNthHolder(NODE, size_t) const;'");
    static_assert(!complain ||  std::is_same_v<_Value,void> || (has_setNthHolderValue || (has_getNthHolderRef && has_setHolderValue)), 
                                                                      "should provide value accessor for non-void value");
    static_assert(!complain ||  has_interiorGetNthKey,                "should implement 'KEY interiorGetNthKey(NODE, size_t) const;'");
    static_assert(!complain ||  has_interiorSetNthKey,                "should implement 'void interiorSetNthKey(NODE, size_t, const KEY&);'");
    static_assert(!complain ||  has_interiorClearNthKey,              "should implement 'void interiorClearNthKey(NODE, size_t);'");
    static_assert(!complain ||  has_extractNthHolder,                 "should implement 'HOLDER extractNthHolder(NODE, size_t);'");
    static_assert(!complain ||  has_setNthHolder,                     "should implement 'void setNthHolder(NODE, size_t, HOLDER&& holder);'");
    static_assert(!complain ||  has_leafGetOrder,                     "should implement 'size_t leafGetOrder() const;'");
    static_assert(!complain ||  has_getNumberOfChildren,              "should implement 'size_t getNumberOfChildren(NODE);'");
    static_assert(!complain ||  has_interiorGetOrder,                 "should implement 'size_t interiorGetOrder() const;'");
    static_assert(!complain ||  has_leafGetNumberOfKeys,              "should implement 'size_t leafGetNumberOfKeys(NODE);'");
    static_assert(!complain ||  has_interiorGetNumberOfKeys,          "should implement 'size_t interiorGetNumberOfKeys(NODE);'");
    static_assert(!complain ||  has_isNullNode,                       "should implement 'bool isNullNode(NODE) const;'");
    static_assert(!complain ||  has_getNullNode,                      "should implement 'NODE getNullNode() const;'");
    static_assert(!complain ||  has_leafGetNext,                      "should implement 'NODE leafGetNext(NODE) const;'");
    static_assert(!complain ||  has_leafSetNext,                      "should implement 'void leafSetNext(NODE, NODE);'");
    static_assert(!complain ||  has_leafCreateEmptyNode,              "should implement 'NODE leafCreateEmptyNode();'");
    static_assert(!complain ||  has_interiorCreateEmptyNode,          "should implement 'NODE interiorCreateEmptyNode();'");
    static_assert(!complain ||  has_releaseEmptyNode,                 "should implement 'void releaseEmptyNode(NODE);'");
    static_assert(!complain ||  has_getKey,                           "should implement 'KEY getKey(const HOLDER&) const;'");
    static_assert(!complain ||  has_keyCompareLess,                   "should implement 'bool keyCompareLess(const KEY&, const KEY&) const;'");
    static_assert(!complain ||  has_nodeCompareEqual,                 "should implement 'bool nodeCompareEqual(NODE, NODE) const;'");

    static constexpr bool value = !std::is_reference_v<_Node> && !std::is_const_v<_Node> && std::is_copy_assignable_v<_Node> &&
                                  !std::is_reference_v<_Holder>  && !std::is_const_v<_Holder> &&
                                   std::is_move_constructible_v<_Holder> &&
                                  !std::is_reference_v<_Key>  && !std::is_const_v<_Key> && std::is_copy_assignable_v<_Key> &&
                                  (std::is_same_v<_Value,void> || (has_setNthHolderValue || (has_getNthHolderRef && has_setHolderValue))) &&
                                  has_getNthChild && has_setNthChild  &&
                                  (has_getNthHolder || has_getNthHolderRef) && has_extractNthHolder && has_setNthHolder  &&
                                  has_interiorGetNthKey && has_interiorSetNthKey && has_interiorClearNthKey &&
                                  has_leafGetOrder && has_getNumberOfChildren && has_leafGetNumberOfKeys &&
                                  has_interiorGetOrder && has_interiorGetNumberOfKeys &&
                                  has_isNullNode && has_getNullNode &&
                                  has_leafGetNext && has_leafSetNext &&
                                  has_leafCreateEmptyNode && has_interiorCreateEmptyNode &&
                                  has_getKey && has_keyCompareLess &&
                                  has_nodeCompareEqual;
};

template<typename _T, typename _Node, typename _Holder, typename _Key, typename _Value,
         std::enable_if_t<treeop_traits<_T,_Node,_Holder,_Key,_Value>::value,bool> = true>
struct BPTreeOpWrapper {
    using traits = treeop_traits<_T,_Node,_Holder,_Key,_Value>;

    inline explicit BPTreeOpWrapper(_T treeop): m_ops (treeop) {}

    inline static constexpr bool allowEmptyLeaf() {
        if constexpr (traits::has_allowEmptyLeaf) {
            return _T::allowEmptyLeaf();
        } else {
            return false;
        }
    }

    inline bool isLeaf(_Node node) const { return m_ops.isLeaf(node); }

    inline _Node   getNthChild(_Node node, size_t nth) const { return m_ops.getNthChild(node, nth); }
    inline void   setNthChild(_Node node, size_t nth, _Node n)  { m_ops.setNthChild(node, nth, n); }

    inline _Holder getNthHolder(_Node node, size_t nth) const {
        if constexpr (traits::has_getNthHolder) {
            return m_ops.getNthHolder(node, nth);
        } else {
            return m_ops.getNthHolderRef(node, nth);
        }
    }

    inline _Holder& getNthHolderRef(_Node node, size_t nth) {
        if constexpr (traits::has_getNthHolderRef) {
            return m_ops.getNthHolderRef(node, nth);
        } else {
            return std::declval<_Holder&>();
        }
    }

    using HolderValue = std::conditional_t<std::is_same_v<_Value,void>,BPTreeOpWrapper,_Value>;
    inline void setHolderValue(_Holder& holder, HolderValue val) {
        if constexpr (traits::has_setHolderValue) {
            m_ops.setHolderValue(holder, val);
        } else {
            assert(false);
        }
    }

    inline void setNthHolderValue(_Node node, size_t nth, HolderValue val) {
        if constexpr (traits::has_setNthHolderValue) {
            return m_ops.setNthHolderValue(node, nth, val);
        } else if constexpr (traits::has_getNthHolderRef && traits::has_setHolderValue) {
            m_ops.setHolderValue(m_ops.getNthHolderRef(node, nth), val);
        } else if constexpr (std::is_same_v<_Value,void>) {
        } else {
            static_assert(std::is_same_v<_Value,void> ||traits::has_setNthHolderValue || (traits::has_getNthHolderRef && traits::has_setHolderValue),
                          "failed to implement setNthHolderValue");
        }
    }

    inline _Key    leafGetNthKey(_Node node, size_t nth) const {
        if constexpr (traits::has_leafGetNthKey) {
            return m_ops.leafGetNthKey(node, nth);
        } else {
            return this->getKey(this->getNthHolder(node, nth));
        }
    }
    inline _Key interiorGetNthKey(_Node node, size_t nth) const { return m_ops.interiorGetNthKey(node, nth); }
    inline void interiorSetNthKey(_Node node, size_t nth, const _Key& key) { m_ops.interiorSetNthKey(node, nth, key); }
    inline void interiorClearNthKey(_Node node, size_t nth) { return m_ops.interiorClearNthKey(node, nth); }
    inline _Key interiorExtractNthKey(_Node node, size_t nth) { 
        auto ans = this->interiorGetNthKey(node, nth);
        this->interiorClearNthKey(node, nth);
        return ans;
    }

    inline _Node getFirstChild(_Node node) const { return this->getNthChild(node, 0); }
    inline _Node getLastChild (_Node node) const { return this->getNthChild(node, m_ops.getNumberOfChildren(node)-1); }

    inline _Holder getFirstHolder(_Node node) const { return this->getNthHolder(node, 0); }
    inline _Holder getLastHolder (_Node node) const { return this->getNthHolder(node, m_ops.leafGetNumberOfKeys(node)-1); }

    inline _Key leafGetFirstKey(_Node node) const { return this->leafGetNthKey(node, 0); }
    inline _Key leafGetLastKey (_Node node) const { return this->leafGetNthKey(node, m_ops.leafGetNumberOfKeys(node)-1); }

    inline _Key interiorGetFirstKey(_Node node) const { return this->interiorGetNthKey(node, 0); }
    inline _Key interiorGetLastKey (_Node node) const { return this->interiorGetNthKey(node, m_ops.interiorGetNumberOfKeys(node)-1); }

    inline _Holder extractNthHolder(_Node node, size_t nth) { return m_ops.extractNthHolder(node, nth); }

    inline void clearNthChild (_Node node, size_t nth)  {
        if constexpr (traits::has_clearNthChild) {
            m_ops.clearNthChild(node, nth);
        } else {
            m_ops.setNthChild(node, nth, m_ops.getNullNode());
        }
    }
    inline void setNthHolder(_Node node, size_t nth, _Holder&& holder) { m_ops.setNthHolder(node, nth, std::move(holder)); }

    inline _Node getParent(_Node node) const  {
        if constexpr (traits::has_getParent) {
            return m_ops.getParent(node);
        }
        return node;
    }
    inline void setParent(_Node node, _Node n)  {
        if constexpr (traits::has_setParent) {
            return m_ops.setParent(node, n);
        }
    }

    inline _Node leafGetNext(_Node node) const  { return m_ops.leafGetNext(node); }
    inline void leafSetNext(_Node node, _Node n)  { m_ops.leafSetNext(node, n); }

    inline _Node leafGetPrev(_Node node) const  {
        if constexpr (traits::has_leafGetPrev) {
            return m_ops.leafGetPrev(node);
        } else {
            assert(false);
            return this->getNullNode();
        }
    }
    inline void leafSetPrev(_Node node, _Node n)  {
        if constexpr (traits::has_leafSetPrev) {
            m_ops.leafSetPrev(node, n);
        }
    }

    inline size_t leafGetOrder() const { return m_ops.leafGetOrder(); }
    inline size_t getNumberOfChildren(_Node node) const { return m_ops.getNumberOfChildren(node); }
    inline size_t leafGetNumberOfKeys(_Node node) const { return m_ops.leafGetNumberOfKeys(node); }

    inline size_t interiorGetOrder() const { return m_ops.interiorGetOrder(); }
    inline size_t interiorGetNumberOfKeys(_Node node) const { return m_ops.interiorGetNumberOfKeys(node); }

    inline bool leafIsFull(_Node node) const { return this->leafGetNumberOfKeys(node) == 2 * this->leafGetOrder() - 1; }
    inline bool interiorIsFull(_Node node) const { return this->interiorGetNumberOfKeys(node) == 2 * this->interiorGetOrder() - 1; }

    inline bool isNullNode(_Node node) const { return m_ops.isNullNode(node); }
    inline _Node getNullNode() const { return m_ops.getNullNode(); }
    inline _Node leafCreateEmptyNode() { return m_ops.leafCreateEmptyNode(); }
    inline _Node interiorCreateEmptyNode() { return m_ops.interiorCreateEmptyNode(); }
    inline bool interiorIsEmptyNode(_Node node) const { return !m_ops.isNullNode(node) && m_ops.getNumberOfChildren(node) == 0 && m_ops.interiorGetNumberOfKeys(node) == 0; }
    inline bool leafIsEmptyNode(_Node node) const { return !m_ops.isNullNode(node) && m_ops.leafGetNumberOfKeys(node) == 0; }
    inline void releaseEmptyNode(_Node&& node) { return m_ops.releaseEmptyNode(std::move(node)); }

    inline _Key getKey(const _Holder& n) const { return m_ops.getKey(n); }

    inline bool keyCompareLess(const _Key& lhs, const _Key& rhs) const { return m_ops.keyCompareLess(lhs, rhs); }

    inline bool keyCompareEqual(const _Key& lhs, const _Key& rhs) const {
        if constexpr (traits::has_keyCompareEqual) {
            return m_ops.keyCompareEqual(lhs, rhs);
        } else {
            return !this->keyCompareLess(lhs, rhs) && !this->keyCompareLess(rhs, lhs);
        }
    }

    inline bool nodeCompareEqual(_Node lhs, _Node rhs) const {
        if constexpr (traits::has_nodeCompareEqual) {
            return m_ops.nodeCompareEqual(lhs, rhs);
        } else {
            return lhs == rhs;
        }
    }

    inline size_t interior_lower_bound(_Node node, const _Key& key) {
        size_t lower = 0;
        size_t upper = this->interiorGetNumberOfKeys(node);

        while (lower < upper) {
            auto n = (upper + lower) / 2;
            if (!this->keyCompareLess(this->interiorGetNthKey(node, n), key)) {
                upper = n;
            } else {
                lower = n + 1;
            }
        }

        return upper;
    }

    inline size_t interior_upper_bound(_Node node, const _Key& key) {
        size_t lower = 0;
        size_t upper = this->interiorGetNumberOfKeys(node);

        while (lower < upper) {
            auto n = (upper + lower) / 2;
            if (this->keyCompareLess(key, this->interiorGetNthKey(node, n))) {
                upper = n;
            } else {
                lower = n + 1;
            }
        }

        return upper;
    }

    inline size_t leaf_lower_bound(_Node node, const _Key& key) {
        size_t lower = 0;
        size_t upper = this->leafGetNumberOfKeys(node);

        while (lower < upper) {
            auto n = (upper + lower) / 2;
            if (!this->keyCompareLess(this->leafGetNthKey(node, n), key)) {
                upper = n;
            } else {
                lower = n + 1;
            }
        }

        return upper;
    }

    inline size_t leaf_upper_bound(_Node node, const _Key& key) {
        size_t lower = 0;
        size_t upper = this->leafGetNumberOfKeys(node);

        while (lower < upper) {
            auto n = (upper + lower) / 2;
            if (this->keyCompareLess(key, this->leafGetNthKey(node, n))) {
                upper = n;
            } else {
                lower = n + 1;
            }
        }

        return upper;
    }

    inline _T& ops() { return m_ops; }
    inline const _T& ops() const { return m_ops; }

private:
    _T m_ops;
};

template<typename _T, typename _Node, typename _Holder, typename _Key, typename _Value, bool enableParent=true,
         bool multikey=false, size_t static_vector_size = 32,
         std::enable_if_t<treeop_traits<_T,_Node,_Holder,_Key,_Value,true>::value,bool> = true>
class BPTreeAlgorithm {
public:
    using traits = treeop_traits<_T,_Node,_Holder,_Key,_Value>;
    static constexpr bool parents_ops  = traits::has_getParent && traits::has_getParent && enableParent;
    static constexpr bool ref_accessor = traits::has_getNthHolderRef;

    using NodePath = std::conditional_t<parents_ops, _Node,
                         std::conditional_t<static_cast<bool>(static_vector_size > 0), maxsize_vector<std::pair<_Node,size_t>,static_vector_size>,std::vector<std::pair<_Node,size_t>>>>;

    class HolderPath {
        friend BPTreeAlgorithm;
        NodePath m_path;
        size_t   m_index;

        HolderPath(NodePath&& path, size_t idx): m_path(std::move(path)), m_index(idx) { }
    };

    class RoHolderPath {
        friend BPTreeAlgorithm;
        _Node   m_node;
        size_t m_index;

        RoHolderPath(_Node leaf, size_t idx): m_node(leaf), m_index(idx) { }
    };

private:
    inline RoHolderPath toRoHolderPath(const HolderPath& hp) const {
        auto node = this->exists(hp) ? this->GetNodeAncestor(hp.m_path, 0) : m_ops.getNullNode();
        return RoHolderPath(node, hp.m_index);
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
    inline NodePath InitPath() const {
        return m_ops.getNullNode();
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline NodePath InitPath() const {
        return NodePath();
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
    inline void NodePathPush(NodePath& path, _Node n, size_t nth) const {
        path = n;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline void NodePathPush(NodePath& path, _Node n, size_t nth) const {
        path.push_back(std::make_pair(n, nth));
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
    inline void NodePathPop(NodePath& path) const {
        path = m_ops.getParent(path);;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline void NodePathPop(NodePath& path) const {
        path.pop_back();
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
    inline _Node GetNodeAncestor(N node, size_t n) const {
        for (size_t i=0;i<n;i++) {
            assert(!m_ops.isNullNode(node));
            node = m_ops.getParent(node);
        }
        return node;
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
    inline _Node GetRoot(N node) const {
        for (;!m_ops.isNullNode(m_ops.getParent(node));node = m_ops.getParent(node));
        return node;
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
    inline size_t GetPathDepth(N node) const {
        size_t i = 0;
        for (;!m_ops.isNullNode(node);i++) {
            node = m_ops.getParent(node);
        }
        return i;
    }

    template<typename N,std::enable_if_t<std::is_same_v<N,_Node>,bool> = true>
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

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline _Node GetNodeAncestor(N np, size_t n) const {
        if (n == np.size()) return m_ops.getNullNode();
        assert(n < np.size());
        return np[np.size() - n - 1].first;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline _Node GetRoot(N np) const {
        if (np.empty()) return m_ops.getNullNode();

        return np[0].first;
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline size_t GetPathDepth(N np) const {
        return np.size();
    }

    template<typename N,std::enable_if_t<!std::is_same_v<N,_Node>,bool> = true>
    inline size_t GetNodeIndex(N np, size_t n) const {
        assert(n < np.size());
        return np[np.size() - n - 1].second;
    }

    inline void nodeShiftRight(_Node node, size_t index) {
        assert(!m_ops.isLeaf(node));
        assert(!m_ops.interiorIsFull(node));
        const auto hs = m_ops.interiorGetNumberOfKeys(node);
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

    inline void interiorKeyShiftRight(_Node node, size_t index) {
        assert(!m_ops.isLeaf(node));
        assert(!m_ops.interiorIsFull(node));
        const auto hs = m_ops.interiorGetNumberOfKeys(node);
        assert(hs > 0);

        for (size_t i=hs;i>index;i--) {
            auto h = m_ops.interiorExtractNthKey(node, i-1);
            m_ops.interiorSetNthKey(node, i, std::move(h));
        }
    }

    inline void interiorShiftRight(_Node node, size_t index) {
        this->nodeShiftRight(node, index);
        this->interiorKeyShiftRight(node, index);
    }

    inline void leafShiftRight(_Node node, size_t index) {
        assert(m_ops.isLeaf(node));
        assert(!m_ops.leafIsFull(node));
        const auto hs = m_ops.leafGetNumberOfKeys(node);
        assert(hs > 0);

        for (size_t i=hs;i>index;i--) {
            auto h = m_ops.extractNthHolder(node, i-1);
            m_ops.setNthHolder(node, i, std::move(h));
        }
    }

    inline void nodeShiftLeft(_Node node, size_t index) {
        assert(!m_ops.isLeaf(node));
        const auto hs = m_ops.interiorGetNumberOfKeys(node);
        assert(hs < m_ops.interiorGetOrder() * 2 - 1);

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

    inline void interiorKeyShiftLeft(_Node node, size_t index) {
        assert(!m_ops.isLeaf(node));
        assert(m_ops.interiorGetNumberOfKeys(node) > 0);
        const auto hs = m_ops.interiorGetNumberOfKeys(node);
        assert(hs < m_ops.interiorGetOrder() * 2 - 1);

        for (size_t i=index;i+1<=hs;i++) {
            auto h = m_ops.interiorExtractNthKey(node, i+1);
            m_ops.interiorSetNthKey(node, i, std::move(h));
        }
    }

    inline void interiorShiftLeft(_Node node, size_t index) {
        this->nodeShiftLeft(node, index);
        this->interiorKeyShiftLeft(node, index);
    }

    inline void leafShiftLeft(_Node node, size_t index) {
        assert(m_ops.isLeaf(node));
        assert(m_ops.leafGetNumberOfKeys(node) > 0);
        const auto hs = m_ops.leafGetNumberOfKeys(node);
        assert(hs < m_ops.leafGetOrder() * 2 - 1);

        for (size_t i=index;i+1<=hs;i++) {
            auto h = m_ops.extractNthHolder(node, i+1);
            m_ops.setNthHolder(node, i, std::move(h));
        }
    }

    inline std::pair<_Key,_Node> interiorSplitFullNode(_Node& root, _Node parent, size_t nodeIdx, _Node node) {
        assert(!m_ops.isLeaf(node));
        assert(m_ops.isNullNode(parent) || !m_ops.interiorIsFull(parent));
        assert(m_ops.interiorIsFull(node));
        const auto t = m_ops.interiorGetOrder();
        auto newNode = m_ops.interiorCreateEmptyNode();

        for (auto i=t;i<2*t-1;i++) {
            auto h = m_ops.interiorExtractNthKey(node, i);
            m_ops.interiorSetNthKey(newNode, i-t, std::move(h));
        }

        for (auto i=t;i<2*t;i++) {
            auto knode = m_ops.getNthChild(node, i);
            m_ops.setNthChild(newNode, i-t, knode);
            m_ops.clearNthChild(node, i);
            if constexpr (parents_ops) {
                m_ops.setParent(knode, newNode);
            }
        }

        auto middle_key = m_ops.interiorExtractNthKey(node, t-1);
        if (m_ops.isNullNode(parent)) {
            assert(m_ops.nodeCompareEqual(root, node));
            parent = root = m_ops.interiorCreateEmptyNode();
            m_ops.interiorSetNthKey(root, 0, std::move(middle_key));
            m_ops.setNthChild(root, 0, node);
            m_ops.setNthChild(root, 1, newNode);
            if constexpr (parents_ops) {
                m_ops.setParent(node, root);
                m_ops.setParent(newNode, root);
            }
        } else {
            this->interiorKeyShiftRight(parent, nodeIdx);
            this->nodeShiftRight(parent, nodeIdx + 1);
            m_ops.interiorSetNthKey(parent, nodeIdx, std::move(middle_key));
            m_ops.setNthChild(parent, nodeIdx + 1, newNode);
            if constexpr (parents_ops) {
                m_ops.setParent(newNode, parent);
            }
        }

        if constexpr (parents_ops) {
            m_ops.setParent(newNode, parent);
        }
        return std::make_pair(middle_key, parent);
    }

    inline std::pair<_Key,_Node> leafSplitFullNode(_Node& root, _Node parent, size_t nodeIdx, _Node node) {
        assert(m_ops.isLeaf(node));
        assert(m_ops.isNullNode(parent) || !m_ops.interiorIsFull(parent));
        assert(m_ops.leafIsFull(node));
        const auto t = m_ops.leafGetOrder();
        auto newNode = m_ops.leafCreateEmptyNode();

        for (auto i=t;i<2*t-1;i++) {
            auto h = m_ops.extractNthHolder(node, i);
            m_ops.setNthHolder(newNode, i-t, std::move(h));
        }

        auto middle_key = m_ops.leafGetLastKey(node);
        if (m_ops.isNullNode(parent)) {
            assert(m_ops.nodeCompareEqual(root, node));
            parent = root = m_ops.interiorCreateEmptyNode();
            m_ops.interiorSetNthKey(root, 0, std::move(middle_key));
            m_ops.setNthChild(root, 0, node);
            m_ops.setNthChild(root, 1, newNode);
            if constexpr (parents_ops) {
                m_ops.setParent(node, root);
                m_ops.setParent(newNode, root);
            }
        } else {
            this->interiorKeyShiftRight(parent, nodeIdx);
            this->nodeShiftRight(parent, nodeIdx + 1);
            m_ops.interiorSetNthKey(parent, nodeIdx, std::move(middle_key));
            m_ops.setNthChild(parent, nodeIdx + 1, newNode);
            if constexpr (parents_ops) {
                m_ops.setParent(newNode, root);
            }
        }

        if constexpr (parents_ops) {
            m_ops.setParent(newNode, parent);
        }
        const auto next = m_ops.leafGetNext(node);
        m_ops.leafSetNext(node, newNode);
        m_ops.leafSetNext(newNode, next);
        if constexpr (traits::has_leafSetPrev) {
            if (!m_ops.isNullNode(next)) {
                m_ops.leafSetPrev(next, newNode);
            }
            m_ops.leafSetPrev(newNode, node);
        }
        return std::make_pair(middle_key, newNode);
    }

    inline void interiorMergeTwoNodes(_Node parent, size_t firstIdx) {
        assert(!m_ops.isLeaf(parent));
        assert(m_ops.getNumberOfChildren(parent) > firstIdx + 1);
        auto n1 = m_ops.getNthChild(parent, firstIdx);
        auto n2 = m_ops.getNthChild(parent, firstIdx + 1);
        assert(!m_ops.isLeaf(n1) && !m_ops.isLeaf(n2));
        const auto t = m_ops.interiorGetOrder();
        assert(!m_ops.isNullNode(n1) && m_ops.interiorGetNumberOfKeys(n1) == t - 1);
        assert(!m_ops.isNullNode(n2) && m_ops.interiorGetNumberOfKeys(n2) == t - 1);

        auto h = m_ops.interiorExtractNthKey(parent, firstIdx);
        m_ops.interiorSetNthKey(n1, t-1, std::move(h));

        for (size_t i=0;i<t-1;i++) {
            auto h = m_ops.interiorExtractNthKey(n2, i);
            m_ops.interiorSetNthKey(n1, i+t, std::move(h));
        }

        for (size_t i=0;i<t;i++) {
            auto n = m_ops.getNthChild(n2, i);
            m_ops.setNthChild(n1, i+t, n);
            m_ops.clearNthChild(n2, i);
            if constexpr (parents_ops) {
                m_ops.setParent(n , n1);
            }
        }

        assert(m_ops.interiorIsEmptyNode(n2));
        m_ops.clearNthChild(parent, firstIdx + 1);
        m_ops.releaseEmptyNode(std::move(n2));

        if (m_ops.interiorGetNumberOfKeys(parent) > 0) {
            this->interiorKeyShiftLeft(parent, firstIdx);
        } else {
            // TODO assert parent
        }
        this->nodeShiftLeft(parent, firstIdx + 1);
    }

    inline void leafMergeTwoNodes(NodePath ppath, size_t firstIdx) {
        auto parent = this->GetNodeAncestor(ppath, 0);
        assert(!m_ops.isLeaf(parent));
        assert(m_ops.getNumberOfChildren(parent) > firstIdx + 1);
        auto n1 = m_ops.getNthChild(parent, firstIdx);
        auto n2 = m_ops.getNthChild(parent, firstIdx + 1);
        assert(m_ops.isLeaf(n1) && m_ops.isLeaf(n2));
        const auto t = m_ops.leafGetOrder();
        assert(!m_ops.isNullNode(n1));
        assert(!m_ops.isNullNode(n2));

        const auto num_n1 = m_ops.leafGetNumberOfKeys(n1);
        const auto num_n2 = m_ops.leafGetNumberOfKeys(n2);
        assert(num_n1 + num_n2 <= 2 * t - 1);

        m_ops.interiorExtractNthKey(parent, firstIdx);
        for (size_t i=0;i<num_n2;i++) {
            auto h = m_ops.extractNthHolder(n2, i);
            m_ops.setNthHolder(n1, i+num_n1, std::move(h));
        }

        auto n2_next = m_ops.leafGetNext(n2);
        m_ops.leafSetNext(n1, n2_next);
        if constexpr (traits::has_leafSetPrev) {
            if (!m_ops.isNullNode(n2_next)) {
                m_ops.leafSetPrev(n2_next, n1);
            }
        }
        assert(m_ops.leafIsEmptyNode(n2));
        m_ops.clearNthChild(parent, firstIdx + 1);
        m_ops.releaseEmptyNode(std::move(n2));

        if (m_ops.interiorGetNumberOfKeys(parent) > 0) {
            this->interiorKeyShiftLeft(parent, firstIdx);
        } else {
            // TODO assert parent
        }
        this->nodeShiftLeft(parent, firstIdx + 1);
    }

    inline void clearEmptyLeafNode() {
        if constexpr (!t_allowEmptyLeaf) return;
    }

    inline void fixInteriorKey(const NodePath& path) {
        auto leaf = this->GetNodeAncestor(path, 0);
        assert(m_ops.isLeaf(leaf));
        const auto newkey = m_ops.leafGetLastKey(leaf);

        const auto pdepth = this->GetPathDepth(path);
        for (auto i=1;i<pdepth;i++) {
            auto pp = this->GetNodeAncestor(path, i);
            auto p = this->GetNodeAncestor(path, i-1);
            auto pidx = this->GetNodeIndex(path, i-1);
            auto ppss = m_ops.interiorGetNumberOfKeys(pp);
            assert(pidx <= ppss);
            if (pidx < ppss) {
                m_ops.interiorClearNthKey(pp, pidx);
                m_ops.interiorSetNthKey(pp, pidx, newkey);
                break;
            }
        }
    }

    inline _Node getFirstLeaf(_Node root) {
        if (m_ops.isNullNode(root) || m_ops.isLeaf(root)) return root;

        for (;!m_ops.isLeaf(root);root=m_ops.getFirstChild(root));

        return root;
    }

    inline _Node getLastLeaf(_Node root) {
        if (m_ops.isNullNode(root) || m_ops.isLeaf(root)) return root;

        for (;!m_ops.isLeaf(root);root=m_ops.getLastChild(root));

        return root;
    }

    inline NodePath getLastLeafPath(_Node root) {
        auto ans = this->InitPath<NodePath>();
        if (m_ops.isNullNode(root)) return ans;

        auto node=root;
        auto ss = 1;
        for (;!m_ops.isLeaf(node);ss=m_ops.getNumberOfChildren(node), node=m_ops.getLastChild(node)) {
            assert(!m_ops.isNullNode(node));
            if constexpr (parents_ops) {
                ans = node;
            } else {
                ans.push_back(std::make_pair(node,ss-1));
            }
        }

        if constexpr (parents_ops) {
            ans = node;
        } else {
            ans.push_back(std::make_pair(node,ss-1));
        }

        return ans;
    }

    inline _Node nextLeafNode(_Node node) { return m_ops.leafGetNext(node); }

    inline _Node prevLeafNode(_Node root, _Node node) {
        _Node prev = m_ops.getNullNode();
        if constexpr (traits::has_leafGetPrev && traits::has_leafSetPrev) {
            prev = m_ops.isNullNode(node) ? this->getLastLeaf(root) : m_ops.leafGetPrev(node);
        } else {
            auto u = this->getFirstLeaf(root);
            for (;!m_ops.isNullNode(u);u = m_ops.leafGetNext(u)) {
                if (m_ops.nodeCompareEqual(m_ops.leafGetNext(u), node)) {
                    prev = u;
                    break;
                }
            }
        }
        return prev;
    }

    inline void emptyLeafNodeForward(HolderPath& path) {
        for (;this->exists(path) && m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path,0)) == 0;) {
            this->nextLeafPath(path.m_path);
            path.m_index = 0;
        }
    }

    inline void nextLeafPath(NodePath& path)
    {
        if constexpr (parents_ops) {
            assert(!m_ops.isNullNode(path));
            path = this->nextLeafNode(path);
        } else {
            const size_t v = this->GetPathDepth(path);
            assert(v > 0);
            for (size_t i=1;i<=v;i++) {
                if (i==v) {
                    path = this->InitPath<NodePath>();
                    break;
                }

                auto pp = this->GetNodeAncestor(path, 1);
                auto kk = this->GetNodeAncestor(path, 0);
                auto kk_idx = this->GetNodeIndex(path, 0);
                this->NodePathPop<NodePath>(path);
                if (m_ops.getNumberOfChildren(pp) > kk_idx + 1) {
                    this->NodePathPush<NodePath>(path, m_ops.getNthChild(pp, kk_idx + 1), kk_idx + 1);

                    for (size_t j=i-1;j>=1;j--) {
                        auto upp = this->GetNodeAncestor(path, 0);
                        this->NodePathPush<NodePath>(path, m_ops.getNthChild(upp, 0), 0);
                    }
                    break;
                }
            }
        }
    }

    inline void emptyLeafNodeBackward(HolderPath& path) {
        for (;this->exists(path) && m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path,0)) == 0;) {
            this->prevLeafPath(path.m_path);
            const auto ss = m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path, 0));
            path.m_index = ss - 1;
        }
    }

    inline void prevLeafPath(NodePath& path) {
        if constexpr (parents_ops) {
            assert(!m_ops.isNullNode(path));
            path = this->prevLeafNode(this->GetRoot(path), path);
        } else {
            const size_t v = this->GetPathDepth(path);
            assert(v > 0);
            for (size_t i=1;i<=v;i++) {
                if (i==v) {
                    path = this->InitPath<NodePath>();
                    break;
                }

                auto pp = this->GetNodeAncestor(path, 1);
                auto kk = this->GetNodeAncestor(path, 0);
                auto kk_idx = this->GetNodeIndex(path, 0);
                this->NodePathPop<NodePath>(path);
                if (kk_idx > 0) {
                    this->NodePathPush<NodePath>(path, m_ops.getNthChild(pp, kk_idx - 1), kk_idx - 1);

                    for (size_t j=i-1;j>=1;j--) {
                        auto upp = this->GetNodeAncestor(path, 0);
                        const auto ss = m_ops.getNumberOfChildren(upp);
                        this->NodePathPush<NodePath>(path, m_ops.getNthChild(upp, ss - 1), ss-1);
                    }
                    break;
                }
            }
        }
    }

public:
    inline explicit BPTreeAlgorithm(_T ops): m_ops(ops) {}

    inline void check_consistency(_Node root) const {
        if (m_ops.isNullNode(root)) return;
        std::queue<std::pair<_Node,size_t>> queue;
        queue.push(std::make_pair(root, 1));
        std::vector<_Node> leaf_list;
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
                assert(m_ops.interiorGetNumberOfKeys(node) + 1 == m_ops.getNumberOfChildren(node));
            }

            if (front.second != 1 && !m_ops.isLeaf(node)) {
                assert(m_ops.interiorGetNumberOfKeys(node) >= m_ops.interiorGetOrder() - 1);
            }

            if (m_ops.isLeaf(node)) {
                if (!leaf_list.empty()) {
                    m_ops.nodeCompareEqual(m_ops.leafGetNext(leaf_list.back()), node);
                }
                leaf_list.push_back(node);

                const auto n1 = m_ops.leafGetNumberOfKeys(node);
                for (size_t i=0;i<n1;i++) {
                    auto h1 = m_ops.getNthHolder(node, i);
                    if (i>0) {
                        auto h0 = m_ops.getNthHolder(node, i-1);
                        assert(m_ops.keyCompareLess(m_ops.getKey(h0), m_ops.getKey(h1)));
                    }
                }
            } else {
                const auto n1 = m_ops.interiorGetNumberOfKeys(node);
                for (size_t i=0;i<n1;i++) {
                    auto h1 = m_ops.interiorGetNthKey(node, i);
                    if (i>0) {
                        auto h0 = m_ops.interiorGetNthKey(node, i-1);
                        assert(m_ops.keyCompareLess(h0, h1));
                    }
                }

                const auto n2 = m_ops.getNumberOfChildren(node);
                for (size_t i=0;i<n2;i++) {
                    auto nn = m_ops.getNthChild(node, i);
                    assert(!m_ops.isNullNode(nn));
                    queue.push(std::make_pair(nn, front.second + 1));
                }

                for (size_t i=0;i<n1;i++) {
                    auto h1 = m_ops.interiorGetNthKey(node, i);
                    auto n1 = m_ops.getNthChild(node, i);
                    auto n2 = m_ops.getNthChild(node, i+1);
                    assert(!m_ops.isNullNode(n1));
                    assert(!m_ops.isNullNode(n2));

                    if (!m_ops.isLeaf(n1) || m_ops.leafGetNumberOfKeys(n1) > 0) {
                        if (!m_ops.isLeaf(n1)) {
                            assert(m_ops.interiorGetNumberOfKeys(n2) > 0);
                        }
                        const auto& k1 = m_ops.isLeaf(n1) ? m_ops.leafGetLastKey(n1) : m_ops.interiorGetLastKey(n1);
                        if (m_ops.isLeaf(n1)) {
                            assert(!m_ops.keyCompareLess(h1, k1));
                        } else {
                            assert(m_ops.keyCompareLess(k1, h1));
                        }
                    }

                    if (!m_ops.isLeaf(n2) || m_ops.leafGetNumberOfKeys(n2) > 0) {
                        if (!m_ops.isLeaf(n2)) {
                            assert(m_ops.interiorGetNumberOfKeys(n2) > 0);
                        }
                        const auto& k2 = m_ops.isLeaf(n2) ? m_ops.leafGetFirstKey(n2) : m_ops.interiorGetFirstKey(n2);
                        if constexpr (t_allowEmptyLeaf) {
                            assert(!m_ops.keyCompareLess(k2, h1));
                        } else {
                            assert(m_ops.keyCompareLess(h1, k2));
                        }
                    }
                }
            }
        }

        assert(!leaf_list.empty());
        assert(m_ops.isNullNode(m_ops.leafGetNext(leaf_list.back())));
    }

    inline HolderPath insertHolder(_Node& root, _Holder&& holder) {
        HolderPath ans = { this->InitPath<NodePath>(), 0 };
        if (m_ops.isNullNode(root)) {
            root = m_ops.leafCreateEmptyNode();
            m_ops.setNthHolder(root, 0, std::move(holder));
            this->NodePathPush<NodePath>(ans.m_path, root, 0);
            return ans;
        }

        const auto t = m_ops.interiorGetOrder();
        const auto leaf_t = m_ops.leafGetOrder();
        const auto key = m_ops.getKey(holder);

        auto parentNode = m_ops.getNullNode();
        auto currentIdx = 0;
        auto current = root;

        while (!m_ops.isLeaf(current)) {
            this->NodePathPush<NodePath>(ans.m_path, current, currentIdx);
            if (m_ops.interiorIsFull(current)) {
                this->NodePathPop<NodePath>(ans.m_path);
                auto result = this->interiorSplitFullNode(root, parentNode, currentIdx, current);
                if (!m_ops.keyCompareLess(key, result.first)) {
                    current = result.second;
                    currentIdx++;
                }

                if (m_ops.isNullNode(parentNode)) {
                    this->NodePathPush<NodePath>(ans.m_path, root, 0);
                }
                this->NodePathPush<NodePath>(ans.m_path, current, currentIdx);
            }

            parentNode = current;
            currentIdx = m_ops.interior_upper_bound(current, key);
            current = m_ops.getNthChild(parentNode, currentIdx);
        }

        this->NodePathPush<NodePath>(ans.m_path, current, currentIdx);
        if (m_ops.leafIsFull(current)) {
            this->NodePathPop<NodePath>(ans.m_path);
            auto result = this->leafSplitFullNode(root, parentNode, currentIdx, current);
            if (m_ops.keyCompareLess(result.first, key)) {
                current = result.second;
                currentIdx++;
            }

            if (m_ops.isNullNode(parentNode)) {
                this->NodePathPush<NodePath>(ans.m_path, root, 0);
            }
            this->NodePathPush<NodePath>(ans.m_path, current, currentIdx);
        }

        const auto insertPos = m_ops.leaf_upper_bound(current, key);
        if constexpr (!multikey) {
            std::optional<_Key> prevKey = std::nullopt;
            if (insertPos > 0) {
                prevKey = m_ops.leafGetNthKey(current, insertPos-1);
            } else {
                auto prev = this->prevLeafNode(root, current);
                if constexpr (t_allowEmptyLeaf) {
                    while (!m_ops.isNullNode(prev) && m_ops.leafGetNumberOfKeys(prev) == 0) {
                        prev = this->prevLeafNode(root, prev);
                    }
                }
                if (!m_ops.isNullNode(prev)) {
                    prevKey = m_ops.leafGetLastKey(prev);
                }
            }

            if (prevKey.has_value() && m_ops.keyCompareEqual(key, prevKey.value()))
                return { this->InitPath<NodePath>(), 0 };
        }

        if (insertPos < m_ops.leafGetNumberOfKeys(current))
            this->leafShiftRight(current, insertPos);

        ans.m_index = insertPos;
        m_ops.setNthHolder(current, insertPos, std::move(holder));
        return ans;
    }

    inline _Holder deleteHolder(_Node& root, HolderPath path) {
        assert(!m_ops.isNullNode(root));
        const auto t = m_ops.interiorGetOrder();
        auto node = this->GetNodeAncestor(path.m_path, 0);
        const auto depth = this->GetPathDepth(path.m_path);
        const auto s = m_ops.leafGetNumberOfKeys(node);
        assert(m_ops.isLeaf(node));
        assert(depth >= 1);
        assert(s > 0 && path.m_index < s);

        if constexpr (t_allowEmptyLeaf) {
            auto ans = m_ops.extractNthHolder(node, path.m_index);
            if (path.m_index + 1 < s) {
                this->leafShiftLeft(node, path.m_index);
            }
            return ans;
        }

        if (s > 1) {
            auto ans = m_ops.extractNthHolder(node, path.m_index);
            if (path.m_index + 1 < s) {
                this->leafShiftLeft(node, path.m_index);
            } else if (path.m_index + 1 == s) {
                this->fixInteriorKey(path.m_path);
            }
            if (depth > 1) {
                // TODO merge leaf node if criterion is meeted
            }
            return ans;
        } else if (depth == 1) {
            assert(m_ops.nodeCompareEqual(node, root));
            auto ans = m_ops.extractNthHolder(node, 0);
            m_ops.releaseEmptyNode(std::move(node));
            root = m_ops.getNullNode();
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
            if (m_ops.isLeaf(n)) continue;

            assert(!m_ops.isNullNode(p));
            assert(i > 1);
            if (m_ops.interiorGetNumberOfKeys(n) <= t - 1) {
                assert(m_ops.interiorGetNumberOfKeys(n) == t - 1);

                if (idx > 0) {
                    auto sibling_prev = m_ops.getNthChild(p, idx-1);
                    if (m_ops.interiorGetNumberOfKeys(sibling_prev) > t - 1) {
                        this->interiorShiftRight(n, 0);
                        const auto sib_prev_n = m_ops.interiorGetNumberOfKeys(sibling_prev);
                        auto ph = m_ops.interiorExtractNthKey(sibling_prev, sib_prev_n-1);
                        auto parent_holder = m_ops.interiorExtractNthKey(p, idx-1);
                        m_ops.interiorSetNthKey(p, idx-1, std::move(ph));
                        {
                            auto pn = m_ops.getNthChild(sibling_prev, sib_prev_n);
                            assert(!m_ops.isNullNode(pn));
                            m_ops.clearNthChild(sibling_prev, sib_prev_n);
                            m_ops.setNthChild(n, 0, pn);
                            if constexpr (parents_ops) {
                                m_ops.setParent(pn, n);
                            }
                        }
                        m_ops.interiorSetNthKey(n, 0, std::move(parent_holder));

                        if constexpr (!parents_ops) {
                            path.m_path[depth+1-i].second++;
                        }
                    } else {
                        assert(m_ops.interiorGetNumberOfKeys(sibling_prev) == t - 1);
                        this->interiorMergeTwoNodes(p, idx-1);
                        if constexpr (parents_ops) {
                            n = m_ops.getNthChild(p, idx-1);
                        } else {
                            auto mergedNode = m_ops.getNthChild(p, idx-1);
                            path.m_path[depth-i].first = n = mergedNode;
                            path.m_path[depth-i].second = idx = idx - 1;
                            path.m_path[depth-i+1].second += t;
                        }
                    }
                } else {
                    assert(idx + 1 < m_ops.getNumberOfChildren(p));
                    auto sibling_next = m_ops.getNthChild(p, idx+1);
                    if (m_ops.interiorGetNumberOfKeys(sibling_next) > t - 1) {
                        const auto sib_next_n = m_ops.interiorGetNumberOfKeys(sibling_next);
                        auto nh = m_ops.interiorExtractNthKey(sibling_next, 0);
                        auto parent_holder = m_ops.interiorExtractNthKey(p, idx);
                        m_ops.interiorSetNthKey(p, idx, std::move(nh));
                        {
                            auto nn = m_ops.getNthChild(sibling_next, 0);
                            m_ops.clearNthChild(sibling_next, 0);
                            m_ops.setNthChild(n, t, nn);
                            if constexpr (parents_ops) {
                                m_ops.setParent(nn, n);
                            }
                        }
                        this->interiorShiftLeft(sibling_next, 0);
                        m_ops.interiorSetNthKey(n, t-1, std::move(parent_holder));
                    } else {
                        assert(m_ops.interiorGetNumberOfKeys(sibling_next) == t - 1);
                        this->interiorMergeTwoNodes(p, idx);
                    }
                }
            }
        }

        auto parent = this->GetNodeAncestor(path.m_path, 1);
        auto leafNode = this->GetNodeAncestor(path.m_path, 0);
        const auto idx = this->GetNodeIndex(path.m_path, 0);
        assert(m_ops.leafGetNumberOfKeys(leafNode) == 1);
        assert(m_ops.isLeaf(leafNode));
        auto ans = m_ops.extractNthHolder(leafNode, path.m_index);
        auto ppath = path.m_path;
        this->NodePathPop<NodePath>(ppath);
        this->leafMergeTwoNodes(ppath, idx > 0 ? idx - 1 : idx);

        if (!m_ops.isLeaf(root) && m_ops.interiorGetNumberOfKeys(root) == 0) {
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

    inline HolderPath findKey(_Node root, const _Key& key) {
        const auto invalid_idx = m_ops.leafGetOrder() * 2;
        HolderPath path { this->InitPath<NodePath>(), invalid_idx };
        if (m_ops.isNullNode(root))
            return path;

        auto node = root;
        size_t node_index = 0;

        for (;!m_ops.isLeaf(node);) {
            this->NodePathPush<NodePath>(path.m_path, node, node_index);

            auto kn = m_ops.interior_lower_bound(node, key);
            node = m_ops.getNthChild(node, kn);
            node_index = kn;
        }

        this->NodePathPush<NodePath>(path.m_path, node, node_index);
        auto kn = m_ops.leaf_lower_bound(node, key);
        if (kn < m_ops.leafGetNumberOfKeys(node)) {
            assert(kn < invalid_idx);
            path.m_index = kn;
        } else {
            do {
                if constexpr (t_allowEmptyLeaf) {
                    const auto nn = this->GetNodeAncestor(path.m_path, 1);
                    const auto ii = this->GetNodeIndex(path.m_path, 0);
                    if (!m_ops.isNullNode(nn) && ii < m_ops.interiorGetNumberOfKeys(nn)) {
                        if (m_ops.keyCompareLess(key, m_ops.interiorGetNthKey(nn, ii))) {
                            path = { this->InitPath<NodePath>(), 0 };
                            break;
                        }
                    }
                }

                this->nextLeafPath(path.m_path);
            } while (this->exists(path) && m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path, 0)) == 0);
            path.m_index = 0;
        }

        if (this->exists(path)) {
            auto h = this->getHolder(path);

            if (!m_ops.keyCompareEqual(m_ops.getKey(h), key)) {
                path = { this->InitPath<NodePath>(), 0 };
            }
        }

        return path;
    }

    inline HolderPath lower_bound(_Node root, const _Key& key) {
        const auto invalid_idx = m_ops.leafGetOrder() * 2;
        HolderPath path { this->InitPath<NodePath>(), invalid_idx };
        if (m_ops.isNullNode(root))
            return path;

        auto node = root;
        size_t node_index = 0;

        for (;!m_ops.isLeaf(node);) {
            this->NodePathPush<NodePath>(path.m_path, node, node_index);

            auto kn = m_ops.interior_lower_bound(node, key);
            node = m_ops.getNthChild(node, kn);
            node_index = kn;
        }

        this->NodePathPush<NodePath>(path.m_path, node, node_index);
        auto kn = m_ops.leaf_lower_bound(node, key);
        if (kn < m_ops.leafGetNumberOfKeys(node)) {
            assert(kn < invalid_idx);
            path.m_index = kn;
        } else {
            do {
                this->nextLeafPath(path.m_path);
            } while (this->exists(path) && m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path, 0)) == 0);
            path.m_index = 0;
        }

        if (this->exists(path)) {
            const auto k1 = m_ops.getKey(this->getHolder(path));
            assert(!m_ops.keyCompareLess(k1, key));
        }

        return path;
    }

    inline HolderPath upper_bound(_Node root, const _Key& key) {
        const auto invalid_idx = m_ops.leafGetOrder() * 2;
        HolderPath path { this->InitPath<NodePath>(), invalid_idx };
        if (m_ops.isNullNode(root))
            return path;

        auto node = root;
        size_t node_index = 0;

        for (;!m_ops.isLeaf(node);) {
            this->NodePathPush<NodePath>(path.m_path, node, node_index);

            auto kn = m_ops.interior_upper_bound(node, key);
            node = m_ops.getNthChild(node, kn);
            node_index = kn;
        }

        this->NodePathPush<NodePath>(path.m_path, node, node_index);
        auto kn = m_ops.leaf_upper_bound(node, key);
        if (kn < m_ops.leafGetNumberOfKeys(node)) {
            assert(kn < invalid_idx);
            path.m_index = kn;
        } else {
            do {
                this->nextLeafPath(path.m_path);
            } while (this->exists(path) && m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path, 0)) == 0);
            path.m_index = 0;
        }

        if (this->exists(path)) {
            const auto k1 = m_ops.getKey(this->getHolder(path));
            assert(!m_ops.keyCompareLess(k1, key));
        }

        return path;
    }

    inline HolderPath begin(_Node root) {
        auto ans = HolderPath(this->InitPath<NodePath>(), 0);
        if (m_ops.isNullNode(root)) return ans;

        auto node=root;
        for (;!m_ops.isLeaf(node);node=m_ops.getFirstChild(node)) {
            assert(!m_ops.isNullNode(node));
            if constexpr (parents_ops) {
                ans.m_path = node;
            } else {
                ans.m_path.push_back(std::make_pair(node,0));
            }
        }

        if constexpr (parents_ops) {
            ans.m_path = node;
        } else {
            ans.m_path.push_back(std::make_pair(node,0));
        }

        if constexpr (t_allowEmptyLeaf) this->emptyLeafNodeForward(ans);
        return ans;
    }

    inline HolderPath end(_Node) {
        return HolderPath(this->InitPath<NodePath>(), 0);
    }

    inline void forward(_Node root, HolderPath& path) {
        const auto leaf = this->GetNodeAncestor(path.m_path, 0);
        assert(m_ops.isLeaf(leaf));
        if (path.m_index + 1 < m_ops.leafGetNumberOfKeys(leaf)) {
            path.m_index++;
            return;
        }

        path.m_index = 0;
        this->nextLeafPath(path.m_path);
        if constexpr (t_allowEmptyLeaf) this->emptyLeafNodeForward(path);
    }

    inline void backward(_Node root, HolderPath& path) {
        if (this->exists(path) && path.m_index > 0) {
            path.m_index--;
            return;
        } else if (!this->exists(path)) {
            path = { this->getLastLeafPath(root), 0 };
            if constexpr (t_allowEmptyLeaf) this->emptyLeafNodeBackward(path);
            const auto ss = m_ops.leafGetNumberOfKeys(this->GetNodeAncestor(path.m_path, 0));
            path.m_index = ss - 1;
            return;
        }

        this->prevLeafPath(path.m_path);
        if constexpr (t_allowEmptyLeaf) this->emptyLeafNodeBackward(path);
        if (this->exists(path)) {
            const auto leaf = this->GetNodeAncestor(path.m_path, 0);
            path.m_index = m_ops.leafGetNumberOfKeys(leaf) - 1;
        }
    }

    inline _Holder getHolder(const HolderPath& path) {
        assert(this->exists(path));
        return m_ops.getNthHolder(this->GetNodeAncestor(path.m_path, 0), path.m_index);
    }

    inline void setHolderValue(const HolderPath& path, typename BPTreeOpWrapper<_T,_Node,_Holder,_Key,_Value>::HolderValue value) {
        assert(this->exists(path));
        m_ops.setNthHolderValue(this->GetNodeAncestor(path.m_path, 0), path.m_index, value);
    }

    inline _Holder& getHolderRef(const HolderPath& path) {
        assert(this->exists(path));
        return m_ops.getNthHolderRef(this->GetNodeAncestor(path.m_path, 0), path.m_index);
    }

    inline _Key getHolderKey(const HolderPath& path) {
        assert(this->exists(path));
        return m_ops.getKey(m_ops.getNthHolder(this->GetNodeAncestor(path.m_path, 0), path.m_index));
    }

    inline bool exists(const HolderPath& path) const {
        if constexpr (parents_ops) {
            return !m_ops.isNullNode(path.m_path);
        } else {
            return !path.m_path.empty();
        }
    }

    inline bool exists(const RoHolderPath& path) const {
        return !m_ops.isNullNode(path.m_node);
    }

    inline int compareHolderPath(const HolderPath& p1, const HolderPath& p2) const {
        return this->compareHolderPath(this->toRoHolderPath(p1), this->toRoHolderPath(p2));
    }

    inline int compareHolderPath(const RoHolderPath& p1, const RoHolderPath& p2) const {
        if (!this->exists(p1)) {
            if (this->exists(p2)) {
                return 1;
            } else {
                return 0;
            }
        } else if (!this->exists(p2)) {
            return -1;
        }

        if (m_ops.nodeCompareEqual(p1.m_node, p2.m_node)) {
            if (p1.m_index == p2.m_index) {
                return 0;
            } else if (p1.m_index < p2.m_index) {
                return -1;
            } else {
                return 1;
            }
        }

        const auto& k1 = m_ops.leafGetNthKey(p1.m_node, p1.m_index);
        const auto& k2 = m_ops.leafGetNthKey(p2.m_node, p2.m_index);
        if (m_ops.keyCompareLess(k1, k2)) return -1;
        if (m_ops.keyCompareLess(k2, k1)) return 1;

        if constexpr (multikey) {
            const auto n1 = p1.m_node;
            const auto n2 = p2.m_node;
            auto nn1=n1, nn2=n2;
            for (;!m_ops.isNullNode(nn1) && !m_ops.isNullNode(nn2);
                  nn1 = m_ops.leafGetNext(nn1), nn2=m_ops.leafGetNext(nn2))
            {
                if (m_ops.nodeCompareEqual(nn1, n2)) {
                    return -1;
                } else if (m_ops.nodeCompareEqual(nn2, n1)) {
                    return 1;
                }
            }

            return m_ops.isNullNode(nn1) ? 1 : -1;
        } else {
            assert(false);
            return 0;
        }
    }

    inline void releaseNode(_Node node) {
        if (!m_ops.isLeaf(node)) {
            const auto n = m_ops.getNumberOfChildren(node);
            for (size_t i=0;i<n;i++) {
                this->releaseNode(m_ops.getNthChild(node, i));
                m_ops.setNthChild(node, i, m_ops.getNullNode());
            }
        }
        m_ops.releaseEmptyNode(std::move(node));
    }

    template<typename U, std::enable_if_t<std::is_same_v<_Holder,decltype(std::declval<U>()())>,bool> = true>
    inline _Node initWithAscSequence(size_t size, U iterFunc) {
        assert(size > 0);
        _Holder first = iterFunc();
        const auto k1 = m_ops.getKey(first);

        const auto ti = m_ops.interiorGetOrder();
        const auto tl = m_ops.leafGetOrder();
        const auto leafCount = (size-1) / (2*tl - 1) + 1;
        size_t depth = 1;
        if (leafCount > 1) {
            depth++;
            const auto vn = 2 * ti;
            auto count = vn;
            for (;count<leafCount;count*=vn,depth++);
        }
        auto prev = m_ops.getNullNode();
        size_t leafN = 0;
        const std::function<_Node(size_t)> initNode = [&](size_t d) {
            if (d == depth) {
                auto nd = m_ops.leafCreateEmptyNode();
                if (!m_ops.isNullNode(prev)) {
                    m_ops.leafSetNext(prev, nd);
                }
                if constexpr (traits::has_leafGetPrev && traits::has_leafSetPrev) {
                    m_ops.leafSetPrev(nd, prev);
                }
                prev = nd;
                leafN++;
                return nd;
            } 

            auto nd = m_ops.interiorCreateEmptyNode();
            for (size_t i=0;i<2*ti&&leafN<leafCount;i++) {
                auto cn = initNode(d+1);
                m_ops.setNthChild(nd, i, cn);
                if (i>0) m_ops.interiorSetNthKey(nd, i-1, k1);
                if constexpr (parents_ops) {
                    m_ops.setParent(cn, nd);
                }
            }
            return nd;
        };

        const auto root = initNode(1);

        for (auto node=root;!m_ops.isLeaf(node);node=m_ops.getLastChild(node)) {
            assert(m_ops.getNumberOfChildren(node) > 1);

            auto last = m_ops.getLastChild(node);
            if (!m_ops.isLeaf(last) && m_ops.getNumberOfChildren(last) < ti) {
                const auto r = m_ops.getNumberOfChildren(last);
                const auto s = m_ops.getNumberOfChildren(node);
                auto ll = m_ops.getNthChild(node, s - 2);
                assert(m_ops.getNumberOfChildren(ll) == 2 * ti);

                for (size_t i=r;i>0;i--) {
                    const auto exnode = m_ops.getNthChild(last, i-1);
                    m_ops.clearNthChild(last, i-1);
                    m_ops.setNthChild(last, i-1+ti-r, exnode);

                    if (i<r) {
                        const auto exkey = m_ops.interiorExtractNthKey(last, i-1);
                        m_ops.interiorSetNthKey(last, i-1+ti-r, exkey);
                    }
                }

                for (size_t i=0;i<ti-r;i++) {
                    const auto exnode = m_ops.getNthChild(ll, 2 *ti-1-i);
                    m_ops.clearNthChild(ll, 2*ti-1-i);
                    const auto exkey  = m_ops.interiorExtractNthKey(ll, 2*ti-2-i);
                    m_ops.setNthChild(last,ti-r-1-i, exnode);
                    m_ops.interiorSetNthKey(last,ti-r-1-i, exkey);

                    if constexpr (parents_ops) {
                        m_ops.setParent(exnode, last);
                    }
                }
            }
        }

        NodePath pn = this->InitPath<NodePath>();
        {
            auto cn = root;
            for (;!m_ops.isLeaf(cn);cn=m_ops.getFirstChild(cn)) {
                this->NodePathPush<NodePath>(pn, cn, 0);
            }
            this->NodePathPush<NodePath>(pn, cn, 0);
        }

        size_t m=0;
        for (size_t i=0;i<leafCount;i++) {
            auto ln = this->GetNodeAncestor(pn, 0);

            for (size_t j=0;j<tl*2-1&&m<size;j++,m++) {
                if (size - m == tl && j + tl > 2*tl-1)
                    break;

                if (i==0 && j==0) {
                    m_ops.setNthHolder(ln, j, std::move(first));
                } else {
                    m_ops.setNthHolder(ln, j, iterFunc());
                }
            }

            this->fixInteriorKey(pn);
            this->nextLeafPath(pn);
        }

        return root;
    }

protected:
    using OpWrapper = BPTreeOpWrapper<_T,_Node,_Holder,_Key,_Value>;
    OpWrapper m_ops;
    static constexpr bool t_allowEmptyLeaf = OpWrapper::allowEmptyLeaf();
};
}


#include "./unarray.h"
#include "./ldc_utils.h"
#include <variant>
namespace ldc::BPTreeBasicContainerImpl {
template<typename _Key, typename _Value, size_t Order, size_t LeafOrder, bool parentsOps, bool prevOp>
struct TreeNode {
    using KVPair = std::conditional_t<std::is_same_v<_Value,void>,_Key,std::pair<const _Key,_Value>>;
    std::conditional_t<parentsOps,TreeNode*,dummy_struct> m_parent;
    bool m_isLeaf;

    struct InteriorNode {
        unarray<TreeNode*,2*Order> children;
        unarray<const _Key,2*Order - 1> keys;

        InteriorNode() = default;
    };
    struct LeafNode {
        unarray<KVPair,2*LeafOrder - 1> datas;
        using TNB = std::conditional_t<prevOp,std::tuple<TreeNode*,TreeNode*>,std::tuple<TreeNode*>>;
        TNB prevnext;
       
        inline LeafNode() {
            if constexpr (prevOp) {
                prevnext = { nullptr, nullptr };
            } else {
                prevnext = { nullptr };
            }
        }
    };
    std::variant<InteriorNode,LeafNode> m_nodeimpl;

    inline explicit TreeNode(bool isLeaf):
        m_isLeaf(isLeaf),
        m_nodeimpl(isLeaf ? decltype(m_nodeimpl)(LeafNode()) : decltype(m_nodeimpl)(InteriorNode()))
    {
        if constexpr (parentsOps) {
            m_parent = nullptr;
        }
    }

    inline bool isLeaf() const { return m_isLeaf; }

    InteriorNode& interior() {
        assert(!this->isLeaf());
        return std::get<InteriorNode>(m_nodeimpl);
    }

    const InteriorNode& interior() const {
        assert(!this->isLeaf());
        return std::get<InteriorNode>(m_nodeimpl);
    }

    LeafNode& leaf() {
        assert(this->isLeaf());
        return std::get<LeafNode>(m_nodeimpl);
    }

    const LeafNode& leaf() const {
        assert(this->isLeaf());
        return std::get<LeafNode>(m_nodeimpl);
    }

    ~TreeNode() = default;
};

template<typename _Key, typename _Value, size_t Order, size_t LeafOrder, typename _CmpLess, typename _Allocator,
         bool VallowEmptyLeaf, bool parentsOps, bool prevOps>
struct TreeNodeOps {
    using TNODE  = TreeNode<_Key,_Value,Order,LeafOrder,parentsOps,prevOps>;
    using KVPair = typename TNODE::KVPair;
    using _Node  = TNODE*;
    using _storage_allocator = typename std::allocator_traits<_Allocator>::template rebind_alloc<TNODE>;
    _CmpLess           m_cmp;
    _storage_allocator m_allocator;

    TreeNodeOps(const _CmpLess& cmp, const _Allocator& allocator): m_cmp(cmp), m_allocator(allocator) {}

    inline bool isLeaf(_Node node) const { return node->isLeaf(); }

    inline static constexpr bool allowEmptyLeaf() { return VallowEmptyLeaf; }

    inline _Node getNthChild(_Node node, size_t nth) const {
        return node->interior().children.at(nth);
    }

    inline void setNthChild(_Node node, size_t nth, _Node n)  {
        if (n == nullptr) {
            node->interior().children.destroy(nth);
        } else {
            node->interior().children.construct(nth, n);
        }
    }

    inline KVPair& getNthHolderRef(_Node node, size_t nth) const {
        return node->leaf().datas.at(nth);
    }
    inline void setHolderValue(KVPair& h, std::conditional_t<std::is_same_v<_Value,void>,int,_Value> val) const {
        if constexpr (!std::is_same_v<_Value,void>) {
            h.second = val;
        }
    }

    inline _Key interiorGetNthKey(_Node node, size_t nth) const {
        return node->interior().keys.at(nth);
    }
    inline void interiorSetNthKey(_Node node, size_t nth, const _Key& key) {
        node->interior().keys.construct(nth, key);
    }
    inline void interiorClearNthKey(_Node node, size_t nth) {
        node->interior().keys.destroy(nth);
    }

    inline KVPair extractNthHolder(_Node node, size_t nth) {
        auto n = std::move(node->leaf().datas.at(nth));
        node->leaf().datas.destroy(nth);
        return n;
    }

    inline void setNthHolder(_Node node, size_t nth, KVPair&& holder) {
        node->leaf().datas.construct(nth, std::move(holder));
    }

    inline _Node getParent(_Node node) const  {
        if constexpr (parentsOps) {
            return std::get<1>(node->isLeaf_parent);
        } else {
            return nullptr;
        }
    }
    inline void setParent(_Node node, _Node n)  {
        if constexpr (parentsOps) {
            std::get<1>(node->isLeaf_parent) = n;
        }
    }

    inline _Node leafGetNext(_Node node) const  {
        return std::get<0>(node->leaf().prevnext);
    }
    inline void leafSetNext(_Node node, _Node n)  {
        std::get<0>(node->leaf().prevnext) = n;
    }
    inline _Node leafGetPrev(_Node node) const  {
        if constexpr (prevOps) {
            return std::get<1>(node->leaf().prevnext);
        } else {
            return nullptr;
        }
    }
    inline void leafSetPrev(_Node node, _Node n)  {
        if constexpr (prevOps) {
            std::get<1>(node->leaf().prevnext) = n;
        }
    }

    inline size_t leafGetOrder() const { return LeafOrder; }

    inline size_t getNumberOfChildren(_Node node) const { return node->interior().children.size(); }
    inline size_t leafGetNumberOfKeys(_Node node) const { return node->leaf().datas.size(); }

    inline size_t interiorGetOrder() const { return Order; }
    inline size_t interiorGetNumberOfKeys(_Node node) const { return node->interior().keys.size(); }

    inline bool isNullNode(_Node node) const { return node == nullptr; }
    inline _Node getNullNode() const { return nullptr; }
    inline _Node interiorCreateEmptyNode() {
        auto ptr = m_allocator.allocate(1);
        return new (ptr) TNODE(false);
    }
    inline _Node leafCreateEmptyNode() {
        auto ptr = m_allocator.allocate(1);
        return new (ptr) TNODE(true);
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

template<typename _Key, typename _Value, size_t Order, size_t LeafOrder, typename _CmpLess, 
         typename _Allocator, bool VallowEmptyLeaf, bool parentsOps, bool prevOps, bool multikey>
using BPTreeAlgo = BPTreeAlgorithmImpl::BPTreeAlgorithm<
                         TreeNodeOps<_Key,_Value,Order,LeafOrder,_CmpLess,_Allocator,VallowEmptyLeaf,parentsOps,prevOps>,
                         TreeNode<_Key,_Value,Order,LeafOrder,parentsOps,prevOps>*,
                         typename TreeNodeOps<_Key,_Value,Order,LeafOrder,_CmpLess,_Allocator,VallowEmptyLeaf,parentsOps,prevOps>::KVPair,
                         _Key, _Value, parentsOps, multikey>;

template<typename _Key, typename _Value, size_t _Order, size_t _LeafOrder, typename _CmpLess, 
         typename _Allocator, bool _VallowEmptyLeaf, bool _parentsOps, bool _prevOps, bool _multikey>
struct BPTreeInMemory: protected BPTreeAlgo<_Key,_Value,_Order,_LeafOrder,_CmpLess,_Allocator,_VallowEmptyLeaf,_parentsOps,_prevOps,_multikey> {
    using BASE      = BPTreeAlgo<_Key,_Value,_Order,_LeafOrder,_CmpLess,_Allocator,_VallowEmptyLeaf,_parentsOps,_prevOps,_multikey>;
    using treeops_t = TreeNodeOps<_Key,_Value,_Order,_LeafOrder,_CmpLess,_Allocator,_VallowEmptyLeaf,_parentsOps,_prevOps>;
    using TNODE     = typename treeops_t::TNODE;
    using _Node     = typename treeops_t::_Node;
    using KVPair    = typename treeops_t::KVPair;
    using ITERATOR  = typename BASE::HolderPath;
    static constexpr auto  ref_accessor = BASE::ref_accessor;


private:
    _Node  m_root;
    size_t m_size;


public:
    inline BPTreeInMemory(const _CmpLess& cmp, const _Allocator& alloc): 
        BASE(treeops_t(cmp, alloc)),
        m_root(nullptr), m_size(0) {}

    inline BPTreeInMemory(const BPTreeInMemory& _oth):
        BASE(treeops_t(_oth.m_ops.ops().m_cmp, _oth.m_ops.ops().m_allocator)),
        m_root(nullptr), m_size(0)
    {
        if (_oth.size() > 0) {
            auto& oth = const_cast<BPTreeInMemory&>(_oth);
            auto b = oth.begin();
            const auto fn = [&]() {
                const auto ans = oth.getHolder(b);
                oth.forward(b);
                return ans;
            };
            m_root = this->initWithAscSequence(oth.size(), fn);
            m_size = oth.size();
        }
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
        }
    }

    ~BPTreeInMemory() { this->clear(); }
};
}
