//
// Created by 20132 on 2022/11/30.
//

#include "rpc_server.h"
#include "rpc_session.h"
#include "utils/config.h"
#include "utils/log.h"
namespace xzmjx::rpc {

xzmjx::ConfigVar<uint64_t>::ptr g_client_alive_time = Config::Lookup("rpc_server.client_alive_time",(uint64_t)(40),"rpc client alive time");
xzmjx::ConfigVar<uint64_t>::ptr g_connect_registry_timeout = Config::Lookup("rpc_server.connect_registry_timeout",(uint64_t)(5),"rpc server connect to registry timeout");
xzmjx::ConfigVar<uint64_t>::ptr g_registry_heartbeat = Config::Lookup("rpc_server.registry_heartbeat_timeout",(uint64_t)(40),"rpc registry heartbeat timeout");

static auto g_logger = log::GetDefault();

static std::function<void()> registry_heartbeat_cb = nullptr;

RpcServer::RpcServer(co::Scheduler* worker,co::Scheduler* accept_worker)
        : TcpServer(worker,worker,accept_worker)
        , m_client_timer(std::chrono::milliseconds(1),worker)
        , m_register_timer(std::chrono::milliseconds(1),worker)
        , m_registry_heartbeat_rsp(false)
        , m_client_alive_time(g_client_alive_time->getValue())
        , m_registry_send_chan(200){}

bool RpcServer::bind(Address::ptr addr) {
    auto ipv4_addr = std::dynamic_pointer_cast<IPv4Address>(addr);
    m_port = ipv4_addr->getPort();
    SPDLOG_LOGGER_DEBUG(g_logger,"server provider service with port :{}",m_port);
    return TcpServer::bind(addr);
}

bool RpcServer::bindRegistry(Address::ptr addr) {
    Socket::ptr sock = Socket::CreateTCP(addr);
    if (!sock->connect(std::dynamic_pointer_cast<IPv4Address>(addr))) {
        SPDLOG_LOGGER_ERROR(g_logger,"cannot connect to registry");
        return false;
    }

    m_registry_session = std::make_shared<RpcSession>(sock);

    Serializer serializer;
    serializer<<m_port;
    serializer.reset();
    Protocol::ptr provider = Protocol::Create(Protocol::Type::kRpcServiceProvider,serializer.toString());
    m_registry_send_chan<<provider;
    return true;
}

void RpcServer::handleRegistrySessionSend() {
    Protocol::ptr req;
    while(m_registry_send_chan.pop(req)) {
        if(m_registry_session&&m_registry_session->isConnected()) {
            m_registry_session->sendProtocol(req);
        } else {
            break;
        }
    }
}

void RpcServer::registerServiceToRegistry(const std::string& service) {
    Serializer serializer;
    serializer<<service;
    serializer.reset();
    Protocol::ptr req = Protocol::Create(Protocol::Type::kRpcServiceRegisterRequest,serializer.toString());
    m_registry_send_chan<<req;
}

void RpcServer::handleRegistrySessionRecv() {
    while(true) {
        auto req = m_registry_session->recvProtocol();
        if(req == nullptr) {
            SPDLOG_LOGGER_ERROR(g_logger,"registry closed");
            m_registry_session->close();
            m_register_timer_id.StopTimer();
            break;
        }

        auto type = uint8_t (req->getType());
        switch (type) {
            case (uint8_t)Protocol::Type::kRpcServiceRegisterResponse :
                break;
            case (uint8_t)Protocol::Type::kRpcHeartbeat :
                SPDLOG_LOGGER_DEBUG(g_logger,"heartbeat from registry");
                m_registry_heartbeat_rsp = true;
                break;
            default:
                break;
        }
    }
}

bool RpcServer::start() {
    if(!isStop()) {
        SPDLOG_LOGGER_WARN(g_logger,"already start");
        return false;
    }

    /// 向注册中心注册服务
    if(m_registry_session&&m_registry_session->isConnected()) {
        SPDLOG_LOGGER_DEBUG(g_logger,"register msg to registry");
        for(auto&& it:m_services) {
            registerServiceToRegistry(it.first);
        }
        m_registry_heartbeat_rsp = true;
        registry_heartbeat_cb = [this]() {
            if(!m_registry_heartbeat_rsp) {
                m_registry_session->close();
                return;
            }
            Protocol::ptr heartbeat = Protocol::HeartbeatProtocol();
            SPDLOG_LOGGER_DEBUG(g_logger,"heartbeat to registry");
            m_registry_send_chan<<heartbeat;
            m_register_timer_id = m_register_timer.ExpireAt(std::chrono::seconds(g_registry_heartbeat->getValue()),registry_heartbeat_cb);
            m_registry_heartbeat_rsp = false;
        };
        m_register_timer_id = m_register_timer.ExpireAt(std::chrono::seconds(g_registry_heartbeat->getValue()),registry_heartbeat_cb);

        go [this]() {
            handleRegistrySessionRecv();
        };

        go [this]() {
            handleRegistrySessionSend();
        };
    }

    return TcpServer::start();
}

void RpcServer::handleClient(Socket::ptr client) {
    SPDLOG_LOGGER_DEBUG(g_logger,"handle client: {}",client->toString());
    RpcSession::ptr session = std::make_shared<RpcSession>(client);
    auto on_timeout = [client]() {
        SPDLOG_LOGGER_WARN(g_logger,"client: {} alive timeout",client->toString());
        client->close();
    };
    co_timer_id time_id = m_client_timer.ExpireAt(std::chrono::seconds(m_client_alive_time),on_timeout);
    while(client->isConnected()) {
        Protocol::ptr req = session->recvProtocol();
        if(req == nullptr) {
            time_id.StopTimer();
            break;
        }

        /// 重设心跳定时器
        time_id.StopTimer();
        time_id = m_client_timer.ExpireAt(std::chrono::seconds(m_client_alive_time),on_timeout);

        /// 处理请求
        go co_scheduler(m_worker) [session,req,this]() {
            uint8_t type = (uint8_t)(req->getType());
            Protocol::ptr rsp;
            switch (type) {
                case (uint8_t)Protocol::Type::kRpcMethodRequest:
                    rsp = handleMethodCall(req);
                    break;
                case (uint8_t)Protocol::Type::kRpcHeartbeat:
                    rsp = handleHeartbeat();
                    break;
                default:
                    break;
            }
            if(rsp&&session->isConnected()) {
                session->sendProtocol(rsp);
            }
        };
    }
}

Protocol::ptr RpcServer::handleHeartbeat() {
    SPDLOG_LOGGER_DEBUG(g_logger,"heartbeat");
    return Protocol::HeartbeatProtocol();
}

Protocol::ptr RpcServer::handleMethodCall(Protocol::ptr req) {
    SPDLOG_LOGGER_DEBUG(g_logger,"begin handle function call");
    std::string content = req->getPayload();
    Serializer s(content);
    std::string func_name;
    s>>func_name;

    Serializer res;
    Context ctx;
    SPDLOG_LOGGER_DEBUG(g_logger,"receive a function call, name = {}",func_name);
    if(m_services.find(func_name) == m_services.end()) {
        SPDLOG_LOGGER_WARN(g_logger,"unknown method name = {}",func_name);
        ctx.setCode(Protocol::ErrCode::kRpcNoMethod);
        ctx.setMsg(fmt::format("no function named {}",func_name));
    } else {
        auto f = m_services[func_name];
        std::tie(res,ctx) = f(func_name,s.toString());
    }
    s.reset();
    return Protocol::Create(Protocol::Type::kRpcMethodResponse,std::move(ctx),s.toString(),req->getSeqId());
}
}
