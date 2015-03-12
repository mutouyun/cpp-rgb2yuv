/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

////////////////////////////////////////////////////////////////
/// Converting between RGB & YUV/YCbCr planes
////////////////////////////////////////////////////////////////

struct pixel_t
{
    GLB_ uint8_t c_, b_, a_;

    template <typename P> static pixel_t       & cast(P       & p) { return reinterpret_cast<pixel_t       &>(p); }
    template <typename P> static pixel_t const & cast(P const & p) { return reinterpret_cast<pixel_t const &>(p); }
};

struct convertor
{
    static const GLB_ int32_t MAX = static_cast<GLB_ uint8_t>(~0);

    static GLB_ uint8_t clip(GLB_ int32_t value)
    {
        return static_cast<GLB_ uint8_t>( (value < 0) ? 0 :
                                          (value > convertor::MAX) ? convertor::MAX : value );
    }

    GLB_ int32_t tb_[3][convertor::MAX + 1];

    void init(GLB_ int32_t (& matrix)[4])
    {
        for (GLB_ int32_t i = 0; i <= convertor::MAX; ++i)
            for (GLB_ size_t n = 0; n < (sizeof(tb_) / sizeof(tb_[0])); ++n)
                tb_[n][i] = matrix[n] * i;
        for (GLB_ int32_t i = 0; i <= convertor::MAX; ++i)
            tb_[2][i] += matrix[3];
    }

    GLB_ int32_t convert(R2Y_ pixel_t const & in_p) const
    {
        return (tb_[0][in_p.a_] + tb_[1][in_p.b_] + tb_[2][in_p.c_]) >> 8;
    }

    template <bool NeedClip>
    typename R2Y_ enable_if<(NeedClip == true),
    GLB_ uint8_t>::type convert(R2Y_ pixel_t const & in_p) const
    {
        return clip(this->convert(in_p));
    }

    template <bool NeedClip>
    typename R2Y_ enable_if<(NeedClip == false),
    GLB_ uint8_t>::type convert(R2Y_ pixel_t const & in_p) const
    {
        return this->convert(in_p);
    }
};

inline R2Y_ convertor const * factor_matrix(void)
{
    /*
     * The factors for converting between YUV and RGB
     * See: https://msdn.microsoft.com/en-us/library/ms893078.aspx
     */
    static GLB_ int32_t matrix[R2Y_ plane_MAX][4] =
    {
        {  66 ,  129,  25 ,  4224  },  // RGB -> Y
        { -38 , -74 ,  112,  32896 },  // RGB -> U/Cb
        {  112, -94 , -18 ,  32896 },  // RGB -> V/Cr

        {  298,  0  ,  409, -56992 },  // YUV -> R
        {  298, -100, -208,  34784 },  // YUV -> G
        {  298,  516,  0  , -70688 }   // YUV -> B
    };
    // Create and initialize the convertors
    static R2Y_ convertor conv[R2Y_ plane_MAX];
    static struct run
    {
        run(void)
        {
            for (GLB_ size_t i = 0; i < R2Y_ plane_MAX; ++i)
                conv[i].init(matrix[i]);
        }
    // It makes initialization run only once when the funtion "factor_matrix" be called
    } R2Y_UNUSED_ init_once;
    // Return the convertors
    return conv;
}

template <R2Y_ plane_type P>
GLB_ uint8_t pixel_convert(R2Y_ pixel_t const & in_p)
{
    return factor_matrix()[P].convert<R2Y_ is_rgb_plane<P>::value>(in_p);
}

template <R2Y_ plane_type P>
typename R2Y_ enable_if<R2Y_ is_yuv_plane<P>::value,
GLB_ uint8_t>::type pixel_convert(R2Y_ rgb_t const & in_p)
{
    return pixel_convert<P>(R2Y_ pixel_t::cast(in_p));
}

template <R2Y_ plane_type P>
typename R2Y_ enable_if<R2Y_ is_rgb_plane<P>::value,
GLB_ uint8_t>::type pixel_convert(R2Y_ yuv_t const & in_p)
{
    return pixel_convert<P>(R2Y_ pixel_t::cast(in_p));
}

R2Y_ yuv_t pixel_convert(R2Y_ rgb_t const & in_p)
{
    R2Y_ yuv_t ret =
    {
        pixel_convert<R2Y_ plane_V>(in_p),
        pixel_convert<R2Y_ plane_U>(in_p),
        pixel_convert<R2Y_ plane_Y>(in_p)
    };
    return ret;
}

R2Y_ rgb_t pixel_convert(R2Y_ yuv_t const & in_p)
{
    R2Y_ rgb_t ret =
    {
        pixel_convert<R2Y_ plane_B>(in_p),
        pixel_convert<R2Y_ plane_G>(in_p),
        pixel_convert<R2Y_ plane_R>(in_p)
    };
    return ret;
}