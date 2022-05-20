#ifndef PGE_VECTOR_H_INCLUDED
#define PGE_VECTOR_H_INCLUDED

#include <PGE/String/String.h>
#include <PGE/Math/Math.h>

// If you modify one class here, apply your changes to the others as well!
namespace PGE {

class Vector2f {
    public:
        float x; float y;

        constexpr Vector2f() : x(0.f), y(0.f) { }
        constexpr explicit Vector2f(float s) : x(s), y(s) { }
        constexpr Vector2f(float ix, float iy) : x(ix), y(iy) { }

        constexpr bool operator==(const Vector2f& other) const = default;

        constexpr bool equals(const Vector2f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return Math::equalFloats(x, other.x, epsilon) && Math::equalFloats(y, other.y, epsilon);
        }

        constexpr Vector2f operator-() const { return Vector2f(-x, -y); }

        constexpr void operator+=(const Vector2f& other) { x += other.x; y += other.y; }
        constexpr void operator-=(const Vector2f& other) { x -= other.x; y -= other.y; }
        constexpr void operator+=(float f) { x += f; y += f; }
        constexpr void operator-=(float f) { x -= f; y -= f; }
        constexpr void operator*=(float f) { x *= f; y *= f; }
        constexpr void operator/=(float f) { x /= f; y /= f; }

        constexpr Vector2f operator+(const Vector2f& other) const { return Vector2f(x + other.x, y + other.y); }
        constexpr Vector2f operator-(const Vector2f& other) const { return Vector2f(x - other.x, y - other.y); }
        constexpr Vector2f operator+(float f) const { return Vector2f(x + f, y + f); }
        constexpr Vector2f operator-(float f) const { return Vector2f(x - f, y - f); }
        constexpr Vector2f operator*(float f) const { return Vector2f(x * f, y * f); }
        constexpr Vector2f operator/(float f) const { return Vector2f(x / f, y / f); }

        constexpr float dotProduct(const Vector2f& other) const { return x * other.x + y * other.y; }
        constexpr Vector2f entrywiseProduct(const Vector2f& other) const { return Vector2f(x * other.x, y * other.y); }

        constexpr Vector2f normalize() const {
            float lenSqr = lengthSquared();
            if (Math::equalFloats(0.f, lenSqr)) { return Vector2f(); }
            if (Math::equalFloats(1.f, lenSqr)) { }
            return *this / Math::sqrt(lenSqr);
        }

        constexpr Vector2f reflect(const Vector2f& normal) const {
            Vector2f reflectedVector = normalize();
            reflectedVector = normal * 2.f * reflectedVector.dotProduct(normal) - reflectedVector;
            return reflectedVector.normalize();
        }

        constexpr float lengthSquared() const { return x * x + y * y; }
        constexpr float length() const { return Math::sqrt(lengthSquared()); }

        constexpr float distanceSquared(const Vector2f& other) const { return (*this - other).lengthSquared(); }
        constexpr float distance(const Vector2f& other) const { return Math::sqrt(distanceSquared(other)); }
};
namespace Vector2fs {
    constexpr Vector2f ZERO(0.f);
    constexpr Vector2f ONE(1.f);
}
constexpr Vector2f operator*(float f, const Vector2f& vec) { return vec * f; }
static_assert(sizeof(Vector2f) == 2 * sizeof(float));

class Vector3f {
    public:
        float x; float y; float z;

        constexpr Vector3f() : x(0.f), y(0.f), z(0.f) { }
        constexpr explicit Vector3f(float s) : x(s), y(s), z(s) { }
        constexpr Vector3f(float ix, float iy, float iz) : x(ix), y(iy), z(iz) { }
        constexpr Vector3f(const Vector2f& v2, float iz) : x(v2.x), y(v2.y), z(iz) { }
        constexpr Vector3f(float ix, const Vector2f& v2) : x(ix), y(v2.x), z(v2.y) { }

        constexpr bool operator==(const Vector3f& other) const = default;
        
        constexpr bool equals(const Vector3f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return Math::equalFloats(x, other.x, epsilon) && Math::equalFloats(y, other.y, epsilon) && Math::equalFloats(z, other.z, epsilon);
        }

        constexpr Vector3f operator-() const { return Vector3f(-x, -y, -z); }

        constexpr void operator+=(const Vector3f& other) { x += other.x; y += other.y; z += other.z; }
        constexpr void operator-=(const Vector3f& other) { x -= other.x; y -= other.y; z -= other.z; }
        constexpr void operator+=(float f) { x += f; y += f; z += f; }
        constexpr void operator-=(float f) { x -= f; y -= f; z -= f; }
        constexpr void operator*=(float f) { x *= f; y *= f; z *= f; }
        constexpr void operator/=(float f) { x /= f; y /= f; z /= f; }

