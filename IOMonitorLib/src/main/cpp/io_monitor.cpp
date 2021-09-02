//
// Created by chenlong on 2021/8/27.
//
#define TAG "IOMonitor"

#include "io_monitor.h"
#include <string>
#include <memory>
#include <utility>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>
#include "utils.h"
#include "io_log.h"

iomonitor::JavaContext::JavaContext(std::string stack, std::string thread_name, long tid): stack_(std::move(stack)), thread_name_(std::move(thread_name)), tid_(tid) {}

iomonitor::FileDesc::FileDesc(int _fd, std::string _path, JavaContext _java_context) :
        fd(_fd),
        path(std::move(_path)),
        start_time_us(current_time()),
        op_type(FileOpType::kInit),
        java_context(std::move(_java_context)){}

iomonitor::IOMonitor &iomonitor::IOMonitor::get() {
    static IOMonitor kIoMonitor;
    return kIoMonitor;
}

void iomonitor::IOMonitor::on_open(int fd, const char *path, int flags, mode_t mode, JavaContext java_context) {
    auto file_desc = std::make_shared<FileDesc>(fd, path, java_context);
    open_file_desc_.insert(std::make_pair(fd, file_desc));
}

void iomonitor::IOMonitor::on_read(int fd, void *buf, size_t count, ssize_t read_ret, long read_cost) {
    if (read_ret == -1 || read_cost < 0) {
        return;
    }
    if (open_file_desc_.find(fd) == open_file_desc_.end()) {
        return;
    }
    trace_read_write(fd, FileOpType::kRead, count, read_cost);
}

void iomonitor::IOMonitor::on_write(int fd, const void *buf, size_t count, ssize_t write_ret, long write_cost) {
    if (write_ret == -1 || write_cost < 0) {
        return;
    }
    if (open_file_desc_.find(fd) == open_file_desc_.end()) {
        return;
    }
    trace_read_write(fd, FileOpType::kWrite, count, write_cost);
}

void iomonitor::IOMonitor::on_close(int fd) {
    if (open_file_desc_.find(fd) == open_file_desc_.end()) {
        return;
    }
    auto file = open_file_desc_[fd];
    int64_t now = current_time();
    file->total_cost_us = now - file->start_time_us;
    struct stat file_stat{};
    int ret = stat(file->path.c_str(), &file_stat);
    if (ret == 0) {
        file->file_size = static_cast<long>(file_stat.st_size);
    }

    open_file_desc_.erase(fd);

    //dump io info;
    std::string info;
    info.append("----------------------------------------------\n");
    info.append("path:").append(file->path).append("\n");
    if (file->op_type == FileOpType::kWrite) {
        info.append("write").append("\n");
    } else {
        info.append("read").append("\n");
    }
    info.append("buffer size:").append(std::to_string(file->buffer_size)).append("\n");
    info.append("op count:").append(std::to_string(file->op_cnt)).append("\n");
    info.append("rw_cost_us:").append(std::to_string(file->rw_cost_us)).append("\n");
    info.append("max_once_rw_cost_time_us:").append(std::to_string(file->max_once_rw_cost_time_us)).append("\n");
    info.append("total_cost_us:").append(std::to_string(file->total_cost_us)).append("\n");
    info.append("max_continual_rw_cost_time_us:").append(std::to_string(file->max_continual_rw_cost_time_us)).append("\n");
    info.append("file_size:").append(std::to_string(file->file_size)).append("\n");
    info.append("thread:").append(std::to_string(file->java_context.tid_)).append(":").append(file->java_context.thread_name_).append("\n");
    info.append("\n");
    info.append(file->java_context.stack_).append("\n");
    info.append("----------------------------------------------\n");
    LOGI("%s", info.c_str());

}

void iomonitor::IOMonitor::trace_read_write(int fd, iomonitor::FileOpType opType, long op_size,
                                            long rw_cost) {
    std::shared_ptr<FileDesc> file = open_file_desc_[fd];
    long now = current_time();
    if (file->op_type == FileOpType::kInit) {//第一次读写操作确定本次操作的类型，所以一个文件以读写模式打开会咋样？
        file->op_type = opType;
    }
    //累加读写次数
    file->op_cnt ++;
    //找到最大的一次buffer_size
    if (op_size > file->buffer_size) {
        file->buffer_size = op_size;
    }
    //累积读写大小
    file->op_size += op_size;
    //累加读写耗时
    file->rw_cost_us += rw_cost;
    //最大单次读写耗时
    if (rw_cost > file->max_once_rw_cost_time_us) {
        file->max_once_rw_cost_time_us = rw_cost;
    }
    if (now - file->last_rw_time_us < 8000) {
        file->current_continual_rw_time_us += rw_cost;
    } else {
        file->current_continual_rw_time_us = rw_cost;
    }
    file->last_rw_time_us = now;
    //最大连续读写耗时
    file->max_continual_rw_cost_time_us = file->current_continual_rw_time_us;

}