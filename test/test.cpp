// test.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "../include/rgb2yuv.hpp"

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace r2y;

    uint32_t xx = 2179683; // 00000000 00100001 01000010 01100011 (33, 66, 99)
    rgb_t yy;
    rgb_format<rgb_888X>((uint8_t*)&xx, 4, &yy);
    printf("%03d %03d %03d\n", yy.r_, yy.g_, yy.b_);

    yuv_t zz;
    zz.y_ = pixel_convert<plane_Y>(yy);
    zz.u_ = pixel_convert<plane_U>(yy);
    zz.v_ = pixel_convert<plane_V>(yy);
    printf("%03d %03d %03d\n", zz.y_, zz.u_, zz.v_);

    yy.r_ = pixel_convert<plane_R>(zz);
    yy.g_ = pixel_convert<plane_G>(zz);
    yy.b_ = pixel_convert<plane_B>(zz);
    printf("%03d %03d %03d\n", yy.r_, yy.g_, yy.b_);

    uint32_t data[] =
    {
        2179683, 2179683, 2179683, 2179683,
        2179683, 2179683, 2179683, 2179683,
        2179683, 2179683, 2179683, 2179683,
        2179683, 2179683, 2179683, 2179683
    };
    scope_block<uint8_t> yuv;

    printf("RGB-888: ");
    rgb_t rgb_888[sizeof(data) / sizeof(uint32_t)];
    rgb_format<rgb_888X>((uint8_t*)data, sizeof(data), rgb_888);
    uint8_t* rgb = (uint8_t*)rgb_888;
    for (size_t i = 0; i < sizeof(rgb_888); ++i)
        printf("%02X ", rgb[i]);
    printf("\n");

#define TEST_(FROM, TO)                                               \
    yuv.move( create_buffer<yuv_##TO>(4, 4) );                        \
    transform<rgb_##FROM, yuv_##TO>((uint8_t*)data, 4, 4, &yuv);      \
    printf("-> %s: ", #TO);                                           \
    for (size_t i = 0; i < yuv.count(); ++i) printf("%02X ", yuv[i]); \
    printf("\n")

    TEST_(888X, YV12);
    TEST_(888X, YU12);
    TEST_(888X, NV12);
    TEST_(888X, NV21);
    TEST_(888X, YUY2);
    TEST_(888X, VYUY);
    TEST_(888X, Y41P);
    TEST_(888X, 411P);
    TEST_(888X, 422P);
    TEST_(888X, NV24);
    TEST_(888X, NV42);
    TEST_(888X, YUV9);
    TEST_(888X, YVU9);

    return 0;
}
