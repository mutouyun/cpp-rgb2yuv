/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

////////////////////////////////////////////////////////////////
/// It's a pixel walker to walk each pixel and execute a closure with it.
////////////////////////////////////////////////////////////////

/* 888 */

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_888 && F::iterator_size == 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % sizeof(R2Y_ rgb_t)) == 0); // 3 input bytes convert to 1 pixel
    R2Y_ rgb_t * cur_pixel = reinterpret_cast<R2Y_ rgb_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += 3, ++cur_pixel)
    {
        STD_ forward<T>(do_sth)(*cur_pixel);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_888 && F::iterator_size > 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % sizeof(R2Y_ rgb_t)) == 0); // 3 input bytes convert to 1 pixel
    R2Y_ rgb_t * cur_pixel = reinterpret_cast<R2Y_ rgb_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += (3 * F::iterator_size), cur_pixel += F::iterator_size)
    {
        STD_ forward<T>(do_sth)(*reinterpret_cast<R2Y_ rgb_t (*)[F::iterator_size]>(& cur_pixel));
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_888 && F::iterator_size > 1 && F::is_block == 1)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    assert((in_w % F::iterator_size) == 0);
    assert((in_h % F::iterator_size) == 0);
    R2Y_ rgb_t tmp[F::iterator_size * F::iterator_size];
    GLB_ size_t row_offset = in_w - F::iterator_size;
    R2Y_ rgb_t * cur_pixel = reinterpret_cast<R2Y_ rgb_t *>(in_data);
    for (GLB_ size_t i = 0; i < in_h; i += F::iterator_size, cur_pixel += (in_w * (F::iterator_size - 1)))
    {
        for (GLB_ size_t j = 0; j < in_w; j += F::iterator_size, cur_pixel += F::iterator_size)
        {
            R2Y_ rgb_t * block_iter = cur_pixel;
            for (int n = 0, index = 0; n < F::iterator_size; ++n, block_iter += row_offset)
            {
                for (int m = 0; m < F::iterator_size; ++m, ++index, ++block_iter)
                {
                    tmp[index] = *block_iter;
                }
            }
            STD_ forward<T>(do_sth)(tmp);
        }
    }
}

/* 565 */

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_565 && F::iterator_size == 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size & 1) == 0); // in_size must be an even number
    R2Y_ rgb_t tmp;
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += 2, ++cur_pixel)
    {
        tmp.r_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0xF800) >> 8 );
        tmp.g_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x07E0) >> 3 );
        tmp.b_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x001F) << 3 );
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_565 && F::iterator_size > 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size & 1) == 0); // in_size must be an even number
    R2Y_ rgb_t tmp[F::iterator_size];
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += (2 * F::iterator_size))
    {
        for (int n = 0; n < F::iterator_size; ++n, ++cur_pixel)
        {
            R2Y_ rgb_t & ref = tmp[n];
            ref.r_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0xF800) >> 8 );
            ref.g_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x07E0) >> 3 );
            ref.b_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x001F) << 3 );
        }
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_565 && F::iterator_size > 1 && F::is_block == 1)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    assert((in_w % F::iterator_size) == 0);
    assert((in_h % F::iterator_size) == 0);
    R2Y_ rgb_t tmp[F::iterator_size * F::iterator_size];
    GLB_ size_t row_offset = in_w - F::iterator_size;
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);
    for (GLB_ size_t i = 0; i < in_h; i += F::iterator_size, cur_pixel += (in_w * (F::iterator_size - 1)))
    {
        for (GLB_ size_t j = 0; j < in_w; j += F::iterator_size, cur_pixel += F::iterator_size)
        {
            GLB_ uint16_t * block_iter = cur_pixel;
            for (int n = 0, index = 0; n < F::iterator_size; ++n, block_iter += row_offset)
            {
                for (int m = 0; m < F::iterator_size; ++m, ++index, ++block_iter)
                {
                    R2Y_ rgb_t & ref = tmp[index];
                    ref.r_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0xF800) >> 8 );
                    ref.g_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x07E0) >> 3 );
                    ref.b_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x001F) << 3 );
                }
            }
            STD_ forward<T>(do_sth)(tmp);
        }
    }
}

