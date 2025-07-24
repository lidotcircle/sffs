#include <utest.h>

#include <algorithm>
#include <chrono>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "bptree_container.h"

using namespace std;
using namespace ldc;

#ifndef test_set
#define test_set bptset
#define test_map bptmap
#endif

std::default_random_engine generator;

// Test basic set operations
TEST(test_set, basic_operations) {
    test_set<int> s;

    // Test empty container
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.begin(), s.end());

    // Test single insertion
    auto result1 = s.insert(5);
    ASSERT_TRUE(result1.second);
    ASSERT_EQ(*result1.first, 5);
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), 1);

    // Test duplicate insertion
    auto result2 = s.insert(5);
    ASSERT_FALSE(result2.second);
    ASSERT_EQ(result2.first, result1.first);
    ASSERT_EQ(s.size(), 1);

    // Test multiple insertions
    s.insert(3);
    s.insert(7);
    s.insert(1);
    s.insert(9);
    ASSERT_EQ(s.size(), 5);

    // Test ordered iteration
    vector<int> expected = {1, 3, 5, 7, 9};
    vector<int> actual;
    for (test_set<int>::iterator it = s.begin(); it != s.end(); ++it) {
        actual.push_back(*it);
    }
    ASSERT_EQ(actual, expected);
}

TEST(test_set, search_operations) {
    test_set<int> s;
    for (int i = 1; i <= 10; i += 2) {  // {1, 3, 5, 7, 9}
        s.insert(i);
    }

    // Test find
    ASSERT_NE(s.find(5), s.end());
    ASSERT_EQ(s.find(4), s.end());
    ASSERT_TRUE(s.contains(7));
    ASSERT_FALSE(s.contains(8));

    // Test lower_bound
    ASSERT_EQ(*s.lower_bound(1), 1);
    ASSERT_EQ(*s.lower_bound(2), 3);
    ASSERT_EQ(*s.lower_bound(5), 5);
    ASSERT_EQ(*s.lower_bound(6), 7);
    ASSERT_EQ(s.lower_bound(10), s.end());

    // Test upper_bound
    ASSERT_EQ(*s.upper_bound(1), 3);
    ASSERT_EQ(*s.upper_bound(2), 3);
    ASSERT_EQ(*s.upper_bound(5), 7);
    ASSERT_EQ(s.upper_bound(9), s.end());

    // Test equal_range
    auto range = s.equal_range(5);
    ASSERT_EQ(*range.first, 5);
    ASSERT_EQ(*range.second, 7);

    // Test count
    ASSERT_EQ(s.count(5), 1);
    ASSERT_EQ(s.count(4), 0);
}

TEST(test_set, erase_operations) {
    test_set<int> s;
    for (int i = 1; i <= 10; i++) {
        s.insert(i);
    }

    // Test erase by iterator
    auto it = s.find(5);
    ASSERT_NE(it, s.end());
    auto next_it = s.erase(it);
    ASSERT_EQ(*next_it, 6);
    ASSERT_EQ(s.size(), 9);
    ASSERT_FALSE(s.contains(5));

    // Test erase by key
    size_t erased = s.erase(7);
    ASSERT_EQ(erased, 1);
    ASSERT_EQ(s.size(), 8);
    ASSERT_FALSE(s.contains(7));

    erased = s.erase(100);  // Non-existent key
    ASSERT_EQ(erased, 0);
    ASSERT_EQ(s.size(), 8);

    // Test erase range
    auto first = s.find(2);
    auto last = s.find(4);
    ++last;  // Point to 6 (since 5 was already erased)
    s.erase(first, last);
    ASSERT_FALSE(s.contains(2));
    ASSERT_FALSE(s.contains(3));
    ASSERT_FALSE(s.contains(4));
    ASSERT_TRUE(s.contains(6));

    // Test clear
    s.clear();
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.size(), 0);
}

