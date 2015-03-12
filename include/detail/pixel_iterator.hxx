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

#pragma push_macro("R2Y_SET_AND_NEXT_")
#undef  R2Y_SET_AND_NEXT_
#define R2Y_SET_AND_NEXT_(I, YI, OP, ...) \
    case I:                               \
    {                                     \
        YI   = rhs.y_;                    \
        u_k_ OP rhs.u_;                   \
        v_k_ OP rhs.v_;                   \
        __VA_ARGS__                       \
    }                                     \
    break

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

    p_t *         yuv_;
    bool          flag_;
    GLB_ uint16_t u_k_, v_k_; // No need initialization

public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
        : yuv_(reinterpret_cast<p_t *>(in_data)), flag_(false)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
    {
        if (flag_)
        {
            yuv_->y1_ = rhs.y_;
            yuv_->cb_ = (u_k_ + rhs.u_) >> 1;
            yuv_->cr_ = (v_k_ + rhs.v_) >> 1;
            ++yuv_;
        }
        else
        {
            yuv_->y0_ = rhs.y_;
            u_k_      = rhs.u_;
            v_k_      = rhs.v_;
        }
        flag_ = !flag_;
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

    p_t *         yuv_;
    GLB_ uint8_t  flag_;
    GLB_ uint16_t u_k_, v_k_; // No need initialization

public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t /*in_w*/, GLB_ size_t /*in_h*/)
        : yuv_(reinterpret_cast<p_t *>(in_data)), flag_(0)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
    {
        switch (flag_)
        {
        R2Y_SET_AND_NEXT_(0, yuv_->y0_, = , ++flag_;);
        R2Y_SET_AND_NEXT_(1, yuv_->y1_, +=, ++flag_;);
        R2Y_SET_AND_NEXT_(2, yuv_->y2_, +=, ++flag_;);
        R2Y_SET_AND_NEXT_(3, yuv_->y3_, +=, yuv_->u0_ = u_k_ >> 2; yuv_->v0_ = v_k_ >> 2; ++flag_;);
        R2Y_SET_AND_NEXT_(4, yuv_->y4_, = , ++flag_;);
        R2Y_SET_AND_NEXT_(5, yuv_->y5_, +=, ++flag_;);
        R2Y_SET_AND_NEXT_(6, yuv_->y6_, +=, ++flag_;);
        R2Y_SET_AND_NEXT_(7, yuv_->y7_, +=, yuv_->u1_ = u_k_ >> 2; yuv_->v1_ = v_k_ >> 2; flag_ = 0; ++yuv_;);
        }
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
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
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
    bool          flag_;
    GLB_ uint16_t u_k_, v_k_; // No need initialization

public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
        , flag_(false)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
    {
        (*y_) = rhs.y_; ++y_;
        if (flag_)
        {
            R2Y_DETAIL_ set_and_next((u_k_ + rhs.u_) >> 1, 
                                     (v_k_ + rhs.v_) >> 1, uv_);
        }
        else
        {
            u_k_ = rhs.u_;
            v_k_ = rhs.v_;
        }
        flag_ = !flag_;
    }
};

/* 4:2:0 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YV12, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;
    GLB_ size_t   k_;
    bool          w_flag_, h_flag_;
    R2Y_ scope_block<GLB_ uint16_t> u_tmp_, v_tmp_;

public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
        , k_(0), w_flag_(false), h_flag_(false)
        , u_tmp_(in_w >> 1), v_tmp_(in_w >> 1)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
    {
        (*y_) = rhs.y_; ++y_;
        GLB_ size_t i = k_ >> 1;
        if (h_flag_ && w_flag_)
        {
            R2Y_DETAIL_ set_and_next((u_tmp_[i] + rhs.u_) >> 2, 
                                     (v_tmp_[i] + rhs.v_) >> 2, uv_);
        }
        else if (h_flag_ || w_flag_)
        {
            u_tmp_[i] += rhs.u_;
            v_tmp_[i] += rhs.v_;
        }
        else
        {
            u_tmp_[i] = rhs.u_;
            v_tmp_[i] = rhs.v_;
        }
        ++k_;
        w_flag_ = !w_flag_;
        if (k_ >= u_tmp_.size())
        {
            k_ = 0;
            h_flag_ = !h_flag_;
        }
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
    GLB_ uint8_t  flag_;
    GLB_ uint16_t u_k_, v_k_; // No need initialization

public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
        , flag_(0)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
    {
        switch (flag_)
        {
        R2Y_SET_AND_NEXT_(0, (*y_),  =, ++y_; ++flag_;);
        R2Y_SET_AND_NEXT_(1, (*y_), +=, ++y_; ++flag_;);
        R2Y_SET_AND_NEXT_(2, (*y_), +=, ++y_; ++flag_;);
        R2Y_SET_AND_NEXT_(3, (*y_), +=, ++y_; R2Y_DETAIL_ set_and_next<S>(u_k_ >> 2, v_k_ >> 2, uv_); flag_ = 0;);
        }
    }
};

/* 4:1:0 */

template <R2Y_ supported S> class impl_<R2Y_ yuv_YUV9, S> : yuv_planar<S>
{
    typedef R2Y_DETAIL_ planar_uv_t<S> uv_t;

    R2Y_ byte_t * y_;
    uv_t          uv_;
    GLB_ size_t   k_;
    GLB_ uint8_t  w_flag_, h_flag_;
    R2Y_ scope_block<GLB_ uint16_t> u_tmp_, v_tmp_;

public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : yuv_planar<S>(y_, uv_, in_data, in_w, in_h)
        , k_(0), w_flag_(0), h_flag_(0)
        , u_tmp_(in_w >> 2), v_tmp_(in_w >> 2)
    {}

    void set_and_next(const R2Y_ yuv_t & rhs)
    {
        (*y_) = rhs.y_; ++y_;
        GLB_ size_t i = k_ >> 2;
        if (h_flag_ == 0 && w_flag_ == 0)
        {
            u_tmp_[i] = rhs.u_;
            v_tmp_[i] = rhs.v_;
        }
        else if (h_flag_ < 3 || w_flag_ < 3) // 0, 1, 2
        {
            u_tmp_[i] += rhs.u_;
            v_tmp_[i] += rhs.v_;
        }
        else // h_flag_ == 3 && w_flag_ == 3
        {
            assert(h_flag_ == 3 && w_flag_ == 3);
            R2Y_DETAIL_ set_and_next((u_tmp_[i] + rhs.u_) >> 4,
                                     (v_tmp_[i] + rhs.v_) >> 4, uv_);
        }
        if (w_flag_ == 3)
            w_flag_ = 0;
        else
            ++w_flag_;
        ++k_;
        if (k_ >= (u_tmp_.size() << 1))
        {
            k_ = 0;
            if (h_flag_ == 3)
                h_flag_ = 0;
            else
                ++h_flag_;
        }
    }
};

template <R2Y_ supported S> class impl_<R2Y_ yuv_YVU9, S> : public impl_<R2Y_ yuv_YUV9, S>
{
public:
    impl_(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : impl_<R2Y_ yuv_YUV9, S>(in_data, in_w, in_h)
    {}
};

#pragma pop_macro("R2Y_SET_AND_NEXT_")
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
