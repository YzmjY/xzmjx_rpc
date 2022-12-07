//
// Created by 20132 on 2022/12/2.
//
#include "rpc/rpc_server.h"
#include "utils/env.h"
#include "utils/log.h"
#include "add.h"

using namespace xzmjx;
using namespace xzmjx::rpc;
using namespace xzmjx::log;
int main(int argc,char** argv) {
    EnvMgr().addArg("p","8686");
    EnvMgr().addArgHelp("p","server listen port,8686 by default");
    EnvMgr().init(argc,argv);

    int port = std::stoi(EnvMgr().get("p"));
    RpcServer::ptr server = std::make_shared<RpcServer>();
    server->registerMethod("add",test::add);
    server->registerMethod("say_hello",test::say_hello);
    Address::ptr addr = IPv4Address::Create("127.0.0.1",port);
    Address::ptr registry_addr = IPv4Address::Create("127.0.0.1",9999);
    server->bind(addr);
    if(server->bindRegistry(registry_addr) == false) {
        SPDLOG_LOGGER_ERROR(GetDefault(),"bind registry failed");
    }

    server->start();
}