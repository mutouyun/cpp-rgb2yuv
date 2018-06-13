/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://orzz.org)
*/

namespace detail_helper_ {

/* YUV Packed */

template <R2Y_ supported> struct packed_yuv_t;

template <> struct packed_yuv_t<R2Y_ yuv_YUY2> { GLB_ uint8_t y0_, cb_, y1_, cr_; };
template <> struct packed_yuv_t<R2Y_ yuv_YVYU> { GLB_ uint8_t y0_, cr_, y1_, cb_; };
template <> struct packed_yuv_t<R2Y_ yuv_UYVY> { GLB_ uint8_t cb_, y0_, cr_, y1_; };
template <> struct packed_yuv_t<R2Y_ yuv_VYUY> { GLB_ uint8_t cr_, y0_, cb_, y1_; };
template <> struct packed_yuv_t<R2Y_ yuv_Y41P> { GLB_ uint8_t u0_, y0_, v0_, y1_;
                                                 GLB_ uint8_t u1_, y2_, v1_, y3_;
                                                 GLB_ uint8_t y4_, y5_, y6_, y7_; };

/* YUV Planar */

template <R2Y_ supported> struct planar_uv_t { GLB_ uint8_t * cb_, *cr_; };

template <> struct planar_uv_t<R2Y_ yuv_NV24> { struct { GLB_ uint8_t cb_, cr_; } * uv_; };
template <> struct planar_uv_t<R2Y_ yuv_NV42> { struct { GLB_ uint8_t cr_, cb_; } * uv_; };
template <> struct planar_uv_t<R2Y_ yuv_NV12> { struct { GLB_ uint8_t cb_, cr_; } * uv_; };
template <> struct planar_uv_t<R2Y_ yuv_NV21> { struct { GLB_ uint8_t cr_, cb_; } * uv_; };

template <R2Y_ supported S, R2Y_ plane_type P>
auto split(R2Y_ byte_t * in_data, GLB_ size_t /*in_size*/)
    -> STD_ enable_if_t<(P == R2Y_ plane_Y), R2Y_ byte_t *>
{
    return in_data;
}

template <R2Y_ supported S, R2Y_ plane_type P>
auto split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
    -> STD_ enable_if_t<((P == R2Y_ plane_U) && (S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P ||
                                                 S == R2Y_ yuv_422P || S == R2Y_ yuv_YUV9)) ||
                        ((P == R2Y_ plane_V) && (S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YVU9)), R2Y_ byte_t *>
{
    return in_data + in_size;
}

template <R2Y_ supported S, R2Y_ plane_type P>
auto split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
    -> STD_ enable_if_t<((P == R2Y_ plane_V) && (S == R2Y_ yuv_422P)), R2Y_ byte_t *>
{
    return in_data + in_size + (in_size >> 1);
}

template <R2Y_ supported S, R2Y_ plane_type P>
auto split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
    -> STD_ enable_if_t<((P == R2Y_ plane_V) && (S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P)) ||
                        ((P == R2Y_ plane_U) && (S == R2Y_ yuv_YV12)), R2Y_ byte_t *>
{
    return in_data + in_size + (in_size >> 2);
}

template <R2Y_ supported S, R2Y_ plane_type P>
auto split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
    -> STD_ enable_if_t<((P == R2Y_ plane_V) && (S == R2Y_ yuv_YUV9)) ||
                        ((P == R2Y_ plane_U) && (S == R2Y_ yuv_YVU9)), R2Y_ byte_t *>

{
    return in_data + in_size + (in_size >> 4);
}

template <R2Y_ supported S>
auto fill(R2Y_ byte_t * in_data, GLB_ size_t in_size)
    -> STD_ enable_if_t<(S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YU12 ||
                         S == R2Y_ yuv_411P || S == R2Y_ yuv_422P ||
                         S == R2Y_ yuv_YUV9 || S == R2Y_ yuv_YVU9), R2Y_ detail_helper_::planar_uv_t<S>>

{
    return
    {
        R2Y_ detail_helper_::split<S, R2Y_ plane_U>(in_data, in_size),
        R2Y_ detail_helper_::split<S, R2Y_ plane_V>(in_data, in_size)
    };
}

template <R2Y_ supported S>
auto fill(R2Y_ byte_t * in_data, GLB_ size_t in_size)
    -> STD_ enable_if_t<(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42 ||
                         S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21), R2Y_ detail_helper_::planar_uv_t<S>>
{
    return
    {
        reinterpret_cast<decltype(std::declval<R2Y_ detail_helper_::planar_uv_t<S>>().uv_)>(
            R2Y_ detail_helper_::split<R2Y_ yuv_YU12, R2Y_ plane_U>(in_data, in_size))
    };
}

template <R2Y_ supported S>
struct yuv_planar
{
    template <typename Y, typename UV>
    yuv_planar(Y & y, UV & uv, R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
    {
        GLB_ size_t size = in_w * in_h;
        y  = R2Y_ detail_helper_::split<S, R2Y_ plane_Y>(in_data, size);
        uv = R2Y_ detail_helper_::fill<S>(in_data, size);
    }
};

} // namespace detail_helper_
