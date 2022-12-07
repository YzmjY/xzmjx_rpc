//
// Created by 20132 on 2022/11/29.
//

#ifndef XZMJX_STREAM_H
#define XZMJX_STREAM_H
#include <memory>
#include "utils/byte_array.h"
namespace xzmjx{
class Stream{
public:
    typedef std::shared_ptr<Stream> ptr;

    virtual ~Stream() { }

    virtual int read(void* buf,size_t length) = 0;
    virtual int read(ByteArray::ptr ba,size_t length) = 0;
    virtual int readFixedSize(void* buf,size_t length);
    virtual int readFixedSize(ByteArray::ptr ba,size_t length);

    virtual int write(const void* buf,size_t length) = 0;
    virtual int write(ByteArray::ptr ba,size_t length) = 0;
    virtual int writeFixedSize(const void* buf,size_t length);
    virtual int writeFixedSize(ByteArray::ptr ba,size_t length);

    virtual void close() = 0;
};
}///namespace xzmjx



#endif //XZMJX_STREAM_H
