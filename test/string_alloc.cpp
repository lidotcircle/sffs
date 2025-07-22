#include <utest.h>

#include <iostream>
#include <random>

#include "sffs.h"
using namespace ldc::SFFS;

TEST(sffs, block_stride_view_test) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
    BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);

    ASSERT_EQ(view1.maxsize(), 1024 * 1024 * 5);
    ASSERT_EQ(view2.maxsize(), 1024 * 1024 * 5);

    std::vector<char> n1(view1.maxsize(), 0);
    std::vector<char> n2(view1.maxsize(), 1);
    view1.write(0, n1.data(), n1.size());
    view2.write(0, n2.data(), n2.size());

    size_t count0 = 0, count1 = 0;
    for (auto& v : ms.data()) {
        ASSERT_TRUE(v == 0 || v == 1);
        if (v == 0) {
            count0++;
        } else if (v == 1) {
            count1++;
        }
    }

    {
        auto ms = MemorySpace(120);
        BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
        BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);
        ASSERT_EQ(view1.maxsize(), 120);
        ASSERT_EQ(view2.maxsize(), 0);
    }
    {
        auto ms = MemorySpace(512);
        BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
        BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);
        ASSERT_EQ(view1.maxsize(), 512);
        ASSERT_EQ(view2.maxsize(), 0);
    }
    {
        auto ms = MemorySpace(513);
        BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
        BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);
        ASSERT_EQ(view1.maxsize(), 512);
        ASSERT_EQ(view2.maxsize(), 1);
    }
    {
        auto ms = MemorySpace(1023);
        BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
        BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);
        ASSERT_EQ(view1.maxsize(), 512);
        ASSERT_EQ(view2.maxsize(), 511);
    }
    {
        auto ms = MemorySpace(1024);
        BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
        BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);
        ASSERT_EQ(view1.maxsize(), 512);
        ASSERT_EQ(view2.maxsize(), 512);
    }
    {
        auto ms = MemorySpace(1025);
        BlockStrideView<MemorySpace> view1(ms, 1024, 512, 0);
        BlockStrideView<MemorySpace> view2(ms, 1024, 512, 512);
        ASSERT_EQ(view1.maxsize(), 513);
        ASSERT_EQ(view2.maxsize(), 512);
    }
}

static std::string random_string(size_t len) {
    static std::default_random_engine rd(0);
    static std::uniform_int_distribution<int> dist('a', 'z');
    std::string s;
    s.reserve(len);
    for (size_t i = 0; i < len; i++) {
        s.push_back(dist(rd));
    }
    return s;
}

TEST(sffs, alloc_test) {
    auto ms = MemorySpace(1024 * 1024 * 100);
    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    // Test space usage after initialization
    auto usage_empty = alloc.getSpaceUsage();
    ASSERT_GT(usage_empty.total_capacity, 0);
    ASSERT_GT(usage_empty.free_space, 0);
    ASSERT_EQ(usage_empty.used_space, 0);
    ASSERT_EQ(usage_empty.total_strings, 0);

    alloc.store("hello");
    ASSERT_EQ(alloc.count("hello"), 1);
    ASSERT_EQ(alloc.count("world"), 0);

    // Test space usage after storing a string
    auto usage_after_store = alloc.getSpaceUsage();
    ASSERT_GT(usage_after_store.used_space, 0);
    ASSERT_EQ(usage_after_store.total_strings, 1);
    ASSERT_EQ(usage_after_store.total_string_refs, 1);
    ASSERT_LT(usage_after_store.free_space, usage_empty.free_space);

    alloc.free(alloc.locationOf("hello").value());
    ASSERT_EQ(alloc.count("hello"), 0);

    // Test space usage after freeing
    auto usage_after_free = alloc.getSpaceUsage();
    ASSERT_EQ(usage_after_free.total_strings, 0);
    ASSERT_EQ(usage_after_free.total_string_refs, 0);
}

