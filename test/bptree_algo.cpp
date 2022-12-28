#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <optional>
#include <variant>
#include "sffs.h"
using namespace SFFS;


template<size_t Order=128, size_t Order2=Order*2>
struct TreeNode {
    bool isLeaf;
    struct InteriorNode {
        std::array<TreeNode*,2*Order> children;
        std::array<std::optional<int>,2*Order - 1> keys;
        size_t numOfChildren, numOfKeys;

        InteriorNode():
            numOfChildren(0), numOfKeys(0)
        {
            std::fill(children.begin(), children.end(), nullptr);
            std::fill(keys.begin(), keys.end(), std::nullopt);
        };
    };
    struct LeafNode {
        std::array<std::optional<int>,2*Order2 - 1> datas;
        size_t numOfHolders;
        TreeNode *prev, *next;

        LeafNode(): numOfHolders(0), prev(nullptr), next(nullptr)
        {
            std::fill(datas.begin(), datas.end(), std::nullopt);
        }
    };
    std::variant<InteriorNode,LeafNode> nn;
    TreeNode* parent;

    explicit TreeNode(bool isLeaf):
        parent(nullptr), isLeaf(isLeaf),
        nn(isLeaf ? decltype(nn)(LeafNode()) : decltype(nn)(InteriorNode()))
    {
    }

    InteriorNode& interior() {
        assert(!this->isLeaf);
        return std::get<InteriorNode>(nn);
    }

    const InteriorNode& interior() const {
        assert(!this->isLeaf);
        return std::get<InteriorNode>(nn);
    }

    LeafNode& leaf() {
        assert(this->isLeaf);
        return std::get<LeafNode>(nn);
    }

    const LeafNode& leaf() const {
        assert(this->isLeaf);
        return std::get<LeafNode>(nn);
    }

    ~TreeNode() {
        if (!this->isLeaf) {
            auto& vn = std::get<InteriorNode>(nn);
            for (auto child: vn.children) {
                if (child != nullptr) {
                    delete child;
                }
            }
        }
    }
};

template<size_t Order=128, size_t Order2=2*Order>
struct TreeNodeOps {
    typedef TreeNode<Order,Order2> *NODE;
    using HOLDER = int;
    using KEY = int;

    inline bool isLeaf(NODE node) const { return node->isLeaf; }

    inline NODE getNthChild(NODE node, size_t nth) const {
        assert(nth < node->interior().children.size());
        return node->interior().children[nth];
    }
    inline void setNthChild (NODE node, size_t nth, NODE n)  {
        assert(nth < node->interior().children.size());
        if (n == nullptr) {
            assert(node->interior().numOfChildren > 0);
            assert(node->interior().children[nth] != nullptr);
            node->interior().numOfChildren--;
        } else {
            assert(node->interior().children[nth] == nullptr);
            node->interior().numOfChildren++;
        }
        node->interior().children[nth] = n;
    }

    inline HOLDER getNthHolder(NODE node, size_t nth) const {
        assert(nth < node->leaf().datas.size());
        return node->leaf().datas[nth].value();
    }
    inline KEY interiorGetNthKey(NODE node, size_t nth) const {
        assert(nth < node->interior().keys.size());
        return node->interior().keys[nth].value();
    }
    inline void interiorSetNthKey(NODE node, size_t nth, const KEY& key) {
        assert(!node->interior().keys[nth].has_value());
        node->interior().keys[nth] = key;
    }
    inline void interiorClearNthKey(NODE node, size_t nth) {
        assert(!node->interior().keys[nth].has_value());
        node->interior().keys[nth] = std::nullopt;
    }

    inline HOLDER extractNthHolder(NODE node, size_t nth) {
        assert(nth < node->leaf().datas.size());
        assert(node->leaf().numOfHolders > 0);
        auto n = node->leaf().datas[nth].value();
        node->leaf().datas[nth] = std::nullopt;
        node->leaf().numOfHolders--;
        return n;
    }

    inline void setNthHolder(NODE node, size_t nth, HOLDER&& holder) {
        assert(nth < node->leaf().datas.size());
        auto k = node->leaf().datas[nth];
        assert(!k.has_value());
        node->leaf().datas[nth] = std::move(holder);
        node->leaf().numOfHolders++;
    }

    inline NODE getParent(NODE node) const  {
        return node->parent;
    }
    inline void setParent(NODE node, NODE n)  {
        node->parent = n;
    }

    inline NODE leafGetNext(NODE node) const  {
        return node->leaf().next;
    }
    inline void leafSetNext(NODE node, NODE n)  {
        node->leaf().next = n;
    }
    inline NODE leafGetPrev(NODE node) const  {
        return node->leaf().prev;
    }
    inline void leafSetPrev(NODE node, NODE n)  {
        node->leaf().prev = n;
    }

    inline size_t leafGetOrder() const { return Order2; }

    inline size_t getNumberOfChildren(NODE node) const { return node->interior().numOfChildren; }
    inline size_t leafGetNumberOfKeys(NODE node) const { return node->leaf().numOfHolders; }

    inline size_t interiorGetOrder() const { return Order; }
    inline size_t interiorGetNumberOfKeys(NODE node) const { return node->interior().numOfKeys; }

    inline bool isNullNode(NODE node) const { return node == nullptr; }
    inline NODE getNullNode() const { return nullptr; }
    inline NODE interiorCreateEmptyNode() { return new TreeNode<Order,Order2>(false); }
    inline NODE leafCreateEmptyNode() { return new TreeNode<Order,Order2>(true); }
    inline void releaseEmptyNode(NODE&& node) { delete node; }

