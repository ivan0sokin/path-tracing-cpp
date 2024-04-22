#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include <numbers>

namespace Math {
    namespace Constants {
        template<typename T> constexpr static T Pi = static_cast<T>(std::numbers::pi);
        template<typename T> constexpr static T Tau = static_cast<T>(2.0 * Pi<T>);
        template<typename T> constexpr static T InversePi = static_cast<T>(std::numbers::inv_pi);

        template<typename T> constexpr static T HalfCircumferenceInDegrees = static_cast<T>(180.0);
        template<typename T> constexpr static T InverseHalfCircumferenceInDegrees = static_cast<T>(1) / HalfCircumferenceInDegrees<T>;
        template<typename T> constexpr static T HalfCircumferenceInRadians = Pi<T>;
        template<typename T> constexpr static T InverseHalfCircumferenceInRadians = InversePi<T>;
    
        template<typename T> constexpr static T Epsilon = std::numeric_limits<T>::epsilon();
        template<typename T> constexpr static T Infinity = std::numeric_limits<T>::infinity();
    }
}

#endif