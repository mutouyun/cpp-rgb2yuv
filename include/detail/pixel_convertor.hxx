/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://orzz.org)
*/

////////////////////////////////////////////////////////////////
/// Converting between RGB & YUV/YCbCr planes
////////////////////////////////////////////////////////////////

struct pixel_t
{
    GLB_ uint8_t c_, b_, a_;

    template <typename P> static pixel_t const & cast(P const & p)
    {
        return reinterpret_cast<pixel_t const &>(p);
    }
};

struct convertor
{
    enum : GLB_ int32_t { MAX = static_cast<GLB_ uint8_t>(~0) };

    R2Y_FORCE_INLINE_ static GLB_ uint8_t clip(GLB_ int32_t value)
    {
        return static_cast<GLB_ uint8_t>( (value < 0) ? 0 :
                                          (value > convertor::MAX) ? convertor::MAX : value );
    }

    convertor(GLB_ int32_t const (& matrix)[4])
    {
        for (GLB_ int32_t i = 0; i <= convertor::MAX; ++i)
            for (GLB_ size_t n = 0; n < (sizeof(tb_) / sizeof(tb_[0])); ++n)
                tb_[n][i] = matrix[n] * i;
        for (GLB_ int32_t i = 0; i <= convertor::MAX; ++i)
            tb_[2][i] += matrix[3];
    }

    R2Y_FORCE_INLINE_ GLB_ int32_t convert(R2Y_ pixel_t const & in_p) const
    {
        return (tb_[0][in_p.a_] + tb_[1][in_p.b_] + tb_[2][in_p.c_]) >> 8;
    }

    template <bool NeedClip>
    R2Y_FORCE_INLINE_ auto convert(R2Y_ pixel_t const & in_p) const
        -> STD_ enable_if_t<NeedClip, GLB_ uint8_t>
    {
        return clip(this->convert(in_p));
    }

    template <bool NeedClip>
    R2Y_FORCE_INLINE_ auto convert(R2Y_ pixel_t const & in_p) const
        -> STD_ enable_if_t<!NeedClip, GLB_ uint8_t>
    {
        return this->convert(in_p);
    }

private:
    GLB_ int32_t tb_[3][convertor::MAX + 1];
};

R2Y_FORCE_INLINE_ convertor const * factor_matrix(void)
{
    /*
     * The factors for converting between YUV and RGB
     * See: https://msdn.microsoft.com/en-us/library/ms893078.aspx
     */
    static GLB_ int32_t const matrix[R2Y_ plane_MAX][4] =
    {
        {  66 ,  129,  25 ,  4224  },  // RGB -> Y
        { -38 , -74 ,  112,  32896 },  // RGB -> U/Cb
        {  112, -94 , -18 ,  32896 },  // RGB -> V/Cr

        {  298,  0  ,  409, -56992 },  // YUV -> R
        {  298, -100, -208,  34784 },  // YUV -> G
        {  298,  516,  0  , -70688 }   // YUV -> B
    };
    // Create and initialize the convertors
    static R2Y_ convertor const conv[R2Y_ plane_MAX] =
    {
        matrix[0], matrix[1], matrix[2], 
        matrix[3], matrix[4], matrix[5]
    };
    // Return the convertors
    return conv;
}

template <R2Y_ plane_type P>
R2Y_FORCE_INLINE_ GLB_ uint8_t pixel_convert(R2Y_ pixel_t const & in_p)
{
    return factor_matrix()[P].convert<R2Y_ is_rgb_plane<P>::value>(in_p);
}

template <R2Y_ plane_type P, typename T>
R2Y_FORCE_INLINE_ auto pixel_convert(T const & in_p)
    -> STD_ enable_if_t<(R2Y_ is_yuv_plane<P>::value && STD_ is_same<T, R2Y_ rgb_t>::value) ||
                        (R2Y_ is_rgb_plane<P>::value && STD_ is_same<T, R2Y_ yuv_t>::value), GLB_ uint8_t>
{
    return pixel_convert<P>(R2Y_ pixel_t::cast(in_p));
}

R2Y_FORCE_INLINE_ R2Y_ yuv_t pixel_convert(R2Y_ rgb_t const & in_p)
{
    return
    {
        pixel_convert<R2Y_ plane_V>(in_p),
        pixel_convert<R2Y_ plane_U>(in_p),
        pixel_convert<R2Y_ plane_Y>(in_p)
    };
}

R2Y_FORCE_INLINE_ R2Y_ rgb_t pixel_convert(R2Y_ yuv_t const & in_p)
{
    return
    {
        pixel_convert<R2Y_ plane_B>(in_p),
        pixel_convert<R2Y_ plane_G>(in_p),
        pixel_convert<R2Y_ plane_R>(in_p)
    };
}