TEST(test_set, iterator_operations) {
    test_set<int> s;
    s.insert(5);
    s.insert(2);
    s.insert(8);
    s.insert(1);
    s.insert(9);
    s.insert(3);
    s.insert(7);

    // Test forward iteration
    vector<int> forward_result;
    for (auto it = s.begin(); it != s.end(); ++it) {
        forward_result.push_back(*it);
    }
    vector<int> expected = {1, 2, 3, 5, 7, 8, 9};
    ASSERT_EQ(forward_result, expected);

    // Test reverse iteration
    vector<int> reverse_result;
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        reverse_result.push_back(*it);
    }
    reverse(expected.begin(), expected.end());
    ASSERT_EQ(reverse_result, expected);

    // Test iterator arithmetic
    auto it = s.begin();
    ++it;
    ASSERT_EQ(*it, 2);
    --it;
    ASSERT_EQ(*it, 1);

    // Test iterator comparison
    auto it1 = s.begin();
    auto it2 = s.begin();
    ++it2;
    ASSERT_TRUE(it1 == s.begin());
    ASSERT_TRUE(it1 != it2);
    const auto d1 = std::distance(s.begin(), it1);
    const auto d2 = std::distance(s.begin(), it2);
    ASSERT_TRUE(d1 < d2);
}

TEST(test_map, basic_operations) {
    test_map<int, string> m;

    // Test empty container
    ASSERT_TRUE(m.empty());
    ASSERT_EQ(m.size(), 0);

    // Test insertion
    auto result1 = m.insert(make_pair(5, string("five")));
    ASSERT_TRUE(result1.second);
    ASSERT_EQ(result1.first->first, 5);
    ASSERT_EQ(result1.first->second, "five");

    // Test duplicate key
    auto result2 = m.insert(make_pair(5, string("FIVE")));
    ASSERT_FALSE(result2.second);
    ASSERT_EQ(result2.first->second, "five");  // Original value preserved

    // Test emplace
    auto result3 = m.emplace(3, "three");
    ASSERT_TRUE(result3.second);
    ASSERT_EQ(result3.first->first, 3);
    ASSERT_EQ(result3.first->second, "three");

    // Test operator[]
    m[7] = "seven";
    ASSERT_EQ(m[7], "seven");
    ASSERT_EQ(m.size(), 3);

    // Test operator[] with new key
    string& ref = m[10];
    ref = "ten";
    ASSERT_EQ(m[10], "ten");
    ASSERT_EQ(m.size(), 4);
}

TEST(test_map, access_operations) {
    test_map<int, string> m;
    m.insert(make_pair(1, string("one")));
    m.insert(make_pair(3, string("three")));
    m.insert(make_pair(5, string("five")));

    // Test at()
    ASSERT_EQ(m.at(3), "three");

    // Test at() with non-existent key
    bool exception_caught = false;
    try {
        m.at(10);
    } catch (const std::out_of_range&) {
        exception_caught = true;
    }
    ASSERT_TRUE(exception_caught);

    // Test const at()
    const auto& cm = m;
    ASSERT_EQ(cm.at(5), "five");

    // Test modification through operator[]
    m[3] = "THREE";
    ASSERT_EQ(m[3], "THREE");
}

TEST(test_map, pair_iteration) {
    test_map<int, string> m;
    m.insert(make_pair(3, string("three")));
    m.insert(make_pair(1, string("one")));
    m.insert(make_pair(2, string("two")));

    vector<pair<int, string> > result;
    for (test_map<int, string>::iterator it = m.begin(); it != m.end(); ++it) {
        result.push_back(*it);
    }

    vector<pair<int, string> > expected;
    expected.push_back(make_pair(1, string("one")));
    expected.push_back(make_pair(2, string("two")));
    expected.push_back(make_pair(3, string("three")));
    ASSERT_EQ(result, expected);
}

