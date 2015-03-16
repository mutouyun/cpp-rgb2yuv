/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://darkc.at)
*/

#ifndef RGB2YUV_HPP__
#define RGB2YUV_HPP__

#include <stddef.h>  // size_t, ...
#include <stdint.h>  // uint8_t, ...
#include <assert.h>  // assert
#include <new>       // placement new, std::nothrow
#include <algorithm> // std::swap

#include "detail/predefine.hxx"

namespace R2Y_NAMESPACE_ {

#include "detail/basic_concept.hxx"
#include "detail/scope_block.hxx"
#include "detail/buffer_creator.hxx"
#include "detail/pixel_walker.hxx"
#include "detail/pixel_iterator.hxx"
#include "detail/pixel_convertor.hxx"

#ifdef R2Y_OLD_API_
#include "rgb2yuv_old.hpp"
#endif/*R2Y_OLD_API_*/

////////////////////////////////////////////////////////////////
/// Transforming between RGB & YUV/YCbCr blocks
////////////////////////////////////////////////////////////////

template <R2Y_ supported S>
struct do_convert_t
{
    enum
    {
        iterator_size = R2Y_ iterator<S>::iterator_size,
        is_block      = R2Y_ iterator<S>::is_block
    };

    R2Y_ iterator<S> iter_;

    do_convert_t(R2Y_ scope_block<R2Y_ byte_t> * ot_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : iter_(ot_data->data(), in_w, in_h)
    {}

    template <typename T, int> struct convert_pixel_t;
    template <int Dummy>       struct convert_pixel_t<R2Y_ rgb_t, Dummy> { typedef R2Y_ yuv_t type; };
    template <int Dummy>       struct convert_pixel_t<R2Y_ yuv_t, Dummy> { typedef R2Y_ rgb_t type; };

    template <typename T>
    void operator()(T const & pix)
    {
        iter_.set_and_next(pixel_convert(pix));
    }

    template <typename T, GLB_ size_t N>
    void operator()(T const (& pix)[N])
    {
        typename convert_pixel_t<T, 0>::type c_pix[N];
        for (GLB_ size_t i = 0; i < N; ++i)
        {
            c_pix[i] = pixel_convert(pix[i]);
        }
        iter_.set_and_next(c_pix);
    }
};

template <R2Y_ supported In, R2Y_ supported Ot>
typename R2Y_ enable_if<(In != Ot)
>::type transform(R2Y_ byte_t * in_data, GLB_ size_t in_w, GLB_ size_t in_h,
                  R2Y_ scope_block<R2Y_ byte_t> * ot_data)
{
    assert(in_data != NULL);
    assert(ot_data != NULL);
    assert(in_w > 0 && in_h > 0);

    R2Y_ do_convert_t<Ot> dc(ot_data, in_w, in_h);
    R2Y_ pixel_foreach<In>(in_data, in_w, in_h, dc);
}
    
} // namespace R2Y_NAMESPACE_

#include "detail/undefine.hxx"

#endif // RGB2YUV_HPP__
