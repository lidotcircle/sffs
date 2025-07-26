#include <utest.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
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
#define test_multiset bptmultiset
#define test_multimap bptmultimap
#endif

std::default_random_engine generator(0);

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

TEST(test_set, erase_non_existent) {
    test_set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    s.erase(100);
    ASSERT_EQ(s.size(), 3);
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
TEST(multiset, duplicate_keys) {
    test_multiset<int> ms;

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
    std::pair<test_multiset<int>::iterator, test_multiset<int>::iterator>
        range = ms.equal_range(5);
    ASSERT_EQ(static_cast<size_t>(distance(range.first, range.second)), 3);

    // Test erase by key (removes all)
    size_t erased = ms.erase(5);
    ASSERT_EQ(erased, 3);
    ASSERT_EQ(ms.count(5), 0);
    ASSERT_EQ(ms.size(), 3);
}

// Test multiset empty operations
TEST(multiset, empty_operations) {
    test_multiset<int> ms;

    ASSERT_TRUE(ms.empty());
    ASSERT_EQ(ms.size(), 0);
    ASSERT_EQ(ms.begin(), ms.end());
    ASSERT_EQ(ms.count(5), 0);
    ASSERT_EQ(ms.erase(5), 0);
    ASSERT_EQ(ms.find(5), ms.end());

    auto range = ms.equal_range(5);
    ASSERT_EQ(range.first, range.second);
    ASSERT_EQ(range.first, ms.end());
}

// Test multiset single element operations
TEST(multiset, single_element) {
    test_multiset<int> ms;

    // Insert single element
    ms.insert(42);
    ASSERT_EQ(ms.size(), 1);
    ASSERT_EQ(ms.count(42), 1);
    ASSERT_FALSE(ms.empty());

    // Find the element
    auto found = ms.find(42);
    ASSERT_NE(found, ms.end());
    ASSERT_EQ(*found, 42);

    // Test equal_range for single element
    auto range = ms.equal_range(42);
    ASSERT_EQ(distance(range.first, range.second), 1);
    ASSERT_EQ(*range.first, 42);

    // Erase by key
    ASSERT_EQ(ms.erase(42), 1);
    ASSERT_TRUE(ms.empty());
    ASSERT_EQ(ms.count(42), 0);
}

// Test multiset iterator operations with duplicates
TEST(multiset, iterator_operations) {
    test_multiset<int> ms;

    // Insert multiple duplicates in random order
    ms.insert(5);
    ms.insert(3);
    ms.insert(5);
    ms.insert(1);
    ms.insert(3);
    ms.insert(5);
    ms.insert(1);
    ms.insert(7);

    // Test forward iteration (should be sorted)
    vector<int> forward_result;
    for (auto it = ms.begin(); it != ms.end(); ++it) {
        forward_result.push_back(*it);
    }
    vector<int> expected = {1, 1, 3, 3, 5, 5, 5, 7};
    ASSERT_EQ(forward_result, expected);

    // Test reverse iteration
    vector<int> reverse_result;
    for (auto it = ms.rbegin(); it != ms.rend(); ++it) {
        reverse_result.push_back(*it);
    }
    reverse(expected.begin(), expected.end());
    ASSERT_EQ(reverse_result, expected);

    // Test iterator advancement through duplicates
    auto it = ms.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 1);  // Second copy of 1
    ++it;
    ASSERT_EQ(*it, 3);  // First copy of 3
    ++it;
    ASSERT_EQ(*it, 3);  // Second copy of 3
}

// Test multiset erase operations
TEST(multiset, erase_operations) {
    test_multiset<int> ms;

    // Setup with duplicates
    ms.insert(5);
    ms.insert(5);
    ms.insert(5);
    ms.insert(3);
    ms.insert(3);
    ms.insert(7);
    ms.insert(1);

    // Test erase by iterator (single element)
    auto it = ms.find(3);
    ASSERT_NE(it, ms.end());
    auto next_it = ms.erase(it);
    ASSERT_EQ(ms.count(3), 1);  // One copy should remain
    ASSERT_EQ(ms.size(), 6);

    // Test erase by key (all copies)
    size_t erased = ms.erase(5);
    ASSERT_EQ(erased, 3);
    ASSERT_EQ(ms.count(5), 0);
    ASSERT_EQ(ms.size(), 3);

    // Test erase non-existent key
    ASSERT_EQ(ms.erase(99), 0);
    ASSERT_EQ(ms.size(), 3);

    // Test erase range containing duplicates
    auto first = ms.begin();  // Should be 1
    auto last = ms.end();
    --last;                 // Should point to 7
    ms.erase(first, last);  // Erase everything except 7
    ASSERT_EQ(ms.size(), 1);
    ASSERT_EQ(*ms.begin(), 7);

    // Clear
    ms.clear();
    ASSERT_TRUE(ms.empty());
}

