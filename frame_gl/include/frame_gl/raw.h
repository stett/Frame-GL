#pragma once

// Matrix math for raw array style data. Useful for fast physicsy stuff.

namespace frame
{
    namespace raw
    {
        // TODO: Make this fancy! You could do cool static looping shit for happy fun times!

        ///\brief Matrix transform
        template <size_t dimension>
        void transform(float* result, const float** matrix, const float* vector);

        template <>
        inline void transform<3>(float* result, const float** matrix, const float* vector) {
            result[0] = matrix[0][0] * vector[0] + matrix[0][1] * vector[1] + matrix[0][2] * vector[2];
            result[1] = matrix[1][0] * vector[0] + matrix[1][1] * vector[1] + matrix[1][2] * vector[2];
            result[2] = matrix[2][0] * vector[0] + matrix[2][1] * vector[1] + matrix[2][2] * vector[2];
        }

        template <>
        inline void transform<4>(float* result, const float** matrix, const float* vector) {
            result[0] = matrix[0][0] * vector[0] + matrix[0][1] * vector[1] + matrix[0][2] * vector[2] + matrix[0][3] * vector[3];
            result[1] = matrix[1][0] * vector[0] + matrix[1][1] * vector[1] + matrix[1][2] * vector[2] + matrix[1][3] * vector[3];
            result[2] = matrix[2][0] * vector[0] + matrix[2][1] * vector[1] + matrix[2][2] * vector[2] + matrix[2][3] * vector[3];
            result[3] = matrix[3][3] * vector[0] + matrix[3][1] * vector[1] + matrix[3][2] * vector[2] + matrix[3][3] * vector[3];
        }

        ///\brief Vector cross product
        template <size_t dimension>
        void cross(float* result, const float* a, const float* b);

        template <>
        inline void cross<3>(float* result, const float* a, const float* b) {
            result[0] = a[1]*b[2] - a[2]*b[1];
            result[1] = a[2]*b[0] - a[0]*b[2];
            result[2] = a[0]*b[1] - a[1]*b[0];
        }

        ///\brief Vector dot product
        template <size_t dimension>
        float dot(const float* a, const float* b);

        template <>
        inline float dot<0>(const float* a, const float* b) {
            return a[0] * b[0];
        }

        template <size_t dimension>
        inline float dot(const float* a, const float* b) {
            return dot<dimension-1>(a, b) + a[dimension-1] * b[dimension-1];
        }

        ///\brief Vector addition
        template <size_t dimension>
        void add(float* result, const float* a, const float* b);

        template <>
        inline void add<0>(float* result, const float* a, const float* b) {
            result[0] = a[0] + b[0];
        }

        template <size_t dimension>
        void add(float* result, const float* a, const float* b) {
            result[dimension-1] = a[dimension-1] + b[dimension-1];
            add<dimension-1>(result, a, b);
        }

        ///\brief Vector subtraction
        template <size_t dimension>
        void subtract(float* result, const float* a, const float* b);

        template <>
        inline void subtract<0>(float* result, const float* a, const float* b) {
            result[0] = a[0] - b[0];
        }

        template <size_t dimension>
        void subtract(float* result, const float* a, const float* b) {
            result[dimension-1] = a[dimension-1] - b[dimension-1];
            subtract<dimension-1>(result, a, b);
        }
    }
}