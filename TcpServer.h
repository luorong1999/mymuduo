#ifndef MYMUDUO_TCPSERVER
#define MYMUDUO_TCPSERVER

#include <functional>
#include <string>
#include <memory>

#include "EventLoop.h"
#include "noncopyable.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"
#include "Callbacks.h"

namespace mymuduo
{

/*
用户使用muduo编写服务器程序
*/

// 对外使用的TcpServer
class TcpServer
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop *loop,
                const InetAddress& listenAddr,
                const std::string& nameArg,
                Option option = kNoReusePort);
    ~TcpServer();

    // 以下未被调用的皆为用户设置的事件驱动回调函数
    void setThreadInitCallback(const ThreadInitCallback& cb) { threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    //设置底层subloop的个数
    void setThreadNum(int numThreads);

    //开启服务器监听
    void start();

    std::string name() const { return name_; }
    std::string ipPort() const { return ipPort_; }
    EventLoop* getLoop() const { return loop_; }

private:

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    // 在loop中移除，不会发生多线程的错误
    void removeConnectionInLoop(const TcpConnectionPtr& conn); 

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;    //用户定义的baseloop

    const std::string ipPort_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;    //运行在mainloop， 任务是监听连接事件
    std::shared_ptr<EventLoopThreadPool> threadPool_;   // one loop per thread

    ConnectionCallback connectionCallback_; // 新连接建立之后的回调
    MessageCallback messageCallback_;   // 业务逻辑处理的回调
    WriteCompleteCallback writeCompleteCallback_;  //消息发送完成以后的回调
    ThreadInitCallback threadInitCallback_; //线程初始化回调

    std::atomic_int32_t started_;  // 原子变量，防止一个TcpServer对象被启动多次

    int nextConnId_;  // 用于拼接出每个连接TcpConnection的名字
    ConnectionMap connections_; //保存所有的连接
};



}

#endif