#include "rbtree_container.h"

#include <utest.h>

#include <map>
#include <random>
#include <set>
using namespace std;
using namespace ldc;

std::default_random_engine generator;
static void insert_test(const size_t n_vals) {
    rbtset<int> ordered_set;
    std::set<int> stl_set;
    ASSERT_EQ(ordered_set.size(), 0);

    const size_t freq = n_vals / 8 > 0 ? n_vals / 8 : 1;
    std::uniform_int_distribution<int> distribution(-n_vals * 3, n_vals * 3);
    for (size_t i = 0; i < n_vals; i++) {
        auto val = distribution(generator);
        ordered_set.insert(val);
        stl_set.insert(val);
    }

    auto stl_iter = stl_set.begin();
    for (auto& val : ordered_set) {
        ASSERT_EQ(*stl_iter++, val);
    }

    auto cp2 = ordered_set;
    ASSERT_EQ(cp2, ordered_set);
}

TEST(set, insert) {
    for (size_t i = 1; i <= 100; i++) {
        insert_test(i);
        insert_test(i * 10);
        insert_test(i * 100);
    }
}

static void map_insert_test(const size_t n_vals) {
    rbtmap<int, int> ordered_map;
    std::map<int, int> stl_map;
    ASSERT_EQ(ordered_map.size(), 0);

    const size_t freq = n_vals / 8 > 0 ? n_vals / 8 : 1;
    std::uniform_int_distribution<int> distribution(-n_vals * 3, n_vals * 3);
    for (size_t i = 0; i < n_vals; i++) {
        auto val = distribution(generator);
        ordered_map.insert(make_pair(val, val));
        stl_map.insert(make_pair(val, val));
    }

    auto stl_iter = stl_map.begin();
    for (auto& val : ordered_map) {
        ASSERT_EQ(*stl_iter++, val);
    }

    auto cp2 = ordered_map;
    ASSERT_EQ(cp2, ordered_map);
}

TEST(map, insert) {
    for (size_t i = 1; i <= 100; i++) {
        map_insert_test(i);
        map_insert_test(i * 10);
        map_insert_test(i * 100);
    }
}
