//
// Created by chenlong on 2021/9/14.
//

#ifndef IOMONITOR_X_LOG_BUFFER_HEADER_H
#define IOMONITOR_X_LOG_BUFFER_HEADER_H


#include <cstdint>

namespace xlog {
    class BufferHeader {
    private:
        const static uint32_t kMagic = 0xCEBFAABB;
        const static uint32_t kVersion = 1;
        constexpr static uint32_t kHeaderLen = 4 * sizeof(uint32_t);
        static char* magic_ptr(char *ptr) {
            return ptr;
        }
        static char* version_ptr(char *ptr) {
            return ptr + sizeof(uint32_t);
        }
        static char* header_len_ptr(char *ptr) {
            return ptr + 2* sizeof(uint32_t);
        }
        static char* buffer_pos_ptr(char *ptr) {
            return ptr + 3* sizeof(uint32_t);
        }

    private:
        uint32_t magic_ = kMagic;
        uint32_t version_ = kVersion;
        uint32_t header_len_ = kHeaderLen;
        uint32_t buffer_pos_ = 0;
        char* buffer_;
    private:
        void read_exist_header();
        void make_buffer_header();
    public:
        BufferHeader();
        BufferHeader(char *buffer);
        ~BufferHeader();
//        BufferHeader(const BufferHeader&);
//        BufferHeader& operator=(const BufferHeader&);
        uint32_t get_header_len() const;
        uint32_t get_buffer_pos() const;
        void increase_buffer_pos(uint32_t pos);
        void reset_buffer();
    };

}

#endif //IOMONITOR_X_LOG_BUFFER_HEADER_H