        constexpr Vector3f operator+(const Vector3f& other) const { return Vector3f(x + other.x, y + other.y, z + other.z); }
        constexpr Vector3f operator-(const Vector3f& other) const { return Vector3f(x - other.x, y - other.y, z - other.z); }
        constexpr Vector3f operator+(float f) const { return Vector3f(x + f, y + f, z + f); }
        constexpr Vector3f operator-(float f) const { return Vector3f(x - f, y - f, z - f); }
        constexpr Vector3f operator*(float f) const { return Vector3f(x * f, y * f, z * f); }
        constexpr Vector3f operator/(float f) const { return Vector3f(x / f, y / f, z / f); }

        constexpr float dotProduct(const Vector3f& other) const { return x * other.x + y * other.y + z * other.z; }
        constexpr Vector3f entrywiseProduct(const Vector3f& other) const { return Vector3f(x * other.x, y * other.y, z * other.z); }
        constexpr Vector3f crossProduct(const Vector3f& other) const { return Vector3f(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }

        constexpr Vector3f normalize() const {
            float lenSqr = lengthSquared();
            if (Math::equalFloats(0.f, lenSqr)) { return Vector3f(); }
            if (Math::equalFloats(1.f, lenSqr)) { return *this; }
            return *this / Math::sqrt(lenSqr);
        }

        constexpr Vector3f reflect(const Vector3f& normal) const {
            Vector3f reflectedVector = normalize();
            reflectedVector = normal * 2.f * reflectedVector.dotProduct(normal) - reflectedVector;
            return reflectedVector.normalize();
        }

        constexpr float lengthSquared() const { return x * x + y * y + z * z; }
        constexpr float length() const { return Math::sqrt(lengthSquared()); }

        constexpr float distanceSquared(const Vector3f& other) const { return (*this - other).lengthSquared(); }
        constexpr float distance(const Vector3f& other) const { return Math::sqrt(distanceSquared(other)); }
};
namespace Vector3fs {
    constexpr Vector3f ZERO(0.f);
    constexpr Vector3f ONE(1.f);
}
constexpr Vector3f operator*(float f, const Vector3f& vec) { return vec * f; }
static_assert(sizeof(Vector3f) == 3 * sizeof(float));

class Vector4f {
    public:
        float x; float y; float z; float w;

        constexpr Vector4f() : x(0.f), y(0.f), z(0.f), w(0.f) { }
        constexpr explicit Vector4f(float s) : x(s), y(s), z(s), w(s) { }
        constexpr Vector4f(float ix,float iy,float iz,float iw) : x(ix), y(iy), z(iz), w(iw) { }
        constexpr Vector4f(const Vector3f& v3, float iw) : x(v3.x), y(v3.y), z(v3.z), w(iw) { }
        constexpr Vector4f(float ix, const Vector3f& v3) : x(ix), y(v3.x), z(v3.y), w(v3.z) { }
        constexpr Vector4f(const Vector2f& v2a, const Vector2f& v2b) : x(v2a.x), y(v2a.y), z(v2b.x), w(v2b.y) { }
        constexpr Vector4f(float ix, float iy, const Vector2f& v2a) : x(ix), y(iy), z(v2a.x), w(v2a.y) { }
        constexpr Vector4f(const Vector2f& v2a, float iz, float iw) : x(v2a.x), y(v2a.y), z(iz), w(iw) { }
        constexpr Vector4f(float ix, const Vector2f& v2a, float iw) : x(ix), y(v2a.x), z(v2a.y), w(iw) { }

        constexpr bool operator==(const Vector4f& other) const = default;

        constexpr bool equals(const Vector4f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return Math::equalFloats(x, other.x, epsilon) && Math::equalFloats(y, other.y, epsilon) && Math::equalFloats(z, other.z, epsilon) && Math::equalFloats(w, other.w, epsilon);
        }

        constexpr Vector4f operator-() const { return Vector4f(-x, -y, -z, -w); }

        constexpr void operator+=(const Vector4f& other) { x += other.x; y += other.y; z += other.z; w += other.w; }
        constexpr void operator-=(const Vector4f& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; }
        constexpr void operator+=(float f) { x += f; y += f; z += f; w += f; }
        constexpr void operator-=(float f) { x -= f; y -= f; z -= f; w -= f; }
        constexpr void operator*=(float f) { x *= f; y *= f; z *= f; w *= f; }
        constexpr void operator/=(float f) { x /= f; y /= f; z /= f; w /= f; }

