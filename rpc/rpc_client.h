//
// Created by 20132 on 2022/12/1.
//

#ifndef XZMJX_RPC_CLIENT_H
#define XZMJX_RPC_CLIENT_H
#include "libgo.h"
#include "protocol.h"
#include "net/address.h"
#include "utils/log.h"
#include "rpc_session.h"
#include "context.h"
#include "serializer.h"
#include "spdlog/fmt/fmt.h"
#include <memory>
#include <map>
#include <optional>
namespace xzmjx::rpc {
template<class T>
concept VoidType = std::is_void_v<T>;
class RpcClient {
public:
    using ptr = std::shared_ptr<RpcClient>;
    explicit RpcClient(co::Scheduler* worker = &co_sched);
    ~RpcClient();

    bool connect(Address::ptr address);

    template<class T,class... Types>
    std::pair<std::optional<T>,Context> call(const std::string& func_name,Types... args) {
        std::tuple<std::decay_t<Types>...> args_tuple{args...};
        Serializer s;
        s<<func_name<<args_tuple;
        s.reset();
        return call_internal<T>(func_name,s);
    }

    template<VoidType T,class... Types>
    std::pair<std::nullopt_t,Context> call(const std::string& func_name, Types... args) {
        std::tuple<std::decay_t<Types>...> args_tuple{args...};
        Serializer s;
        s<<func_name<<args_tuple;
        s.reset();
        return call_internal<void>(func_name,s);
    }

    void close();

private:
    void handleSend();
    void handleRecv();

    void handleHeartbeat();
    void handleMethodResponse(Protocol::ptr msg);

    template<class T>
    std::pair<std::optional<T>,Context> call_internal(const std::string& name,Serializer s) {
        Context ctx;
        if(m_session == nullptr||m_is_closed) {
            ctx.setCode(RPCErrorCode::kRpcServerClosed);
            ctx.setMsg("server closed");
            return {std::nullopt,ctx};
        }

        /// 自增序列号，对应接收响应通道
        co_chan<Protocol::ptr> recv_chan;
        m_guard.lock();
        uint64_t id = m_seq_id++;
        m_outstanding_call.insert({id,recv_chan});
        m_guard.unlock();

        /// 发送调用请求
        Protocol::ptr req = Protocol::Create(Protocol::Type::kRpcMethodRequest,s.toString(),id);
        SPDLOG_LOGGER_INFO(log::GetDefault(),"call method {} ,protocol:{}",name,req->debugString());
        m_chan<<req;

        /// 接收调用响应
        Protocol::ptr rsp;
        bool timeout = false;
        if(m_call_timeout == uint64_t(-1)) {
            recv_chan>>rsp;
        } else {
            if(!recv_chan.TimedPop(rsp,std::chrono::seconds(m_call_timeout))) {
                ctx.setCode(RPCErrorCode::kRpcCallTimeout);
                ctx.setMsg(fmt::format("call method {} timeout",name));
                timeout = true;
            }
        }

        m_guard.lock();
        m_outstanding_call.erase(id);
        recv_chan.close();
        m_guard.unlock();

        if(timeout) {
            return {std::nullopt,ctx};
        }

        /// 反序列化结果
        ctx.setCode(rsp->getErrCode());
        ctx.setMsg(rsp->getErrMsg());
        if(rsp->getErrCode() != RPCErrorCode::kRpcSuccess) {
            SPDLOG_LOGGER_WARN(log::GetDefault(),"rpc call failed");
            return {std::nullopt,ctx};
        }

        if(std::is_same_v<void,T>) {
            return {std::nullopt,ctx};
        }
        T ret;
        Serializer se(rsp->getPayload());
        try {
            se>>ret;
        } catch (std::exception& ex) {
            SPDLOG_LOGGER_ERROR(log::GetDefault(),"call method {} error",name);
            ctx.setCode(RPCErrorCode::kRpcRetTypeNotMatch);
            ctx.setMsg(ex.what());
            return {std::nullopt,ctx};;
        }

        return {ret,ctx};;
    }

    template<VoidType T>
    std::pair<std::nullopt_t,Context> call_internal(const std::string& name,Serializer s) {
        Context ctx;
        if (m_session == nullptr || m_is_closed) {
            ctx.setCode(RPCErrorCode::kRpcServerClosed);
            ctx.setMsg("server closed");
            return {std::nullopt, ctx};
        }

        /// 自增序列号，对应接收响应通道
        co_chan<Protocol::ptr> recv_chan;
        m_guard.lock();
        uint64_t id = m_seq_id++;
        m_outstanding_call.insert({id, recv_chan});
        m_guard.unlock();

        /// 发送调用请求
        Protocol::ptr req = Protocol::Create(Protocol::Type::kRpcMethodRequest, s.toString(), id);
        SPDLOG_LOGGER_INFO(log::GetDefault(), "call method {} ,protocol:{}", name, req->debugString());
        m_chan << req;

        /// 接收调用响应
        Protocol::ptr rsp;
        bool timeout = false;
        if (m_call_timeout == uint64_t(-1)) {
            recv_chan >> rsp;
        } else {
            if (!recv_chan.TimedPop(rsp, std::chrono::seconds(m_call_timeout))) {
                ctx.setCode(RPCErrorCode::kRpcCallTimeout);
                ctx.setMsg(fmt::format("call method {} timeout", name));
                timeout = true;
            }
        }

        m_guard.lock();
        m_outstanding_call.erase(id);
        recv_chan.close();
        m_guard.unlock();

        if (timeout) {
            return {std::nullopt, ctx};
        }

        /// 反序列化结果
        ctx.setCode(rsp->getErrCode());
        ctx.setMsg(rsp->getErrMsg());
        if (rsp->getErrCode() != RPCErrorCode::kRpcSuccess) {
            SPDLOG_LOGGER_WARN(log::GetDefault(), "rpc call failed");
            return {std::nullopt, ctx};
        }

        return {std::nullopt, ctx};
    }

private:
    /// 协议发送通道
    co_chan<Protocol::ptr> m_chan;

    /// 心跳计时器
    co_timer m_heartbeat_timer;
    co_timer_id m_heartbeat_timer_id;

    /// 连接是否断开
    bool m_is_closed{true};

    /// 连接超时时间
    uint64_t m_conn_timeout;

    /// 心跳超时时间
    uint64_t m_heartbeat_timeout;
    std::function<void(void)> m_heartbeat_cb;

    /// 调用超时时间
    uint64_t m_call_timeout;

    /// 服务端是否回应心跳
    bool m_is_heartbeat_response{false};

    /// 连接会话
    RpcSession::ptr m_session;
    co::Scheduler* m_worker;

    /// 等待方法调用结果的协程通道
    std::map<uint64_t,co_chan<Protocol::ptr>> m_outstanding_call;

    /// 序列号
    uint64_t m_seq_id{0};

    /// 保护序列号和未决的调用map
    co_mutex m_guard;
};
}
#endif //XZMJX_RPC_CLIENT_H
