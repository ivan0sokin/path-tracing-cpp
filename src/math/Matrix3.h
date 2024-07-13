#ifndef _MATRIX_3_H
#define _MATRIX_3_H

#include "Constants.h"
#include "Types.h"
#include "GeometricFunctions.h"

#include <array>

namespace Math {
    namespace Types {
        template<typename T>
        struct Matrix<T, 3, 3> {
            union {
                struct { T data[9]; };
                struct { T table[3][3]; };
            };
            
            constexpr Matrix() noexcept {
                for (int i = 0; i < 9; ++i) {
                    data[i] = Constants::Zero<T>;
                }
            }

            constexpr Matrix(const std::array<T, 9> &elements) noexcept {
                for (int i = 0; i < 9; ++i) {
                    data[i] = elements[i];
                }
            }

            constexpr Matrix(const std::array<Vector<T, 3>, 3> &rows) noexcept {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
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