//
// Created by chenlong on 2021/8/27.
//

#ifndef IOMONITOR_IO_MONITOR_H
#define IOMONITOR_IO_MONITOR_H

#include "io_log.h"
#include <jni.h>
#include <fcntl.h>
#include <string>
#include <map>
#include <vector>
#define TAG "IOMonitor"

namespace iomonitor {
    enum FileOpType {
        kInit,
        kRead,
        kWrite
    };

    class JavaContext {
    public:
        std::string stack_;
        std::string thread_name_;
        long tid_;

        JavaContext(std::string stack, std::string thread_name, long tid);
    };

    class FileDesc {

    public:
        int fd;
        std::string path;
        int64_t start_time_us;
        FileOpType op_type;
        JavaContext java_context;
        int op_cnt = 0;
        long buffer_size = 0;
        long op_size = 0;
        long rw_cost_us = 0;
        long max_continual_rw_cost_time_us = 0;
        long max_once_rw_cost_time_us = 0;
        long current_continual_rw_time_us = 0;
        int64_t last_rw_time_us = 0;
        long file_size = 0;
        long total_cost_us = 0;

        FileDesc(int _fd, std::string _path, JavaContext _javaContext);
    };

    class IOMonitor {
    private:

        std::map<int, std::shared_ptr<FileDesc>> open_file_desc_;

        IOMonitor() {}

        ~IOMonitor() {}

        void trace_read_write(int fd, FileOpType opType, long op_size, long rw_cost);

    public:
        IOMonitor(const IOMonitor &) = delete;

        IOMonitor &operator=(const IOMonitor &) = delete;

        static IOMonitor &get();

        void on_open(int fd, const char *path, int flags, mode_t mode, JavaContext java_context);

        void on_read(int fd, void *buf, size_t count, ssize_t read_ret, long read_cost);

        void on_write(int fd, const void *buf, size_t count, ssize_t read_ret, long read_cost);

        void on_close(int fd);
    };
}


#endif //IOMONITOR_IO_MONITOR_H
