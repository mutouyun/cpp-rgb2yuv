#ifndef DETECT_PLAT_HPP
#define DETECT_PLAT_HPP

#if defined(WINCE) || defined(_WIN32_WCE)
#   define SIMPLE_WINCE__
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#   define SIMPLE_WIN64__
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   define SIMPLE_WIN32__
#elif defined(__linux__) || defined(__linux)
#   define SIMPLE_LINUX__
#else
#   error "This OS is unsupported."
#endif

#if defined(SIMPLE_WIN32__) || defined(SIMPLE_WIN64__) || defined(SIMPLE_WINCE__)
#   define SIMPLE_WINDOWS__
#endif

#if defined(SIMPLE_WINDOWS__)

#   ifndef _WIN32_WINNT
#       define _WIN32_WINNT     0x0501
#   endif
#   ifndef WINVER
#       define WINVER           _WIN32_WINNT
#   endif
#   ifndef _WIN32_WINDOWS
#       define _WIN32_WINDOWS   0x0410
#   endif
#   ifndef _WIN32_IE
#       define _WIN32_IE        0x0600
#   endif

#endif

#endif // DETECT_PLAT_HPP
