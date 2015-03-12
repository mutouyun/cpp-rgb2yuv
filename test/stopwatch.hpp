#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include "detect_plat.hpp"

#if defined(SIMPLE_WINDOWS__)
#include <windows.h>
#elif defined(SIMPLE_LINUX__)
#include <time.h>
#endif

namespace simple {

////////////////////////////////////////////////////////////////

inline unsigned long tickcount(void)
{
#if defined(SIMPLE_WINDOWS__)
    return static_cast<unsigned long>(::GetTickCount());
#elif defined(SIMPLE_LINUX__)
    /* need -lrt */
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return (tv.tv_sec * 1000) + (tv.tv_nsec / 1000000);
#else
    return 0;
#endif
}

namespace use
{
    struct clock_default // Default clock policy
    {
        typedef unsigned long clock_t;
        clock_t clock(void) { return simple::tickcount(); }
        static double second(clock_t cl) { return double(cl) / double(1000); }
    };
}

template <typename ModelT = simple::use::clock_default>
class stopwatch : public ModelT
{
public:
    typedef typename ModelT::clock_t clock_t;

protected:
    clock_t start_time_;
    clock_t pause_elap_;
    bool    is_stopped_;

public:
    stopwatch(bool start_watch = false)
        : ModelT()
        , start_time_(0)
        , pause_elap_(0)
        , is_stopped_(true)
    {
        if (start_watch) start();
    }

public:
    bool check(void)
    {
        return (start_time_ <= ModelT::clock());
    }

    double value(void)
    {
        return ModelT::second(elapsed());
    }

    clock_t elapsed(void)
    {
        if (is_stopped()) return 0;
        else
        if (is_paused())  return pause_elap_;
        else
        {
            clock_t now = ModelT::clock();
            if (start_time_ > now)
            {
                stop();
                return 0;
            }
            return now - start_time_;
        }
    }

    void start(void)
    {
        elapsed(); // if (start_time_ > now), stopwatch will restart
        start_time_ = ModelT::clock();
        start_time_ -= pause_elap_;
        pause_elap_ = 0;
        is_stopped_ = false;
    }

    void pause(void)
    {
        pause_elap_ = elapsed();
    }

    void stop(void)
    {
        start_time_ = 0;
        pause_elap_ = 0;
        is_stopped_ = true;
    }

    bool is_stopped(void)
    {
        return is_stopped_;
    }

    bool is_paused(void)
    {
        return (pause_elap_ != 0);
    }
};

////////////////////////////////////////////////////////////////

} // namespace simple

#endif // STOPWATCH_HPP
