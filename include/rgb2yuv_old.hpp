/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

////////////////////////////////////////////////////////////////
/// RGB formatting to 888
////////////////////////////////////////////////////////////////

namespace detail_rgb_format_ {

template <R2Y_ supported S> struct impl_;

template <> struct impl_<R2Y_ rgb_888>
{
    static void to_888(R2Y_UNUSED_ R2Y_ byte_t * in_rgb, R2Y_UNUSED_ GLB_ size_t in_size,
                       R2Y_UNUSED_ R2Y_ rgb_t  * ot_rgb)
    {
        assert(in_rgb == reinterpret_cast<R2Y_ byte_t *>(ot_rgb));
        /* Do nothing. */
    }
};

#pragma push_macro("R2Y_BMP_16_TRANSFORM_HELPER_")
#undef  R2Y_BMP_16_TRANSFORM_HELPER_
#define R2Y_BMP_16_TRANSFORM_HELPER_(MASK_R, MASK_G, MASK_B, SHIFT_R, SHIFT_G, SHIFT_B) do \
{                                                                                          \
    assert(in_rgb != reinterpret_cast<R2Y_ byte_t *>(ot_rgb));                             \
    assert((in_size & 1) == 0); /* in_size must be an even number */                       \
                                                                                           \
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_rgb);                 \
    for (GLB_ size_t i = 0; i < in_size; i += 2, ++cur_pixel, ++ot_rgb)                    \
    {                                                                                      \
        ot_rgb->r_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & (MASK_R)) SHIFT_R );       \
        ot_rgb->g_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & (MASK_G)) SHIFT_G );       \
        ot_rgb->b_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & (MASK_B)) SHIFT_B );       \
    }                                                                                      \
} while(0)

template <> struct impl_<R2Y_ rgb_565>
{
    static void to_888(R2Y_ byte_t * in_rgb, GLB_ size_t in_size, R2Y_ rgb_t * ot_rgb)
    {
        R2Y_BMP_16_TRANSFORM_HELPER_(0xF800, 0x07E0, 0x001F, >> 8, >> 3, << 3);
    }
};

template <> struct impl_<R2Y_ rgb_555>
{
    static void to_888(R2Y_ byte_t * in_rgb, GLB_ size_t in_size, R2Y_ rgb_t * ot_rgb)
    {
        R2Y_BMP_16_TRANSFORM_HELPER_(0x7C00, 0x03E0, 0x001F, >> 7, >> 2, << 3);
    }
};

#pragma pop_macro("R2Y_BMP_16_TRANSFORM_HELPER_")

template <> struct impl_<R2Y_ rgb_444>
{
    static void to_888(R2Y_ byte_t * in_rgb, GLB_ size_t in_size, R2Y_ rgb_t * ot_rgb)
    {
        assert(in_rgb != reinterpret_cast<R2Y_ byte_t *>(ot_rgb));
        assert((in_size % 3) == 0); // 3 input bytes convert to 2 pixels (6 bytes)

        for (GLB_ size_t i = 0; i < in_size; i += 3)
        {
            ot_rgb->b_ = static_cast<GLB_ uint8_t>( ((*in_rgb) & 0x0F) << 4 );
            ot_rgb->g_ = static_cast<GLB_ uint8_t>  ((*in_rgb) & 0xF0);        ++in_rgb;
            ot_rgb->r_ = static_cast<GLB_ uint8_t>( ((*in_rgb) & 0x0F) << 4 );
            ++ot_rgb;
            ot_rgb->b_ = static_cast<GLB_ uint8_t>  ((*in_rgb) & 0xF0);        ++in_rgb;
            ot_rgb->g_ = static_cast<GLB_ uint8_t>( ((*in_rgb) & 0x0F) << 4 );
            ot_rgb->r_ = static_cast<GLB_ uint8_t>  ((*in_rgb) & 0xF0);        ++in_rgb;
        }
    }
};

