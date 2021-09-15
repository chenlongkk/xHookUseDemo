//
// Created by chenlong on 2021/9/7.
//

#ifndef IOMONITOR_X_LOG_BUFFER_H
#define IOMONITOR_X_LOG_BUFFER_H

#include <mutex>
#include "x_log_buffer_header.h"
namespace xlog {
    //非线程安全的，使用时需要加锁
    class LogBuffer {
    private:
        bool use_mmap_;
        char* msg_buffer_;
        char* current_ptr_;
        BufferHeader header_;
    private:
        void* open_mmap_file(const char *path, size_t buffer_size);
    public:
        LogBuffer(const char *file_path);
        ~LogBuffer();
        bool append(const char*msg);
        void flush(void*target);
        int length();
    };
}



#endif //IOMONITOR_X_LOG_BUFFER_H
