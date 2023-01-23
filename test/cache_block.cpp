#include <gtest/gtest.h>
#include <vector>
#include <map>
#include "sffs.h"
using namespace ldc::SFFS;


TEST(cache, write) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = FileSystem<BlockDeviceRefWrapper<MemorySpace>>::format(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);

    ASSERT_TRUE(fs.mkdir({"hello"}));
    fs.open({"hello", "world"}, fileopenmode::CREATE );
    const auto hellostat = fs.stat({"hello"});
    const auto wwstat = fs.stat({"ww"});
    const auto rootstat = fs.stat({});

    ASSERT_TRUE(rootstat.has_value());
    ASSERT_TRUE(hellostat.has_value());
    ASSERT_FALSE(wwstat.has_value());

    ASSERT_EQ(rootstat.value().m_type, EntryType::RootStorage);
    ASSERT_EQ(hellostat.value().m_type, EntryType::UserStorage);

    const auto worldstat = fs.stat({"hello", "world"});
    ASSERT_TRUE(worldstat.has_value());
    ASSERT_EQ(worldstat->m_size, 0);
    ASSERT_EQ(worldstat->m_type, EntryType::UserStream);

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