template <> struct impl_<R2Y_ rgb_888X>
{
    static void to_888(R2Y_ byte_t * in_rgb, GLB_ size_t in_size, R2Y_ rgb_t * ot_rgb)
    {
        assert(in_rgb != reinterpret_cast<R2Y_ byte_t *>(ot_rgb));
        assert((in_size % 4) == 0); // 4 input bytes convert to 1 pixel (3 bytes)

        GLB_ uint32_t * cur_pixel = reinterpret_cast<GLB_ uint32_t *>(in_rgb);
        for (GLB_ size_t i = 0; i < in_size; i += 4, ++cur_pixel, ++ot_rgb)
        {
            (*ot_rgb) = ( * reinterpret_cast<R2Y_ rgb_t *>(cur_pixel) );
        }
    }
};

} // namespace detail_rgb_format_

template <R2Y_ supported S>
void rgb_format(R2Y_ byte_t * in_rgb, GLB_ size_t in_size, R2Y_ rgb_t * ot_rgb)
{
    R2Y_ detail_rgb_format_::impl_<S>::to_888(in_rgb, in_size, ot_rgb);
}

////////////////////////////////////////////////////////////////
/// Transforming between RGB & YUV/YCbCr blocks
////////////////////////////////////////////////////////////////

namespace detail_transform_old_ {

#pragma push_macro("R2Y_DETAIL_")
#undef  R2Y_DETAIL_
#define R2Y_DETAIL_ R2Y_ detail_transform_old_::

template <R2Y_ supported S, int = R2Y_ is_rgb<S>::value>
struct rgb_formatter;

template <> struct rgb_formatter<R2Y_ rgb_888, 1>
{
    static R2Y_ scope_block<R2Y_ byte_t> to_888(R2Y_ byte_t * in_rgb, GLB_ size_t in_w, GLB_ size_t in_h)
    {
        R2Y_ scope_block<R2Y_ rgb_t> ret( reinterpret_cast<R2Y_ rgb_t *>(in_rgb), in_w * in_h );
        R2Y_ rgb_format<R2Y_ rgb_888>(in_rgb, ret.size(), ret.data());
        return ret;
    }
};

template <R2Y_ supported S> struct rgb_formatter<S, 1>
{
    static R2Y_ scope_block<R2Y_ byte_t> to_888(R2Y_ byte_t * in_rgb, GLB_ size_t in_w, GLB_ size_t in_h)
    {
        R2Y_ scope_block<R2Y_ rgb_t> ret( R2Y_ create_buffer<R2Y_ rgb_888>(in_w, in_h) );
        R2Y_ rgb_format<S>(in_rgb, R2Y_ calculate_size<S>(in_w, in_h), ret.data());
        return ret;
    }
};

/* Packed */

template <R2Y_ supported> struct yuv_t;
template <>               struct yuv_t<R2Y_ yuv_YUY2> { GLB_ uint8_t y0_, cb_, y1_, cr_; };
template <>               struct yuv_t<R2Y_ yuv_YVYU> { GLB_ uint8_t y0_, cr_, y1_, cb_; };
template <>               struct yuv_t<R2Y_ yuv_UYVY> { GLB_ uint8_t cb_, y0_, cr_, y1_; };
template <>               struct yuv_t<R2Y_ yuv_VYUY> { GLB_ uint8_t cr_, y0_, cb_, y1_; };

template <> struct yuv_t<R2Y_ yuv_Y41P>
{
    GLB_ uint8_t u0_, y0_, v0_, y1_;
    GLB_ uint8_t u1_, y2_, v1_, y3_;
    GLB_ uint8_t y4_, y5_, y6_, y7_;
};

/* YUV 4:2:2 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YUY2) || (S == R2Y_ yuv_YVYU) || 
                        (S == R2Y_ yuv_UYVY) || (S == R2Y_ yuv_VYUY)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_DETAIL_ yuv_t<S> * p_yuv = reinterpret_cast<R2Y_DETAIL_ yuv_t<S> *>( ot_data.data() );
    GLB_ size_t s = in_w * in_h;
    GLB_ uint16_t u_k, v_k;
    for (GLB_ size_t i = 0; i < s; ++p_yuv)
    {
        {
            R2Y_ rgb_t const & rgb = in_data[i]; ++i;
            p_yuv->y0_ = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb);
            u_k        = R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
            v_k        = R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
        }
        {
            R2Y_ rgb_t const & rgb = in_data[i]; ++i;
            p_yuv->y1_ = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb);
            p_yuv->cb_ = ( u_k += R2Y_ pixel_convert<R2Y_ plane_U>(rgb) ) >> 1;
            p_yuv->cr_ = ( v_k += R2Y_ pixel_convert<R2Y_ plane_V>(rgb) ) >> 1;
        }
    }
}

/* YUV 4:1:1 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_Y41P)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_DETAIL_ yuv_t<S> * p_yuv = reinterpret_cast<R2Y_DETAIL_ yuv_t<S> *>( ot_data.data() );
    GLB_ size_t s = in_w * in_h;
    GLB_ uint16_t u_k, v_k;
    for (GLB_ size_t i = 0; i < s; ++p_yuv)
    {
#   pragma push_macro("R2Y_TRANSFORM_TO_")
#   undef  R2Y_TRANSFORM_TO_
#   define R2Y_TRANSFORM_TO_(I, OP, ...) do                         \
        {                                                           \
            R2Y_ rgb_t const & rgb = in_data[i]; ++i;               \
            p_yuv->y##I##_ = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); \
            u_k           OP R2Y_ pixel_convert<R2Y_ plane_U>(rgb); \
            v_k           OP R2Y_ pixel_convert<R2Y_ plane_V>(rgb); \
            __VA_ARGS__                                             \
        } while(0)

        R2Y_TRANSFORM_TO_(0,  =);
        R2Y_TRANSFORM_TO_(1, +=);
        R2Y_TRANSFORM_TO_(2, +=);
        R2Y_TRANSFORM_TO_(3, +=, p_yuv->u0_ = u_k >> 2; p_yuv->v0_ = v_k >> 2;);
        R2Y_TRANSFORM_TO_(4,  =);
        R2Y_TRANSFORM_TO_(5, +=);
        R2Y_TRANSFORM_TO_(6, +=);
        R2Y_TRANSFORM_TO_(7, +=, p_yuv->u1_ = u_k >> 2; p_yuv->v1_ = v_k >> 2;);

#   pragma pop_macro("R2Y_TRANSFORM_TO_")
    }
}

/* Planar */

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<(P == R2Y_ plane_Y),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
{
    return in_data;
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_U) && (S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P || 
                                                  S == R2Y_ yuv_422P || S == R2Y_ yuv_YUV9) ) ||
                        ( (P == R2Y_ plane_V) && (S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YVU9) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
{
    return in_data + (in_w * in_h);
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_V) && (S == R2Y_ yuv_422P) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    return in_data + s + (s >> 1);
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_V) && (S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P) ) ||
                        ( (P == R2Y_ plane_U) && (S == R2Y_ yuv_YV12) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    return in_data + s + (s >> 2);
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_V) && (S == R2Y_ yuv_YUV9) ) ||
                        ( (P == R2Y_ plane_U) && (S == R2Y_ yuv_YVU9) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    return in_data + s + (s >> 4);
}

