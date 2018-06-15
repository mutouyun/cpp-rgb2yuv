/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://orzz.org)
*/

////////////////////////////////////////////////////////////////
/// The pixel iterator for an image
////////////////////////////////////////////////////////////////

namespace detail_iterator_ {

#pragma push_macro("R2Y_DETAIL_")
#undef  R2Y_DETAIL_
#define R2Y_DETAIL_ R2Y_ detail_iterator_::

#pragma push_macro("R2Y_HELPER_")
#undef  R2Y_HELPER_
#define R2Y_HELPER_ R2Y_ detail_helper_::

#pragma push_macro("R2Y_DETAIL_INHERIT_")
#undef  R2Y_DETAIL_INHERIT_
#define R2Y_DETAIL_INHERIT_(SP, P)                    \
    template <R2Y_ supported S>                       \
    class impl_<R2Y_ SP, S> : public impl_<R2Y_ P, S> \
    {                                                 \
    public:                                           \
        using impl_<R2Y_ P, S>::impl_;                \
    };

template <R2Y_ supported T, R2Y_ supported S = T>
class impl_;

/* RGB 888 */

template <R2Y_ supported S> class impl_<R2Y_ rgb_888, S>
{
    R2Y_ rgb_t * rgb_;

public:
    enum { iterator_size = 1, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
        : rgb_(reinterpret_cast<R2Y_ rgb_t *>(in_data))
    {}

    void set_and_next(R2Y_ rgb_t const & rhs)
    {
        (*rgb_) = rhs;
        ++rgb_;
    }

    template <GLB_ size_t N>
    void set_and_next(R2Y_ rgb_t const (&rhs)[N])
    {
        memcpy(rgb_, rhs, sizeof(rhs));
        rgb_ += N;
    }
};

/* RGB 888X */

template <R2Y_ supported S> class impl_<R2Y_ rgb_888X, S>
{
    GLB_ uint32_t * rgb_;

public:
    enum { iterator_size = 1, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
        : rgb_(reinterpret_cast<GLB_ uint32_t *>(in_data))
    {}

    void set_and_next(R2Y_ rgb_t const & rhs)
    {
        (*reinterpret_cast<R2Y_ rgb_t *>(rgb_)) = rhs;
        ++rgb_;
    }

    template <GLB_ size_t N>
    void set_and_next(R2Y_ rgb_t const (&rhs)[N])
    {
        for (GLB_ size_t i = 0; i < N; ++i)
        {
            set_and_next(rhs[i]);
        }
    }
};

/* YUV Packed */

/* 4:2:2 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YUY2, S>
{
    typedef R2Y_HELPER_ packed_yuv_t<S> p_t;

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

R2Y_DETAIL_INHERIT_(yuv_YVYU, yuv_YUY2)
R2Y_DETAIL_INHERIT_(yuv_UYVY, yuv_YUY2)
R2Y_DETAIL_INHERIT_(yuv_VYUY, yuv_YUY2)

/* 4:1:1 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_Y41P, S>
{
    typedef R2Y_HELPER_ packed_yuv_t<S> p_t;

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

/* 4:4:4 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_NV24, S> : R2Y_HELPER_ yuv_planar<S>
{
    typedef R2Y_HELPER_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;

public:
    enum { iterator_size = 1, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : R2Y_HELPER_ yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
    {}

    void set_and_next(R2Y_ yuv_t const & rhs)
    {
        (*y_) = rhs.y_; ++y_;
        R2Y_HELPER_  set_planar_uv(rhs.u_, rhs.v_, uv_);
        R2Y_HELPER_ next_planar_uv(uv_);
    }
};

R2Y_DETAIL_INHERIT_(yuv_NV42, yuv_NV24)

/* 4:2:2 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_422P, S> : R2Y_HELPER_ yuv_planar<S>
{
    typedef R2Y_HELPER_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;

public:
    enum { iterator_size = 2, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : R2Y_HELPER_ yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
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
            R2Y_HELPER_ set_planar_uv((u_k + pix.u_) >> 1,
                                      (v_k + pix.v_) >> 1, uv_);
            R2Y_HELPER_ next_planar_uv(uv_);
        }
    }
};

/* 4:2:0 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YV12, S> : R2Y_HELPER_ yuv_planar<S>
{
    typedef R2Y_HELPER_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_, * y1_, * ye_;
    uv_t          uv_;
    GLB_ size_t   w_;

public:
    enum { iterator_size = 2, is_block = 1 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : R2Y_HELPER_ yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
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
        R2Y_HELPER_ set_planar_uv((rhs[0].u_ + rhs[1].u_ + rhs[2].u_ + rhs[3].u_) >> 2,
                                  (rhs[0].v_ + rhs[1].v_ + rhs[2].v_ + rhs[3].v_) >> 2, uv_);
        R2Y_HELPER_ next_planar_uv(uv_);
    }
};

R2Y_DETAIL_INHERIT_(yuv_YU12, yuv_YV12)
R2Y_DETAIL_INHERIT_(yuv_NV12, yuv_YV12)
R2Y_DETAIL_INHERIT_(yuv_NV21, yuv_YV12)

/* 4:1:1 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_411P, S> : R2Y_HELPER_ yuv_planar<S>
{
    typedef R2Y_HELPER_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;

public:
    enum { iterator_size = 4, is_block = 0 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : R2Y_HELPER_ yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
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
        R2Y_SET_AND_NEXT_(3, +=, R2Y_HELPER_  set_planar_uv<S>(u_k >> 2, v_k >> 2, uv_);
                                 R2Y_HELPER_ next_planar_uv<S>(uv_););

#   pragma pop_macro("R2Y_SET_AND_NEXT_")
    }
};

/* 4:1:0 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YUV9, S> : R2Y_HELPER_ yuv_planar<S>
{
    typedef R2Y_HELPER_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_, * y1_, * y2_, * y3_, * ye_;
    uv_t          uv_;
    GLB_ size_t   w_;

public:
    enum { iterator_size = 4, is_block = 1 };

    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : R2Y_HELPER_ yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
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
        R2Y_HELPER_ set_planar_uv((rhs[0 ].u_ + rhs[1 ].u_ + rhs[2 ].u_ + rhs[3 ].u_ +
                                   rhs[4 ].u_ + rhs[5 ].u_ + rhs[6 ].u_ + rhs[7 ].u_ +
                                   rhs[8 ].u_ + rhs[9 ].u_ + rhs[10].u_ + rhs[11].u_ +
                                   rhs[12].u_ + rhs[13].u_ + rhs[14].u_ + rhs[15].u_) >> 4,
                                  (rhs[0 ].v_ + rhs[1 ].v_ + rhs[2 ].v_ + rhs[3 ].v_ +
                                   rhs[4 ].v_ + rhs[5 ].v_ + rhs[6 ].v_ + rhs[7 ].v_ +
                                   rhs[8 ].v_ + rhs[9 ].v_ + rhs[10].v_ + rhs[11].v_ +
                                   rhs[12].v_ + rhs[13].v_ + rhs[14].v_ + rhs[15].v_) >> 4, uv_);
        R2Y_HELPER_ next_planar_uv(uv_);
    }
};

R2Y_DETAIL_INHERIT_(yuv_YVU9, yuv_YUV9)

#pragma pop_macro("R2Y_DETAIL_INHERIT_")
#pragma pop_macro("R2Y_HELPER_")
#pragma pop_macro("R2Y_DETAIL_")

} // namespace detail_iterator_

template <R2Y_ supported S>
class iterator : public R2Y_ detail_iterator_::impl_<S>
{
    typedef R2Y_ detail_iterator_::impl_<S> base_t;

public:
    using base_t::base_t;
};
