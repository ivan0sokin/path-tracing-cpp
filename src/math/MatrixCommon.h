#ifndef _MATRIX_COMMON_H
#define _MATRIX_COMMON_H

#include "Types.h"
#include "Constants.h"

namespace Math {
    template<typename T, std::size_t N>
    constexpr Types::Matrix<T, N, N> IdentityMatrix() noexcept {
        Types::Matrix<T, N, N> identity;

        for (int i = 0; i < N; ++i) {
            identity.table[i][i] = 1;
        }

        return identity;
    }

    namespace Types {
        template<typename T, std::size_t N, std::size_t M, std::size_t L>
        constexpr Types::Matrix<T, N, L> operator*(const Types::Matrix<T, N, M> &a, const Types::Matrix<T, M, L> &b) noexcept {
            Types::Matrix<T, N, L> result;
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < L; ++j) {
                    for (int k = 0; k < M; ++k) {
                        result[i][j] += a[i][k] * b[k][j];
                    }
                }
            }

            return result;
        }

        template<typename T, std::size_t N, std::size_t M>
        constexpr Types::Vector<T, N> operator*(const Types::Matrix<T, N, M> &m, const Types::Vector<T, M> &v) noexcept {
            Types::Vector<T, N> result;
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < M; ++j) {
                    result[i] += m[i][j] * v[j];
                }
            }

            return result;
        }

        template<typename T, std::size_t N, std::size_t M>
        constexpr Types::Vector<T, M> operator*(const Types::Vector<T, N> &v, const Types::Matrix<T, N, M> &m) noexcept {
            Types::Vector<T, M> result;
            for (int i = 0; i < M; ++i) {
                for (int j = 0; j < N; ++j) {
                    result[i] += v[j] * m[j][i];
                }
            }

            return result;
        }
    }

    template<typename T, std::size_t N>
    constexpr Types::Matrix<T, N, N> Transpose(const Types::Matrix<T, N, N> &m) noexcept {
        Types::Matrix<T, N, N> result;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                result[i][j] = m[j][i];
            }
        }

        return result;
    }

    template<typename T>
    constexpr Types::Matrix<T, 4, 4> Inverse(const Types::Matrix<T, 4, 4> &m) noexcept {
        Types::Matrix<T, 4, 4> inverse;

        inverse.data[0] = m.data[5]  * m.data[10] * m.data[15] - 
                          m.data[5]  * m.data[11] * m.data[14] - 
                          m.data[9]  * m.data[6]  * m.data[15] + 
                          m.data[9]  * m.data[7]  * m.data[14] +
                          m.data[13] * m.data[6]  * m.data[11] - 
                          m.data[13] * m.data[7]  * m.data[10];

        inverse.data[4] = -m.data[4]  * m.data[10] * m.data[15] + 
                           m.data[4]  * m.data[11] * m.data[14] + 
                           m.data[8]  * m.data[6]  * m.data[15] - 
                           m.data[8]  * m.data[7]  * m.data[14] - 
                           m.data[12] * m.data[6]  * m.data[11] + 
                           m.data[12] * m.data[7]  * m.data[10];

        inverse.data[8] = m.data[4]  * m.data[9]  * m.data[15] - 
                          m.data[4]  * m.data[11] * m.data[13] - 
                          m.data[8]  * m.data[5]  * m.data[15] + 
                          m.data[8]  * m.data[7]  * m.data[13] + 
                          m.data[12] * m.data[5]  * m.data[11] - 
                          m.data[12] * m.data[7]  * m.data[9];

        inverse.data[12] = -m.data[4]  * m.data[9]  * m.data[14] + 
                            m.data[4]  * m.data[10] * m.data[13] +
                            m.data[8]  * m.data[5]  * m.data[14] - 
                            m.data[8]  * m.data[6]  * m.data[13] - 
                            m.data[12] * m.data[5]  * m.data[10] + 
                            m.data[12] * m.data[6]  * m.data[9];

        inverse.data[1] = -m.data[1]  * m.data[10] * m.data[15] + 
                           m.data[1]  * m.data[11] * m.data[14] + 
                           m.data[9]  * m.data[2]  * m.data[15] - 
                           m.data[9]  * m.data[3]  * m.data[14] - 
                           m.data[13] * m.data[2]  * m.data[11] + 
                           m.data[13] * m.data[3]  * m.data[10];

        inverse.data[5] = m.data[0]  * m.data[10] * m.data[15] - 
                          m.data[0]  * m.data[11] * m.data[14] - 
                          m.data[8]  * m.data[2]  * m.data[15] + 
                          m.data[8]  * m.data[3]  * m.data[14] + 
                          m.data[12] * m.data[2]  * m.data[11] - 
                          m.data[12] * m.data[3]  * m.data[10];

        inverse.data[9] = -m.data[0]  * m.data[9]  * m.data[15] + 
                           m.data[0]  * m.data[11] * m.data[13] + 
                           m.data[8]  * m.data[1]  * m.data[15] - 
                           m.data[8]  * m.data[3]  * m.data[13] - 
                           m.data[12] * m.data[1]  * m.data[11] + 
                           m.data[12] * m.data[3]  * m.data[9];

        inverse.data[13] = m.data[0]  * m.data[9]  * m.data[14] - 
                           m.data[0]  * m.data[10] * m.data[13] - 
                           m.data[8]  * m.data[1]  * m.data[14] + 
                           m.data[8]  * m.data[2]  * m.data[13] + 
                           m.data[12] * m.data[1]  * m.data[10] - 
                           m.data[12] * m.data[2]  * m.data[9];

        inverse.data[2] = m.data[1]  * m.data[6] * m.data[15] - 
                          m.data[1]  * m.data[7] * m.data[14] - 
                          m.data[5]  * m.data[2] * m.data[15] + 
                          m.data[5]  * m.data[3] * m.data[14] + 
                          m.data[13] * m.data[2] * m.data[7] - 
                          m.data[13] * m.data[3] * m.data[6];

        inverse.data[6] = -m.data[0]  * m.data[6] * m.data[15] + 
                           m.data[0]  * m.data[7] * m.data[14] + 
                           m.data[4]  * m.data[2] * m.data[15] - 
                           m.data[4]  * m.data[3] * m.data[14] - 
                           m.data[12] * m.data[2] * m.data[7] + 
                           m.data[12] * m.data[3] * m.data[6];

        inverse.data[10] = m.data[0]  * m.data[5] * m.data[15] - 
                           m.data[0]  * m.data[7] * m.data[13] - 
                           m.data[4]  * m.data[1] * m.data[15] + 
                           m.data[4]  * m.data[3] * m.data[13] + 
                           m.data[12] * m.data[1] * m.data[7] - 
                           m.data[12] * m.data[3] * m.data[5];

        inverse.data[14] = -m.data[0]  * m.data[5] * m.data[14] + 
                            m.data[0]  * m.data[6] * m.data[13] + 
                            m.data[4]  * m.data[1] * m.data[14] - 
                            m.data[4]  * m.data[2] * m.data[13] - 
                            m.data[12] * m.data[1] * m.data[6] + 
                            m.data[12] * m.data[2] * m.data[5];

        inverse.data[3] = -m.data[1] * m.data[6] * m.data[11] + 
                           m.data[1] * m.data[7] * m.data[10] + 
                           m.data[5] * m.data[2] * m.data[11] - 
                           m.data[5] * m.data[3] * m.data[10] - 
                           m.data[9] * m.data[2] * m.data[7] + 
                           m.data[9] * m.data[3] * m.data[6];

        inverse.data[7] = m.data[0] * m.data[6] * m.data[11] - 
                          m.data[0] * m.data[7] * m.data[10] - 
                          m.data[4] * m.data[2] * m.data[11] + 
                          m.data[4] * m.data[3] * m.data[10] + 
                          m.data[8] * m.data[2] * m.data[7] - 
                          m.data[8] * m.data[3] * m.data[6];

        inverse.data[11] = -m.data[0] * m.data[5] * m.data[11] + 
                            m.data[0] * m.data[7] * m.data[9] + 
                            m.data[4] * m.data[1] * m.data[11] - 
                            m.data[4] * m.data[3] * m.data[9] - 
                            m.data[8] * m.data[1] * m.data[7] + 
                            m.data[8] * m.data[3] * m.data[5];

        inverse.data[15] = m.data[0] * m.data[5] * m.data[10] - 
                           m.data[0] * m.data[6] * m.data[9] - 
                           m.data[4] * m.data[1] * m.data[10] + 
                           m.data[4] * m.data[2] * m.data[9] + 
                           m.data[8] * m.data[1] * m.data[6] - 
                           m.data[8] * m.data[2] * m.data[5];

        T determinant = m.data[0] * inverse.data[0] +
                        m.data[1] * inverse.data[4] +
                        m.data[2] * inverse.data[8] +
                        m.data[3] * inverse.data[12];

        if (Abs(determinant) <= Constants::Epsilon<T>) {
            return IdentityMatrix<T, 4>();
        }

        T oneOverDeterminant = Constants::One<T> / determinant;

        for (int i = 0; i < 16; i++) {
            inverse.data[i] *= oneOverDeterminant;
        }

        return inverse;
    }
}

#endif