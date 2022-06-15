#ifndef __DEEPBLUE_THREAD_H__
#define __DEEPBLUE_THREAD_H__

#include "mutex.h"

namespace deepblue
{

    // 线程类
    class Thread : Noncopyable
    {
    public:
        // 线程智能指针类型
        typedef std::shared_ptr<Thread> ptr;

        /**
         * @brief 构造函数
         * @param[in] cb 线程执行函数
         * @param[in] name 线程名称
         */
        Thread(std::function<void()> cb, const std::string &name);

        // 析构函数
        ~Thread();

        // 获取线程ID
        pid_t getId() const { return m_id; }

        // 获取线程名称
        const std::string &getName() const { return m_name; }

        // 等待线程执行完成
        void join();

        // 获取当前的线程指针
        static Thread *GetThis();

        // 获取当前的线程名称
        static const std::string &GetName();

        /**
         * @brief 设置当前线程名称
         * @param[in] name 线程名称
         */
        static void SetName(const std::string &name);

    private:
        // 线程执行函数
        static void *run(void *arg);

    private:
        pid_t m_id = -1;            // 线程id
        pthread_t m_thread = 0;     // 线程结构
        std::function<void()> m_cb; // 线程执行函数
        std::string m_name;         // 线程名称
        Semaphore m_semaphore;      // 信号量
    };

} // namespace deepblue

#endif
