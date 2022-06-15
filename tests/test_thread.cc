#include "deepblue.h"
#include "unistd.h"
deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

int count = 0;
deepblue::RWMutex s_mutex;
deepblue::Mutex m_mutex;

void fun1()
{
    DEEPBLUE_LOG_INFO(g_logger) << "name: " << deepblue::Thread::GetName()
                                << " this.name: " << deepblue::Thread::GetThis()->getName()
                                << " id: " << deepblue::GetThreadId()
                                << " this.id: " << deepblue::Thread::GetThis()->getId();

    for (int i = 0; i < 10000000; i++)
    {
        // deepblue::RWMutex::WriteLock lock(s_mutex);
        deepblue::Mutex::Lock lock(m_mutex);
        ++count;
    }
}

void fun2()
{
    while (true)
    {
        DEEPBLUE_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3()
{
    while (true)
    {
        DEEPBLUE_LOG_INFO(g_logger) << "=====================================";
    }
}

int main(int argc, char **argv)
{
    DEEPBLUE_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("./bin/conf/log2.yml");
    deepblue::Config::LoadFromYaml(root);

    std::vector<deepblue::Thread::ptr> thrs;
    for (int i = 0; i < 2; i++)
    {
        // deepblue::Thread::ptr thr1(new deepblue::Thread(&fun1, "name_" + std::to_string(i)));
        deepblue::Thread::ptr thr2(new deepblue::Thread(&fun2, "name_" + std::to_string(i * 2)));
        deepblue::Thread::ptr thr3(new deepblue::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));

        // thrs.push_back(thr1);
        thrs.push_back(thr2);
        thrs.push_back(thr3);
    }

    for (size_t i = 0; i < thrs.size(); i++)
    {
        thrs[i]->join();
    }

    DEEPBLUE_LOG_INFO(g_logger) << "count = " << count;

    DEEPBLUE_LOG_INFO(g_logger) << "thread test end";

    return 0;
}