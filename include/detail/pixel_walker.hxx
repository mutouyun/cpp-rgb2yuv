/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

////////////////////////////////////////////////////////////////
/// It's a pixel walker to walk each pixel and execute a closure with it.
////////////////////////////////////////////////////////////////

template <R2Y_ supported S, typename F>
typename R2Y_ enable_if<(S == R2Y_ rgb_888)
>::type pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, F & do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % sizeof(R2Y_ rgb_t)) == 0); // 3 input bytes convert to 1 pixel
    R2Y_ rgb_t * cur_pixel = reinterpret_cast<R2Y_ rgb_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += 3, ++cur_pixel)
    {
        do_sth(*cur_pixel);
    }
}

#pragma push_macro("R2Y_BMP_16_FOREACH_HELPER_")
#undef  R2Y_BMP_16_FOREACH_HELPER_
#define R2Y_BMP_16_FOREACH_HELPER_(MASK_R, MASK_G, MASK_B, SHIFT_R, SHIFT_G, SHIFT_B) do \
{                                                                                        \
    GLB_ size_t size = calculate_size<S>(in_w, in_h);                                    \
    assert((size & 1) == 0); /* in_size must be an even number */                        \
    R2Y_ rgb_t tmp;                                                                      \
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);              \
    for (GLB_ size_t i = 0; i < size; i += 2, ++cur_pixel)                               \
    {                                                                                    \
        tmp.r_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & (MASK_R)) SHIFT_R );         \
        tmp.g_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & (MASK_G)) SHIFT_G );         \
        tmp.b_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & (MASK_B)) SHIFT_B );         \
        do_sth(tmp);                                                                     \
    }                                                                                    \
} while(0)

template <R2Y_ supported S, typename F>
typename R2Y_ enable_if<(S == R2Y_ rgb_565)
>::type pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, F & do_sth)
{
    R2Y_BMP_16_FOREACH_HELPER_(0xF800, 0x07E0, 0x001F, >> 8, >> 3, << 3);
}

template <R2Y_ supported S, typename F>
typename R2Y_ enable_if<(S == R2Y_ rgb_555)
>::type pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, F & do_sth)
{
    R2Y_BMP_16_FOREACH_HELPER_(0x7C00, 0x03E0, 0x001F, >> 7, >> 2, << 3);
}

#pragma pop_macro("R2Y_BMP_16_FOREACH_HELPER_")

template <R2Y_ supported S, typename F>
typename R2Y_ enable_if<(S == R2Y_ rgb_444)
>::type pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, F & do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % 3) == 0); // 3 input bytes convert to 2 pixels (6 bytes)
    R2Y_ rgb_t tmp;
    for (GLB_ size_t i = 0; i < size; i += 3)
    {
        tmp.b_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
        tmp.g_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
        tmp.r_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
        do_sth(tmp);
        tmp.b_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
        tmp.g_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
        tmp.r_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
        do_sth(tmp);
    }
}

template <R2Y_ supported S, typename F>
typename R2Y_ enable_if<(S == R2Y_ rgb_888X)
>::type pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, F & do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % 4) == 0); // 4 input bytes convert to 1 pixel (3 bytes)
    GLB_ uint32_t * cur_pixel = reinterpret_cast<GLB_ uint32_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += 4, ++cur_pixel)
    {
        do_sth(*reinterpret_cast<R2Y_ rgb_t *>(cur_pixel));
    }
}