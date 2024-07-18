#ifndef _MATRIX_4_H
#define _MATRIX_4_H

#include "Constants.h"
#include "Types.h"
#include "GeometricFunctions.h"

#include <array>

namespace Math {
    namespace Types {
        //! 4x4 Matrix
        template<typename T>
        struct Matrix<T, 4, 4> {
            union {
                struct { T data[16]; };
                struct { T table[4][4]; };
            };
            
            constexpr Matrix() noexcept {
                for (int i = 0; i < 16; ++i) {
                    data[i] = Constants::Zero<T>;
                }
            }

            constexpr Matrix(const std::array<T, 16> &elements) noexcept {
                for (int i = 0; i < 16; ++i) {
                    data[i] = elements[i];
                }
            }

            constexpr Matrix(const std::array<Vector<T, 4>, 4> &rows) noexcept {
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        table[i][j] = rows[i][j];
                    }
                }
            }

            constexpr Matrix(const Matrix &other) noexcept = default;

            constexpr const T* operator[](std::size_t index) const noexcept {
                return table[index];
            }

            constexpr T* operator[](std::size_t index) noexcept {
                return table[index];
            }
        };
    }
}

#endif