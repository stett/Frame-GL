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
        ~quat_t() {}

    public:
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

        quat_t operator*(const quat_t& other) {
            return quat_t(s * other.s, glm::dot(v, other.v));
        }

        quat_t& operator*=(const quat_t& other) {
            T s_new = (s * other.s) + glm::dot(v, other.v);
            // NOTE: I think one of these scalar/vector products will need to be re-ordered...
            vtype v_new = (s * other.v) + (v * other.s) + glm::cross(v, other.v);
            return *this;
        }

        quat_t& operator*=(float c) {
            s *= c;
            v *= c;
            return *this;
        }

    public:
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

        /*
        glm::tvec3<T> euler() const {
            return glm::tvec3<T>(
                atan2(2.0f*(s*v.x + v.y*v.z), 1.0f - 2.0f*(v.x*v.x + v.y*v.y)),
                asin(2.0f*(s*v.y - v.x*v.z)),
                atan2(2.0f*(s*v.z + v.x*v.y), 1.0f - 2.0f*(v.y*v.y + v.z*v.z)));
        }
        */

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
        return (a.s * b.s) + glm::dot(a.v, b.v);
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
    //T angle_diff(const quat_t<T>& a, const quat_t<T>& b) {
	T angle_diff(quat_t<T> a, quat_t<T> b) {
		//T d = operator*(a.conjugate(), b).s;
        //return (T)2.0 * (T)acos(operator*(a.conjugate(), b).s);
		a = a.normalized();
		b = b.normalized();
		T d = dot(a, b);
        return (T)acos(dot(a, b));
    }

    // Specialized quaternion types
    typedef quat_t<float> quat;
}