    inline KEY getKey(const HOLDER& n) const { return n; }

    inline bool keyCompareLess(const KEY& lhs, const KEY& rhs) const { return lhs < rhs; }

    inline bool keyCompareEqual(const KEY& lhs, const KEY& rhs) const { return lhs  == rhs; }

    inline bool nodeCompareEqual(NODE lhs, NODE rhs) const { return lhs == rhs; }
};

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
using BASE_T = BPTreeAlgorithmImpl::BPTreeAlgorithm<TreeNodeOps<Order,Order2>,TreeNode<Order,Order2>*,int,int,parent_ops>;

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
struct BPTREE: public BASE_T<Order,parent_ops,prev_ops,Order2> {
    typedef TreeNode<Order>* NODE;

    using NN   = std::remove_pointer_t<NODE>;
    using BASE = BASE_T<Order,parent_ops,prev_ops,Order2>;
    using HolderPath = typename BASE::HolderPath;
    NODE root;

    BPTREE(): BASE(TreeNodeOps<Order>()), root(nullptr) {}

    bool insert(int val) {
        return this->insertHolder(this->root, std::move(val));
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

    ~BPTREE() { if (root) delete root; }
};


static std::default_random_engine generator;
template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_insert(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_find(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_delete(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_delete2(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_lower_bound(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_upper_bound(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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

template<size_t Order, bool parent_ops, bool prev_ops, size_t Order2=2*Order>
static void test_BPTREE_forward_backward(size_t n) {
    std::uniform_int_distribution<int> distribution(-static_cast<int>(n) * 3,n * 3);
    BPTREE<Order,parent_ops,prev_ops,Order2> tree;
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


#define SETUP_TEST_FUNC_N(func, Order, parent_ops,prev_ops,Order2) \
    func<Order,parent_ops,prev_ops,Order2>(0); \
    func<Order,parent_ops,prev_ops,Order2>(1); \
    func<Order,parent_ops,prev_ops,Order2>(2); \
    func<Order,parent_ops,prev_ops,Order2>(3); \
    func<Order,parent_ops,prev_ops,Order2>(4); \
    func<Order,parent_ops,prev_ops,Order2>(5); \
    func<Order,parent_ops,prev_ops,Order2>(10); \
    func<Order,parent_ops,prev_ops,Order2>(100); \
    func<Order,parent_ops,prev_ops,Order2>(1000); \
    func<Order,parent_ops,prev_ops,Order2>(10000); \
    func<Order,parent_ops,prev_ops,Order2>(100000); \
    func<Order,parent_ops,prev_ops,Order2>(1000000); \
    // func<Order,parent_ops,prev_ops,Order2>(10000000); \
    func<Order,parent_ops,prev_ops,Order2>(100000000)

#define SETUP_TEST_FUNC(func, parent_ops) \
    SETUP_TEST_FUNC_N(func, 2, parent_ops,false,2*2); \
    SETUP_TEST_FUNC_N(func, 3, parent_ops,false,2*3); \
    SETUP_TEST_FUNC_N(func, 4, parent_ops,false,2*4); \
    SETUP_TEST_FUNC_N(func, 8, parent_ops,false,2*8); \
    SETUP_TEST_FUNC_N(func, 16, parent_ops,false,2*16); \
    SETUP_TEST_FUNC_N(func, 32, parent_ops,false,2*32); \
    SETUP_TEST_FUNC_N(func, 64, parent_ops,false,2*64); \
    SETUP_TEST_FUNC_N(func, 128, parent_ops,false,2*128);


TEST(BPTREE_without_parent_ops, insert) {
    SETUP_TEST_FUNC(test_BPTREE_insert, false);
}
TEST(BPTREE_with_parent_ops, insert) {
    SETUP_TEST_FUNC(test_BPTREE_insert, true);
}

TEST(BPTREE_without_parent_ops, find) {
    SETUP_TEST_FUNC(test_BPTREE_find, false);
}
TEST(BPTREE_with_parent_ops, find) {
    SETUP_TEST_FUNC(test_BPTREE_find, true);
}

TEST(BPTREE_without_parent_ops, delete) {
    SETUP_TEST_FUNC(test_BPTREE_delete, false);
}
TEST(BPTREE_with_parent_ops, delete) {
    SETUP_TEST_FUNC(test_BPTREE_delete, true);
}

TEST(BPTREE_without_parent_ops, delete2) {
    SETUP_TEST_FUNC(test_BPTREE_delete2, false);
}
TEST(BPTREE_with_parent_ops, delete2) {
    SETUP_TEST_FUNC(test_BPTREE_delete2, true);
}

TEST(BPTREE_without_parent_ops, lower_bound) {
    SETUP_TEST_FUNC(test_BPTREE_lower_bound, false);
}
TEST(BPTREE_with_parent_ops, lower_bound) {
    SETUP_TEST_FUNC(test_BPTREE_lower_bound, true);
}

TEST(BPTREE_without_parent_ops, upper_bound) {
    SETUP_TEST_FUNC(test_BPTREE_upper_bound, false);
}
TEST(BPTREE_with_parent_ops, upper_bound) {
    SETUP_TEST_FUNC(test_BPTREE_upper_bound, true);
}

TEST(BPTREE_without_parent_ops, forward_backward) {
    SETUP_TEST_FUNC(test_BPTREE_forward_backward, false);
}
TEST(BPTREE_with_parent_ops, forward_backward) {
    SETUP_TEST_FUNC(test_BPTREE_forward_backward, true);
}
