#pragma once
#include "defines.hpp"
#include <cmath>

// ----------------- VECTORS ----------------- //

// Generic
template <int n>
struct Vector
{
    float data[n];
    constexpr float &operator[](int i)
    {
        return data[i];
    }

    constexpr float operator[](int i) const
    {
        return data[i];
    }
};

template <int n>
constexpr Vector<n> operator- (const Vector<n> &v)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = -v[i];
    return vec;
}

template <int n>
constexpr Vector<n> operator+(const Vector<n> &v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] + v2[i];
    return vec;
}


template <int n>
constexpr Vector<n> operator-(const Vector<n> &v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] - v2[i];
    return vec;
}


template <int n>
constexpr Vector<n> operator*(const Vector<n> &v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] * v2[i];
    return vec;
}
template <int n>
constexpr Vector<n> operator/(const Vector<n> &v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] / v2[i];
    return vec;
}
template <int n>
constexpr Vector<n> operator*(const Vector<n> &v1, const float v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] * v2;
    return vec;
}
template <int n>
constexpr Vector<n> operator*(const float v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v2[i] * v1;
    return vec;
}
template <int n>
constexpr Vector<n> operator/(const Vector<n> &v1, const float v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] / v2;
    return vec;
}
template <int n>
constexpr Vector<n> operator/(const float v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1 / v2[i];
    return vec;
}

template <int n>
constexpr Vector<n> operator+(const Vector<n> &v1, const float v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] + v2;
    return vec;
}
template <int n>
constexpr Vector<n> operator+(const float v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1 + v2[i];
    return vec;
}

template <int n>
constexpr Vector<n> operator-(const Vector<n> &v1, const float v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1[i] - v2;
    return vec;
}
template <int n>
constexpr Vector<n> operator-(const float v1, const Vector<n> &v2)
{
    Vector<n> vec;
    for (int i = 0; i < n; ++i)
        vec[i] = v1 - v2[i];
    return vec;
}

template <int n>
constexpr float dot(const Vector<n> &v1, const Vector<n> &v2)
{
    float res = 0; 
    for (int i = 0; i < n; ++i)
        res += v1[i] * v2[i];
    return res;
}

template <int n>
constexpr float lengthSq(const Vector<n> &v1)
{
    float d = 0; 
    for (int i = 0; i < n; ++i)
        d += v1[i] * v1[i];
    return d;
}

template <int n>
constexpr float length(const Vector<n> &v1)
{
    float d = 0; 
    for (int i = 0; i < n; ++i)
        d += v1[i] * v1[i];
    return sqrt(d);
}

template <int n>
constexpr float distanceSq(const Vector<n> &v1, const Vector<n> &v2)
{
    return lengthSq(v2 - v1);
}

template <int n>
constexpr float distance(const Vector<n> &v1, const Vector<n> &v2)
{
    return length(v2 - v1);
}

template <int n>
constexpr Vector<n> normalize(const Vector<n> &v)
{
    return v / length(v);
}

// Common type
typedef Vector<2> vec2;
typedef Vector<3> vec3;
typedef Vector<4> vec4;

// Template specialization

// vec2
template <>
struct Vector<2>
{
    union
    {
        float data[2];
        struct
        {
            float x, y;
        };
        struct
        {
            float u, v;
        };
    };
    constexpr float &operator[](int i)
    {
        return data[i];
    }
    constexpr float operator[](int i) const
    {
        return data[i];
    }
    Vector<2>(float a, float b)
    {
        x = a;
        y = b;
    }
    Vector<2>(float a)
    {
        x = a;
        y = a;
    }
    Vector<2>()
    {
        x = 0;
        y = 0;
    }
};

// vec3
template <>
struct Vector<3>
{
    union
    {
        float data[3];
        struct
        {
            float x, y, z;
        };
        struct
        {
            float r, g, b;
        };
        Vector<2> xy;
        Vector<2> rg;
    };
    constexpr float &operator[](int i)
    {
        return data[i];
    }
    constexpr float operator[](int i) const
    {
        return data[i];
    }
    Vector<3>(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }
    Vector<3>(float a)
    {
        x = a;
        y = a;
        z = a;
    }
    Vector<3>()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    Vector<3>(const Vector<2> &vec, float v)
    {
        x = vec.x;
        y = vec.y;
        z = v;
    }
    
};

