#ifndef __DEEPBLUE_BYTEARRAY_H__
#define __DEEPBLUE_BYTEARRAY_H__

#include <memory>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

namespace deepblue
{
    // 二进制数组,提供基础类型的序列化,反序列化功能
    class ByteArray
    {
    public:
        typedef std::shared_ptr<ByteArray> ptr;

        // ByteArray的存储节点
        struct Node
        {
            // 无参构造函数
            Node();

            // 构造指定大小的内存块
            Node(size_t s);

            // 析构函数,释放内存
            ~Node();

            size_t size; // 内存块大小
            char *ptr;   // 内存块地址指针
            Node *next;  // 下一个内存块地址
        };

        // 使用指定长度的内存块构造ByteArray
        ByteArray(size_t base_size = 4096);

        // 析构函数
        ~ByteArray();

        void writeFint8(int8_t value);
        void writeFuint8(uint8_t value);
        void writeFint16(int16_t value);
        void writeFuint16(uint16_t value);
        void writeFint32(int32_t value);
        void writeFuint32(uint32_t value);
        void writeFint64(int64_t value);
        void writeFuint64(uint64_t value);

        void writeInt32(int32_t value);
        void writeUint32(uint32_t value);
        void writeInt64(int64_t value);
        void writeUint64(uint64_t value);

        void writeFloat(float value);
        void writeDouble(double value);

        void writeStringF16(const std::string &value);
        void writeStringF32(const std::string &value);
        void writeStringF64(const std::string &value);
        void writeStringVint(const std::string &value);
        void writeStringWithoutLength(const std::string &value);

        int8_t readFint8();
        uint8_t readFuint8();
        int16_t readFint16();
        uint16_t readFuint16();
        int32_t readFint32();
        uint32_t readFuint32();
        int64_t readFint64();
        uint64_t readFuint64();

        int32_t readInt32();
        uint32_t readUint32();
        int64_t readInt64();
        uint64_t readUint64();

        float readFloat();
        double readDouble();

        std::string readStringF16();
        std::string readStringF32();
        std::string readStringF64();
        std::string readStringVint();

        // 清空ByteArray
        void clear();

        // 写入size长度的数据
        void write(const void *buf, size_t size);

        // 读取size长度的数据
        void read(void *buf, size_t size);

        // 从position, 读取size长度的数据
        void read(void *buf, size_t size, size_t position) const;

        // 把ByteArray的数据写入到文件中
        bool writeToFile(const std::string &name) const;

        // 从文件中读取数据
        bool readFromFile(const std::string &name);

        // 返回ByteArray当前位置
        size_t getPosition() const { return m_position; }

        // 设置ByteArray当前位置
        void setPosition(size_t v);

        // 返回内存块的大小
        size_t getBaseSize() const { return m_baseSize; }

        // 返回可读取数据大小
        size_t getReadSize() const { return m_size - m_position; }

        // 是否是小端
        bool isLittleEndian() const;

        // 设置是否为小端
        void setIsLittleEndian(bool val);

        // 将ByteArray里面的数据[m_position, m_size)转成std::string
        std::string toString() const;

        //  将ByteArray里面的数据[m_position, m_size)转成16进制的std::string(格式:FF FF FF)
        std::string toHexString() const;

        // 获取可读取的缓存,保存成iovec数组
        uint64_t getReadBuffers(std::vector<iovec> &buffers, uint64_t len = ~0ull) const;

        // 获取可读取的缓存,保存成iovec数组,从position位置开始
        uint64_t getReadBuffers(std::vector<iovec> &buffers, uint64_t len, uint64_t position) const;

        // 获取可写入的缓存,保存成iovec数组
        uint64_t getWriteBuffers(std::vector<iovec> &buffers, uint64_t len);

        // 返回数据的长度
        size_t getSize() const { return m_size; }

    private:
        // 扩容ByteArray, 使其可以容纳size个数据(如果原本可以可以容纳, 则不扩容)
        void addCapacity(size_t size);

        // 获取当前的可写入容量
        size_t getCapacity() const { return m_capacity - m_position; }

    private:
        size_t m_baseSize; // 内存块大小
        size_t m_position; // 当前操作位置
        size_t m_capacity; // 当前的总容量
        size_t m_size;     // 当前的大小
        int8_t m_endian;   // 字节序，默认大端
        Node *m_root;      // 第一个内存块指针
        Node *m_cur;       // 当前操作的内存块指针
    };
}

#endif