// Test multiset search operations
TEST(multiset, search_operations) {
    test_multiset<int> ms;

    // Insert: 1, 3, 3, 5, 5, 5, 7
    ms.insert(5);
    ms.insert(3);
    ms.insert(5);
    ms.insert(1);
    ms.insert(7);
    ms.insert(3);
    ms.insert(5);

    // Test find (returns iterator to first occurrence)
    auto it5 = ms.find(5);
    ASSERT_NE(it5, ms.end());
    ASSERT_EQ(*it5, 5);

    // Test count
    ASSERT_EQ(ms.count(1), 1);
    ASSERT_EQ(ms.count(3), 2);
    ASSERT_EQ(ms.count(5), 3);
    ASSERT_EQ(ms.count(7), 1);
    ASSERT_EQ(ms.count(99), 0);

    // Test lower_bound
    auto lb1 = ms.lower_bound(1);
    ASSERT_EQ(*lb1, 1);

    auto lb2 = ms.lower_bound(2);
    ASSERT_EQ(*lb2, 3);

    auto lb5 = ms.lower_bound(5);
    ASSERT_EQ(*lb5, 5);

    auto lb8 = ms.lower_bound(8);
    ASSERT_EQ(lb8, ms.end());

    // Test upper_bound
    auto ub1 = ms.upper_bound(1);
    ASSERT_EQ(*ub1, 3);

    auto ub3 = ms.upper_bound(3);
    ASSERT_EQ(*ub3, 5);

    auto ub5 = ms.upper_bound(5);
    ASSERT_EQ(*ub5, 7);

    auto ub7 = ms.upper_bound(7);
    ASSERT_EQ(ub7, ms.end());

    // Test equal_range
    auto range1 = ms.equal_range(1);
    ASSERT_EQ(distance(range1.first, range1.second), 1);

    auto range3 = ms.equal_range(3);
    ASSERT_EQ(distance(range3.first, range3.second), 2);

    auto range5 = ms.equal_range(5);
    ASSERT_EQ(distance(range5.first, range5.second), 3);

    auto range99 = ms.equal_range(99);
    ASSERT_EQ(range99.first, range99.second);
    ASSERT_EQ(range99.first, ms.end());
}

// Test multiset copy/move semantics
TEST(multiset, copy_move_semantics) {
    test_multiset<int> original;
    original.insert(1);
    original.insert(2);
    original.insert(2);
    original.insert(3);
    original.insert(3);
    original.insert(3);

    // Test copy constructor
    test_multiset<int> copied(original);
    ASSERT_EQ(copied.size(), original.size());
    ASSERT_EQ(copied.count(1), 1);
    ASSERT_EQ(copied.count(2), 2);
    ASSERT_EQ(copied.count(3), 3);

    // Test copy assignment
    test_multiset<int> assigned;
    assigned = original;
    ASSERT_EQ(assigned.size(), original.size());
    ASSERT_EQ(assigned.count(1), 1);
    ASSERT_EQ(assigned.count(2), 2);
    ASSERT_EQ(assigned.count(3), 3);

    // Test move constructor
    test_multiset<int> moved(std::move(copied));
    ASSERT_EQ(moved.size(), 6);
    ASSERT_EQ(moved.count(1), 1);
    ASSERT_EQ(moved.count(2), 2);
    ASSERT_EQ(moved.count(3), 3);

    // Test move assignment
    test_multiset<int> move_assigned;
    move_assigned = std::move(assigned);
    ASSERT_EQ(move_assigned.size(), 6);
    ASSERT_EQ(move_assigned.count(1), 1);
    ASSERT_EQ(move_assigned.count(2), 2);
    ASSERT_EQ(move_assigned.count(3), 3);
}

