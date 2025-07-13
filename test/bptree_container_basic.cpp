#include <utest.h>

#include "bptree_container.h"
using namespace std;
using namespace ldc;

TEST(set, basic) {
    bptset<int> m;
    ASSERT_EQ(m.begin(), m.begin());
    ASSERT_EQ(m.begin(), m.end());
    m.insert({20, 0});
    ASSERT_EQ(m.begin(), m.begin());
    ASSERT_NE(m.find(20), m.end());
    ASSERT_NE(m.lower_bound(20), m.end());
}

TEST(map, basic) {
    bptmap<int, int> m;
    ASSERT_EQ(m.begin(), m.begin());
    ASSERT_EQ(m.begin(), m.end());
    m[10] = 0;
    ASSERT_EQ(m.at(10), 0);
    m.insert({20, 0});
    m[30];
    ASSERT_EQ(m.at(30), 0);
    ASSERT_EQ(m.begin(), m.begin());
    m.find(20);
    m.lower_bound(20);
}
