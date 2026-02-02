#ifndef _GAME_MATHS_H_
#define _GAME_MATHS_H_

#include <cmath>
#include <string>
#include <sstream>
#include "../consts.h"

template <typename T>
union Vec2
{
    T values[2];
    struct { T x; T y; };
    struct { T w; T h; };
    Vec2() : x{0}, y{0} {}
    Vec2(T v) : x{v}, y{v} {}
    Vec2(T x, T y) : x{x}, y{y} {}
    Vec2(const Vec2<T>& other) : x{other.x}, y{other.y} {}
    Vec2(Vec2<T>&& other) : x{other.x}, y{other.y} {}
    Vec2& operator=(const Vec2<T>& other)
    {
        if (this==&other)
            return *this;
        x = other.x;
        y = other.y;
        return *this;
    }
    std::string toString() const
    {
        std::stringstream ss;
        ss << "{" << x << ", " << y << "}";
        return ss.str();
    }
    T operator[](size_t i){ return values[i]; }
    bool operator==(const Vec2<T>& other){ return x==other.x&&y==other.y; }
    Vec2<T>& operator+=(const Vec2<T>& other)
    {
        x+=other.x;
        y+=other.y;
        return *this;
    }
    Vec2<T>& operator-=(const Vec2<T>& other)
    {
        x-=other.x;
        y-=other.y;
        return *this;
    }
    Vec2<T>& operator*=(const Vec2<T>& other)
    {
        x*=other.x;
        y*=other.y;
        return *this;
    }
    Vec2<T>& operator/=(const Vec2<T>& other)
    {
        x/=other.x;
        y/=other.y;
        return *this;
    }
    Vec2<T>& operator%=(const Vec2<T>& other)
    {
        x%=other.x;
        y%=other.y;
        return *this;
    }
    Vec2<T>& operator+=(float scalar)
    {
        x+=scalar;
        y+=scalar;
        return *this;
    }
    Vec2<T>& operator-=(float scalar)
    {
        x-=scalar;
        y-=scalar;
        return *this;
    }
    Vec2<T>& operator*=(float scalar)
    {
        x*=scalar;
        y*=scalar;
        return *this;
    }
    Vec2<T>& operator/=(float scalar)
    {
        x/=scalar;
        y/=scalar;
        return *this;
    }
    Vec2<T>& operator%=(float scalar)
    {
        x%=scalar;
        y%=scalar;
        return *this;
    }
    
};

template <typename T>
Vec2<T> operator+(const Vec2<T>& a, const Vec2<T>& b){ return {a.x+b.x, a.y+b.y}; }

template <typename T>
Vec2<T> operator-(const Vec2<T>& a, const Vec2<T>& b){ return {a.x-b.x, a.y-b.y}; }

template <typename T>
Vec2<T> operator*(const Vec2<T>& a, const Vec2<T>& b){ return {a.x*b.x, a.y*b.y}; }

template <typename T>
Vec2<T> operator/(const Vec2<T>& a, const Vec2<T>& b){ return {a.x/b.x, a.y/b.y}; }

template <typename T>
Vec2<T> operator%(const Vec2<T>& a, const Vec2<T>& b){ return {a.x%b.x, a.y%b.y}; }

template <typename T>
bool operator==(const Vec2<T>& a, const Vec2<T>& b){ return a.x==b.x&&a.y==b.y; }

using IVec2 = Vec2<int>;
using FVec2 = Vec2<float>;

template <typename T>
union Rect
{
    T values[4];
    struct { T x; T y; T w; T h; };
    Rect() : x{0}, y{0}, w{0}, h{0} {}
    Rect(T x, T y, T w, T h) : x{x}, y{y}, w{w}, h{h} {}
    Rect(const Rect<T>& other) : x{other.x}, y{other.y}, w{other.w}, h{other.h} {}
    Rect(Rect<T>&& other) : x{other.x}, y{other.y}, w{other.w}, h{other.h} {}
    Rect& operator=(const Rect& other)
    {
        if (this==&other)
            return *this;
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        return *this;
    }
    std::string toString() const
    {
        std::stringstream ss;
        ss << "{" << x << ", " << y << ", " << w << ", " << h << "}";
        return ss.str();
    }
    T operator[](size_t i){ return values[i]; }
    bool operator==(const Rect<T>& other){ return x==other.x&&y==other.y&&w==other.w&&h==other.h; }
};

using IRect = Rect<int>;
using FRect = Rect<float>;

template <typename T>
inline bool intersection(const Rect<T>& a, const Rect<T>& b)
{
    return !(b.x>a.x+a.w||
        b.x+b.w<a.x||
        b.y>a.y+a.h||
        b.y+b.h<a.y);
}

template <typename T, typename U>
inline void lerpRect(Rect<T>& rect, const Vec2<U>& vel, float interpolation)
{
    rect.x = std::lerp(rect.x, rect.x+vel.x*STEP_TIME, interpolation);
    rect.y = std::lerp(rect.y, rect.y+vel.x*STEP_TIME, interpolation);
}

#endif