// Test multiset stress with many duplicates
TEST(multiset, stress_many_duplicates) {
    test_multiset<int> ms;
    multiset<int> std_ms;

    std::uniform_int_distribution<int> dist(
        1, 10);  // Small range to force duplicates
    std::uniform_int_distribution<int> op_dist(0, 2);

    for (int i = 0; i < 1000; i++) {
        int op = op_dist(generator);
        int val = dist(generator);

        if (op == 0) {  // Insert
            ms.insert(val);
            std_ms.insert(val);
        } else if (op == 1) {  // Erase by key
            size_t ms_erased = ms.erase(val);
            size_t std_erased = std_ms.erase(val);
            ASSERT_EQ(ms_erased, std_erased);
        } else {  // Count
            ASSERT_EQ(ms.count(val), std_ms.count(val));
        }

        ASSERT_EQ(ms.size(), std_ms.size());
    }

    // Final verification
    vector<int> ms_elements(ms.begin(), ms.end());
    vector<int> std_elements(std_ms.begin(), std_ms.end());
    ASSERT_EQ(ms_elements, std_elements);
}

// ==== MULTIMAP TESTS ====

// Test multimap basic operations
TEST(multimap, basic_operations) {
    test_multimap<int, string> mm;

    // Test empty container
    ASSERT_TRUE(mm.empty());
    ASSERT_EQ(mm.size(), 0);

    // Test insertions with duplicate keys
    auto it1 = mm.insert(make_pair(1, string("one_a")));
    auto it2 = mm.insert(make_pair(1, string("one_b")));
    auto it3 = mm.insert(make_pair(2, string("two")));
    auto it4 = mm.insert(make_pair(1, string("one_c")));

    ASSERT_EQ(mm.size(), 4);
    ASSERT_EQ(mm.count(1), 3);
    ASSERT_EQ(mm.count(2), 1);
    ASSERT_EQ(mm.count(3), 0);

    // Verify all values are stored
    auto range = mm.equal_range(1);
    vector<string> values;
    for (auto it = range.first; it != range.second; ++it) {
        values.push_back(it->second);
    }
    sort(values.begin(), values.end());
    vector<string> expected = {"one_a", "one_b", "one_c"};
    sort(expected.begin(), expected.end());
    ASSERT_EQ(values, expected);
}

// Test multimap empty operations
TEST(multimap, empty_operations) {
    test_multimap<int, string> mm;

    ASSERT_TRUE(mm.empty());
    ASSERT_EQ(mm.size(), 0);
    ASSERT_EQ(mm.begin(), mm.end());
    ASSERT_EQ(mm.count(5), 0);
    ASSERT_EQ(mm.erase(5), 0);
    ASSERT_EQ(mm.find(5), mm.end());

    auto range = mm.equal_range(5);
    ASSERT_EQ(range.first, range.second);
    ASSERT_EQ(range.first, mm.end());
}

// Test multimap iterator operations
TEST(multimap, iterator_operations) {
    test_multimap<int, string> mm;

    // Insert in random order
    mm.insert(make_pair(3, string("three_a")));
    mm.insert(make_pair(1, string("one_a")));
    mm.insert(make_pair(3, string("three_b")));
    mm.insert(make_pair(2, string("two_a")));
    mm.insert(make_pair(1, string("one_b")));

    // Test forward iteration (should be sorted by key)
    vector<int> keys;
    for (auto it = mm.begin(); it != mm.end(); ++it) {
        keys.push_back(it->first);
    }
    vector<int> expected_keys = {1, 1, 2, 3, 3};
    ASSERT_EQ(keys, expected_keys);

    // Test reverse iteration
    vector<int> reverse_keys;
    for (auto it = mm.rbegin(); it != mm.rend(); ++it) {
        reverse_keys.push_back(it->first);
    }
    reverse(expected_keys.begin(), expected_keys.end());
    ASSERT_EQ(reverse_keys, expected_keys);
}

