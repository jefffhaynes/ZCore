#pragma once

// here because we need constexpr for unit tests
// switch to std when available

class CoreMath
{
public:
    static constexpr float Factorial(int n) 
    {
        return (n <= 1) ? 1 : (n * Factorial(n - 1));
    }

    static constexpr float Power(float base, int exp) 
    {
        return (exp == 0) ? 1 : (base * Power(base, exp - 1));
    }

    static constexpr float Exp(float x, int terms = 10) 
    {
        float sum = 0;
        for (int n = 0; n < terms; ++n) 
        {
            sum += Power(x, n) / Factorial(n);
        }
        return sum;
    }
};

