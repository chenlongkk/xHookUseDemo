package com.cck.io.monitor;

/**
 * 3.1 检测主线程 I/O
 * 耗时的 IO 操作不能占据主线程太久。检测条件：
 * 1. 操作线程为主线程
 * 2. 连续读写耗时超过一定阈值或单次 write\read 耗时超过一定阈值
 * <p>
 * 3.2 读写Buffer过小
 * Buffer 过小，会导致 read/write 的次数增多，从而影响了性能。检测条件：
 * 1. buffer 小于一定阈值
 * 2. read/write 的次数超过一定的阈值
 * <p>
 * 3.3 重复读
 * 如果频繁地读某个文件，证明这个文件的内容很常被用到，可以通过缓存来提高效率。检测条件如下：
 * 1. 同一线程读某个文件的次数超过一定阈值
 */
public enum IOEvent {
    MAIN_THREAD_IO,
    SMALL_BUFFER,
    DUP_READ,
}