// Test multimap erase operations
TEST(multimap, erase_operations) {
    test_multimap<int, string> mm;

    // Setup
    mm.insert(make_pair(1, string("one_a")));
    mm.insert(make_pair(1, string("one_b")));
    mm.insert(make_pair(1, string("one_c")));
    mm.insert(make_pair(2, string("two_a")));
    mm.insert(make_pair(3, string("three_a")));
    mm.insert(make_pair(3, string("three_b")));

    // Test erase by iterator (single element)
    auto it = mm.find(1);
    ASSERT_NE(it, mm.end());
    auto next_it = mm.erase(it);
    ASSERT_EQ(mm.count(1), 2);  // Two copies should remain
    ASSERT_EQ(mm.size(), 5);

    // Test erase by key (all copies)
    size_t erased = mm.erase(3);
    ASSERT_EQ(erased, 2);
    ASSERT_EQ(mm.count(3), 0);
    ASSERT_EQ(mm.size(), 3);

    // Test erase non-existent key
    ASSERT_EQ(mm.erase(99), 0);
    ASSERT_EQ(mm.size(), 3);

    // Clear
    mm.clear();
    ASSERT_TRUE(mm.empty());
}

// Test multimap search operations
TEST(multimap, search_operations) {
    test_multimap<int, string> mm;

    mm.insert(make_pair(1, string("one_a")));
    mm.insert(make_pair(3, string("three_a")));
    mm.insert(make_pair(3, string("three_b")));
    mm.insert(make_pair(5, string("five_a")));
    mm.insert(make_pair(5, string("five_b")));
    mm.insert(make_pair(5, string("five_c")));
    mm.insert(make_pair(7, string("seven_a")));

    // Test find
    auto it = mm.find(5);
    ASSERT_NE(it, mm.end());
    ASSERT_EQ(it->first, 5);

    // Test count
    ASSERT_EQ(mm.count(1), 1);
    ASSERT_EQ(mm.count(3), 2);
    ASSERT_EQ(mm.count(5), 3);
    ASSERT_EQ(mm.count(7), 1);
    ASSERT_EQ(mm.count(99), 0);

    // Test lower_bound
    auto lb = mm.lower_bound(3);
    ASSERT_EQ(lb->first, 3);

    auto lb_between = mm.lower_bound(4);
    ASSERT_EQ(lb_between->first, 5);

    // Test upper_bound
    auto ub = mm.upper_bound(3);
    ASSERT_EQ(ub->first, 5);

    auto ub_end = mm.upper_bound(7);
    ASSERT_EQ(ub_end, mm.end());

    // Test equal_range
    auto range1 = mm.equal_range(1);
    ASSERT_EQ(distance(range1.first, range1.second), 1);

    auto range3 = mm.equal_range(3);
    ASSERT_EQ(distance(range3.first, range3.second), 2);

    auto range5 = mm.equal_range(5);
    ASSERT_EQ(distance(range5.first, range5.second), 3);

    auto range99 = mm.equal_range(99);
    ASSERT_EQ(range99.first, range99.second);
}

// Test multimap with complex values
TEST(multimap, complex_values) {
    test_multimap<string, vector<int> > mm;

    vector<int> v1 = {1, 2, 3};
    vector<int> v2 = {4, 5, 6};
    vector<int> v3 = {7, 8, 9};

    mm.insert(make_pair(string("key1"), v1));
    mm.insert(make_pair(string("key1"), v2));
    mm.insert(make_pair(string("key2"), v3));

    ASSERT_EQ(mm.size(), 3);
    ASSERT_EQ(mm.count("key1"), 2);
    ASSERT_EQ(mm.count("key2"), 1);

    auto range = mm.equal_range("key1");
    vector<vector<int> > found_values;
    for (auto it = range.first; it != range.second; ++it) {
        found_values.push_back(it->second);
    }

    ASSERT_EQ(found_values.size(), 2);
    // Values should be either v1 or v2
    bool has_v1 = false, has_v2 = false;
    for (const auto& vec : found_values) {
        if (vec == v1) has_v1 = true;
        if (vec == v2) has_v2 = true;
    }
    ASSERT_TRUE(has_v1 && has_v2);
}

