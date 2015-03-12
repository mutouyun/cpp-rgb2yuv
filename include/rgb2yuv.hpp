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

////////////////////////////////////////////////////////////////
/// Predefine functional macros
////////////////////////////////////////////////////////////////

#pragma push_macro("GLB_")
#undef  GLB_
#define GLB_ ::

#pragma push_macro("STD_")
#undef  STD_
#define STD_ GLB_ std::

#pragma push_macro("R2Y_NAMESPACE_")
#undef  R2Y_NAMESPACE_
#define R2Y_NAMESPACE_ r2y

#pragma push_macro("R2Y_")
#undef  R2Y_
#define R2Y_ GLB_ R2Y_NAMESPACE_ ::

#pragma push_macro("R2Y_ALLOC_")
#ifndef R2Y_ALLOC_
#define R2Y_ALLOC_ R2Y_ allocator
#endif // R2Y_ALLOC_

#pragma push_macro("R2Y_UNUSED_")
#undef  R2Y_UNUSED_
#if defined(_MSC_VER)
#   define R2Y_UNUSED_ __pragma(warning(suppress:4100))
#elif defined(__GNUC__)
#   define R2Y_UNUSED_ __attribute__((__unused__))
#else
#   define R2Y_UNUSED_
#endif

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
    R2Y_ iterator<S> iter_;

    do_convert_t(R2Y_ scope_block<R2Y_ byte_t> * ot_data, GLB_ size_t in_w, GLB_ size_t in_h)
        : iter_(ot_data->data(), in_w, in_h)
    {}

    template <typename T>
    void operator()(T const & pixel)
    {
        iter_.set_and_next(pixel_convert(pixel));
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

////////////////////////////////////////////////////////////////
/// Pop previous definition of used macros
////////////////////////////////////////////////////////////////

#pragma pop_macro("R2Y_UNUSED_")
#pragma pop_macro("R2Y_ALLOC_")
#pragma pop_macro("R2Y_")
#pragma pop_macro("R2Y_NAMESPACE_")
#pragma pop_macro("STD_")
#pragma pop_macro("GLB_")

#endif // RGB2YUV_HPP__
