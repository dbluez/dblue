#ifndef __FD_MANAGER_H__
#define __FD_MANAGER_H__

#include <memory>
#include <vector>
#include "thread.h"
#include "singleton.h"

namespace deepblue
{

    /**
     * @brief 文件句柄上下文类
     * @details 管理文件句柄类型(是否socket)
     *          是否阻塞,是否关闭,读/写超时时间
     */
    class FdCtx : public std::enable_shared_from_this<FdCtx>
    {
    public:
        typedef std::shared_ptr<FdCtx> ptr;

        // 通过文件句柄构造FdCtx
        FdCtx(int fd);

        // 析构函数
        ~FdCtx();

        // 是否初始化完成
        bool isInit() const { return m_isInit; }

        // 是否socket
        bool isSocket() const { return m_isSocket; }

        // 是否已关闭
        bool isClose() const { return m_isClosed; }

        // 设置用户主动设置非阻塞
        void setUserNonblock(bool v) { m_userNonblock = v; }

        // 获取是否用户主动设置的非阻塞
        bool getUserNonblock() const { return m_userNonblock; }

        // 设置系统非阻塞
        void setSysNonblock(bool v) { m_sysNonblock = v; }

        // 获取系统非阻塞
        bool getSysNonblock() const { return m_sysNonblock; }

        /**
         * @brief 设置超时时间
         * @param[in] type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
         * @param[in] v 时间毫秒
         */
        void setTimeout(int type, uint64_t v);

        /**
         * @brief 获取超时时间
         * @param[in] type 类型SO_RCVTIMEO(读超时), SO_SNDTIMEO(写超时)
         * @return 超时时间毫秒
         */
        uint64_t getTimeout(int type);

    private:
        // 初始化
        bool init();

    private:
        bool m_isInit : 1;       // 是否初始化
        bool m_isSocket : 1;     // 是否socket
        bool m_sysNonblock : 1;  // 是否hook非阻塞
        bool m_userNonblock : 1; // 是否用户主动设置非阻塞
        bool m_isClosed : 1;     // 是否关闭
        int m_fd;                // 文件句柄
        uint64_t m_recvTimeout;  // 读超时时间毫秒
        uint64_t m_sendTimeout;  // 写超时时间毫秒
    };

    // 文件句柄管理类
    class FdManager
    {
    public:
        typedef RWMutex RWMutexType;

        FdManager();

        // 获取/创建文件句柄类FdCtx
        FdCtx::ptr get(int fd, bool auto_create = false);

        // 删除文件句柄类
        void del(int fd);

    private:
        RWMutexType m_mutex;             // 读写锁
        std::vector<FdCtx::ptr> m_datas; // 文件句柄集合
    };

    typedef Singleton<FdManager> FdMgr; // 文件句柄单例

}

#endif
