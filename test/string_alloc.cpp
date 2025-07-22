#include <utest.h>

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

TEST(sffs, alloc_test) {
    auto ms = MemorySpace(1024 * 1024 * 100);
    StringAllocator<BlockDeviceRefWrapper<MemorySpace>> alloc(
        (BlockDeviceRefWrapper<MemorySpace>(ms)));

    alloc.store("hello");
    ASSERT_EQ(alloc.count("hello"), 1);
    ASSERT_EQ(alloc.count("world"), 0);

    alloc.free(alloc.locationOf("hello").value());
    ASSERT_EQ(alloc.count("hello"), 0);
}