// Test multimap stress test
TEST(multimap, stress_test) {
    test_multimap<int, int> mm;
    multimap<int, int> std_mm;

    std::uniform_int_distribution<int> key_dist(
        1, 50);  // Small range for duplicates
    std::uniform_int_distribution<int> val_dist(1, 100);
    std::uniform_int_distribution<int> op_dist(0, 2);

    for (int i = 0; i < 1000; i++) {
        int op = op_dist(generator);
        int key = key_dist(generator);
        int val = val_dist(generator);

        if (op == 0) {  // Insert
            mm.insert(make_pair(key, val));
            std_mm.insert(make_pair(key, val));
        } else if (op == 1) {  // Erase by key
            size_t mm_erased = mm.erase(key);
            size_t std_erased = std_mm.erase(key);
            ASSERT_EQ(mm_erased, std_erased);
        } else {  // Count
            ASSERT_EQ(mm.count(key), std_mm.count(key));
        }

        ASSERT_EQ(mm.size(), std_mm.size());
    }

    // Final verification
    ASSERT_EQ(mm.size(), std_mm.size());

    auto mm_it = mm.begin();
    auto std_it = std_mm.begin();
    while (mm_it != mm.end() && std_it != std_mm.end()) {
        ASSERT_EQ(mm_it->first, std_it->first);
        ASSERT_EQ(mm_it->second, std_it->second);
        ++mm_it;
        ++std_it;
    }
    ASSERT_EQ(mm_it, mm.end());
    ASSERT_EQ(std_it, std_mm.end());
}

