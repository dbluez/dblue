#ifndef __DEEPBLUE_MACRO_H__
#define __DEEPBLUE_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"
#include "log.h"

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#define DEEPBLUE_LIKELY(x) __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#define DEEPBLUE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define DEEPBLUE_LIKELY(x) (x)
#define DEEPBLUE_UNLIKELY(x) (x)
#endif

/// 断言宏封装
#define DEEPBLUE_ASSERT(x)                                                                      \
    if (DEEPBLUE_UNLIKELY(!(x)))                                                                \
    {                                                                                           \
        DEEPBLUE_LOG_ERROR(DEEPBLUE_LOG_ROOT()) << "ASSERTION: " #x                             \
                                                << "\nbacktrace:\n"                             \
                                                << deepblue::BacktraceToString(100, 2, "    "); \
        assert(x);                                                                              \
    }

/// 断言宏封装
#define DEEPBLUE_ASSERT2(x, w)                                                                  \
    if (DEEPBLUE_UNLIKELY(!(x)))                                                                \
    {                                                                                           \
        DEEPBLUE_LOG_ERROR(DEEPBLUE_LOG_ROOT()) << "ASSERTION: " #x                             \
                                                << "\n"                                         \
                                                << w                                            \
                                                << "\nbacktrace:\n"                             \
                                                << deepblue::BacktraceToString(100, 2, "    "); \
        assert(x);                                                                              \
    }
// #define DEEPBLUE_ASSERT(x)
//     if (!(x))
//     {
//         DEEPBLUE_LOG_ERROR(DEEPBLUE_LOG_ROOT()) << "ASSERTION: " #x
//                                                 << "\nbacktrace\n"
//                                                 << deepblue::BacktraceToString(100, 2, "     ");
//         assert(x);
//     }

// #define DEEPBLUE_ASSERT2(x, w)
//     if (!(x))
//     {
//         DEEPBLUE_LOG_ERROR(DEEPBLUE_LOG_ROOT()) << "ASSERTION: " #x
//                                                 << "\n"
//                                                 << w
//                                                 << "\nbacktrace\n"
//                                                 << deepblue::BacktraceToString(100, 2, "     ");
//         assert(x);
//     }

#endif