#include <ipfs_client/logger.h>

#include <gtest/gtest.h>

namespace l = ipfs::log;

TEST(LoggerTest,LevelDescriptor_error) {
  EXPECT_EQ(l::LevelDescriptor(l::Level::ERROR),"error");
}
