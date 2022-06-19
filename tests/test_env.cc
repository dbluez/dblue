#include "deepblue.h"

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

deepblue::Env *g_env = deepblue::EnvMgr::GetInstance();

int main(int argc, char *argv[])
{
    g_env->addHelp("h", "print this help message");
    g_env->addHelp("s", "start daemon");

    bool is_print_help = false;
    if (!g_env->init(argc, argv))
    {
        is_print_help = true;
    }
    if (g_env->has("h"))
    {
        is_print_help = true;
    }

    if (is_print_help)
    {
        g_env->printHelp();
        return false;
    }

    DEEPBLUE_LOG_INFO(g_logger) << "exe: " << g_env->getExe();
    DEEPBLUE_LOG_INFO(g_logger) << "cwd: " << g_env->getCwd();
    DEEPBLUE_LOG_INFO(g_logger) << "absoluth path of test: " << g_env->getAbsolutePath("test");
    DEEPBLUE_LOG_INFO(g_logger) << "conf path:" << g_env->getConfigPath();

    g_env->add("key1", "value1");
    DEEPBLUE_LOG_INFO(g_logger) << "key1: " << g_env->get("key1");

    g_env->setEnv("key2", "value2");
    DEEPBLUE_LOG_INFO(g_logger) << "key2: " << g_env->getEnv("key2");

    DEEPBLUE_LOG_INFO(g_logger) << g_env->getEnv("PATH");

    return 0;
}