//
// Created by 20132 on 2022/12/2.
//
#include "rpc/rpc_client.h"
#include "utils/env.h"
#include "utils/log.h"
#include <iostream>
using namespace xzmjx;
using namespace xzmjx::rpc;
RpcClient::ptr client;

auto logger = log::GetDefault();
int Main(int argc,char** argv) {
    EnvMgr().addArg("a","127.0.0.1");
    EnvMgr().addArgHelp("a","server address,localhost by default");
    EnvMgr().addArg("p","8686");
    EnvMgr().addArgHelp("p","server port,8686 by default");
    EnvMgr().init(argc,argv);

    int port = std::stoi(EnvMgr().get("p"));
     client = std::make_shared<RpcClient>();
    Address::ptr addr = IPv4Address::Create("127.0.0.1",port);
    client->connect(addr);
    SPDLOG_LOGGER_INFO(logger,"connected");
    for(int i = 0; i < 10; i++) {
        go []() {
            uint32_t a = 1;
            uint32_t b = 2;
            auto ret = client->call<uint32_t>("add",a,b);
            if(ret.second.getCode() != RPCErrorCode::kRpcSuccess) {
                SPDLOG_LOGGER_ERROR(logger,"add({},{}) error",a,b,ret.second.getMsg());
            }else {
                SPDLOG_LOGGER_INFO(logger,"add({},{})={}",a,b,ret.first.value());
            }

            std::string s = "xzmjx";
            auto ss = client->call<void>("say_hello","xzmjx");
        };
    }

    return 0;
}
int main(int argc,char** argv) {
    SPDLOG_LOGGER_DEBUG(logger,"4");
    SPDLOG_LOGGER_INFO(logger,"4");
    go [=](){
        Main(argc,argv);
    };
    co_sched.Start(0);
    return 0;
}