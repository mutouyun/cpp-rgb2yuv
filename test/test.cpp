// test.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#include "../include/rgb2yuv.hpp"

#include "stopwatch.hpp"

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace r2y;

    uint32_t xx = 2179683; // 00000000 00100001 01000010 01100011 (33, 66, 99)
    rgb_t yy;
	memcpy(&yy, &xx, sizeof(yy));
    //yy.r_ = 76; yy.g_ = 255; yy.b_ = 29;
	printf("%03d %03d %03d\n", yy.r_, yy.g_, yy.b_);

    yuv_t zz;
    zz.y_ = pixel_convert<plane_Y>(yy);
    zz.u_ = pixel_convert<plane_U>(yy);
    zz.v_ = pixel_convert<plane_V>(yy);
    printf("%03d %03d %03d\n", zz.y_, zz.u_, zz.v_);

    //zz.y_ = 255; zz.u_ = zz.v_ = 0;
    yy.r_ = pixel_convert<plane_R>(zz);
    yy.g_ = pixel_convert<plane_G>(zz);
    yy.b_ = pixel_convert<plane_B>(zz);
    printf("%03d %03d %03d\n", yy.r_, yy.g_, yy.b_);

    uint32_t data[] =
    {
        6504993, 2179683, 2179683, 2179683,
        2179683, 8421504, 2179683, 2179683,
        2179683, 2179683, 2179683, 2179683,
        2179683, 2179683, 2179683, 8421504
    };
    scope_block<uint8_t> yuv;

    printf("RGB-888X: ");
    //rgb_t rgb_888[sizeof(data) / sizeof(uint32_t)];
    //rgb_format<rgb_888X>((uint8_t*)data, sizeof(data), rgb_888);
    uint8_t* rgb = (uint8_t*)data;
    for (size_t i = 0; i < sizeof(data); ++i)
        printf("%02X ", rgb[i]);
    printf("\n");

#define TEST_(TO)                                                     \
    yuv = transform<r2y::rgb_888X, yuv_##TO>((uint8_t*)data, 4, 4);   \
    printf("-> %s: ", #TO);                                           \
    for (size_t i = 0; i < yuv.count(); ++i) printf("%02X ", yuv[i]); \
    printf("\n")

    TEST_(YV12);
    TEST_(YU12);
    TEST_(NV12);
    TEST_(NV21);
    TEST_(YUY2);
    TEST_(VYUY);
    TEST_(Y41P);
    TEST_(411P);
    TEST_(422P);
    TEST_(NV24);
    TEST_(NV42);
    TEST_(YUV9);
    TEST_(YVU9);

    simple::stopwatch<> sw(false);
    printf("\n");
#define TEST_SPEED_(TO, ...)                                                    \
    sw.start();                                                                 \
    for (int i = 0; i < 2000000; ++i)                                           \
    {                                                                           \
        yuv = transform##__VA_ARGS__<rgb_888X, yuv_##TO>((uint8_t*)data, 4, 4); \
    }                                                                           \
    printf("%s: %d ms. %s\n", #TO, static_cast<size_t>(sw.value() * 1000), ""#__VA_ARGS__)

    TEST_SPEED_(YUV9);
    TEST_SPEED_(NV12);
    TEST_SPEED_(NV24);
    TEST_SPEED_(YUY2);
    TEST_SPEED_(411P);
    TEST_SPEED_(Y41P);

    return 0;
}
