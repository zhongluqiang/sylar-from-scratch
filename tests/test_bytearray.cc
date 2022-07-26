/**
 * @file test_bytearray.cc
 * @brief Bytearray类测试
 * @version 0.1
 * @date 2021-09-18
 */
#include <algorithm>
#include <sylar/sylar.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test() {
/* 
 * 测试用例设计：
 * 随机生成长度为len，类型为type的数组，调用write_fun将这个数组全部写入块大小为base_len的ByteArray对象中，
 * 再将ByteArray的当前操作位置重设为0，也就是从起点开始，用read_fun重复读取数据，并与写入的数据比较，
 * 当读取出的数据与写入的数据全部相等时，该测试用例通过
 */
#define XX(type, len, write_fun, read_fun, base_len)               \
    {                                                              \
        std::vector<type> vec;                                     \
        for (int i = 0; i < len; ++i) {                            \
            vec.push_back(rand());                                 \
        }                                                          \
        sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len));  \
        for (auto &i : vec) {                                      \
            ba->write_fun(i);                                      \
        }                                                          \
        ba->setPosition(0);                                        \
        for (size_t i = 0; i < vec.size(); ++i) {                  \
            type v = ba->read_fun();                               \
            SYLAR_ASSERT(v == vec[i]);                             \
        }                                                          \
        SYLAR_ASSERT(ba->getReadSize() == 0);                      \
        SYLAR_LOG_INFO(g_logger) << #write_fun "/" #read_fun       \
                                               " (" #type ") len=" \
                                 << len                            \
                                 << " base_len=" << base_len       \
                                 << " size=" << ba->getSize();     \
    }

    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

/* 
 * 测试用例设计：
 * 在前面的测试用例基础上，增加文件序列化和反序列化操作，
 * 当写入文件的内容与从文件读取出的内容完全一致时，测试用例通过
 */
#define XX(type, len, write_fun, read_fun, base_len)                                  \
    {                                                                                 \
        std::vector<type> vec;                                                        \
        for (int i = 0; i < len; ++i) {                                               \
            vec.push_back(rand());                                                    \
        }                                                                             \
        sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len));                     \
        for (auto &i : vec) {                                                         \
            ba->write_fun(i);                                                         \
        }                                                                             \
        ba->setPosition(0);                                                           \
        for (size_t i = 0; i < vec.size(); ++i) {                                     \
            type v = ba->read_fun();                                                  \
            SYLAR_ASSERT(v == vec[i]);                                                \
        }                                                                             \
        SYLAR_ASSERT(ba->getReadSize() == 0);                                         \
        SYLAR_LOG_INFO(g_logger) << #write_fun "/" #read_fun                          \
                                               " (" #type ") len="                    \
                                 << len                                               \
                                 << " base_len=" << base_len                          \
                                 << " size=" << ba->getSize();                        \
        ba->setPosition(0);                                                           \
        SYLAR_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-" #read_fun ".dat"));   \
        sylar::ByteArray::ptr ba2(new sylar::ByteArray(base_len * 2));                \
        SYLAR_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
        ba2->setPosition(0);                                                          \
        SYLAR_ASSERT(ba->toString() == ba2->toString());                              \
        SYLAR_ASSERT(ba->getPosition() == 0);                                         \
        SYLAR_ASSERT(ba2->getPosition() == 0);                                        \
    }
    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

/* 
 * 测试用例设计：
 * 在前面的测试基础上，增加对字符串序列化/反序列化的测试
 */
#define XX(len, write_fun, read_fun, base_len)                    \
    {                                                             \
        std::string s = "qwertyuiopasdfghjklzxcvbnm";             \
        std::vector<std::string> vec;                             \
        for (int i = 0; i < len; i++) {                           \
            random_shuffle(s.begin(), s.end());                   \
            vec.push_back(s);                                     \
        }                                                         \
        sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len)); \
        for (auto &i : vec) {                                     \
            ba->write_fun(i);                                     \
        }                                                         \
        ba->setPosition(0);                                       \
        for (size_t i = 0; i < vec.size(); ++i) {                 \
            std::string v = ba->read_fun();                       \
            SYLAR_ASSERT(v == vec[i]);                            \
        }                                                         \
        SYLAR_ASSERT(ba->getReadSize() == 0);                     \
        SYLAR_LOG_INFO(g_logger) << #write_fun "/" #read_fun      \
                                               " ("               \
                                               "string"           \
                                               ") len="           \
                                 << len                           \
                                 << " base_len=" << base_len      \
                                 << " size=" << ba->getSize();    \
    }
    XX(100, writeStringF16, readStringF16, 10);
    XX(100, writeStringF32, readStringF32, 10);
    XX(100, writeStringF64, readStringF64, 10);
    XX(100, writeStringVint, readStringVint, 26);
#undef XX
}

int main(int argc, char *argv[]) {
    test();
    return 0;
}