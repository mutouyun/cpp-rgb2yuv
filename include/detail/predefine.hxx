/*
    rgb2yuv - Code covered by the MIT License
    Author: mutouyun (http://orzz.org)
*/

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

#pragma push_macro("R2Y_FORCE_INLINE_")
#if defined(_MSC_VER)
#   define R2Y_FORCE_INLINE_ __forceinline
#elif defined(__GNUC__)
#   define R2Y_FORCE_INLINE_ __inline__ __attribute__((always_inline))
#else
#   define R2Y_FORCE_INLINE_ inline
#endif
