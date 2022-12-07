//
// Created by 20132 on 2022/12/3.
//

#include "rpc_service_registry.h"
#include "rpc_session.h"
#include "context.h"
#include "utils/log.h"
#include "serializer.h"

namespace xzmjx::rpc {

static auto g_logger = log::GetDefault();

void RpcServiceRegistry::handleClient(Socket::ptr client) {
    SPDLOG_LOGGER_INFO(g_logger,"handle client : {}",client->toString());
    RpcSession::ptr session = std::make_shared<RpcSession>(client);
    bool is_service_provider = false;
    std::string provider_addr;
    while(true) {
        Protocol::ptr req = session->recvProtocol();
        if(req == nullptr) {
            if(is_service_provider) {
                /// 服务提供者下线，移除服务
                unregisterService(provider_addr);
                break;
            }
        }

        uint8_t type = uint8_t(req->getType());
        Protocol::ptr rsp;
        switch (type) {
            case (uint8_t) Protocol::Type::kRpcHeartbeat :
                SPDLOG_LOGGER_DEBUG(g_logger,"heartbeat to service provider:{}",session->getPeerAddressString());
                rsp = handleHeartbeat();
                break;
            case (uint8_t) Protocol::Type::kRpcServiceDiscoverRequest :
                rsp = handleServiceDiscover(req);
                break;
            case (uint8_t)Protocol::Type::kRpcServiceProvider :
                is_service_provider = true;
                provider_addr = handleProvider(session,req);
                continue;
            case (uint8_t) Protocol::Type::kRpcServiceRegisterRequest :
                rsp = handleRegisterService(provider_addr,req);
                break;
            default:
                break;
        }

        if(rsp&&session&&session->isConnected()) {
            session->sendProtocol(rsp);
        }
    }
}

std::string RpcServiceRegistry::handleProvider(RpcSession::ptr session , Protocol::ptr req) {
    uint32_t port = 0;
    Serializer serializer(req->getPayload());

    serializer>>port;
    SPDLOG_LOGGER_DEBUG(g_logger,"recv a service provider,port:{}",port);
    IPv4Address::ptr addr = std::dynamic_pointer_cast<IPv4Address>(session->getPeerAddress());
    addr->setPort(port);
    return addr->toString();
}

void RpcServiceRegistry::unregisterService(const std::string & server_addr) {
    std::unique_lock<co_rmutex> lock(m_rwmutex.Reader());
    auto range = m_address_to_service.equal_range(server_addr);
    if(range.first == range.second) {
        return;
    } else {
        std::vector<std::multimap<std::string,std::string>::iterator> del;
        for(auto iter = range.first; iter != range.second;iter++) {
            auto r = m_services_provider.equal_range(iter->second);
            for(auto iter1 = r.first; iter1 != r.second;iter1++) {
                if(iter1->second == server_addr) {
                    del.push_back(iter1);
                }
            }
        }

        for(auto i:del) {
            SPDLOG_LOGGER_DEBUG(g_logger,"unregister service : [{}] from server : [{}]",i->first,i->second);
            m_services_provider.erase(i);
        }
    }
    m_address_to_service.erase(range.first,range.second);
}

Protocol::ptr RpcServiceRegistry::handleRegisterService(const std::string& addr,Protocol::ptr req) {
    if(req->getPayload().empty()) {
        return Protocol::Create(Protocol::Type::kRpcServiceRegisterResponse,"");
    }
    Serializer serializer(req->getPayload());
    std::string service;

    RPCErrorCode code = RPCErrorCode::kRpcSuccess;
    std::string msg;
    do{
        try {
            serializer >> service;
        } catch(...) {
            code = RPCErrorCode::kRpcProtocolFormatError;
            msg = "cannot parse msg content";
            break;
        }
        {
            std::unique_lock<co_wmutex> lock(m_rwmutex.Writer());
            SPDLOG_LOGGER_DEBUG(g_logger,"register service : [{}] from server : [{}]",service,addr);
            m_services_provider.emplace(service,addr);
            m_address_to_service.emplace(addr,service);
        }
    }while(0);

    Context ctx;
    ctx.setCode(code);
    ctx.setMsg(msg);
    serializer.clear();
    return Protocol::Create(Protocol::Type::kRpcServiceRegisterResponse,std::move(ctx),serializer.toString(),req->getSeqId());
}

Protocol::ptr RpcServiceRegistry::handleHeartbeat() {
    return Protocol::HeartbeatProtocol();
}

Protocol::ptr RpcServiceRegistry::handleServiceDiscover(Protocol::ptr req) {
    Serializer s(req->getPayload());
    std::string name;
    Context ctx;
    std::vector<std::string> ret;

    do {
        try {
            s>>name;
        } catch (...) {
            SPDLOG_LOGGER_WARN(g_logger,"service discover with unknown name format");
            ctx.setCode(RPCErrorCode::kRpcNoMethod);
            ctx.setMsg("service discover with unknown name format");
        }
        /// 查询提供服务者
        std::unique_lock<co_rmutex> lock(m_rwmutex.Reader());
        auto range = m_services_provider.equal_range(name);
        if(range.first == range.second) {
            SPDLOG_LOGGER_WARN(g_logger,"service discover with unregister name:{}",name);
            ctx.setCode(RPCErrorCode::kRpcNoMethod);
            ctx.setMsg(fmt::format("service discover with unregister name:{}",name));
            break;
        }
        ctx.setCode(RPCErrorCode::kRpcSuccess);
        for(auto it = range.first;it != range.second; it++) {
            ret.push_back(it->second);
        }
        lock.unlock();
    }while(0);
    Serializer serializer;
    serializer<<ret;
    serializer.reset();
    return Protocol::Create(Protocol::Type::kRpcServiceDiscoverResponse,std::move(ctx),serializer.toString(),req->getSeqId());
}
}