/* 555 */

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_555 && F::iterator_size == 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size & 1) == 0); // in_size must be an even number
    R2Y_ rgb_t tmp;
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += 2, ++cur_pixel)
    {
        tmp.r_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x7C00) >> 7 );
        tmp.g_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x03E0) >> 2 );
        tmp.b_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x001F) << 3 );
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_555 && F::iterator_size > 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size & 1) == 0); // in_size must be an even number
    R2Y_ rgb_t tmp[F::iterator_size];
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += (2 * F::iterator_size))
    {
        for (int n = 0; n < F::iterator_size; ++n, ++cur_pixel)
        {
            R2Y_ rgb_t & ref = tmp[n];
            ref.r_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x7C00) >> 7 );
            ref.g_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x03E0) >> 2 );
            ref.b_ = static_cast<GLB_ uint8_t>( ((*cur_pixel) & 0x001F) << 3 );
        }
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_555 && F::iterator_size > 1 && F::is_block == 1)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    assert((in_w % F::iterator_size) == 0);
    assert((in_h % F::iterator_size) == 0);
    R2Y_ rgb_t tmp[F::iterator_size * F::iterator_size];
    GLB_ size_t row_offset = in_w - F::iterator_size;
    GLB_ uint16_t * cur_pixel = reinterpret_cast<GLB_ uint16_t *>(in_data);
    for (GLB_ size_t i = 0; i < in_h; i += F::iterator_size, cur_pixel += (in_w * (F::iterator_size - 1)))
    {
        for (GLB_ size_t j = 0; j < in_w; j += F::iterator_size, cur_pixel += F::iterator_size)
        {
            GLB_ uint16_t * block_iter = cur_pixel;
            for (int n = 0, index = 0; n < F::iterator_size; ++n, block_iter += row_offset)
            {
                for (int m = 0; m < F::iterator_size; ++m, ++index, ++block_iter)
                {
                    R2Y_ rgb_t & ref = tmp[index];
                    ref.r_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x7C00) >> 7 );
                    ref.g_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x03E0) >> 2 );
                    ref.b_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x001F) << 3 );
                }
            }
            STD_ forward<T>(do_sth)(tmp);
        }
    }
}

/* 444 */

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_444 && F::iterator_size == 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % 3) == 0); // 3 input bytes convert to 2 pixels (6 bytes)
    R2Y_ rgb_t tmp;
    for (GLB_ size_t i = 0; i < size; i += 3)
    {
        tmp.b_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
        tmp.g_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
        tmp.r_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
        STD_ forward<T>(do_sth)(tmp);
        tmp.b_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
        tmp.g_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
        tmp.r_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_444 && F::iterator_size > 1 && (F::iterator_size & 1) == 0 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % 3) == 0); // 3 input bytes convert to 2 pixels (6 bytes)
    R2Y_ rgb_t tmp[F::iterator_size];
    for (GLB_ size_t i = 0; i < size; i += (3 * (F::iterator_size >> 1)))
    {
        for (int n = 0; n < F::iterator_size;)
        {
            {
                R2Y_ rgb_t & ref = tmp[n]; ++n;
                ref.b_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
                ref.g_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
                ref.r_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
            }
            {
                R2Y_ rgb_t & ref = tmp[n]; ++n;
                ref.b_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
                ref.g_ = static_cast<GLB_ uint8_t>( ((*in_data) & 0x0F) << 4 );
                ref.r_ = static_cast<GLB_ uint8_t>  ((*in_data) & 0xF0);        ++in_data;
            }
        }
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_444 && F::iterator_size > 1 && (F::iterator_size & 1) == 0 && F::is_block == 1)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    assert((in_w % F::iterator_size) == 0);
    assert((in_h % F::iterator_size) == 0);
    R2Y_ rgb_t tmp[F::iterator_size * F::iterator_size];
    GLB_ size_t row_offset = ((in_w - F::iterator_size) >> 1) * 3;
    for (GLB_ size_t i = 0; i < in_h; i += F::iterator_size, in_data += (in_w * (((F::iterator_size - 1) * 3) >> 1)) )
    {
        for (GLB_ size_t j = 0; j < in_w; j += F::iterator_size, in_data += (3 * (F::iterator_size >> 1)))
        {
            R2Y_ byte_t * block_iter = in_data;
            for (int n = 0, index = 0; n < F::iterator_size; ++n, block_iter += row_offset)
            {
                for (int m = 0; m < F::iterator_size; m += 2)
                {
                    {
                        R2Y_ rgb_t & ref = tmp[index]; ++index;
                        ref.b_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x0F) << 4 );
                        ref.g_ = static_cast<GLB_ uint8_t>  ((*block_iter) & 0xF0);        ++block_iter;
                        ref.r_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x0F) << 4 );
                    }
                    {
                        R2Y_ rgb_t & ref = tmp[index]; ++index;
                        ref.b_ = static_cast<GLB_ uint8_t>  ((*block_iter) & 0xF0);        ++block_iter;
                        ref.g_ = static_cast<GLB_ uint8_t>( ((*block_iter) & 0x0F) << 4 );
                        ref.r_ = static_cast<GLB_ uint8_t>  ((*block_iter) & 0xF0);        ++block_iter;
                    }
                }
            }
            STD_ forward<T>(do_sth)(tmp);
        }
    }
}

