#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <optional>
#include <vector>
#include "maxsize_vector.h"


std::default_random_engine generator;

template<typename VEC1, typename VEC2, typename VALGEN>
static void vectortest(size_t n, VALGEN valgen) {
    VEC1 vec1, vec1_a;
    VEC2 vec2, vec2_a;

    for (size_t i=0;i<n;i++) {
        auto val = valgen();
        vec1.push_back(val);
        vec2.push_back(val);
        ASSERT_EQ(vec1.back(), vec2.back());
    }

    {
        auto v1 = vec1;
        auto v2 = vec2;
        ASSERT_EQ(v1, vec1);
        ASSERT_EQ(v2, vec2);
    }

    {
        ASSERT_EQ(vec1.size(), vec2.size());
        auto v1b = vec1.begin();
        auto v2b = vec2.begin();
        for (;v1b!=vec1.end();v1b++,v2b++) {
            ASSERT_EQ(*v1b, *v2b);
        }
        ASSERT_TRUE(v2b==vec2.end());
    }

    {
        VEC1 m1 = vec1;
        VEC2 m2 = vec2;
        ASSERT_EQ(m1, vec1);
        ASSERT_EQ(m2, vec2);

        m1.clear();
        m2.clear();
        std::swap(m1, vec1);
        std::swap(m2, vec2);

        ASSERT_TRUE(vec1.empty());
        ASSERT_TRUE(vec2.empty());

        std::swap(m1, vec1);
        std::swap(m2, vec2);

        ASSERT_FALSE(vec1.empty());
        ASSERT_FALSE(vec2.empty());
    }

    {
        VEC1 m1(std::move(vec1));
        VEC2 m2(std::move(vec2));

        ASSERT_TRUE(vec1.empty());
        ASSERT_TRUE(vec2.empty());

        std::swap(m1, vec1);
        std::swap(m2, vec2);

        ASSERT_FALSE(vec1.empty());
        ASSERT_FALSE(vec2.empty());
    }

    {
        for (size_t i=0;i<n;i++) {
            vec2.pop_back();
            vec1.pop_back();
        }
    }
    ASSERT_TRUE(vec1.empty());
    ASSERT_TRUE(vec2.empty());
}



TEST(maxsize_vector, general) {
    std::uniform_int_distribution<int> distribution(-30000000, 30000000);

#define testn(n1, n2) \
    vectortest<std::vector<int>,maxsize_vector<int,n1>>(n2, [&](){return distribution(generator);}); \
    vectortest<std::vector<std::shared_ptr<int>>,maxsize_vector<std::shared_ptr<int>,n1>>(n2, [&](){return std::make_shared<int>(distribution(generator));}); \
    vectortest<std::vector<int>,maxsize_vector<int,n1>>(n1, [&](){return distribution(generator);}); \
    vectortest<std::vector<std::shared_ptr<int>>,maxsize_vector<std::shared_ptr<int>,n1>>(n1, [&](){return std::make_shared<int>(distribution(generator));});

    testn(1, 1);
    testn(2, 1);
    testn(3, 1);
    testn(10, 5);
    testn(100, 78);
    testn(1000, 578);
    testn(10000, 7778);
    testn(100000, 77788);
}
