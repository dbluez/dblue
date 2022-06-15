#ifndef __DEEPBLUE_STREAM_H__
#define __DEEPBLUE_STREAM_H__

#include "bytearray.h"
#include <memory>

namespace deepblue
{

    // 流结构
    class Stream
    {
    public:
        typedef std::shared_ptr<Stream> ptr;

        // 析构函数
        virtual ~Stream() {}

        // 读数据
        virtual int read(void *buffer, size_t length) = 0;

        // 读数据
        virtual int read(ByteArray::ptr ba, size_t length) = 0;

        // 读固定长度的数据
        virtual int readFixSize(void *buffer, size_t length);

        // 读固定长度的数据
        virtual int readFixSize(ByteArray::ptr ba, size_t length);

        // 写数据
        virtual int write(const void *buffer, size_t length) = 0;

        // 写数据
        virtual int write(ByteArray::ptr ba, size_t length) = 0;

        // 写固定长度的数据
        virtual int writeFixSize(const void *buffer, size_t length);

        // 写固定长度的数据
        virtual int writeFixSize(ByteArray::ptr ba, size_t length);

        // 关闭流
        virtual void close() = 0;
    };

} // namespace deepblue

#endif
