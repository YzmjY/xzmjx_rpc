//
// Created by 20132 on 2022/12/5.
//
#include "rpc/protocol.h"
#include "gtest/gtest.h"
using namespace xzmjx::rpc;

TEST(protocol,without_msg) {
    Protocol::ptr p = Protocol::Create();
    p->setType(Protocol::Type::kRpcMethodRequest);
    p->setErrCode(Protocol::ErrCode::kRpcSuccess);
    p->setSeqId(100);
    //p->set
}