// vec4
template <>
struct Vector<4>
{
    union
    {
        float data[4];
        struct
        {
            float x, y, z, w;
        };
        struct
        {
            float r, g, b, a;
        };
        Vector<2> xy;
        Vector<3> xyz;
        Vector<2> rg;
        Vector<3> rgb;
    };
    constexpr float &operator[](int i)
    {
        return data[i];
    }
    constexpr float operator[](int i) const
    {
        return data[i];
    }
    Vector<4>(float a, float b, float c, float d)
    {
        x = a;
        y = b;
        z = c;
        w = d;
    }
    Vector<4>(float a)
    {
        x = a;
        y = a;
        z = a;
        w = a;
    }
    Vector<4>()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }
    Vector<4>(const Vector<2> &vec, float v1, float v2)
    {
        x = vec.x;
        y = vec.y;
        z = v1;
        w = v2;
    }
    Vector<4>(const Vector<3> &vec, float v)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        w = v;
    }
};

// ----------------- MATRIX ----------------- //

// Generic
template <int n, int m>
struct Matrix
{
    float data[n * m];
    constexpr float &operator[](const std::pair<int, int> &index)
    {
        return data[index.first * n + index.second];
    }

    constexpr float operator[](const std::pair<int, int> &index) const
    {
        return data[index.first * n + index.second];
    }

    constexpr float &operator[](const int index)
    {
        return data[index];
    }

    constexpr float operator[](const int index) const
    {
        return data[index];
    }

    operator std::string() const
    {
        string s = "[\n";
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
                s += std::to_string(data[j * n + i]) + "  ";
            s += "\n";
        }
        s += "]\n";
        return s;
    }
};

template <int n, int m, int p>
constexpr Matrix<p, m> operator*(const Matrix<n, m> &m1, const Matrix<p, n> &m2)
{
    Matrix<p, n> mat;
    for (int c = 0; c < p; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = 0;
            for (int i = 0; i < n; i++)
                mat[{c, l}] += m1[{i, l}] * m2[{c, i}];
        }
    }
    return mat;
}

template <int n, int m>
constexpr Vector<m> operator*(const Matrix<n, m> &m1, const Vector<n> &v)
{
    Vector<m> vec;
    for (int i = 0; i < m; i++)
    {
        vec[i] = 0;
        for (int j = 0; j < n; j++)
            vec[i] += v[j] * m1[{j, i}];
    }
    return vec;
}

template <int n, int m>
constexpr Matrix<n, m> operator+(const Matrix<n, m> &m1, const Matrix<n, m> &m2)
{
    Matrix<n, m> mat;
    for (int c = 0; c < n; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = m1[{c, l}] + m2[{c, l}];
        }
    }
    return mat;
}

template <int n, int m>
constexpr Matrix<n, m> operator-(const Matrix<n, m> &m1, const Matrix<n, m> &m2)
{
    Matrix<n, m> mat;
    for (int c = 0; c < n; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = m1[{c, l}] - m2[{c, l}];
        }
    }
    return mat;
}

template <int n, int m>
constexpr Matrix<n, m> operator*(const Matrix<n, m> &m1, const float v)
{
    Matrix<n, m> mat;
    for (int c = 0; c < n; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = v * m1[{c, l}];
        }
    }
    return mat;
}

template <int n, int m>
constexpr Matrix<n, m> operator*(const float v, const Matrix<n, m> &m1)
{
    Matrix<n, m> mat;
    for (int c = 0; c < n; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = v * m1[{c, l}];
            ;
        }
    }
    return mat;
}

template <int n, int m>
constexpr Matrix<n, m> operator/(const Matrix<n, m> &m1, const float v)
{
    Matrix<n, m> mat;
    for (int c = 0; c < n; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = m1[{c, l}] / v;
        }
    }
    return mat;
}

template <int n, int m>
constexpr Matrix<n, m> operator/(const float v, const Matrix<n, m> &m1)
{
    Matrix<n, m> mat;
    for (int c = 0; c < n; c++)
    {
        for (int l = 0; l < m; l++)
        {
            mat[{c, l}] = v / m1[{c, l}];
            ;
        }
    }
    return mat;
}

template <int n>
constexpr Matrix<n, n> identity()
{
    Matrix<n, n> mat;
    for (int i = 0; i < n; i++)
        mat[{i, i}] = 1;
    return mat;
}

