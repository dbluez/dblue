#ifndef __DEEPBLUE_SOCKET_STREAM_H__
#define __DEEPBLUE_SOCKET_STREAM_H__

#include "iomanager.h"
#include "mutex.h"
#include "socket.h"
#include "stream.h"

namespace deepblue
{

    // Socket流
    class SocketStream : public Stream
    {
    public:
        typedef std::shared_ptr<SocketStream> ptr;

        // 构造函数
        // sock : Socket类
        // owner : 是否完全控制
        SocketStream(Socket::ptr sock, bool owner = true);

        // 析构函数
        // 如果m_owner=true, 则close
        ~SocketStream();

        // 读取数据
        virtual int read(void *buffer, size_t length) override;

        // 读取数据
        virtual int read(ByteArray::ptr ba, size_t length) override;

        // 写入数据
        virtual int write(const void *buffer, size_t length) override;

        // 写入数据
        virtual int write(ByteArray::ptr ba, size_t length) override;

        // 关闭socket
        virtual void close() override;

        // 返回Socket类
        Socket::ptr getSocket() const { return m_socket; }

        // 返回是否连接
        bool isConnected() const;

        Address::ptr getRemoteAddress();
        Address::ptr getLocalAddress();
        std::string getRemoteAddressString();
        std::string getLocalAddressString();

    protected:
        Socket::ptr m_socket; // Socket类
        bool m_owner;         // 是否主控
    };

} // namespace deepblue

#endif
