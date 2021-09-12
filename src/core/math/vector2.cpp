#include "vector2.hpp"

namespace math
{
    // Vectors
    constexpr Vec2 operator+(const Vec2& u, const Vec2& v){
        return Vec2(u.x+v.x, u.y+v.y);
    }
    constexpr Vec2 operator-(const Vec2& u, const Vec2& v){
        return Vec2(u.x-v.x, u.y-v.y);
    }
    constexpr Vec2 operator*(const Vec2& u, const Vec2& v){
        return Vec2(u.x*v.x, u.y*v.y);
    }
    constexpr Vec2 operator/(const Vec2& u, const Vec2& v){
        return Vec2(u.x/v.x, u.y/v.y);
    }

    // Scalars
    constexpr Vec2 operator*(const Vec2& u, const float v){
        return Vec2(u.x*v, u.y*v);
    }
    constexpr Vec2 operator*(const float v, const Vec2& u){
        return Vec2(v*u.x, v*u.y);
    }
    constexpr Vec2 operator+(const Vec2& u, const float v){
        return Vec2(u.x+v, u.y+v);
    }
    constexpr Vec2 operator+(const float v, const Vec2& u){
        return Vec2(v+u.x, v+u.y);
    }
    constexpr Vec2 operator-(const Vec2& u, const float v){
        return Vec2(u.x-v, u.y-v);
    }
    constexpr Vec2 operator-(const float v, const Vec2& u){
        return Vec2(v-u.x, v-u.y);
    }
    constexpr Vec2 operator/(const Vec2& u, const float v){
        return Vec2(u.x/v, u.y/v);
    }
    constexpr Vec2 operator/(const float v, const Vec2& u){
        return Vec2(v/u.x, v/u.y);
    }

    // Operation
    
}