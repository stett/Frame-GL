#pragma once
#include "frame_gl/math.h"

namespace frame
{
    /// \class quat_t
    /// \brief Quaternion with generic floating point type
    template <typename T>
    class quat_t
    {
    public:
        typedef glm::tvec3<T> vtype;

    public:
        quat_t() : s(static_cast<T>(1.0)) {}
        quat_t(const quat_t& other) : s(other.s), v(other.v) {}
        quat_t(T s, const glm::tvec3<T>& v) : s(s), v(v) {}
        quat_t(const glm::tvec3<T>& angles) { from_euler(angles); }
        quat_t(const glm::quat& glmquat) : s(glmquat.w) {
            v.x = glmquat.x;
            v.y = glmquat.y;
            v.z = glmquat.z;
        }
        quat_t(const glm::mat4& matrix) : quat_t(glm::quat(matrix)) {
            normalize();
        }
        quat_t(const glm::tvec3<T>& a, const glm::tvec3<T>& b) { from_rotation(a,b); }
        ~quat_t() {}

        quat_t& operator=(const quat_t& other) {
            s = other.s;
            v = other.v;
            return *this;
        }

        quat_t& operator+=(const quat_t& other) {
            s += other.s;
            v += other.v;
            return *this;
        }

        quat_t& operator-=(const quat_t& other) {
            s -= other.s;
            v -= other.v;
            return *this;
        }

        quat_t& operator*=(const quat_t& other) {
            T s_old = s;
            s = (s * other.s) - glm::dot(v, other.v);
            v = (s_old * other.v) + (v * other.s) + glm::cross(v, other.v);
            return *this;
        }

        quat_t& operator*=(float c) {
            s *= c;
            v *= c;
            return *this;
        }

        bool operator==(const quat_t& other) const { return (s == other.s) && (v == other.v); }

        bool operator!=(const quat_t& other) const { return !operator==(other); }

        void from_euler(const glm::tvec3<T> angles) {
            float c1 = cos(angles.x/2);
            float s1 = sin(angles.x/2);
            float c2 = cos(angles.y/2);
            float s2 = sin(angles.y/2);
            float c3 = cos(angles.z/2);
            float s3 = sin(angles.z/2);
            s   = c1*c2*c3 + s1*s2*s3;
            v.x = s1*c2*c3 - c1*s2*s3;
            v.y = c1*s2*c3 + s1*c2*s3;
            v.z = c1*c2*s3 - s1*s2*c3;

            // Just to be safe...
            normalize();
        }

        // TODO: Make this an rvalue reference
        static quat_t&& axis_angle(const glm::tvec3<T>& axis, float angle) {
            quat_t q;
            float half_angle = angle * 0.5f;
            q.s = cos(half_angle);
            q.v = axis * sin(half_angle);
            return std::move(q);
        }

        // TODO: Make this an rvalue reference
        static quat_t&& axis_angle(glm::tvec3<T> axis_angle) {
            float angle = length(axis_angle);
            return std::move(quat_t<T>::axis_angle(axis_angle / angle, angle));
        }

        // Adapted from: http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
        void from_rotation(const glm::vec3 a, const glm::vec3 b) {

            float norm_a_norm_b = sqrt(dot(a, a) * dot(b, b));
            if (norm_a_norm_b < std::numeric_limits<float>::epsilon()) {
                v = vec3(0.0f, 1.0f, 0.0f);
                s = 0.0f;
                return;
            }
            float cos_theta = dot(a, b) / norm_a_norm_b;
            if (cos_theta < -1.0f + std::numeric_limits<float>::epsilon()) {
                v = vec3(0.0f, 1.0f, 0.0f);
                s = 0.0f;
                return;
            }

            s = sqrt(0.5f * (1.f + cos_theta));
            v = cross(a, b) / (norm_a_norm_b * 2.f * s);

            // Unsure that this is necessary...
            normalize();
        }


        quat_t& normalize() {
            T m_inv = 1.0f / magnitude();
            s *= m_inv;
            v *= m_inv;
            return *this;
        }

        quat_t& conjugate() {
            v *= -1.0f;
            return *this;
        }

        quat_t normalized() const {
            quat_t q(*this);
            return q.normalize();
        }

        quat_t conjugated() const {
            quat_t q(*this);
            return q.conjugate();
        }

        T magnitude() const {
            return sqrt(dot(*this, *this));
        }

        glm::tmat4x4<T> matrix() const {
            glm::tmat4x4<T> m;

            // Row 1
            m[0][0] = 1.0f - 2.0f*(v.y*v.y + v.z*v.z);
            m[0][1] = 2.0f*(v.x*v.y + s*v.z);
            m[0][2] = 2.0f*(v.x*v.z - s*v.y),
            m[0][3] = 0.0f;

            // Row 2
            m[1][0] = 2.0f*(v.x*v.y - s*v.z);
            m[1][1] = 1.0f - 2.0f*(v.x*v.x + v.z*v.z);
            m[1][2] = 2.0f*(v.y*v.z + s*v.x);
            m[1][3] = 0.0f;

            // Row 3
            m[2][0] = 2.0f*(v.x*v.z + s*v.y);
            m[2][1] = 2.0f*(v.y*v.z - s*v.x);
            m[2][2] = 1.0f - 2.0f*(v.x*v.x + v.y*v.y);
            m[2][3] = 0.0f;

            // Row 4
            m[3][0] = 0.0f;
            m[3][1] = 0.0f;
            m[3][2] = 0.0f;
            m[3][3] = 1.0f;

            return m;
        }

    public:
        T s;
        vtype v;
    };

    template <typename T>
    T dot(const quat_t<T>& a, const quat_t<T>& b) {
        return (a.s * b.s) + (a.v.x * b.v.x) + (a.v.y * b.v.y) + (a.v.z * b.v.z);
    }

    template <typename T>
    quat_t<T> normalize(quat_t<T> q) {
        return q.normalized();
    }

    template <typename T>
    quat_t<T> operator+(quat_t<T> a, const quat_t<T>& b) {
        return a += b;
    }

    template <typename T>
    quat_t<T> operator-(quat_t<T> a, const quat_t<T>& b) {
        return a -= b;
    }

    template <typename T>
    quat_t<T> operator*(quat_t<T> a, const quat_t<T>& b) {
        return a *= b;
    }

    template <typename T>
    quat_t<T> operator*(quat_t<T> q, T c) {
        return q *= c;
    }

    template <typename T>
    quat_t<T> operator*(T c, quat_t<T> q) {
        return q *= c;
    }

    template <typename T>
    glm::tvec3<T> operator*(const quat_t<T>& q, const glm::tvec3<T>& v) {
        return ((q * quat_t<T>(0.0f, v)) * q.conjugated()).v;
    }

    // Specialized quaternion types
    typedef quat_t<float> quat;
}