template <R2Y_ supported> struct uv_t                { GLB_ uint8_t * cb_, * cr_; };
template <>               struct uv_t<R2Y_ yuv_NV24> { struct inner { GLB_ uint8_t cb_, cr_; } * uv_; };
template <>               struct uv_t<R2Y_ yuv_NV42> { struct inner { GLB_ uint8_t cr_, cb_; } * uv_; };
template <>               struct uv_t<R2Y_ yuv_NV12> { struct inner { GLB_ uint8_t cb_, cr_; } * uv_; };
template <>               struct uv_t<R2Y_ yuv_NV21> { struct inner { GLB_ uint8_t cr_, cb_; } * uv_; };

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YU12 || 
                         S == R2Y_ yuv_411P || S == R2Y_ yuv_422P ||
                         S == R2Y_ yuv_YUV9 || S == R2Y_ yuv_YVU9),
R2Y_DETAIL_ uv_t<S> >::type fill(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
{
    R2Y_DETAIL_ uv_t<S> ret = 
    {
        R2Y_DETAIL_ split<S, R2Y_ plane_U>(in_data, in_w, in_h),
        R2Y_DETAIL_ split<S, R2Y_ plane_V>(in_data, in_w, in_h)
    };
    return ret;
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42 || 
                         S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21),
R2Y_DETAIL_ uv_t<S> >::type fill(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
{
    R2Y_DETAIL_ uv_t<S> ret = 
    {
        reinterpret_cast<typename R2Y_DETAIL_ uv_t<S>::inner *>(
                                  R2Y_DETAIL_ split<R2Y_ yuv_YU12, R2Y_ plane_U>(in_data, in_w, in_h))
    };
    return ret;
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_422P)
>::type set_and_next(GLB_ uint16_t in_u, GLB_ uint16_t in_v, R2Y_DETAIL_ uv_t<S> & ot_uv)
{
    (*(ot_uv.cb_)) = in_u >> 1; ++(ot_uv.cb_);
    (*(ot_uv.cr_)) = in_v >> 1; ++(ot_uv.cr_);
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P)
>::type set_and_next(GLB_ uint16_t in_u, GLB_ uint16_t in_v, R2Y_DETAIL_ uv_t<S> & ot_uv)
{
    (*(ot_uv.cb_)) = in_u >> 2; ++(ot_uv.cb_);
    (*(ot_uv.cr_)) = in_v >> 2; ++(ot_uv.cr_);
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21)
>::type set_and_next(GLB_ uint16_t in_u, GLB_ uint16_t in_v, R2Y_DETAIL_ uv_t<S> & ot_uv)
{
    ot_uv.uv_->cb_ = in_u >> 2;
    ot_uv.uv_->cr_ = in_v >> 2;
    ++(ot_uv.uv_);
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YUV9 || S == R2Y_ yuv_YVU9)
>::type set_and_next(GLB_ uint16_t in_u, GLB_ uint16_t in_v, R2Y_DETAIL_ uv_t<S> & ot_uv)
{
    (*(ot_uv.cb_)) = in_u >> 4; ++(ot_uv.cb_);
    (*(ot_uv.cr_)) = in_v >> 4; ++(ot_uv.cr_);
}

/* YUV 4:4:4 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_ byte_t * p_y = R2Y_DETAIL_ split<S, R2Y_ plane_Y>(ot_data.data(), in_w, in_h);
    R2Y_DETAIL_ uv_t<S> p_uv = R2Y_DETAIL_ fill<S>(ot_data.data(), in_w, in_h);
    GLB_ size_t s = in_w * in_h;
    for (GLB_ size_t i = 0; i < s; ++i, ++p_y, ++(p_uv.uv_))
    {
        R2Y_ rgb_t const & rgb = in_data[i];
        (*p_y)        = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb);
        p_uv.uv_->cb_ = R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
        p_uv.uv_->cr_ = R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
    }
}

/* YUV 4:2:2 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_422P)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_ byte_t * p_y = R2Y_DETAIL_ split<S, R2Y_ plane_Y>(ot_data.data(), in_w, in_h);
    R2Y_DETAIL_ uv_t<S> p_uv = R2Y_DETAIL_ fill<S>(ot_data.data(), in_w, in_h);
    GLB_ size_t s = in_w * in_h;
    GLB_ uint16_t u_k, v_k;
    for (GLB_ size_t i = 0; i < s;)
    {
#   pragma push_macro("R2Y_TRANSFORM_TO_")
#   undef  R2Y_TRANSFORM_TO_
#   define R2Y_TRANSFORM_TO_(OP, ...) do                           \
        {                                                          \
            R2Y_ rgb_t const & rgb = in_data[i]; ++i;              \
            (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y; \
            u_k   OP R2Y_ pixel_convert<R2Y_ plane_U>(rgb);        \
            v_k   OP R2Y_ pixel_convert<R2Y_ plane_V>(rgb);        \
            __VA_ARGS__                                            \
        } while(0)

        R2Y_TRANSFORM_TO_( =);
        R2Y_TRANSFORM_TO_(+=, R2Y_DETAIL_ set_and_next<S>(u_k, v_k, p_uv););

#   pragma pop_macro("R2Y_TRANSFORM_TO_")
    }
}

/* YUV 4:2:0 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YU12) ||
                        (S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_ byte_t * p_y = R2Y_DETAIL_ split<S, R2Y_ plane_Y>(ot_data.data(), in_w, in_h);
    R2Y_DETAIL_ uv_t<S> p_uv = R2Y_DETAIL_ fill<S>(ot_data.data(), in_w, in_h);
    R2Y_ scope_block<GLB_ uint16_t> u_tmp(in_w >> 1), v_tmp(in_w >> 1);
    bool is_h_even = false;
    for (GLB_ size_t i = 0, index = 0; i < in_h; ++i, is_h_even = !is_h_even)
    {
        bool is_w_even = false;
        if (is_h_even) for (GLB_ size_t j = 0; j < in_w; ++j, ++index, is_w_even = !is_w_even)
        {
            R2Y_ rgb_t const & rgb = in_data[index];
            (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y;
            GLB_ size_t k = j >> 1;
            if (is_w_even)
            {
                R2Y_DETAIL_ set_and_next<S>(
                    u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb),
                    v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb), p_uv);
            }
            else
            {
                u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
            }
        }
        else for (GLB_ size_t j = 0; j < in_w; ++j, ++index, is_w_even = !is_w_even)
        {
            R2Y_ rgb_t const & rgb = in_data[index];
            (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y;
            GLB_ size_t k = j >> 1;
            if (is_w_even)
            {
                u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
            }
            else
            {
                u_tmp[k] = R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                v_tmp[k] = R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
            }
        }
    }
}

/* YUV 4:1:1 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_411P)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_ byte_t * p_y = R2Y_DETAIL_ split<S, R2Y_ plane_Y>(ot_data.data(), in_w, in_h);
    R2Y_DETAIL_ uv_t<S> p_uv = R2Y_DETAIL_ fill<S>(ot_data.data(), in_w, in_h);
    GLB_ size_t s = in_w * in_h;
    GLB_ uint16_t u_k, v_k;
    for (GLB_ size_t i = 0; i < s;)
    {
#   pragma push_macro("R2Y_TRANSFORM_TO_")
#   undef  R2Y_TRANSFORM_TO_
#   define R2Y_TRANSFORM_TO_(OP, ...) do                           \
        {                                                          \
            R2Y_ rgb_t const & rgb = in_data[i]; ++i;              \
            (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y; \
            u_k   OP R2Y_ pixel_convert<R2Y_ plane_U>(rgb);        \
            v_k   OP R2Y_ pixel_convert<R2Y_ plane_V>(rgb);        \
            __VA_ARGS__                                            \
        } while(0)

        R2Y_TRANSFORM_TO_( =);
        R2Y_TRANSFORM_TO_(+=);
        R2Y_TRANSFORM_TO_(+=);
        R2Y_TRANSFORM_TO_(+=, R2Y_DETAIL_ set_and_next<S>(u_k, v_k, p_uv););

#   pragma pop_macro("R2Y_TRANSFORM_TO_")
    }
}

/* YUV 4:1:0 */

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YUV9 || S == R2Y_ yuv_YVU9)
>::type transform_to(R2Y_ scope_block<R2Y_ rgb_t> const & in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                     R2Y_ scope_block<R2Y_ byte_t>      & ot_data)
{
    R2Y_ byte_t * p_y = R2Y_DETAIL_ split<S, R2Y_ plane_Y>(ot_data.data(), in_w, in_h);
    R2Y_DETAIL_ uv_t<S> p_uv = R2Y_DETAIL_ fill<S>(ot_data.data(), in_w, in_h);
    R2Y_ scope_block<GLB_ uint16_t> u_tmp(in_w >> 2), v_tmp(in_w >> 2);
    GLB_ uint8_t h_flag = 1;
    for (GLB_ size_t i = 0, index = 0; i < in_h; ++i, ++h_flag)
    {
        GLB_ uint8_t w_flag = 1;
        switch (h_flag)
        {
        case 1:
            for (GLB_ size_t j = 0; j < in_w; ++j, ++index, ++w_flag)
            {
                R2Y_ rgb_t const & rgb = in_data[index];
                (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y;
                GLB_ size_t k = j >> 2;
                switch (w_flag)
                {
                case 1:
                    u_tmp[k] = R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                    v_tmp[k] = R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
                    break;
                case 4:
                    w_flag = 0;
                default:
                    u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                    v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (GLB_ size_t j = 0; j < in_w; ++j, ++index)
            {
                R2Y_ rgb_t const & rgb = in_data[index];
                (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y;
                GLB_ size_t k = j >> 2;
                u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
            }
            break;
        case 4:
            for (GLB_ size_t j = 0; j < in_w; ++j, ++index, ++w_flag)
            {
                R2Y_ rgb_t const & rgb = in_data[index];
                (*p_y) = R2Y_ pixel_convert<R2Y_ plane_Y>(rgb); ++p_y;
                GLB_ size_t k = j >> 2;
                switch (w_flag)
                {
                case 4:
                    R2Y_DETAIL_ set_and_next<S>(
                        u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb),
                        v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb), p_uv);
                    w_flag = 0;
                    break;
                default:
                    u_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_U>(rgb);
                    v_tmp[k] += R2Y_ pixel_convert<R2Y_ plane_V>(rgb);
                    break;
                }
            }
            h_flag = 0;
            break;
        }
    }
}

/* Implementing */

enum
{
    rgb = 2, // 10
    yuv = 1  // 01
};

template <R2Y_ supported In, R2Y_ supported Ot, int = (R2Y_ is_rgb<In>::value << 1) | R2Y_ is_yuv<In>::value
                                              , int = (R2Y_ is_rgb<Ot>::value << 1) | R2Y_ is_yuv<Ot>::value>
struct impl_;

template <R2Y_ supported In, R2Y_ supported Ot>
struct impl_<In, Ot, R2Y_DETAIL_ rgb, R2Y_DETAIL_ yuv>
{
    static void transform(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, 
                          R2Y_ scope_block<R2Y_ byte_t> & ot_data)
    {
        assert(ot_data.size() == R2Y_ calculate_size<Ot>(in_w, in_h));
        R2Y_ scope_block<R2Y_ rgb_t> rgb_data( R2Y_DETAIL_ rgb_formatter<In>::to_888(in_data, in_w, in_h) );
        R2Y_DETAIL_ transform_to<Ot>(rgb_data, in_w, in_h, ot_data);
    }
};

#pragma pop_macro("R2Y_DETAIL_")

} // namespace detail_transform_old_

template <R2Y_ supported In, R2Y_ supported Ot>
void transform_old(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                   R2Y_ scope_block<R2Y_ byte_t> * ot_data)
{
    assert(in_data != NULL);
    assert(ot_data != NULL);
    assert(in_w > 0 && in_h > 0);
    R2Y_ detail_transform_old_::impl_<In, Ot>::transform(in_data, in_w, in_h, *ot_data);
}