TEST(sffs, alloc_debug_test) {
    auto ms = MemorySpace(1024 * 1024 *
                          10);  // Smaller memory to trigger issues faster
    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    std::vector<std::string> strings = {"hello", "world", "test", "debug",
                                        "allocator"};
    std::vector<addr_t> addrs;

    for (const auto& s : strings) {
        auto addr = alloc.store(s);
        addrs.push_back(addr);
    }

    for (size_t i = 0; i < strings.size(); i++) {
        alloc.free(addrs[i]);
    }

    for (const auto& s : strings) {
        try {
            auto addr = alloc.store(s);
            (void)addr;  // Suppress unused variable warning
        } catch (const std::exception& e) {
            ASSERT_TRUE(false);
        }
    }

    // Demonstrate space usage information
    auto usage = alloc.getSpaceUsage();

    // Verify the information makes sense
    ASSERT_EQ(usage.total_capacity, 1024 * 1024 * 10);
    ASSERT_EQ(usage.total_strings, strings.size());
    ASSERT_EQ(usage.total_string_refs, strings.size());
    ASSERT_GT(usage.used_space, 0);
    ASSERT_GT(usage.free_space, 0);
    ASSERT_EQ(usage.used_space + usage.free_space + usage.overhead_space,
              usage.total_capacity);
    ASSERT_GT(usage.fragmentation_ratio, 0.0);
    ASSERT_LE(usage.fragmentation_ratio, 1.0);
}

TEST(sffs, alloc_random_test) {
    auto ms = MemorySpace(1024 * 1024 * 100);
    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    static std::default_random_engine rd(0);
    std::uniform_int_distribution<size_t> dist(1, 1000);
    std::set<std::string> strings;
    for (size_t i = 0; i < 5000; i++) {
        strings.insert(random_string(dist(rd)));
    }

    for (auto& s : strings) {
        alloc.store(s);
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 1);
    }

    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    for (auto& s : strings) {
        alloc.store(s);
        alloc.store(s);
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 2);
    }

    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 1);
    }

    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    auto usage = alloc.getSpaceUsage();
    std::cout << usage.to_string() << std::endl;
}

TEST(sffs, alloc_random_test2) {
    auto ms = MemorySpace(1024 * 1024 * 100);
    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    static std::default_random_engine rd(2);
    std::uniform_int_distribution<size_t> dist(1, 2000);
    std::set<std::string> strings_x;
    for (size_t i = 0; i < 5000; i++) {
        strings_x.insert(random_string(dist(rd)));
    }

    std::vector<std::string> strings(strings_x.begin(), strings_x.end());
    for (auto& s : strings) {
        alloc.store(s);
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 1);
    }

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc.store(s);
        alloc.store(s);
    }

    {
        auto usage = alloc.getSpaceUsage();
        std::cout << usage.to_string() << std::endl;
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 2);
    }

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 1);
    }

    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc2(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc2.free(alloc2.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    auto usage = alloc.getSpaceUsage();
    std::cout << usage.to_string() << std::endl;
}

TEST(sffs, alloc_random_test3) {
    auto ms = MemorySpace(1024 * 1024 * 100);
    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    static std::default_random_engine rd(5);
    std::uniform_int_distribution<size_t> dist(1, 2048);
    std::set<std::string> strings_x;
    for (size_t i = 0; i < 5000; i++) {
        strings_x.insert(random_string(dist(rd)));
    }

    std::vector<std::string> strings(strings_x.begin(), strings_x.end());
    for (auto& s : strings) {
        alloc.store(s);
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 1);
    }

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    std::vector<std::string> sss2x(strings.begin() + strings.size() / 2,
                                   strings.end());
    strings.erase(strings.begin() + strings.size() / 2, strings.end());
    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc.store(s);
        alloc.store(s);
    }

    {
        auto usage = alloc.getSpaceUsage();
        std::cout << usage.to_string() << std::endl;
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 2);
    }

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 1);
    }

    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc2(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    std::shuffle(strings.begin(), strings.end(), std::mt19937_64(0));
    for (auto& s : strings) {
        alloc2.free(alloc2.locationOf(s).value());
    }

    for (auto& s : strings) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    {
        auto usage = alloc.getSpaceUsage();
        std::cout << usage.to_string() << std::endl;
    }

    for (auto& s : sss2x) {
        alloc.free(alloc.locationOf(s).value());
    }

    for (auto& s : sss2x) {
        ASSERT_EQ(alloc.count(s), 0);
    }

    {
        auto usage = alloc.getSpaceUsage();
        std::cout << usage.to_string() << std::endl;
    }
}
