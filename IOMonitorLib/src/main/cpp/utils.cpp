//
// Created by chenlong on 2021/9/1.
//

#include "utils.h"
#include <ctime>

int64_t current_time() {
    struct timeval timeval{};
    gettimeofday(&timeval, nullptr);
    return (int64_t)timeval.tv_sec * 1000000 + (int64_t)timeval.tv_usec;
}

int64_t get_tick_count_micros() {
    struct timespec ts{};
    int ret = clock_gettime(CLOCK_BOOTTIME, &ts);
    if (ret != 0) {
        return 0;
    }
    return (int64_t)ts.tv_sec * 1000000 + (int64_t)ts.tv_nsec/1000;
}
