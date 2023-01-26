#include <gtest/gtest.h>
#include <vector>
#include <map>
#include "sffs.h"
using namespace ldc::SFFS;


TEST(filesystem, basic) {
    auto ms = MemorySpace(1024 * 1024 * 10);
    auto fs = FileSystem<BlockDeviceRefWrapper<MemorySpace>>::format(BlockDeviceRefWrapper<MemorySpace>(ms), 3, 9, 6);
    // auto ms = FileWrapper("./testfile", "wr+");
    // auto fs = FileSystem<BlockDeviceRefWrapper<FileWrapper>>::format(BlockDeviceRefWrapper<FileWrapper>(ms), 3, 9, 6);

    ASSERT_TRUE(fs.mkdir({"hello"}));
    auto fn = fs.open({"hello", "world"}, fileopenmode::CREATE | fileopenmode::READ );
    const auto hellostat = fs.stat({"hello"});
    const auto wwstat = fs.stat({"ww"});
    const auto rootstat = fs.stat({});

    ASSERT_TRUE(rootstat.has_value());
    ASSERT_TRUE(hellostat.has_value());
    ASSERT_FALSE(wwstat.has_value());
    ASSERT_TRUE(fn.has_value());
    ASSERT_EQ(fs.listdir({}).size(), 1);
    ASSERT_EQ(fs.listdir({"hello"}).size(), 1);

    ASSERT_EQ(rootstat.value().m_type, EntryType::RootStorage);
    ASSERT_EQ(hellostat.value().m_type, EntryType::UserStorage);

    const auto worldstat = fs.stat({"hello", "world"});
    ASSERT_TRUE(worldstat.has_value());
    ASSERT_EQ(worldstat->m_size, 0);
    ASSERT_EQ(worldstat->m_type, EntryType::UserStream);

    {
        auto fd = fn.value();
        std::string bbx;
        for (size_t i=0;i<10000;i++) {
            const auto buf = std::to_string(i);
            fs.write(fd, buf.c_str(), buf.size());
            bbx += buf;
        }
        const auto fstat = fs.stat({"hello", "world"});
        ASSERT_TRUE(fstat.has_value());
        ASSERT_EQ(bbx.size(), fstat->m_size);

        ASSERT_TRUE(fs.seek(fd, 0, seekwhence::SET));
        std::string rb(bbx.size(), 0);
        ASSERT_EQ(fs.read(fd, rb.data(), bbx.size()), bbx.size());
        ASSERT_EQ(rb, bbx);

        ASSERT_TRUE(fs.truncate(fd, 1000));
        ASSERT_EQ(fs.stat({"hello", "world"})->m_size, 1000);
        ASSERT_TRUE(fs.seek(fd, 0, seekwhence::SET));
        std::string rxx(1000, 0);
        ASSERT_EQ(fs.read(fd, rxx.data(), rxx.size()), rxx.size());
        ASSERT_EQ(rxx, bbx.substr(0, 1000));

        ASSERT_FALSE(fs.unlink({"hello", "world"}));
        ASSERT_TRUE(fs.close(fd));
        ASSERT_TRUE(fs.unlink({"hello", "world"}));
        const auto un = fs.stat({"hello", "world"});
        ASSERT_FALSE(un.has_value());
        ASSERT_FALSE(fs.open({"hello", "world"}, fileopenmode::READ).has_value());
    }

    {
        auto fn = fs.open({"hello", "world"}, fileopenmode::CREATE | fileopenmode::READ );
        ASSERT_TRUE(fn.has_value());
        ASSERT_EQ(fs.write(fn.value(), "nope", 4), 4);
        ASSERT_FALSE(fs.move({"hello", "world"}, {"nope"}));
        ASSERT_TRUE(fs.close(fn.value()));
        ASSERT_TRUE(fs.move({"hello", "world"}, {"nope"}));
        ASSERT_FALSE(fs.stat({"hello", "world"}).has_value());
        ASSERT_TRUE(fs.stat({"nope"}).has_value());
        ASSERT_EQ(fs.stat({"nope"})->m_size, 4);
        ASSERT_EQ(fs.listdir({}).size(), 2);
        ASSERT_TRUE(fs.move({"nope"}, {"nope2"}));
        ASSERT_TRUE(fs.stat({"nope2"}).has_value());
        ASSERT_EQ(fs.listdir({}).size(), 2);
        ASSERT_TRUE(fs.closedir(fs.opendir({"hello"}).value()));
    }

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
