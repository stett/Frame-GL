#pragma once
#include <limits>
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
            //normalize();
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

        operator glm::tmat3x3<T>() const {
            vec3 v2 = v * v;
            return glm::tmat3x3<T>({
                1.0f - 2.0f * (v2.y + v2.z), 2.0f * (v.x*v.y + s*v.z), 2.0f * (v.x*v.z - s*v.y),
                2.0f * (v.x*v.y - s*v.z), 1.0f - 2.0f * (v2.x + v2.z), 2.0f * (v.y*v.z + s*v.x),
                2.0f * (v.x*v.z + s*v.y), 2.0f * (v.y*v.z - s*v.x), 1.0f - 2.0f * (v2.x + v2.y)
            });
        }

        operator glm::tmat4x4<T>() const {
            return glm::tmat4x4<T>( (glm::tmat3x3<T>)(*this) );
        }

        bool operator==(const quat_t& other) const { return (s == other.s) && (v == other.v); }

        bool operator!=(const quat_t& other) const { return !operator==(other); }

        void from_euler(const glm::tvec3<T> angles) {
            float c1 = cos(angles.x*0.5f);
            float s1 = sin(angles.x*0.5f);
            float c2 = cos(angles.y*0.5f);
            float s2 = sin(angles.y*0.5f);
            float c3 = cos(angles.z*0.5f);
            float s3 = sin(angles.z*0.5f);
            s   = c1*c2*c3 + s1*s2*s3;
            v.x = s1*c2*c3 - c1*s2*s3;
            v.y = c1*s2*c3 + s1*c2*s3;
            v.z = c1*c2*s3 - s1*s2*c3;

            // Just to be safe...
            normalize();
        }

        static void axis_angle(quat_t& q, const glm::tvec3<T>& axis, float angle) {
            float half_angle = angle * 0.5f;
            q.s = cos(half_angle);
            q.v = axis * sin(half_angle);
            q.normalize();
        }

        static void axis_angle(quat_t& q, glm::tvec3<T> axis) {
            float angle = length(axis);
            q = quat_t<T>::axis_angle(abs(angle) > std::numeric_limits<float>::epsilon() ? axis / angle : vec3(0.0f), angle);
        }

        static quat_t axis_angle(const glm::tvec3<T>& axis, float angle) {
            quat_t q;
            axis_angle(q, axis, angle);
            return q;
        }

        static quat_t axis_angle(glm::tvec3<T> axis) {
            quat_t q;
            axis_angle(q, axis);
            return q;
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
            return (glm::tmat4x4<T>)(*this);
        }

        /*
        void transform(glm::tvec3<T>& vec) {
            vec = ((q * quat_t<T>(0.0f, v)) * q.conjugated()).v;
            vec = operator*(operator*(quat_t<T>(0.0f, v)), conjugated()).v;
        }

        void transform(glm::tmat3<T>& mat) {

        }
        */

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

    template <typename T>
    glm::tmat3x3<T> operator*(const quat_t<T>& q, const glm::tmat3x3<T>& m) {
        glm::tmat3x3<T> qm(q);
        //glm::tmat3x3 qm_conj(q.conjugated());
        return (qm * m) * transpose(qm);
    }

    // Specialized quaternion types
    //typedef quat_t<float> quat;
    using quat = quat_t<float>;
}