        constexpr Vector4f operator+(const Vector4f& other) const { return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w); }
        constexpr Vector4f operator-(const Vector4f& other) const { return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w); }
        constexpr Vector4f operator+(float f) const { return Vector4f(x + f, y + f, z + f, w + f); }
        constexpr Vector4f operator-(float f) const { return Vector4f(x - f, y - f, z - f, w - f); }
        constexpr Vector4f operator*(float f) const { return Vector4f(x * f, y * f, z * f, w * f); }
        constexpr Vector4f operator/(float f) const { return Vector4f(x / f, y / f, z / f, w / f); }

        constexpr float dotProduct(const Vector4f& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }
        constexpr Vector4f entrywiseProduct(const Vector4f& other) const { return Vector4f(x * other.x, y * other.y, z * other.z, w * other.w); }

        constexpr Vector4f normalize() const {
            float lenSqr = lengthSquared();
            if (Math::equalFloats(0.f, lenSqr)) { return Vector4f(); }
            if (Math::equalFloats(1.f, lenSqr)) { }
            return *this / Math::sqrt(lenSqr);
        }

        constexpr Vector4f reflect(const Vector4f& normal) const {
            Vector4f reflectedVector = normalize();
            reflectedVector = normal * 2.f * reflectedVector.dotProduct(normal) - reflectedVector;
            return reflectedVector.normalize();
        }

        constexpr float lengthSquared() const { return x * x + y * y + z * z + w * w; }
        constexpr float length() const { return Math::sqrt(lengthSquared()); }

        constexpr float distanceSquared(const Vector4f& other) const { return (*this - other).lengthSquared(); }
        constexpr float distance(const Vector4f& other) const { return Math::sqrt(distanceSquared(other)); }
};
namespace Vector4fs {
    constexpr Vector4f ZERO(0.f);
    constexpr Vector4f ONE(1.f);
}
constexpr Vector4f operator*(float f, const Vector4f& vec) { return vec * f; }
static_assert(sizeof(Vector4f) == 4 * sizeof(float));

class Vector2i {
    public:
        i32 x; i32 y;

        constexpr Vector2i() : x(0), y(0) { }
        constexpr explicit Vector2i(i32 s) : x(s), y(s) { }
        constexpr Vector2i(i32 ix, i32 iy) : x(ix), y(iy) { }
        constexpr explicit Vector2i(const Vector2f& vf) : x((i32)vf.x), y((i32)vf.y) { }

        constexpr operator Vector2f() const { return Vector2f((float)x, (float)y); }

        constexpr bool operator==(const Vector2i& other) const = default;

        constexpr Vector2i operator-() const { return Vector2i(-x, -y); }

        constexpr void operator+=(const Vector2i& other) { x += other.x; y += other.y; }
        constexpr void operator-=(const Vector2i& other) { x -= other.x; y -= other.y; }
        constexpr void operator+=(i32 i) { x += i; y += i; }
        constexpr void operator-=(i32 i) { x -= i; y -= i; }
        constexpr void operator*=(i32 i) { x *= i; y *= i; }

        constexpr Vector2i operator+(const Vector2i& other) const { return Vector2i(x + other.x, y + other.y); }
        constexpr Vector2i operator-(const Vector2i& other) const { return Vector2i(x - other.x, y - other.y); }
        constexpr Vector2i operator+(i32 i) const { return Vector2i(x + i, y + i); }
        constexpr Vector2i operator-(i32 i) const { return Vector2i(x - i, y - i); }
        constexpr Vector2i operator*(i32 i) const { return Vector2i(x * i, y * i); }

        constexpr i32 dotProduct(const Vector2i& other) { return x * other.x + y * other.y; }
        constexpr Vector2i entrywiseProduct(const Vector2i& other) const { return Vector2i(x * other.x, y * other.y); }

        constexpr i32 lengthSquared() const { return x * x + y * y; }
        // Sacrificing vendor specific Math::sqrt optimizations are not worth the constexpr here.
        // Fuck you, C++ standard!
        // TODO: C++23 if consteval
        constexpr float length() const { return Math::sqrt((float)lengthSquared()); }

        constexpr i32 distanceSquared(const Vector2i& other) const { return ((*this) - other).lengthSquared(); }
        constexpr float distance(const Vector2i& other) const { return Math::sqrt((float)distanceSquared(other)); }
};
namespace Vector2is {
    constexpr Vector2i ZERO(0);
    constexpr Vector2i ONE(1);
}
constexpr Vector2i operator*(i32 i, const Vector2i& vec) { return vec * i; }
static_assert(sizeof(Vector2i) == 2 * sizeof(i32));

}

#endif // PGE_VECTOR_H_INCLUDED
