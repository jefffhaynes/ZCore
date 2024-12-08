#pragma once

#include <stdint.h>

struct Unit
{
public:
    constexpr Unit() = default;
    constexpr Unit& operator=(const Unit& unit) = default;

protected:
    constexpr Unit(float units) : _units(units)
    {
    }

    static constexpr float FromMicrounits(float microunits) { return microunits * u; }
    static constexpr float FromMilliunits(float milliunits) { return milliunits * m; }
    static constexpr float FromCentiunits(float centiunits) { return centiunits * c; }
    static constexpr float FromUnits(float units) { return units; }
    static constexpr float FromKilounits(float kilounits) { return kilounits * k; }
    static constexpr float FromMegaunits(float megaunits) { return megaunits * M; }
    
    constexpr float ToMicrounits() const { return _units * M; }
    constexpr float ToMilliunits() const { return _units * k; }
    constexpr float ToCentiunits() const { return _units * c; }
    constexpr float ToUnits() const { return _units; }
    constexpr float ToKilounits() const { return _units * m; }
    constexpr float ToMegaunits() const { return _units * u; }

private:
    static constexpr float p = 1e-12;
    static constexpr float n = 1e-9;
    static constexpr float u = 1e-6; 
    static constexpr float m = 1e-3;
    static constexpr float c = 1e-2;
    static constexpr float k = 1e3;
    static constexpr float M = 1e6;

    float _units = 0;
};
