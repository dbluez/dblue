#include "deepblue.h"
#include <assert.h>

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void test_assert()
{
    DEEPBLUE_LOG_INFO(g_logger) << deepblue::BacktraceToString(10);
    DEEPBLUE_ASSERT(false);
    DEEPBLUE_ASSERT2(0 == 1, "abcdef xx");
}

int main(int argc, char **argv)
{
    test_assert();
    return 0;
}
