#pragma once

namespace math
{
    struct Vec2
    {
        float x;
        float y;

        constexpr Vec2() : x(0.0f), y(0.0f){}
        constexpr Vec2(float v) : x(v), y(v) {}
        constexpr Vec2(float u, float v) : x(u), y(v){}
    };

    // Vectors
    constexpr Vec2 operator+(const Vec2& u, const Vec2& v);
    constexpr Vec2 operator-(const Vec2& u, const Vec2& v);
    constexpr Vec2 operator*(const Vec2& u, const Vec2& v);
    constexpr Vec2 operator/(const Vec2& u, const Vec2& v);

    // Scalars
    constexpr Vec2 operator*(const Vec2& u, const float v);
    constexpr Vec2 operator*(const float v, const Vec2& u);
    constexpr Vec2 operator+(const Vec2& u, const float v);
    constexpr Vec2 operator+(const float v, const Vec2& u);

    // Operations
    constexpr float dot(const Vec2& u, const Vec2& v);
    constexpr float normalize(const Vec2& u);
    constexpr float length(const Vec2& u);
    constexpr float lengthSq(const Vec2& u, const Vec2& v);
}
