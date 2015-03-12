/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

////////////////////////////////////////////////////////////////
/// Provide basic universal types and constants
////////////////////////////////////////////////////////////////

typedef          GLB_ uint8_t               byte_t;
typedef struct { GLB_ uint8_t b_, g_, r_; } rgb_t;
typedef struct { GLB_ uint8_t v_, u_, y_; } yuv_t;

enum supported
{
    rgb_MIN,
    rgb_888,
    rgb_565,
    rgb_555,
    rgb_444,
    rgb_888X,
    rgb_MAX,

    /*
     * YUV Formats Chapter 2. Image Formats
     * See: http://www.linuxtv.org/downloads/v4l-dvb-apis/yuv-formats.html
    */
    yuv_MIN,
    yuv_NV24,            // 444 SP
    yuv_NV42,
    yuv_YUY2,            // 422
    yuv_YUYV = yuv_YUY2,
    yuv_YVYU,
    yuv_UYVY,
    yuv_VYUY,
    yuv_422P,            // 422 P
    yuv_YV12,            // 420 P
    yuv_YU12,
    yuv_I420 = yuv_YU12,
    yuv_NV12,            // 420 SP
    yuv_NV21,
    yuv_Y41P,            // 411
    yuv_Y411 = yuv_Y41P,
    yuv_411P,            // 411 P
    yuv_YVU9,            // 410 P
    yuv_YUV9,
    yuv_MAX
};

enum plane_type
{
    plane_Y = 0,
    plane_U,
    plane_V,

    plane_R,
    plane_G,
    plane_B,

    plane_MAX
};

////////////////////////////////////////////////////////////////
/// Useful tools for SFINAE
////////////////////////////////////////////////////////////////

template <bool, typename T = void> struct enable_if          {  /* Nothing */  };
template <      typename T>        struct enable_if<true, T> { typedef T type; };

template <R2Y_ supported S> struct is_rgb
{
    enum { value = ((S > rgb_MIN) && (S < rgb_MAX)) ? 1 : 0 };
};

template <R2Y_ supported S> struct is_yuv
{
    enum { value = ((S > yuv_MIN) && (S < yuv_MAX)) ? 1 : 0 };
};

template <R2Y_ plane_type P> struct is_rgb_plane               { enum { value = 0 }; };
template <>                  struct is_rgb_plane<R2Y_ plane_R> { enum { value = 1 }; };
template <>                  struct is_rgb_plane<R2Y_ plane_G> { enum { value = 1 }; };
template <>                  struct is_rgb_plane<R2Y_ plane_B> { enum { value = 1 }; };

template <R2Y_ plane_type P> struct is_yuv_plane               { enum { value = 0 }; };
template <>                  struct is_yuv_plane<R2Y_ plane_Y> { enum { value = 1 }; };
template <>                  struct is_yuv_plane<R2Y_ plane_U> { enum { value = 1 }; };
template <>                  struct is_yuv_plane<R2Y_ plane_V> { enum { value = 1 }; };