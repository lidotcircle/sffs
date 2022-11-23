#include <gtest/gtest.h>
#include <vector>
#include <map>
#include "sffs.h"
using namespace SFFS;


TEST(cache, write) {
    FileSystem<MemorySpace> fs(MemorySpace(1024 * 1024 * 10));
}
