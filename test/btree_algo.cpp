#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <optional>
#include "btree.h"
using namespace ldc;


template<size_t Order=128>
struct TreeNode {
    std::array<TreeNode*,2*Order> children;
    std::array<std::optional<int>,2*Order - 1> holders;

    TreeNode* parent;
    size_t numOfChildren, numOfKV;

    explicit TreeNode(): parent(nullptr), numOfChildren(0), numOfKV(0) {
        std::fill(children.begin(), children.end(), nullptr);
        std::fill(holders.begin(), holders.end(), std::nullopt);
    }

    ~TreeNode() {
        for (auto child: children) {
            if (child != nullptr) {
                delete child;
            }
        }
    }
};

template<size_t Order=128>
struct TreeNodeOps {
    typedef TreeNode<Order> *NODE;
    using HOLDER = int;
    using KEY = int;

    inline NODE getNthChild(NODE node, size_t nth) const {
        assert(nth < node->children.size());
        return node->children[nth];
    }
    inline HOLDER getNthHolder(NODE node, size_t nth) const {
        assert(nth < node->holders.size());
        return node->holders[nth].value();
    }
    inline KEY getNthKey(NODE node, size_t nth) const {
        assert(nth < node->holders.size());
        return node->holders[nth].value();
    }

    inline HOLDER extractNthHolder(NODE node, size_t nth) {
        assert(nth < node->holders.size());
        assert(node->numOfKV > 0);
        auto n = node->holders[nth].value();
        node->holders[nth] = std::nullopt;
        node->numOfKV--;
        return n;
    }

    inline void setNthChild (NODE node, size_t nth, NODE n)  {
        assert(nth < node->children.size());
        if (n == nullptr) {
            assert(node->numOfChildren > 0);
            assert(node->children[nth] != nullptr);
            node->numOfChildren--;
        } else {
            assert(node->children[nth] == nullptr);
            node->numOfChildren++;
        }
        node->children[nth] = n;
    }

    inline void setNthHolder(NODE node, size_t nth, HOLDER&& holder) {
        assert(nth < node->holders.size());
        auto k = node->holders[nth];
        assert(!k.has_value());
        node->holders[nth] = std::move(holder);
        node->numOfKV++;
    }

    inline NODE getParent(NODE node) const  {
        return node->parent;
    }
    inline void setParent(NODE node, NODE n)  {
        node->parent = n;
    }

    inline size_t getOrder() const { return Order; }
    inline size_t getNumberOfChildren(NODE node) const { return node->numOfChildren; }
    inline size_t getNumberOfKeys(NODE node) const { return node->numOfKV; }

    inline bool isNullNode(NODE node) const { return node == nullptr; }
    inline NODE getNullNode() const { return nullptr; }
    inline NODE createEmptyNode() { return new TreeNode<Order>(); }
    inline void releaseEmptyNode(NODE&& node) { delete node; }

    inline KEY getKey(const HOLDER& n) const { return n; }

    inline bool keyCompareLess(const KEY& lhs, const KEY& rhs) const { return lhs < rhs; }

    inline bool keyCompareEqual(const KEY& lhs, const KEY& rhs) const { return lhs  == rhs; }

    inline bool nodeCompareEqual(NODE lhs, NODE rhs) const { return lhs == rhs; }
};

template<size_t Order, bool parent_ops>
using BASE_T = BTreeAlgorithmImpl::BTreeAlgorithm<TreeNodeOps<Order>,TreeNode<Order>*,int,int,parent_ops>;

template<size_t Order, bool parent_ops>
struct BTREE: public BASE_T<Order,parent_ops> {
    typedef TreeNode<Order>* NODE;

    using NN   = std::remove_pointer_t<NODE>;
    using BASE = BASE_T<Order,parent_ops>;
    using HolderPath = typename BASE::HolderPath;
    NODE root;

    BTREE(): BASE(TreeNodeOps<Order>()), root(nullptr) {}

    bool insert(int val) {
        return this->exists(this->insertHolder(this->root, std::move(val)));
    }

    std::optional<int> find(int key) {
        if (this->root == nullptr) return std::nullopt;

        auto ans = this->findKey(root, key);
        if (!this->exists(ans)) return std::nullopt;

        return this->getHolder(ans);
    }

    std::optional<HolderPath> lower_bound(int key) {
        if (this->root == nullptr) return std::nullopt;

        auto ans = BASE::lower_bound(this->root, key);
        if (!this->exists(ans)) return std::nullopt;

        return ans;
    }

    std::optional<HolderPath> upper_bound(int key) {
        if (this->root == nullptr) return std::nullopt;

        auto ans = BASE::upper_bound(this->root, key);
        if (!this->exists(ans)) return std::nullopt;

        return ans;
    }

    HolderPath begin() {
        return BASE::begin(this->root);
    }

    void forward(HolderPath& path) {
        BASE::forward(this->root, path);
    }

    void backward(HolderPath& path) {
        BASE::backward(this->root, path);
    }

    bool deleteByKey(int key) {
        if (this->root == nullptr) return false;

        auto ans = this->findKey(root, key);
        if (!this->exists(ans)) return false;

        auto node = this->deleteHolder(this->root, ans);
        return true;
    }

    void check_consistency() {
        BASE::check_consistency(this->root);
    }

    ~BTREE() { if (root) delete root; }
};