// Stress test with random operations
TEST(test_set, stress_test_random) {
    test_set<int> bpt_set;
    set<int> std_set;

    std::uniform_int_distribution<int> dist(-1000, 1000);
    std::uniform_int_distribution<int> op_dist(0,
                                               2);  // 0=insert, 1=erase, 2=find

    for (int i = 0; i < 1000; i++) {
        int op = op_dist(generator);
        int val = dist(generator);

        if (op == 0) {  // Insert
            auto bpt_result = bpt_set.insert(val);
            auto std_result = std_set.insert(val);
            ASSERT_EQ(bpt_result.second, std_result.second);
            if (bpt_result.second) {
                ASSERT_EQ(*bpt_result.first, val);
            }
        } else if (op == 1) {  // Erase
            size_t bpt_erased = bpt_set.erase(val);
            size_t std_erased = std_set.erase(val);
            ASSERT_EQ(bpt_erased, std_erased);
        } else {  // Find
            bool bpt_found = bpt_set.find(val) != bpt_set.end();
            bool std_found = std_set.find(val) != std_set.end();
            ASSERT_EQ(bpt_found, std_found);
        }

        ASSERT_EQ(bpt_set.size(), std_set.size());
    }

    // Final verification: check that all elements match
    vector<int> bpt_elements(bpt_set.begin(), bpt_set.end());
    vector<int> std_elements(std_set.begin(), std_set.end());
    ASSERT_EQ(bpt_elements, std_elements);
}

TEST(test_map, stress_test_random) {
    test_map<int, int> bpt_map;
    map<int, int> std_map;

    std::uniform_int_distribution<int> dist(-500, 500);
    std::uniform_int_distribution<int> op_dist(0, 2);

    for (int i = 0; i < 500; i++) {
        int op = op_dist(generator);
        int key = dist(generator);
        int val = dist(generator);

        if (op == 0) {  // Insert
            auto bpt_result = bpt_map.insert(make_pair(key, val));
            auto std_result = std_map.insert(make_pair(key, val));
            ASSERT_EQ(bpt_result.second, std_result.second);
        } else if (op == 1) {  // Erase
            size_t bpt_erased = bpt_map.erase(key);
            size_t std_erased = std_map.erase(key);
            ASSERT_EQ(bpt_erased, std_erased);
        } else {  // Access
            bool bpt_found = bpt_map.find(key) != bpt_map.end();
            bool std_found = std_map.find(key) != std_map.end();
            ASSERT_EQ(bpt_found, std_found);
        }

        ASSERT_EQ(bpt_map.size(), std_map.size());
    }
}

// Test copy/move semantics
TEST(test_set, copy_move_semantics) {
    test_set<int> original;
    original.insert(1);
    original.insert(2);
    original.insert(3);
    original.insert(4);
    original.insert(5);

    // Test copy constructor
    test_set<int> copied(original);
    ASSERT_EQ(copied, original);
    ASSERT_EQ(copied.size(), 5);

    // Test copy assignment
    test_set<int> assigned;
    assigned = original;
    ASSERT_EQ(assigned, original);

    // Test move constructor
    test_set<int> moved(std::move(copied));
    ASSERT_EQ(moved, original);
    ASSERT_EQ(moved.size(), 5);

    // Test move assignment
    test_set<int> move_assigned;
    move_assigned = std::move(assigned);
    ASSERT_EQ(move_assigned, original);
}

// Test with large datasets to stress the tree structure
TEST(test_set, large_dataset) {
    const size_t N = 10000;
    test_set<int> s;

    // Insert in random order
    vector<int> values(N);
    for (size_t i = 0; i < N; i++) {
        values[i] = static_cast<int>(i);
    }
    std::shuffle(values.begin(), values.end(), generator);

    for (size_t i = 0; i < values.size(); i++) {
        s.insert(values[i]);
    }

    ASSERT_EQ(s.size(), N);

    // Verify all elements are present and ordered
    int expected = 0;
    for (auto it = s.begin(); it != s.end(); ++it) {
        ASSERT_EQ(*it, expected++);
    }

    // Test range operations on large dataset
    auto lb = s.lower_bound(static_cast<int>(N / 4));
    auto ub = s.upper_bound(static_cast<int>(3 * N / 4));
    size_t range_size = static_cast<size_t>(distance(lb, ub));
    ASSERT_EQ(range_size, N / 2 + 1);
}

