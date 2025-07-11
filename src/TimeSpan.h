#pragma once

#include <stdint.h>

struct TimeSpan
{
public:
    constexpr TimeSpan() : TimeSpan(0)
    {
    }
    
    static constexpr TimeSpan FromNanoseconds(double nanoseconds) { return TimeSpan(nanoseconds * MillisecondsPerNanosecond); }
    static constexpr TimeSpan FromMicroseconds(double microseconds) { return TimeSpan(microseconds * MillisecondsPerMicrosecond); }
    static constexpr TimeSpan FromMilliseconds(double milliseconds) { return TimeSpan(milliseconds); }
    static constexpr TimeSpan FromSeconds(double seconds) { return TimeSpan(seconds * MillisecondsPerSecond); }

    constexpr double ToNanoseconds() const { return _milliseconds * NanosecondsPerMillisecond; }
    constexpr double ToMicroseconds() const { return _milliseconds * MicrosecondsPerMillisecond; }
    constexpr double ToMilliseconds() const { return _milliseconds; }
    constexpr double ToSeconds() const { return _milliseconds * SecondsPerMillisecond; }

    static const TimeSpan Zero;

    constexpr TimeSpan operator+(const TimeSpan& other) const
    {
        return TimeSpan(_milliseconds + other._milliseconds);
    }

    constexpr TimeSpan operator-(const TimeSpan& other) const
    {
        return TimeSpan(_milliseconds - other._milliseconds);
    }
    
    constexpr double operator/(const TimeSpan& other) const
    {
        return _milliseconds / other._milliseconds;
    }

    constexpr TimeSpan operator/(double scalar) const
    {
        return TimeSpan(_milliseconds / scalar);
    }

    constexpr bool operator<(const TimeSpan& other) const
    {
        return _milliseconds < other._milliseconds;
    }

    constexpr bool operator>(const TimeSpan& other) const
    {
        return _milliseconds > other._milliseconds;
    }
    
    constexpr TimeSpan& operator+=(const TimeSpan& other)
    {
        _milliseconds += other._milliseconds;
        return *this;
    }

    constexpr TimeSpan& operator-=(const TimeSpan& other)
    {
        _milliseconds -= other._milliseconds;
        return *this;
    }

    constexpr bool operator==(const TimeSpan& other) const
    {
        return _milliseconds == other._milliseconds;
    }

    constexpr bool operator!=(const TimeSpan& other) const
    {
        return _milliseconds != other._milliseconds;
    }

    constexpr TimeSpan operator*(double scalar) const
    {
        return TimeSpan(_milliseconds * scalar);
    }

    constexpr TimeSpan operator-() const
    {
        return TimeSpan(-_milliseconds);
    }


private:
    static constexpr double NanosecondsPerMillisecond = 1000000;
    static constexpr double MillisecondsPerNanosecond = 1.0 / NanosecondsPerMillisecond;
    static constexpr double MicrosecondsPerMillisecond = 1000;
    static constexpr double MillisecondsPerMicrosecond = 1.0 / MicrosecondsPerMillisecond;
    static constexpr double MillisecondsPerSecond = 1000;
    static constexpr double SecondsPerMillisecond = 1.0 / MillisecondsPerSecond;

    double _milliseconds;
    
    explicit constexpr TimeSpan(double milliseconds) : _milliseconds(milliseconds)
    {
    }
};

constexpr TimeSpan TimeSpan::Zero = TimeSpan::FromMilliseconds(0);