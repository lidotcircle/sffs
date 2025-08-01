#include <utest.h>

#include <optional>
#include <random>
#include <set>

#include "rbtree.h"
#include "test_scale.h"
using namespace ldc;

struct TreeNode {
    TreeNode *left, *right;
    int key;
    bool black;

    explicit TreeNode(int key)
        : left(nullptr), right(nullptr), key(key), black(false) {}

    ~TreeNode() {
        if (left) delete left;
        if (right) delete right;
    }
};

struct TreeNodeOps {
    TreeNode* getLeft(TreeNode* n) const { return n->left; }
    TreeNode* getRight(TreeNode* n) const { return n->right; }

    void setLeft(TreeNode* n, TreeNode* l) const { n->left = l; }
    void setRight(TreeNode* n, TreeNode* r) const { n->right = r; }

    bool isBlack(TreeNode* n) const { return n->black; }
    void setBlack(TreeNode* n, bool black) const { n->black = black; }

    bool isNullNode(TreeNode* n) const { return n == nullptr; }
    TreeNode* getNullNode() const { return nullptr; }

    int getKey(TreeNode* n) const { return n->key; }
    bool keyCompareLess(const int& a, const int& b) const { return a < b; }
    bool keyCompareEqual(const int& a, const int& b) const { return a == b; }
    bool nodeCompareEqual(TreeNode* n1, TreeNode* n2) const { return n1 == n2; }
};

struct TreeNode2 {
    TreeNode2 *left, *right, *parent;
    int key;
    bool black;

    explicit TreeNode2(int key)
        : left(nullptr),
          right(nullptr),
          parent(nullptr),
          key(key),
          black(false) {}

    ~TreeNode2() {
        if (left) delete left;
        if (right) delete right;
    }
};

struct TreeNode2Ops {
    TreeNode2* getLeft(TreeNode2* n) const { return n->left; }
    TreeNode2* getRight(TreeNode2* n) const { return n->right; }
    TreeNode2* getParent(TreeNode2* n) const { return n->parent; }

    void setLeft(TreeNode2* n, TreeNode2* l) const { n->left = l; }
    void setRight(TreeNode2* n, TreeNode2* r) const { n->right = r; }
    void setParent(TreeNode2* n, TreeNode2* p) const { n->parent = p; }

    bool isBlack(TreeNode2* n) const { return n->black; }
    void setBlack(TreeNode2* n, bool black) const { n->black = black; }

    bool isNullNode(TreeNode2* n) const { return n == nullptr; }
    TreeNode2* getNullNode() const { return nullptr; }

    int getKey(TreeNode2* n) const { return n->key; }
    bool keyCompareLess(const int& a, const int& b) const { return a < b; }
    bool keyCompareEqual(const int& a, const int& b) const { return a == b; }
    bool nodeCompareEqual(TreeNode2* n1, TreeNode2* n2) const {
        return n1 == n2;
    }
};

using base1 = RBTreeAlgorithmImpl::RBTreeAlgorithm<TreeNodeOps, TreeNode*, int,
                                                   false, false>;
using base2 = RBTreeAlgorithmImpl::RBTreeAlgorithm<TreeNode2Ops, TreeNode2*,
                                                   int, false, true>;
using base3 = RBTreeAlgorithmImpl::RBTreeAlgorithm<TreeNodeOps, TreeNode*, int,
                                                   true, false>;
using base4 = RBTreeAlgorithmImpl::RBTreeAlgorithm<TreeNode2Ops, TreeNode2*,
                                                   int, true, true>;

template <typename RBTreeAlgo, typename NODE, typename Ops>
struct rbtree : public RBTreeAlgo {
    using NN = std::remove_pointer_t<NODE>;
    using NodePath = typename RBTreeAlgo::NodePath;
    NODE root;

    rbtree() : RBTreeAlgo(Ops()), root(nullptr) {}

    bool insert(int val) {
        if (root == nullptr) {
            this->root = new NN(val);
            this->root->black = true;
            return true;
        } else {
            auto node = new NN(val);
            auto ans = this->insertNode(this->root, node);
            if (!this->exists(ans)) delete node;
            return this->exists(ans);
        }
    }

    std::optional<NODE> find(int key) {
        if (this->root == nullptr) return std::nullopt;

        auto ans = this->findNode(root, key);
        if (!this->exists(ans)) return std::nullopt;

        return this->getNode(ans);
    }

