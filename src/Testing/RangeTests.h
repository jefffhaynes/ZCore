#include <cmath>
#include <limits>

#include "Range.h"

constexpr Range<float> TestRange{0.0f, 10.0f};

// Test cases for Min and Max methods
static_assert(TestRange.Min() == 0, "Min method failed");
static_assert(TestRange.Max() == 10, "Max method failed");

// Test case for Size method
static_assert(TestRange.Size() == 10, "Size method failed");

// Test case for Reverse method
static_assert(TestRange.Reverse().Min() == 10, "Reverse method failed on min");
static_assert(TestRange.Reverse().Max() == 0, "Reverse method failed on max");

// Test cases for Clamp method
static_assert(TestRange.Clamp(-5) == 0, "Clamp method failed for lower bound");
static_assert(TestRange.Clamp(15) == 10, "Clamp method failed for upper bound");
static_assert(TestRange.Clamp(5) == 5, "Clamp method failed for within range");

// Test cases for Contains method
static_assert(TestRange.Contains(-5) == false, "Contains method failed for lower bound");
static_assert(TestRange.Contains(15) == false, "Contains method failed for upper bound");
static_assert(TestRange.Contains(0) == true, "Contains method failed for within range");
static_assert(TestRange.Contains(5) == true, "Contains method failed for within range");
static_assert(TestRange.Contains(10) == true, "Contains method failed for within range");

constexpr Range<float> TestScaledToRange{0.0f, 1.0f};

// Test cases for ScaleTo method
static_assert(TestRange.ScaleTo(TestScaledToRange, 5) == 0.5f, "ScaleTo method failed");
static_assert(TestRange.ScaleTo(TestRange, 5) == 5.0f, "ScaleTo method failed");

// Test cases for ScaleToFloor method
static_assert(TestRange.ScaleToFloor(TestScaledToRange, 5) == 0.0f, "ScaleToFloor method failed");

// Test cases for ScaleToCeiling method
static_assert(TestRange.ScaleToCeiling(TestScaledToRange, 5) == 1.0f, "ScaleToCeiling method failed");
