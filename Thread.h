#ifndef MYMUDUO_THREAD
#define MYMUDUO_THREAD

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <atomic>
#include <string>

#include "noncopyable.h"
#include "CountDownLatch.h"

namespace mymuduo
{


class Thread : public noncopyable
{

public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string& name = std::string());
    ~Thread();

    void start();
    int join();

    bool started() const { return start_; }

    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

    static int numCreated() { return numCreated_; }

private:
    void setDefaultName();

    bool start_;
    bool joined_;

    std::shared_ptr<std::thread> thread_; // 线程的抽象
    ThreadFunc func_; // 线程绑定的执行函数
    
    std::string name_;  // 线程名
    pid_t tid_; // 线程号
    static std::atomic_int32_t numCreated_; // 记录总的创建线程的数量

    CountDownLatch latch_;
};

}
#endif