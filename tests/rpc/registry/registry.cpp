//
// Created by 20132 on 2022/12/6.
//
#include "rpc/rpc_service_registry.h"
#include "net/address.h"
using namespace xzmjx::rpc;
using namespace xzmjx;
using namespace std;

int main() {
    RpcServiceRegistry::ptr registry = make_shared<RpcServiceRegistry>();
    Address::ptr addr = IPv4Address::Create("127.0.0.1",9999);
    registry->bind(addr);
    registry->start();
}