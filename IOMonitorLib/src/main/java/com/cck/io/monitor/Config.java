package com.cck.io.monitor;

public class Config {
    public final boolean isDebug;
    public final int mainThreadIoTotalTimeMax;//主线程连续io总耗时最大值
    public final int mainThreadIoSingleTimeMax;//主线程单次io耗时最大值
    public final int bufferMin;//最小buffer
    public final int fileReadCountMax;//单个文件最大读次数
    public final int fileWriteCountMax;//单个最大写次数
    public final int sameReadCountMax;//同一个文件同一个线程最大读次数

    public Config(boolean isDebug, int mainThreadIoTotalTimeMax, int mainThreadIoSingleTimeMax, int bufferMin, int fileReadCountMax, int fileWriteCountMax, int sameReadCountMax) {
        this.isDebug = isDebug;
        this.mainThreadIoTotalTimeMax = mainThreadIoTotalTimeMax;
        this.mainThreadIoSingleTimeMax = mainThreadIoSingleTimeMax;
        this.bufferMin = bufferMin;
        this.fileReadCountMax = fileReadCountMax;
        this.fileWriteCountMax = fileWriteCountMax;
        this.sameReadCountMax = sameReadCountMax;
    }
}
