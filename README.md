#  rgb2yuv

一个轻量级的转换工具, 用于把RGB像素块转换为YUV.
使用时仅需include rgb2yuv.hpp, 库本身不需要编译.

## 支持的RGB格式

    RGB 888  - 24位
    RGB 565  - 16位
    RGB 555  - 16位
    RGB 444  - 12位(无padding, 3字节2像素)
    RGB 888X - 32位

## 支持的YUV格式

    NV24 - YUV 4:4:4, Planar, Combined CbCr planes
    NV42 - YUV 4:4:4, Planar, Combined CbCr planes
    YUY2 - YUV 4:2:2, Packed
    YUYV - YUV 4:2:2, Packed, Same as YUY2
    YVYU - YUV 4:2:2, Packed
    UYVY - YUV 4:2:2, Packed
    VYUY - YUV 4:2:2, Packed
    422P - YUV 4:2:2, Planar
    YV12 - YUV 4:2:0, Planar
    YU12 - YUV 4:2:0, Planar
    I420 - YUV 4:2:0, Planar, Same as YU12
    NV12 - YUV 4:2:0, Planar, Combined CbCr planes
    NV21 - YUV 4:2:0, Planar, Combined CbCr planes
    Y41P - YUV 4:1:1, Packed
    Y411 - YUV 4:1:1, Packed, Same as Y41P
    411P - YUV 4:1:1, Planar
    YVU9 - YUV 4:1:0, Planar
    YUV9 - YUV 4:1:0, Planar