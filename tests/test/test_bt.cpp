//
// Created by 20132 on 2022/11/30.
//
#include "utils/byte_array.h"
int main() {
    xzmjx::ByteArray::ptr bt = std::make_shared<xzmjx::ByteArray>();
    bt->writeFint8(9);
    bt->setPosition(0);
    auto ret = bt->readFint8();
    return 0;
}