static std::default_random_engine generator;
template<size_t Order, bool parent_ops>
static void test_btree_insert(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        
        const auto nofound = vals.find(val) == vals.end();
        ASSERT_EQ(tree.insert(val), nofound);
        if (i % 1000 == 0 || i + 1 ==n)
            tree.check_consistency();
        vals.insert(val);
    }
}

template<size_t Order, bool parent_ops>
static void test_btree_find(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        auto a1 = tree.find(val);
        auto a2 = vals.find(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(a1.value(), val);
        }
    }
}

template<size_t Order, bool parent_ops>
static void test_btree_delete(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        auto a1 = tree.deleteByKey(val);
        if (i % 1000 == 0 || i + 1 ==n)
            tree.check_consistency();
        auto k = vals.find(val);
        auto a2 = k != vals.end();
        if (a2) vals.erase(k);

        ASSERT_EQ(a1, a2);
    }
}

template<size_t Order, bool parent_ops>
static void test_btree_delete2(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    auto beg = vals.begin();
    int i = 0;
    for (auto val: vals) {
        auto a1 = tree.deleteByKey(val);
        if (i % 1000 == 0 || i + 1 ==vals.size())
            tree.check_consistency();
        ASSERT_TRUE(a1);
        i++;
    }
}

template<size_t Order, bool parent_ops>
static void test_btree_lower_bound(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        auto a1 = tree.lower_bound(val);
        auto a2 = vals.lower_bound(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(tree.getHolder(a1.value()), *a2);
        }
    }
}

template<size_t Order, bool parent_ops>
static void test_btree_upper_bound(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        auto a1 = tree.upper_bound(val);
        auto a2 = vals.upper_bound(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(tree.getHolder(a1.value()), *a2);
        }
    }
}

template<size_t Order, bool parent_ops>
static void test_btree_forward_backward(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BTREE<Order,parent_ops> tree;
    std::set<int> vals;

    for (size_t i=0;i<n;i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    auto tbeg = tree.begin();
    auto beg  = vals.begin();
    for (;beg != vals.end();beg++,tree.forward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2  = tree.getHolder(tbeg);
        ASSERT_EQ(val, v2);
    }

    ASSERT_TRUE(!tree.exists(tbeg));

    if (n > 0) {
        beg--;
        tree.backward(tbeg);
    }
    for (;beg != vals.begin();beg--,tree.backward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2  = tree.getHolder(tbeg);
        ASSERT_EQ(val, v2);
    }
    if (n > 0) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2  = tree.getHolder(tbeg);
        ASSERT_EQ(val, v2);
    }
}


#define SETUP_TEST_FUNC_N(func, order, parent_ops) \
    func<order,parent_ops>(0); \
    func<order,parent_ops>(1); \
    func<order,parent_ops>(2); \
    func<order,parent_ops>(3); \
    func<order,parent_ops>(4); \
    func<order,parent_ops>(5); \
    func<order,parent_ops>(10); \
    func<order,parent_ops>(100); \
    func<order,parent_ops>(1000); \
    func<order,parent_ops>(10000); \
    func<order,parent_ops>(100000); \
    func<order,parent_ops>(1000000); \
    // func<order,parent_ops>(10000000); \
    func<order,parent_ops>(100000000)

#define SETUP_TEST_FUNC(func, parent_ops) \
    SETUP_TEST_FUNC_N(func, 2, parent_ops); \
    SETUP_TEST_FUNC_N(func, 3, parent_ops); \
    SETUP_TEST_FUNC_N(func, 4, parent_ops); \
    SETUP_TEST_FUNC_N(func, 8, parent_ops); \
    SETUP_TEST_FUNC_N(func, 16, parent_ops); \
    SETUP_TEST_FUNC_N(func, 32, parent_ops); \
    SETUP_TEST_FUNC_N(func, 64, parent_ops); \
    SETUP_TEST_FUNC_N(func, 128, parent_ops);


TEST(btree_without_parent_ops, insert) {
    SETUP_TEST_FUNC(test_btree_insert, false);
}
TEST(btree_with_parent_ops, insert) {
    SETUP_TEST_FUNC(test_btree_insert, true);
}

TEST(btree_without_parent_ops, find) {
    SETUP_TEST_FUNC(test_btree_find, false);
}
TEST(btree_with_parent_ops, find) {
    SETUP_TEST_FUNC(test_btree_find, true);
}

TEST(btree_without_parent_ops, delete) {
    SETUP_TEST_FUNC(test_btree_delete, false);
}
TEST(btree_with_parent_ops, delete) {
    SETUP_TEST_FUNC(test_btree_delete, true);
}

TEST(btree_without_parent_ops, delete2) {
    SETUP_TEST_FUNC(test_btree_delete2, false);
}
TEST(btree_with_parent_ops, delete2) {
    SETUP_TEST_FUNC(test_btree_delete2, true);
}

TEST(btree_without_parent_ops, lower_bound) {
    SETUP_TEST_FUNC(test_btree_lower_bound, false);
}
TEST(btree_with_parent_ops, lower_bound) {
    SETUP_TEST_FUNC(test_btree_lower_bound, true);
}

TEST(btree_without_parent_ops, upper_bound) {
    SETUP_TEST_FUNC(test_btree_upper_bound, false);
}
TEST(btree_with_parent_ops, upper_bound) {
    SETUP_TEST_FUNC(test_btree_upper_bound, true);
}

TEST(btree_without_parent_ops, forward_backward) {
    SETUP_TEST_FUNC(test_btree_forward_backward, false);
}
TEST(btree_with_parent_ops, forward_backward) {
    SETUP_TEST_FUNC(test_btree_forward_backward, true);
}