/* 888X */

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_888X && F::iterator_size == 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % 4) == 0); // 4 input bytes convert to 1 pixel (3 bytes)
    GLB_ uint32_t * cur_pixel = reinterpret_cast<GLB_ uint32_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += 4, ++cur_pixel)
    {
        STD_ forward<T>(do_sth)(*reinterpret_cast<R2Y_ rgb_t *>(cur_pixel));
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_888X && F::iterator_size > 1 && F::is_block == 0)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    GLB_ size_t size = calculate_size<S>(in_w, in_h);
    assert((size % 4) == 0); // 4 input bytes convert to 1 pixel (3 bytes)
    R2Y_ rgb_t tmp[F::iterator_size];
    GLB_ uint32_t * cur_pixel = reinterpret_cast<GLB_ uint32_t *>(in_data);
    for (GLB_ size_t i = 0; i < size; i += (4 * F::iterator_size))
    {
        for (int n = 0; n < F::iterator_size; ++n, ++cur_pixel)
        {
            tmp[n] = *reinterpret_cast<R2Y_ rgb_t *>(cur_pixel);
        }
        STD_ forward<T>(do_sth)(tmp);
    }
}

template <R2Y_ supported S, typename T, typename F = STD_ remove_reference_t<T>>
STD_ enable_if_t<(S == R2Y_ rgb_888X && F::iterator_size > 1 && F::is_block == 1)>
    pixel_foreach(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h, T && do_sth)
{
    assert((in_w % F::iterator_size) == 0);
    assert((in_h % F::iterator_size) == 0);
    R2Y_ rgb_t tmp[F::iterator_size * F::iterator_size];
    GLB_ size_t row_offset = in_w - F::iterator_size;
    GLB_ uint32_t * cur_pixel = reinterpret_cast<GLB_ uint32_t *>(in_data);
    for (GLB_ size_t i = 0; i < in_h; i += F::iterator_size, cur_pixel += (in_w * (F::iterator_size - 1)))
    {
        for (GLB_ size_t j = 0; j < in_w; j += F::iterator_size, cur_pixel += F::iterator_size)
        {
            GLB_ uint32_t * block_iter = cur_pixel;
            for (int n = 0, index = 0; n < F::iterator_size; ++n, block_iter += row_offset)
            {
                for (int m = 0; m < F::iterator_size; ++m, ++index, ++block_iter)
                {
                    tmp[index] = *reinterpret_cast<R2Y_ rgb_t *>(block_iter);
                }
            }
            STD_ forward<T>(do_sth)(tmp);
        }
    }
}
