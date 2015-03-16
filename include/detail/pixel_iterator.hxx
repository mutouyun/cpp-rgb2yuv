/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

////////////////////////////////////////////////////////////////
/// The pixel iterator for an image
////////////////////////////////////////////////////////////////

namespace detail_iterator_ {

#pragma push_macro("R2Y_DETAIL_")
#undef  R2Y_DETAIL_
#define R2Y_DETAIL_ R2Y_ detail_iterator_::

template <R2Y_ supported T, R2Y_ supported S = T>
class impl_;

/* YUV Packed */

/* 4:2:2 */

template <R2Y_ supported> struct packed_yuv_t;
template <>               struct packed_yuv_t<R2Y_ yuv_YUY2> { GLB_ uint8_t y0_, cb_, y1_, cr_; };
template <>               struct packed_yuv_t<R2Y_ yuv_YVYU> { GLB_ uint8_t y0_, cr_, y1_, cb_; };
template <>               struct packed_yuv_t<R2Y_ yuv_UYVY> { GLB_ uint8_t cb_, y0_, cr_, y1_; };
template <>               struct packed_yuv_t<R2Y_ yuv_VYUY> { GLB_ uint8_t cr_, y0_, cb_, y1_; };

template <R2Y_ supported S> class impl_<R2Y_ yuv_YUY2, S>
{
    typedef R2Y_DETAIL_ packed_yuv_t<S> p_t;

    p_t * yuv_;

public:
    enum { iterator_size = 2, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
        : yuv_(reinterpret_cast<p_t *>(in_data))
    {}

    void set_and_next(R2Y_ yuv_t const (& rhs)[2])
    {
        GLB_ uint16_t u_k, v_k;
        {
            R2Y_ yuv_t const & pix = rhs[0];
            yuv_->y0_ = pix.y_;
            u_k       = pix.u_;
            v_k       = pix.v_;
        }
        {
            R2Y_ yuv_t const & pix = rhs[1];
            yuv_->y1_ = pix.y_;
            yuv_->cb_ = (u_k + pix.u_) >> 1;
            yuv_->cr_ = (v_k + pix.v_) >> 1;
        }
        ++yuv_;
    }
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_YVYU, S> : public impl_<R2Y_ yuv_YUY2, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YUY2, S>(in_data, in_w, in_h)
    {}
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_UYVY, S> : public impl_<R2Y_ yuv_YUY2, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YUY2, S>(in_data, in_w, in_h)
    {}
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_VYUY, S> : public impl_<R2Y_ yuv_YUY2, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YUY2, S>(in_data, in_w, in_h)
    {}
};

/* 4:1:1 */

template <> struct packed_yuv_t<R2Y_ yuv_Y41P>
{
    GLB_ uint8_t u0_, y0_, v0_, y1_;
    GLB_ uint8_t u1_, y2_, v1_, y3_;
    GLB_ uint8_t y4_, y5_, y6_, y7_;
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_Y41P, S>
{
    typedef R2Y_DETAIL_ packed_yuv_t<S> p_t;

    p_t * yuv_;

public:
    enum { iterator_size = 8, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
        : yuv_(reinterpret_cast<p_t *>(in_data))
    {}

    void set_and_next(R2Y_ yuv_t const (& rhs)[8])
    {
        GLB_ uint16_t u_k, v_k;

#   pragma push_macro("R2Y_SET_AND_NEXT_")
#   undef  R2Y_SET_AND_NEXT_
#   define R2Y_SET_AND_NEXT_(I, OP, ...) do  \
        {                                    \
            R2Y_ yuv_t const & pix = rhs[I]; \
            yuv_->y##I##_ = pix.y_;          \
            u_k          OP pix.u_;          \
            v_k          OP pix.v_;          \
            __VA_ARGS__                      \
        } while(0)

        R2Y_SET_AND_NEXT_(0, = );
        R2Y_SET_AND_NEXT_(1, +=);
        R2Y_SET_AND_NEXT_(2, +=);
        R2Y_SET_AND_NEXT_(3, +=, yuv_->u0_ = u_k >> 2; yuv_->v0_ = v_k >> 2;);
        R2Y_SET_AND_NEXT_(4, = );
        R2Y_SET_AND_NEXT_(5, +=);
        R2Y_SET_AND_NEXT_(6, +=);
        R2Y_SET_AND_NEXT_(7, +=, yuv_->u1_ = u_k >> 2; yuv_->v1_ = v_k >> 2; ++yuv_;);

#   pragma pop_macro("R2Y_SET_AND_NEXT_")
    }
};

/* YUV Planar */

template <R2Y_ supported> struct planar_uv_t                { GLB_ uint8_t * cb_, *cr_; };
template <>               struct planar_uv_t<R2Y_ yuv_NV24> { struct inner { GLB_ uint8_t cb_, cr_; } * uv_; };
template <>               struct planar_uv_t<R2Y_ yuv_NV42> { struct inner { GLB_ uint8_t cr_, cb_; } * uv_; };
template <>               struct planar_uv_t<R2Y_ yuv_NV12> { struct inner { GLB_ uint8_t cb_, cr_; } * uv_; };
template <>               struct planar_uv_t<R2Y_ yuv_NV21> { struct inner { GLB_ uint8_t cr_, cb_; } * uv_; };

template <R2Y_ supported S>
typename R2Y_ enable_if<!(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42 || S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21)
>::type set_and_next(GLB_ uint8_t in_u, GLB_ uint8_t in_v, R2Y_DETAIL_ planar_uv_t<S> & ot_uv)
{
    (*(ot_uv.cb_)) = in_u; ++(ot_uv.cb_);
    (*(ot_uv.cr_)) = in_v; ++(ot_uv.cr_);
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42 || S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21)
>::type set_and_next(GLB_ uint8_t in_u, GLB_ uint8_t in_v, R2Y_DETAIL_ planar_uv_t<S> & ot_uv)
{
    ot_uv.uv_->cb_ = in_u;
    ot_uv.uv_->cr_ = in_v; ++(ot_uv.uv_);
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<(P == R2Y_ plane_Y),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t /*in_size*/)
{
    return in_data;
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_U) && (S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P || 
                                                  S == R2Y_ yuv_422P || S == R2Y_ yuv_YUV9) ) ||
                        ( (P == R2Y_ plane_V) && (S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YVU9) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
{
    return in_data + in_size;
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_V) && (S == R2Y_ yuv_422P) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
{
    return in_data + in_size + (in_size >> 1);
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_V) && (S == R2Y_ yuv_YU12 || S == R2Y_ yuv_411P) ) ||
                        ( (P == R2Y_ plane_U) && (S == R2Y_ yuv_YV12) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
{
    return in_data + in_size + (in_size >> 2);
}

template <R2Y_ supported S, R2Y_ plane_type P>
typename R2Y_ enable_if<( (P == R2Y_ plane_V) && (S == R2Y_ yuv_YUV9) ) ||
                        ( (P == R2Y_ plane_U) && (S == R2Y_ yuv_YVU9) ),
R2Y_ byte_t *>::type split(R2Y_ byte_t * in_data, GLB_ size_t in_size)
{
    return in_data + in_size + (in_size >> 4);
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YU12 || 
                         S == R2Y_ yuv_411P || S == R2Y_ yuv_422P ||
                         S == R2Y_ yuv_YUV9 || S == R2Y_ yuv_YVU9),
R2Y_DETAIL_ planar_uv_t<S> >::type fill(R2Y_ byte_t * in_data, GLB_ size_t in_size)
{
    R2Y_DETAIL_ planar_uv_t<S> ret =
    {
        R2Y_DETAIL_ split<S, R2Y_ plane_U>(in_data, in_size),
        R2Y_DETAIL_ split<S, R2Y_ plane_V>(in_data, in_size)
    };
    return ret;
}

template <R2Y_ supported S>
typename R2Y_ enable_if<(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42 || 
                         S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21),
R2Y_DETAIL_ planar_uv_t<S> >::type fill(R2Y_ byte_t * in_data, GLB_ size_t in_size)
{
    R2Y_DETAIL_ planar_uv_t<S> ret =
    {
        reinterpret_cast<typename R2Y_DETAIL_ planar_uv_t<S>::inner *>(
                                  R2Y_DETAIL_ split<R2Y_ yuv_YU12, R2Y_ plane_U>(in_data, in_size))
    };
    return ret;
}

template <R2Y_ supported S>
struct yuv_planar
{
    template <typename Y, typename UV>
    yuv_planar(Y & y, UV & uv, R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
    {
        GLB_ size_t size = in_w * in_h;
        y  = R2Y_DETAIL_ split<S, R2Y_ plane_Y>(in_data, size);
        uv = R2Y_DETAIL_ fill<S>(in_data, size);
    }
};

/* 4:4:4 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_NV24, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;

public:
    enum { iterator_size = 1, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
    {}

    void set_and_next(R2Y_ yuv_t const & rhs)
    {
        (*y_) = rhs.y_; ++y_;
        R2Y_DETAIL_ set_and_next(rhs.u_, rhs.v_, uv_);
    }
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_NV42, S> : public impl_<R2Y_ yuv_NV24, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_NV24, S>(in_data, in_w, in_h)
    {}
};

/* 4:2:2 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_422P, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;

public:
    enum { iterator_size = 2, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
    {}

    void set_and_next(R2Y_ yuv_t const (& rhs)[2])
    {
        GLB_ uint16_t u_k, v_k;
        {
            R2Y_ yuv_t const & pix = rhs[0];
            (*y_) = pix.y_; ++y_;
            u_k   = pix.u_;
            v_k   = pix.v_;
        }
        {
            R2Y_ yuv_t const & pix = rhs[1];
            (*y_) = pix.y_; ++y_;
            R2Y_DETAIL_ set_and_next((u_k + pix.u_) >> 1,
                                     (v_k + pix.v_) >> 1, uv_);
        }
    }
};

/* 4:2:0 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YV12, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_, * y1_, * ye_;
    uv_t          uv_;
    GLB_ size_t   w_;

public:
    enum { iterator_size = 2, is_block = 1 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
        , y1_(y_ + in_w), ye_(y1_)
        , w_(in_w)
    {}

    void set_and_next(R2Y_ yuv_t const (& rhs)[4])
    {
        (*y_)  = rhs[0].y_; ++y_;
        (*y_)  = rhs[1].y_; ++y_;
        (*y1_) = rhs[2].y_; ++y1_;
        (*y1_) = rhs[3].y_; ++y1_;
        if (y_ == ye_)
        {
            y_ = y1_;
            y1_ += w_;
            ye_ = y1_;
        }
        R2Y_DETAIL_ set_and_next((rhs[0].u_ + rhs[1].u_ + rhs[2].u_ + rhs[3].u_) >> 2,
                                 (rhs[0].v_ + rhs[1].v_ + rhs[2].v_ + rhs[3].v_) >> 2, uv_);
    }
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_YU12, S> : public impl_<R2Y_ yuv_YV12, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YV12, S>(in_data, in_w, in_h)
    {}
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_NV12, S> : public impl_<R2Y_ yuv_YV12, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YV12, S>(in_data, in_w, in_h)
    {}
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_NV21, S> : public impl_<R2Y_ yuv_YV12, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YV12, S>(in_data, in_w, in_h)
    {}
};

/* 4:1:1 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_411P, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;

public:
    enum { iterator_size = 4, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
    {}

    void set_and_next(R2Y_ yuv_t const (& rhs)[4])
    {
        GLB_ uint16_t u_k, v_k;

#   pragma push_macro("R2Y_SET_AND_NEXT_")
#   undef  R2Y_SET_AND_NEXT_
#   define R2Y_SET_AND_NEXT_(I, OP, ...) do  \
        {                                    \
            R2Y_ yuv_t const & pix = rhs[I]; \
            (*y_) = pix.y_; ++y_;            \
            u_k  OP pix.u_;                  \
            v_k  OP pix.v_;                  \
            __VA_ARGS__                      \
        } while(0)

        R2Y_SET_AND_NEXT_(0, = );
        R2Y_SET_AND_NEXT_(1, +=);
        R2Y_SET_AND_NEXT_(2, +=);
        R2Y_SET_AND_NEXT_(3, +=, R2Y_DETAIL_ set_and_next<S>(u_k >> 2, v_k >> 2, uv_););

#   pragma pop_macro("R2Y_SET_AND_NEXT_")
    }
};

/* 4:1:0 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YUV9, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_, * y1_, * y2_, * y3_, * ye_;
    uv_t          uv_;
    GLB_ size_t   w_;

public:
    enum { iterator_size = 4, is_block = 1 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
        , y1_(y_ + in_w), y2_(y1_ + in_w), y3_(y2_ + in_w), ye_(y1_)
        , w_(in_w)
    {}

    void set_and_next(R2Y_ yuv_t const (& rhs)[16])
    {
        int i = 0;
        for (; i < 4 ; ++i, ++y_ ) (*y_)  = rhs[i].y_;
        for (; i < 8 ; ++i, ++y1_) (*y1_) = rhs[i].y_;
        for (; i < 12; ++i, ++y2_) (*y2_) = rhs[i].y_;
        for (; i < 16; ++i, ++y3_) (*y3_) = rhs[i].y_;
        if (y_ == ye_)
        {
            y_  = y3_;
            y1_ = y_  + w_;
            y2_ = y1_ + w_;
            y3_ = y2_ + w_;
            ye_ = y1_;
        }
        R2Y_DETAIL_ set_and_next((rhs[0 ].u_ + rhs[1 ].u_ + rhs[2 ].u_ + rhs[3 ].u_ +
                                  rhs[4 ].u_ + rhs[5 ].u_ + rhs[6 ].u_ + rhs[7 ].u_ +
                                  rhs[8 ].u_ + rhs[9 ].u_ + rhs[10].u_ + rhs[11].u_ +
                                  rhs[12].u_ + rhs[13].u_ + rhs[14].u_ + rhs[15].u_) >> 4,
                                 (rhs[0 ].v_ + rhs[1 ].v_ + rhs[2 ].v_ + rhs[3 ].v_ +
                                  rhs[4 ].v_ + rhs[5 ].v_ + rhs[6 ].v_ + rhs[7 ].v_ +
                                  rhs[8 ].v_ + rhs[9 ].v_ + rhs[10].v_ + rhs[11].v_ +
                                  rhs[12].v_ + rhs[13].v_ + rhs[14].v_ + rhs[15].v_) >> 4, uv_);
    }
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_YVU9, S> : public impl_<R2Y_ yuv_YUV9, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YUV9, S>(in_data, in_w, in_h)
    {}
};

#pragma pop_macro("R2Y_DETAIL_")

} // namespace detail_iterator_

template <R2Y_ supported S>
class iterator : public R2Y_ detail_iterator_::impl_<S>
{
    typedef R2Y_ detail_iterator_::impl_<S> base_t;

public:
    iterator(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : base_t(in_data, in_w, in_h)
    {}
};
