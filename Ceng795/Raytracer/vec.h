#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>



struct Vec3i
{
    int x, y, z;
};

struct Vec4f
{
    float x, y, z, w;
};

struct Vec4f1i
{
    float x, y, z, w;
    int i;
};

class vec3{
public:
  float x,y,z;
  vec3() {}
  vec3(float x, float y, float z) : x(x), y(y), z(z){}

  inline const vec3& operator+() const {return *this; }
  inline vec3 operator-() const {return vec3(-x,-y,-z);}

  inline vec3& operator +=(const vec3 &v2);
  inline vec3& operator -=(const vec3 &v2);
  inline vec3& operator *=(const vec3 &v2);
  inline vec3& operator /=(const vec3 &v2);
  inline vec3& operator *=(const float t);
  inline vec3& operator /=(const float t);

  inline float length() const {
    return sqrt(x*x + y*y + z*z);
  }
  inline float squared_length() const {
    return x*x + y*y + z*z;
  }
  inline void make_unit_vector();

};

inline std::istream& operator>>(std::istream &is, vec3 &t) {
  is >> t.x >> t.y >> t.z ;
  return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
  os << t.x << " " << t.y << " " << t.z ;
  return os;
}

inline void vec3::make_unit_vector() {
  float k = 1.0 / sqrt(x*x + y*y + z*z) ;
  x *= k ; y *= k; z *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline vec3 operator*(float t, const vec3 &v) {
    return vec3(t*v.x, t*v.y, t*v.z);
}

inline vec3 operator/(vec3 v, float t) {
    return vec3(v.x/t, v.y/t, v.z/t);
}

inline vec3 operator*(const vec3 &v, float t) {
    return vec3(t*v.x, t*v.y, t*v.z);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
    return v1.x *v2.x + v1.y *v2.y  + v1.z *v2.z;
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3( (v1.y*v2.z - v1.z*v2.y),
                (-(v1.x*v2.z - v1.z*v2.x)),
                (v1.x*v2.y - v1.y*v2.x));
}

inline vec3& vec3::operator+=(const vec3 &v){
    x  += v.x;
    y  += v.y;
    z  += v.z;
    return *this;
}

inline vec3& vec3::operator*=(const vec3 &v){
    x  *= v.x;
    y  *= v.y;
    z  *= v.z;
    return *this;
}

inline vec3& vec3::operator/=(const vec3 &v){
    x  /= v.x;
    y  /= v.y;
    z  /= v.z;
    return *this;
}

inline vec3& vec3::operator-=(const vec3& v) {
    x  -= v.x;
    y  -= v.y;
    z  -= v.z;
    return *this;
}

inline vec3& vec3::operator*=(const float t) {
    x  *= t;
    y  *= t;
    z  *= t;
    return *this;
}

inline vec3& vec3::operator/=(const float t) {
    float k = 1.0/t;

    x  *= k;
    y  *= k;
    z  *= k;
    return *this;
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}



#endif
