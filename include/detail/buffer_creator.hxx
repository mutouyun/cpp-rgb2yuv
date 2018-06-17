/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://orzz.org)
*/

////////////////////////////////////////////////////////////////
/// Create a buffer for transforming the image pixels
////////////////////////////////////////////////////////////////

/* Calculate RGB size */

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ rgb_888),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    return (in_w * in_h) * sizeof(R2Y_ rgb_t);
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ rgb_565 || S == R2Y_ rgb_555),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    return (in_w * in_h) * sizeof(GLB_ uint16_t);
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ rgb_444),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    return ( (in_w * in_h * 3 + 1) >> 1 );
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ rgb_888X),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    return (in_w * in_h) * sizeof(GLB_ uint32_t);
}

/* Calculate YUV size */

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ yuv_NV24 || S == R2Y_ yuv_NV42),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    return (in_w * in_h) * sizeof(R2Y_ yuv_t);
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ yuv_YVYU || S == R2Y_ yuv_UYVY || S == R2Y_ yuv_VYUY || 
                         S == R2Y_ yuv_YUY2 || S == R2Y_ yuv_422P),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    assert((s & 1) == 0); // s % 2 == 0
    return (s << 1);
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ yuv_Y41P),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    assert((s & 7) == 0); // s % 8 == 0
    return ( s + (s >> 1) );
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ yuv_Y411 || S == R2Y_ yuv_411P ||
                         S == R2Y_ yuv_YV12 || S == R2Y_ yuv_YU12 ||
                         S == R2Y_ yuv_NV12 || S == R2Y_ yuv_NV21),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    assert((s & 3) == 0); // s % 4 == 0
    return ( s + (s >> 1) );
}

template <R2Y_ supported S>
typename STD_ enable_if<(S == R2Y_ yuv_YVU9 || S == R2Y_ yuv_YUV9),
GLB_ size_t>::type calculate_size(GLB_ size_t in_w, GLB_ size_t in_h)
{
    GLB_ size_t s = in_w * in_h;
    assert((s & 15) == 0); // s % 16 == 0
    return ( s + (s >> 3) );
}

/* Create a buffer with given w & h */

template <R2Y_ supported S>
R2Y_ scope_block<R2Y_ byte_t> create_buffer(GLB_ size_t in_w, GLB_ size_t in_h)
{
    return R2Y_ scope_block<R2Y_ byte_t>{ calculate_size<S>(in_w, in_h) };
}