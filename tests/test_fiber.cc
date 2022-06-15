/**
 * @file test_fiber.cc
 * @brief 协程测试
 * @version 0.1
 * @date 2021-06-15
 */
#include "deepblue.h"
#include <string>
#include <vector>

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void run_in_fiber2()
{
    DEEPBLUE_LOG_INFO(g_logger) << "run_in_fiber2 begin";
    DEEPBLUE_LOG_INFO(g_logger) << "run_in_fiber2 end";
}

void run_in_fiber()
{
    DEEPBLUE_LOG_INFO(g_logger) << "run_in_fiber begin";

    DEEPBLUE_LOG_INFO(g_logger) << "before run_in_fiber yield";
    deepblue::Fiber::GetThis()->yield();
    DEEPBLUE_LOG_INFO(g_logger) << "after run_in_fiber yield";

    DEEPBLUE_LOG_INFO(g_logger) << "run_in_fiber end";
    // fiber结束之后会自动返回主协程运行
}

void test_fiber()
{
    DEEPBLUE_LOG_INFO(g_logger) << "test_fiber begin";

    // 初始化线程主协程
    deepblue::Fiber::GetThis();

    deepblue::Fiber::ptr fiber(new deepblue::Fiber(run_in_fiber, 0, false));
    DEEPBLUE_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 1

    DEEPBLUE_LOG_INFO(g_logger) << "before test_fiber resume";
    fiber->resume();
    DEEPBLUE_LOG_INFO(g_logger) << "after test_fiber resume";

    /**
     * 关于fiber智能指针的引用计数为3的说明：
     * 一份在当前函数的fiber指针，一份在MainFunc的cur指针
     * 还有一份在在run_in_fiber的GetThis()结果的临时变量里
     */
    DEEPBLUE_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 3

    DEEPBLUE_LOG_INFO(g_logger) << "fiber status: " << fiber->getState(); // READY

    DEEPBLUE_LOG_INFO(g_logger) << "before test_fiber resume again";
    fiber->resume();
    DEEPBLUE_LOG_INFO(g_logger) << "after test_fiber resume again";

    DEEPBLUE_LOG_INFO(g_logger) << "use_count:" << fiber.use_count();     // 1
    DEEPBLUE_LOG_INFO(g_logger) << "fiber status: " << fiber->getState(); // TERM

    fiber->reset(run_in_fiber2); // 上一个协程结束之后，复用其栈空间再创建一个新协程
    fiber->resume();

    DEEPBLUE_LOG_INFO(g_logger) << "use_count:" << fiber.use_count(); // 1
    DEEPBLUE_LOG_INFO(g_logger) << "test_fiber end";
}

int main(int argc, char *argv[])
{
    deepblue::EnvMgr::GetInstance()->init(argc, argv);
    // deepblue::Config::LoadFromConfDir(deepblue::EnvMgr::GetInstance()->getConfigPath());

    deepblue::SetThreadName("main_thread");
    DEEPBLUE_LOG_INFO(g_logger) << "main begin";

    std::vector<deepblue::Thread::ptr> thrs;
    for (int i = 0; i < 2; i++)
    {
        thrs.push_back(deepblue::Thread::ptr(
            new deepblue::Thread(&test_fiber, "thread_" + std::to_string(i))));
    }

    for (auto i : thrs)
    {
        i->join();
    }

    DEEPBLUE_LOG_INFO(g_logger) << "main end";
    return 0;
}