template <int n>
constexpr Matrix<n, n> translate(const Matrix<n, n> &m, const Vector<n - 1> &v)
{
    Matrix<n, n> mat = m;
    for (int i = 0; i < n - 1; i++)
        mat[{n - 1, i}] += v[i];
    return mat;
}

template <int n>
constexpr Matrix<n, n> scale(const Matrix<n, n> &m, const Vector<n - 1> &v)
{
    Matrix<n, n> mat = m;
    for (int i = 0; i < n - 1; i++)
        mat[{i, i}] *= v[i];
    return mat;
}

template <int n, int m>
constexpr Matrix<n, n> extract(const Matrix<m, m> &m1)
{
    Matrix<n, n> mat;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[{i, j}] = m1[{i, j}];
    return mat;
}

// Common type
typedef Matrix<3, 3> mat3;
typedef Matrix<4, 4> mat4;

// Template specialization

// mat3
template <>
struct Matrix<3, 3>
{

    float data[9];

    constexpr float &operator[](const std::pair<int, int> &index)
    {
        return data[index.first * 3 + index.second];
    }

    constexpr float operator[](const std::pair<int, int> &index) const
    {
        return data[index.first * 3 + index.second];
    }
    constexpr float &operator[](const int index)
    {
        return data[index];
    }

    constexpr float operator[](const int index) const
    {
        return data[index];
    }

    operator std::string() const
    {
        string s = "[\n";
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
                s += std::to_string(data[j * 3 + i]) + "  ";
            s += "\n";
        }
        s += "]\n";
        return s;
    }

    Matrix<3, 3>(const Vector<3> &c1, const Vector<3> &c2, const Vector<3> &c3)
    {
        data[0] = c1.x;
        data[1] = c1.y;
        data[2] = c1.z;
        data[3] = c2.x;
        data[4] = c2.y;
        data[5] = c2.z;
        data[6] = c3.x;
        data[7] = c3.y;
        data[8] = c3.z;
    }

    Matrix<3, 3>(const Matrix<2, 2> &m)
    {
        data[0] = m[{0, 0}];
        data[1] = m[{0, 1}];
        data[2] = 0;
        data[3] = m[{1, 0}];
        data[4] = m[{1, 1}];
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = 0;
    }

    Matrix<3, 3>()
    {
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;

        data[6] = 0;
        data[7] = 0;
        data[8] = 0;
    }
};

// mat4
template <>
struct Matrix<4, 4>
{
    union
    {
        float data[16];
    };

    constexpr float &operator[](const std::pair<int, int> &index)
    {
        return data[index.first * 4 + index.second];
    }

    constexpr float operator[](const std::pair<int, int> &index) const
    {
        return data[index.first * 4 + index.second];
    }
    constexpr float &operator[](const int index)
    {
        return data[index];
    }

    constexpr float operator[](const int index) const
    {
        return data[index];
    }

    operator std::string() const
    {
        string s = "[\n";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
                s += std::to_string(data[j * 4 + i]) + "  ";
            s += "\n";
        }
        s += "]\n";
        return s;
    }

    Matrix<4, 4>(const Vector<4> &c1, const Vector<4> &c2, const Vector<4> &c3, const Vector<4> &c4)
    {
        data[0] = c1.x;
        data[1] = c1.y;
        data[2] = c1.z;
        data[3] = c1.w;
        data[4] = c2.x;
        data[5] = c2.y;
        data[6] = c2.z;
        data[7] = c2.w;
        data[8] = c3.x;
        data[9] = c3.y;
        data[10] = c3.z;
        data[11] = c3.w;
        data[12] = c4.x;
        data[13] = c4.y;
        data[14] = c4.z;
        data[15] = c4.w;
    }

    Matrix<4, 4>(const Matrix<3, 3> &m)
    {
        data[0] = m[{0, 0}];
        data[1] = m[{0, 1}];
        data[2] = m[{0, 2}];
        data[3] = 0;
        data[4] = m[{1, 0}];
        data[5] = m[{1, 1}];
        data[6] = m[{1, 2}];
        data[7] = 0;
        data[8] = m[{2, 0}];
        data[9] = m[{2, 1}];
        data[10] = m[{2, 2}];
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = 0;
        data[15] = 0;
    }

    Matrix<4, 4>()
    {
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        data[8] = 0;
        data[9] = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = 0;
        data[15] = 0;
    }
};
