#ifndef RAYH
#define RAYH
#include "vec.h"

struct Scene;
class Object;

class ray{
public:
  ray() {}
  ray(const vec3& a, const vec3& b) { A = a; B = b; }
  ray(const vec3& a, const vec3& b, bool is_shadow) { A = a; B = b; is_shadow = is_shadow; }
  vec3 origin() const {return A; }
  vec3 direction() const {return B; }
  vec3 point_at_parameter(float t) const {return A + t*B; }

  vec3 A;
  vec3 B;

  bool is_shadow;

  // static vec3 generate_ray(const ray& r, const Scene& scene, int max_recursion_depth);
  // static bool refraction(const ray& r_in, const vec3& normal, float refraction_index, vec3& refracted, float r_o);
};

struct hit_record {
  int material_id;
  float t;
  vec3 normal;
  const Object* obj;
};




#endif
