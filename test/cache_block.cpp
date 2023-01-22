#include <gtest/gtest.h>
#include <vector>
#include <map>
#include "sffs.h"
using namespace ldc::SFFS;


TEST(cache, write) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = FileSystem<BlockDeviceRefWrapper<MemorySpace>>::format(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);

    for (size_t i=0;i<1024*2;i++) {
        if (i % 16 == 0) {
            printf("0x%.4zX: ", i);
        }
        unsigned char val;
        ms.read(i, &val, 1);
        std::printf("0x%.2X ", val);

        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
}
