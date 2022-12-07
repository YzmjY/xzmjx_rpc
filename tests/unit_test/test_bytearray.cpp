//
// Created by 20132 on 2022/11/30.
//

#include "utils/byte_array.h"
#include "gtest/gtest.h"

TEST(bytearray,int) {
    xzmjx::ByteArray::ptr bt = std::make_shared<xzmjx::ByteArray>();
    bt->writeFint8(9);
    bt->setPosition(0);
    auto ret = bt->readFint8();
    EXPECT_EQ(ret,9);
}