    std::optional<NodePath> lower_bound(int key) {
        if (this->root == nullptr) return std::nullopt;

        auto ans = RBTreeAlgo::lower_bound(this->root, key);
        if (!this->exists(ans)) return std::nullopt;

        return ans;
    }

    std::optional<NodePath> upper_bound(int key) {
        if (this->root == nullptr) return std::nullopt;

        auto ans = RBTreeAlgo::upper_bound(this->root, key);
        if (!this->exists(ans)) return std::nullopt;

        return ans;
    }

    NodePath begin() { return RBTreeAlgo::begin(this->root); }

    void forward(NodePath& path) { RBTreeAlgo::forward(this->root, path); }

    void backward(NodePath& path) { RBTreeAlgo::backward(this->root, path); }

    bool deleteByKey(int key) {
        if (this->root == nullptr) return false;

        auto ans = this->findNode(root, key);
        if (!this->exists(ans)) return false;

        auto node = this->deleteNode(this->root, ans);
        delete node;
        return true;
    }

    void check_consistency() { RBTreeAlgo::check_consistency(this->root); }

    ~rbtree() {
        if (root) delete root;
    }
};

static std::default_random_engine generator;
template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_insert(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);

        const auto nofound = vals.find(val) == vals.end();
        ASSERT_EQ(tree.insert(val), nofound);
        if (i % 1000 == 0 || i + 1 == n) tree.check_consistency();
        vals.insert(val);
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_find(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        auto a1 = tree.find(val);
        auto a2 = vals.find(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(a1.value()->key, val);
        }
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_delete(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        auto a1 = tree.deleteByKey(val);
        if (i % 1000 == 0 || i + 1 == n) tree.check_consistency();
        auto k = vals.find(val);
        auto a2 = k != vals.end();
        if (a2) vals.erase(k);

        ASSERT_EQ(a1, a2);
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_delete2(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    auto beg = vals.begin();
    int i = 0;
    for (auto val : vals) {
        auto a1 = tree.deleteByKey(val);
        if (i % 1000 == 0 || i + 1 == vals.size()) tree.check_consistency();
        ASSERT_TRUE(a1);
        i++;
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_lower_bound(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        auto a1 = tree.lower_bound(val);
        auto a2 = vals.lower_bound(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(tree.getNode(a1.value())->key, *a2);
        }
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_upper_bound(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        auto a1 = tree.upper_bound(val);
        auto a2 = vals.upper_bound(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(tree.getNode(a1.value())->key, *a2);
        }
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_forward_backward(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,
                                                    n * 3);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::set<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    auto tbeg = tree.begin();
    auto beg = vals.begin();
    for (; beg != vals.end(); beg++, tree.forward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }

    ASSERT_TRUE(!tree.exists(tbeg));

    if (n > 0) {
        beg--;
        tree.backward(tbeg);
    }
    for (; beg != vals.begin(); beg--, tree.backward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }
    if (n > 0) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_insert_dup(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);

        ASSERT_TRUE(tree.insert(val));
        if (i % 1000 == 0 || i + 1 == n) tree.check_consistency();
        vals.insert(val);
    }

    auto tbeg = tree.begin();
    auto beg = vals.begin();
    for (; beg != vals.end(); beg++, tree.forward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }
    ASSERT_EQ(vals.size(), n);
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_find_dup_count(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }
    tree.check_consistency();

    for (int i = 0; i < (int)n / 2 + 10; ++i) {
        auto val = distribution(generator);

        auto lb_opt = tree.lower_bound(val);
        size_t count1 = 0;
        if (lb_opt.has_value()) {
            auto path = lb_opt.value();
            while (tree.exists(path)) {
                if (Ops().getKey(tree.getNode(path)) == val) {
                    count1++;
                    tree.forward(path);
                } else {
                    break;
                }
            }
        }

        size_t count2 = vals.count(val);
        ASSERT_EQ(count1, count2);
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_delete_dup(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);

        bool a1 = tree.deleteByKey(val);
        if (i % 1000 == 0 || i + 1 == n) tree.check_consistency();

        auto it = vals.find(val);
        bool a2 = (it != vals.end());
        if (a2) {
            vals.erase(it);
        }
        ASSERT_EQ(a1, a2);
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_delete2_dup(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    auto vals_copy = vals;
    int i = 0;
    for (auto val : vals_copy) {
        auto a1 = tree.deleteByKey(val);
        if (i % 1000 == 0 || i + 1 == vals_copy.size())
            tree.check_consistency();
        ASSERT_TRUE(a1);
        i++;
    }
    ASSERT_EQ(tree.root, nullptr);
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_lower_bound_dup(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        auto a1 = tree.lower_bound(val);
        auto a2 = vals.lower_bound(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(tree.getNode(a1.value())->key, *a2);
        }
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_upper_bound_dup(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        auto a1 = tree.upper_bound(val);
        auto a2 = vals.upper_bound(val);

        ASSERT_EQ(a1.has_value(), a2 != vals.end());
        if (a1.has_value()) {
            ASSERT_EQ(tree.getNode(a1.value())->key, *a2);
        }
    }
}

template <typename RBTreeAlgo, typename NODE, typename Ops>
static void test_rbtree_forward_backward_dup(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) / 2,
                                                    n / 2);
    rbtree<RBTreeAlgo, NODE, Ops> tree;
    std::multiset<int> vals;

    for (size_t i = 0; i < n; i++) {
        auto val = distribution(generator);
        tree.insert(val);
        vals.insert(val);
    }

    auto tbeg = tree.begin();
    auto beg = vals.begin();
    for (; beg != vals.end(); beg++, tree.forward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }

    ASSERT_TRUE(!tree.exists(tbeg));

    if (n > 0) {
        beg--;
        tree.backward(tbeg);
    }
    for (; beg != vals.begin(); beg--, tree.backward(tbeg)) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }
    if (n > 0) {
        ASSERT_TRUE(tree.exists(tbeg));
        auto val = *beg;
        auto v2 = tree.getNode(tbeg)->key;
        ASSERT_EQ(val, v2);
    }
}

#define SETUP_TEST_FUNC_V1(func)                      \
    func<base1, TreeNode*, TreeNodeOps>(SN_0);        \
    func<base1, TreeNode*, TreeNodeOps>(SN_1);        \
    func<base1, TreeNode*, TreeNodeOps>(SN_2);        \
    func<base1, TreeNode*, TreeNodeOps>(SN_3);        \
    func<base1, TreeNode*, TreeNodeOps>(SN_4);        \
    func<base1, TreeNode*, TreeNodeOps>(SN_5);        \
    func<base1, TreeNode*, TreeNodeOps>(SN_10);       \
    func<base1, TreeNode*, TreeNodeOps>(SN_100);      \
    func<base1, TreeNode*, TreeNodeOps>(SN_1000);     \
    func<base1, TreeNode*, TreeNodeOps>(SN_10000);    \
    func<base1, TreeNode*, TreeNodeOps>(SN_100000);   \
    func<base1, TreeNode*, TreeNodeOps>(SN_1000000);  \
    func<base1, TreeNode*, TreeNodeOps>(SN_10000000); \
    func<base1, TreeNode*, TreeNodeOps>(SN_100000000)

#define SETUP_TEST_FUNC_V2(func)                        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_0);        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_1);        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_2);        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_3);        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_4);        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_5);        \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_10);       \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_100);      \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_1000);     \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_10000);    \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_100000);   \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_1000000);  \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_10000000); \
    func<base2, TreeNode2*, TreeNode2Ops>(SN_100000000)

#define SETUP_TEST_FUNC_V3(func)                      \
    func<base3, TreeNode*, TreeNodeOps>(SN_0);        \
    func<base3, TreeNode*, TreeNodeOps>(SN_1);        \
    func<base3, TreeNode*, TreeNodeOps>(SN_2);        \
    func<base3, TreeNode*, TreeNodeOps>(SN_3);        \
    func<base3, TreeNode*, TreeNodeOps>(SN_4);        \
    func<base3, TreeNode*, TreeNodeOps>(SN_5);        \
    func<base3, TreeNode*, TreeNodeOps>(SN_10);       \
    func<base3, TreeNode*, TreeNodeOps>(SN_100);      \
    func<base3, TreeNode*, TreeNodeOps>(SN_1000);     \
    func<base3, TreeNode*, TreeNodeOps>(SN_10000);    \
    func<base3, TreeNode*, TreeNodeOps>(SN_100000);   \
    func<base3, TreeNode*, TreeNodeOps>(SN_1000000);  \
    func<base3, TreeNode*, TreeNodeOps>(SN_10000000); \
    func<base3, TreeNode*, TreeNodeOps>(SN_100000000)

#define SETUP_TEST_FUNC_V4(func)                        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_0);        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_1);        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_2);        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_3);        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_4);        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_5);        \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_10);       \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_100);      \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_1000);     \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_10000);    \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_100000);   \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_1000000);  \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_10000000); \
    func<base4, TreeNode2*, TreeNode2Ops>(SN_100000000)

TEST(rbtree_without_parent_ops, insert) {
    SETUP_TEST_FUNC_V1(test_rbtree_insert);
}
TEST(rbtree_with_parent_ops, insert) { SETUP_TEST_FUNC_V2(test_rbtree_insert); }

TEST(rbtree_without_parent_ops, find) { SETUP_TEST_FUNC_V1(test_rbtree_find); }
TEST(rbtree_with_parent_ops, find) { SETUP_TEST_FUNC_V2(test_rbtree_find); }

TEST(rbtree_without_parent_ops, delete) {
    SETUP_TEST_FUNC_V1(test_rbtree_delete);
}
TEST(rbtree_with_parent_ops, delete) { SETUP_TEST_FUNC_V2(test_rbtree_delete); }

TEST(rbtree_without_parent_ops, delete2) {
    SETUP_TEST_FUNC_V1(test_rbtree_delete2);
}
TEST(rbtree_with_parent_ops, delete2) {
    SETUP_TEST_FUNC_V2(test_rbtree_delete2);
}

TEST(rbtree_without_parent_ops, lower_bound) {
    SETUP_TEST_FUNC_V1(test_rbtree_lower_bound);
}
TEST(rbtree_with_parent_ops, lower_bound) {
    SETUP_TEST_FUNC_V2(test_rbtree_lower_bound);
}

TEST(rbtree_without_parent_ops, upper_bound) {
    SETUP_TEST_FUNC_V1(test_rbtree_upper_bound);
}
TEST(rbtree_with_parent_ops, upper_bound) {
    SETUP_TEST_FUNC_V2(test_rbtree_upper_bound);
}

TEST(rbtree_without_parent_ops, forward_backward) {
    SETUP_TEST_FUNC_V1(test_rbtree_forward_backward);
}
TEST(rbtree_with_parent_ops, forward_backward) {
    SETUP_TEST_FUNC_V2(test_rbtree_forward_backward);
}

TEST(rbtree_without_parent_ops_dup, insert) {
    SETUP_TEST_FUNC_V3(test_rbtree_insert_dup);
}
TEST(rbtree_with_parent_ops_dup, insert) {
    SETUP_TEST_FUNC_V4(test_rbtree_insert_dup);
}

TEST(rbtree_without_parent_ops_dup, find_count) {
    SETUP_TEST_FUNC_V3(test_rbtree_find_dup_count);
}
TEST(rbtree_with_parent_ops_dup, find_count) {
    SETUP_TEST_FUNC_V4(test_rbtree_find_dup_count);
}

TEST(rbtree_without_parent_ops_dup, delete) {
    SETUP_TEST_FUNC_V3(test_rbtree_delete_dup);
}
TEST(rbtree_with_parent_ops_dup, delete) {
    SETUP_TEST_FUNC_V4(test_rbtree_delete_dup);
}

TEST(rbtree_without_parent_ops_dup, delete2) {
    SETUP_TEST_FUNC_V3(test_rbtree_delete2_dup);
}
TEST(rbtree_with_parent_ops_dup, delete2) {
    SETUP_TEST_FUNC_V4(test_rbtree_delete2_dup);
}

TEST(rbtree_without_parent_ops_dup, lower_bound) {
    SETUP_TEST_FUNC_V3(test_rbtree_lower_bound_dup);
}
TEST(rbtree_with_parent_ops_dup, lower_bound) {
    SETUP_TEST_FUNC_V4(test_rbtree_lower_bound_dup);
}

TEST(rbtree_without_parent_ops_dup, upper_bound) {
    SETUP_TEST_FUNC_V3(test_rbtree_upper_bound_dup);
}
TEST(rbtree_with_parent_ops_dup, upper_bound) {
    SETUP_TEST_FUNC_V4(test_rbtree_upper_bound_dup);
}

TEST(rbtree_without_parent_ops_dup, forward_backward) {
    SETUP_TEST_FUNC_V3(test_rbtree_forward_backward_dup);
}
TEST(rbtree_with_parent_ops_dup, forward_backward) {
    SETUP_TEST_FUNC_V4(test_rbtree_forward_backward_dup);
}
