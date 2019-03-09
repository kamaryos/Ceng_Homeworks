#ifndef RAYH
#define RAYH
#include "vec.h"

struct Scene;

class ray{
public:
  ray() {}
  ray(const vec3& a, const vec3& b) { A = a; B = b; }
  vec3 origin() const {return A; }
  vec3 direction() const {return B; }
  vec3 point_at_parameter(float t) const {return A + t*B; }

  vec3 A;
  vec3 B;
  bool inObject = false;

  static vec3 generate_ray(const ray& r, const Scene& scene, int max_recursion_depth);
  static bool refraction(const ray& r_in, const vec3& normal, float refraction_index, vec3& r_direction, float r_o);
};



#endif