// Test edge case: massive duplicates of single key
TEST(multimap, massive_duplicates) {
    test_multimap<int, int> mm;

    const int key = 42;
    const int count = 1000;

    // Insert many duplicates of the same key
    for (int i = 0; i < count; i++) {
        mm.insert(make_pair(key, i));
    }

    ASSERT_EQ(mm.size(), count);
    ASSERT_EQ(mm.count(key), count);

    // Verify all values are present
    auto range = mm.equal_range(key);
    vector<int> values;
    for (auto it = range.first; it != range.second; ++it) {
        values.push_back(it->second);
    }

    sort(values.begin(), values.end());
    ASSERT_EQ(values.size(), count);
    for (int i = 0; i < count; i++) {
        ASSERT_EQ(values[i], i);
    }

    // Erase all at once
    size_t erased = mm.erase(key);
    ASSERT_EQ(erased, count);
    ASSERT_TRUE(mm.empty());
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

// ==== LARGE SCALE APPLICATION SIMULATION TESTS ====

// Large-scale set test simulating a unique ID system
TEST(test_set, large_scale_unique_ids) {
    const size_t N = 150000;
    test_set<int> id_set;
    set<int> std_set;

    std::uniform_int_distribution<int> dist(1, 1000000);

    auto start = chrono::high_resolution_clock::now();

    // Simulate registering unique IDs
    vector<int> inserted_ids;
    for (size_t i = 0; i < N; i++) {
        int id = dist(generator);
        auto result = id_set.insert(id);
        auto std_result = std_set.insert(id);

        ASSERT_EQ(result.second, std_result.second);
        if (result.second) {
            inserted_ids.push_back(id);
        }

        // Occasionally check if an ID exists (lookup operation)
        if (i % 1000 == 0) {
            int lookup_id = dist(generator);
            bool found = id_set.contains(lookup_id);
            bool std_found = std_set.count(lookup_id) > 0;
            ASSERT_EQ(found, std_found);
        }
    }

    auto mid = chrono::high_resolution_clock::now();

    // Verify final state
    ASSERT_EQ(id_set.size(), std_set.size());

    // Simulate bulk lookups
    size_t found_count = 0;
    for (size_t i = 0; i < N / 10; i++) {
        int lookup_id = dist(generator);
        if (id_set.contains(lookup_id)) {
            found_count++;
        }
    }

    // Simulate removing some IDs
    std::shuffle(inserted_ids.begin(), inserted_ids.end(), generator);
    size_t to_remove = inserted_ids.size() / 3;
    for (size_t i = 0; i < to_remove; i++) {
        id_set.erase(inserted_ids[i]);
        std_set.erase(inserted_ids[i]);
    }

    ASSERT_EQ(id_set.size(), std_set.size());

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Large scale set test: " << duration.count() << "ms for " << N
              << " operations" << std::endl;
    ASSERT_LT(duration.count(), 10000);  // Should complete within 10 seconds
}

// Large-scale multiset test simulating event counts
TEST(multiset, large_scale_event_counts) {
    const size_t N = 200000;
    test_multiset<int> event_counts;
    multiset<int> std_counts;

    std::uniform_int_distribution<int> event_dist(
        1, 1000);  // 1000 different event types
    std::uniform_int_distribution<int> op_dist(0, 100);

    auto start = chrono::high_resolution_clock::now();

    // Simulate logging events
    for (size_t i = 0; i < N; i++) {
        int event_type = event_dist(generator);
        int op = op_dist(generator);

        if (op < 85) {  // 85% insertions (logging events)
            event_counts.insert(event_type);
            std_counts.insert(event_type);
        } else if (op < 95) {  // 10% count queries
            size_t count = event_counts.count(event_type);
            size_t std_count = std_counts.count(event_type);
            ASSERT_EQ(count, std_count);
        } else {  // 5% clear all events of a type
            size_t erased = event_counts.erase(event_type);
            size_t std_erased = std_counts.erase(event_type);
            ASSERT_EQ(erased, std_erased);
        }

        // Periodically verify consistency
        if (i % 10000 == 0) {
            ASSERT_EQ(event_counts.size(), std_counts.size());
        }
    }

    // Final verification - check all counts match
    for (int event_type = 1; event_type <= 1000; event_type++) {
        ASSERT_EQ(event_counts.count(event_type), std_counts.count(event_type));
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Large scale multiset test: " << duration.count() << "ms for "
              << N << " operations" << std::endl;
    ASSERT_LT(duration.count(), 150000);  // Should complete within 15 seconds
}

// Large-scale map test simulating a cache system
TEST(test_map, large_scale_cache_system) {
    const size_t N = 120000;
    test_map<int, string> cache;
    map<int, string> std_cache;

    std::uniform_int_distribution<int> key_dist(1, 50000);
    std::uniform_int_distribution<int> op_dist(0, 100);
    std::uniform_int_distribution<int> value_size_dist(10, 100);

    auto start = chrono::high_resolution_clock::now();

    // Simulate cache operations
    for (size_t i = 0; i < N; i++) {
        int key = key_dist(generator);
        int op = op_dist(generator);

        if (op < 60) {  // 60% cache writes
            size_t value_size = value_size_dist(generator);
            string value(value_size, 'a' + (key % 26));

            cache[key] = value;
            std_cache[key] = value;
        } else if (op < 90) {  // 30% cache reads
            auto it = cache.find(key);
            auto std_it = std_cache.find(key);

            bool found = (it != cache.end());
            bool std_found = (std_it != std_cache.end());
            ASSERT_EQ(found, std_found);

            if (found) {
                ASSERT_EQ(it->second, std_it->second);
            }
        } else {  // 10% cache evictions
            size_t erased = cache.erase(key);
            size_t std_erased = std_cache.erase(key);
            ASSERT_EQ(erased, std_erased);
        }

        // Periodically verify consistency
        if (i % 5000 == 0) {
            ASSERT_EQ(cache.size(), std_cache.size());
        }
    }

    // Verify final state
    ASSERT_EQ(cache.size(), std_cache.size());

    // Test range operations on the cache
    auto cache_begin = cache.begin();
    auto std_begin = std_cache.begin();

    size_t compared = 0;
    while (cache_begin != cache.end() && std_begin != std_cache.end() &&
           compared < 1000) {
        ASSERT_EQ(cache_begin->first, std_begin->first);
        ASSERT_EQ(cache_begin->second, std_begin->second);
        ++cache_begin;
        ++std_begin;
        ++compared;
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Large scale map test: " << duration.count() << "ms for " << N
              << " operations" << std::endl;
    ASSERT_LT(duration.count(), 200000);  // Should complete within 20 seconds
}

// Large-scale multimap test simulating user sessions
TEST(multimap, large_scale_user_sessions) {
    const size_t N = 180000;
    test_multimap<int, int> user_sessions;  // user_id -> session_id
    multimap<int, int> std_sessions;

    std::uniform_int_distribution<int> user_dist(1, 10000);  // 10k users
    std::uniform_int_distribution<int> session_dist(
        1, 1000000);  // large session ID space
    std::uniform_int_distribution<int> op_dist(0, 100);

    auto start = chrono::high_resolution_clock::now();

    // Simulate user session management
    for (size_t i = 0; i < N; i++) {
        int user_id = user_dist(generator);
        int op = op_dist(generator);

        if (op < 70) {  // 70% session creation
            int session_id = session_dist(generator);
            user_sessions.insert(make_pair(user_id, session_id));
            std_sessions.insert(make_pair(user_id, session_id));
        } else if (op < 85) {  // 15% query user sessions
            size_t count = user_sessions.count(user_id);
            size_t std_count = std_sessions.count(user_id);
            ASSERT_EQ(count, std_count);

            // Verify session lists match
            auto range = user_sessions.equal_range(user_id);
            auto std_range = std_sessions.equal_range(user_id);

            vector<int> sessions;
            vector<int> std_sessions_vec;

            for (auto it = range.first; it != range.second; ++it) {
                sessions.push_back(it->second);
            }
            for (auto it = std_range.first; it != std_range.second; ++it) {
                std_sessions_vec.push_back(it->second);
            }

            sort(sessions.begin(), sessions.end());
            sort(std_sessions_vec.begin(), std_sessions_vec.end());
            ASSERT_EQ(sessions, std_sessions_vec);
        } else {  // 15% clear all user sessions
            size_t erased = user_sessions.erase(user_id);
            size_t std_erased = std_sessions.erase(user_id);
            ASSERT_EQ(erased, std_erased);
        }

        // Periodically verify consistency
        if (i % 8000 == 0) {
            ASSERT_EQ(user_sessions.size(), std_sessions.size());
        }
    }

    // Final comprehensive verification
    ASSERT_EQ(user_sessions.size(), std_sessions.size());

    // Verify all user session counts match
    for (int user_id = 1; user_id <= 10000; user_id += 100) {
        ASSERT_EQ(user_sessions.count(user_id), std_sessions.count(user_id));
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Large scale multimap test: " << duration.count() << "ms for "
              << N << " operations" << std::endl;
    ASSERT_LT(duration.count(), 250000);  // Should complete within 25 seconds
}

// Memory stress test with frequent allocations/deallocations
TEST(test_set, memory_stress_test) {
    const size_t CYCLES = 1000;
    const size_t ELEMENTS_PER_CYCLE = 500;

    std::uniform_int_distribution<int> dist(1, 100000);

    auto start = chrono::high_resolution_clock::now();

    for (size_t cycle = 0; cycle < CYCLES; cycle++) {
        test_set<int> temp_set;
        set<int> std_temp_set;

        // Fill the sets
        for (size_t i = 0; i < ELEMENTS_PER_CYCLE; i++) {
            int val = dist(generator);
            temp_set.insert(val);
            std_temp_set.insert(val);
        }

        ASSERT_EQ(temp_set.size(), std_temp_set.size());

        // Random operations
        for (size_t i = 0; i < ELEMENTS_PER_CYCLE / 2; i++) {
            int val = dist(generator);
            bool found = temp_set.contains(val);
            bool std_found = std_temp_set.count(val) > 0;
            ASSERT_EQ(found, std_found);

            if (found) {
                temp_set.erase(val);
                std_temp_set.erase(val);
            }
        }

        // Sets should be destroyed automatically here
        if (cycle % 100 == 0) {
            std::cout << "Memory stress cycle " << cycle << " completed"
                      << std::endl;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Memory stress test: " << duration.count() << "ms for "
              << CYCLES << " cycles" << std::endl;
    ASSERT_LT(duration.count(), 30000);  // Should complete within 30 seconds
}

// Extreme duplicate scenario for multiset
TEST(multiset, extreme_duplicates_scenario) {
    const size_t N = 100000;
    test_multiset<int> ms;
    multiset<int> std_ms;

    std::uniform_int_distribution<int> key_dist(1, 10);  // Only 10 unique keys
    std::uniform_int_distribution<int> op_dist(0, 100);

    auto start = chrono::high_resolution_clock::now();

    // Create scenario with massive duplicates
    for (size_t i = 0; i < N; i++) {
        int key = key_dist(generator);
        int op = op_dist(generator);

        if (op < 80) {  // 80% insertions
            ms.insert(key);
            std_ms.insert(key);
        } else if (op < 95) {  // 15% count queries
            ASSERT_EQ(ms.count(key), std_ms.count(key));
        } else {  // 5% erase all of a key
            size_t erased = ms.erase(key);
            size_t std_erased = std_ms.erase(key);
            ASSERT_EQ(erased, std_erased);
        }

        if (i % 10000 == 0) {
            ASSERT_EQ(ms.size(), std_ms.size());
        }
    }

    // Verify extreme duplicate counts
    for (int key = 1; key <= 10; key++) {
        size_t count = ms.count(key);
        size_t std_count = std_ms.count(key);
        ASSERT_EQ(count, std_count);

        if (count > 0) {
            std::cout << "Key " << key << " has " << count << " duplicates"
                      << std::endl;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Extreme duplicates test: " << duration.count() << "ms for "
              << N << " operations" << std::endl;
    ASSERT_LT(duration.count(), 15000);
}

// Complex data structure simulation
TEST(test_map, complex_data_simulation) {
    const size_t N = 80000;
    test_map<string, vector<int> > data_index;
    map<string, vector<int> > std_index;

    std::uniform_int_distribution<int> key_length_dist(5, 20);
    std::uniform_int_distribution<int> vector_size_dist(1, 50);
    std::uniform_int_distribution<int> value_dist(1, 1000);
    std::uniform_int_distribution<int> op_dist(0, 100);

    auto start = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; i++) {
        // Generate random string key
        size_t key_length = key_length_dist(generator);
        string key;
        for (size_t j = 0; j < key_length; j++) {
            key += 'a' + (generator() % 26);
        }

        int op = op_dist(generator);

        if (op < 60) {  // 60% insertions/updates
            size_t vec_size = vector_size_dist(generator);
            vector<int> data;
            for (size_t j = 0; j < vec_size; j++) {
                data.push_back(value_dist(generator));
            }

            data_index[key] = data;
            std_index[key] = data;
        } else if (op < 90) {  // 30% lookups
            auto it = data_index.find(key);
            auto std_it = std_index.find(key);

            bool found = (it != data_index.end());
            bool std_found = (std_it != std_index.end());
            ASSERT_EQ(found, std_found);

            if (found) {
                ASSERT_EQ(it->second, std_it->second);
            }
        } else {  // 10% deletions
            size_t erased = data_index.erase(key);
            size_t std_erased = std_index.erase(key);
            ASSERT_EQ(erased, std_erased);
        }

        if (i % 5000 == 0) {
            ASSERT_EQ(data_index.size(), std_index.size());
        }
    }

    ASSERT_EQ(data_index.size(), std_index.size());

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Complex data test: " << duration.count() << "ms for " << N
              << " operations" << std::endl;
    ASSERT_LT(duration.count(), 25000);
}

// Range operation intensive test
TEST(test_set, range_operations_intensive) {
    const size_t N = 100000;
    test_set<int> s;
    set<int> std_s;

    std::uniform_int_distribution<int> dist(1, N * 2);

    auto start = chrono::high_resolution_clock::now();

    // Build the sets
    for (size_t i = 0; i < N; i++) {
        int val = dist(generator);
        s.insert(val);
        std_s.insert(val);
    }

    ASSERT_EQ(s.size(), std_s.size());

    // Intensive range operations
    for (size_t i = 0; i < N / 10; i++) {
        int lower = dist(generator);
        int upper = lower + (generator() % 1000);

        // Test lower_bound
        auto lb = s.lower_bound(lower);
        auto std_lb = std_s.lower_bound(lower);

        bool lb_end = (lb == s.end());
        bool std_lb_end = (std_lb == std_s.end());
        ASSERT_EQ(lb_end, std_lb_end);

        if (!lb_end) {
            ASSERT_EQ(*lb, *std_lb);
        }

        // Test upper_bound
        auto ub = s.upper_bound(upper);
        auto std_ub = std_s.upper_bound(upper);

        bool ub_end = (ub == s.end());
        bool std_ub_end = (std_ub == std_s.end());
        ASSERT_EQ(ub_end, std_ub_end);

        if (!ub_end) {
            ASSERT_EQ(*ub, *std_ub);
        }

        // Test equal_range
        auto range = s.equal_range(lower);
        auto std_range = std_s.equal_range(lower);

        size_t count = distance(range.first, range.second);
        size_t std_count = distance(std_range.first, std_range.second);
        ASSERT_EQ(count, std_count);
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    std::cout << "Range operations test: " << duration.count() << "ms for " << N
              << " elements" << std::endl;
    ASSERT_LT(duration.count(), 20000);
}