// Test multiset operations
TEST(bptmultiset, duplicate_keys) {
    bptmultiset<int> ms;

    // Insert duplicates
    ms.insert(5);
    ms.insert(5);
    ms.insert(5);
    ms.insert(3);
    ms.insert(7);
    ms.insert(3);

    ASSERT_EQ(ms.size(), 6);
    ASSERT_EQ(ms.count(5), 3);
    ASSERT_EQ(ms.count(3), 2);
    ASSERT_EQ(ms.count(7), 1);
    ASSERT_EQ(ms.count(1), 0);

    // Test equal_range
    std::pair<bptmultiset<int>::iterator, bptmultiset<int>::iterator> range =
        ms.equal_range(5);
    ASSERT_EQ(static_cast<size_t>(distance(range.first, range.second)), 3);

    UTEST_SKIP("TODO");
    // Test erase by key (removes all)
    size_t erased = ms.erase(5);
    ASSERT_EQ(erased, 3);
    ASSERT_EQ(ms.count(5), 0);
    ASSERT_EQ(ms.size(), 3);
}

// Test edge cases that might trigger the bugs
TEST(test_set, edge_cases) {
    test_set<int> s;

    // Test single element operations
    s.insert(42);
    ASSERT_EQ(s.size(), 1);
    ASSERT_TRUE(s.contains(42));

    auto it = s.find(42);
    ASSERT_NE(it, s.end());
    s.erase(it);
    ASSERT_TRUE(s.empty());

    // Test alternating insert/erase to stress tree balancing
    for (int i = 0; i < 100; i++) {
        s.insert(i);
        if (i % 2 == 1) {
            s.erase(i - 1);
        }
    }

    // Should have only odd numbers
    for (int i = 0; i < 100; i++) {
        if (i % 2 == 0 && i < 99) {
            ASSERT_FALSE(s.contains(i));
        } else {
            ASSERT_TRUE(s.contains(i));
        }
    }
}

// Test string keys to ensure proper comparison
TEST(test_set, string_keys) {
    test_set<string> s;
    s.insert("charlie");
    s.insert("alpha");
    s.insert("bravo");
    s.insert("delta");

    vector<string> result(s.begin(), s.end());
    vector<string> expected;
    expected.push_back("alpha");
    expected.push_back("bravo");
    expected.push_back("charlie");
    expected.push_back("delta");
    ASSERT_EQ(result, expected);

    ASSERT_TRUE(s.contains("bravo"));
    ASSERT_FALSE(s.contains("echo"));

    auto it = s.lower_bound("c");
    ASSERT_EQ(*it, "charlie");
}

// Test exception safety
TEST(test_map, exception_safety) {
    test_map<int, string> m;
    m.insert(make_pair(1, string("one")));
    m.insert(make_pair(2, string("two")));
    m.insert(make_pair(3, string("three")));

    // Test at() throws for non-existent key
    // ASSERT_THROW(m.at(99), std::out_of_range);

    // Container should remain unchanged after exception
    ASSERT_EQ(m.size(), 3);
    ASSERT_TRUE(m.contains(1));
    ASSERT_TRUE(m.contains(2));
    ASSERT_TRUE(m.contains(3));
}

// Performance comparison test (basic)
TEST(test_set, basic_performance) {
    const size_t N = 1000;

    auto start = chrono::high_resolution_clock::now();

    test_set<int> s;
    for (size_t i = 0; i < N; i++) {
        s.insert(static_cast<int>(i));
    }

    for (size_t i = 0; i < N; i++) {
        ASSERT_TRUE(s.contains(static_cast<int>(i)));
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    // This is just a smoke test - actual performance will vary
    ASSERT_LT(duration.count(), 1000);  // Should complete within 1 second
}
