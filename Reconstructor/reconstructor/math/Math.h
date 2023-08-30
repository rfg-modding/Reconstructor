#pragma once
#include "common/Typedefs.h"
#include "RFGR_Types/rfg/Vector.h"

namespace math
{
    //Fit a target value in the range of min to max
    static void Range(f32& target, f32 min, f32 max)
    {
        if (target < min)
            target = min;
        else if (target > max)
            target = max;
    }

    //Fit a target value in the range of min to max, but with vectors
    static void Range(vector& target, const vector& min, const vector& max)
    {
        Range(target.x, min.x, max.x);
        Range(target.y, min.y, max.y);
        Range(target.z, min.z, max.z);
    }

    //Fit the elements of the target vector within the range of min to max
    static void Range(vector& target, f32 min, f32 max)
    {
        Range(target.x, min, max);
        Range(target.y, min, max);
        Range(target.z, min, max);
    }

    //Linearly interpolate between currentVal and targetVal. Use interpolant to bias result towards one value or the other.
    //If interpolant == 1.0, the result will be targetVal
    //If interpolant == 0.0, the result will be currentVal
    //If 0.0 < interpolant < 1.0, the result will lie somewhere between currentVal and targetVal
    static vector Lerp(vector currentVal, vector targetVal, f32 interpolant)
    {
        return currentVal * (1.0f - interpolant) + targetVal * interpolant;
    }
}