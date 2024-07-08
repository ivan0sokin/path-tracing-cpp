#ifndef _TRANSFORMATION_H
#define _TRANSFORMATION_H

#include "MatrixCommon.h"
#include "TrigonometricFunctions.h"

namespace Math {
    template<typename T>
    constexpr Types::Matrix<T, 4, 4> TranslationMatrix(const Types::Vector<T, 3> &translation) noexcept {
        Types::Matrix<T, 4, 4> result = IdentityMatrix<T, 4>();
        
        for (int i = 0; i < 3; ++i) {
            result[i][3] = translation[i];
        }

        return result;
    }

    template<typename T>
    constexpr Types::Matrix<T, 4, 4> RotationMatrix(const Types::Vector<T, 3> &angles) noexcept {
        T yaw = angles.z;
        T pitch = angles.y;
        T roll = angles.x;

        T sinYaw = Sin(yaw), cosYaw = Cos(yaw);
        T sinPitch = Sin(pitch), cosPitch = Cos(pitch);
        T sinRoll = Sin(roll), cosRoll = Cos(roll);

        Types::Matrix<T, 4, 4> rotation = IdentityMatrix<T, 4>();
        
        rotation[0][0] = cosYaw * cosPitch;
        rotation[0][1] = cosYaw * sinPitch * sinRoll - sinYaw * cosRoll;
        rotation[0][2] = cosYaw * sinPitch * cosRoll + sinYaw * sinRoll;

        rotation[1][0] = sinYaw * cosPitch;
        rotation[1][1] = sinYaw * sinPitch * sinRoll + cosYaw * cosRoll;
        rotation[1][2] = sinYaw * sinPitch * cosRoll - cosYaw * sinRoll;
        
        rotation[2][0] = -sinPitch;
        rotation[2][1] = cosPitch * sinRoll;
        rotation[2][2] = cosPitch * cosRoll;

        return rotation;
    }

    template<typename T>
    constexpr Types::Vector<T, 3> TransformVector(const Types::Matrix<T, 4, 4> &transform, const Types::Vector<T, 3> &v) noexcept {
        return transform * Types::Vector<T, 4>(v, Constants::Zero<T>);
    }

    template<typename T>
    constexpr Types::Vector<T, 3> TransformPoint(const Types::Matrix<T, 4, 4> &transform, const Types::Vector<T, 3> &v) noexcept {
        return transform * Types::Vector<T, 4>(v, Constants::One<T>);
    }
}

#endif