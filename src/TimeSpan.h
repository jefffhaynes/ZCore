#pragma once

#include <stdint.h>

struct TimeSpan
{
public:
    constexpr TimeSpan() : TimeSpan(0)
    {
    }
    
    static constexpr TimeSpan FromMicroseconds(double microseconds) { return TimeSpan(microseconds * MillisecondsPerMicrosecond); }
    static constexpr TimeSpan FromMilliseconds(double milliseconds) { return TimeSpan(milliseconds); }
    static constexpr TimeSpan FromSeconds(double seconds) { return TimeSpan(seconds * MillsecondsPerSecond); }

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

    constexpr bool operator==(const TimeSpan& other) const
    {
        return _milliseconds == other._milliseconds;
    }

    constexpr bool operator!=(const TimeSpan& other)
    {
        return _milliseconds != other._milliseconds;
    }

    constexpr TimeSpan operator*(double scalar) const
    {
        return TimeSpan(_milliseconds * scalar);
    }


private:
    static constexpr double MicrosecondsPerMillisecond = 1000;
    static constexpr double MillisecondsPerMicrosecond = 1 / MicrosecondsPerMillisecond;
    static constexpr double MillsecondsPerSecond = 1000;
    static constexpr double SecondsPerMillisecond = 1 / MillsecondsPerSecond;

    double _milliseconds;
    
    constexpr TimeSpan(double milliseconds) : _milliseconds(milliseconds)
    {
    }
};

constexpr TimeSpan TimeSpan::Zero = TimeSpan::FromMilliseconds(0);