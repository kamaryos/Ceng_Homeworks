#include "shapes.h"
#include "ray.h"
#include "bvh.h"



float Determinant(vec3 a, vec3 b, vec3 c) {
  // vectors are columns
  return a.x * (b.y * c.z - c.y * b.z) + a.y * (c.x * b.z - b.x * c.z) +
         a.z * (b.x * c.y - c.x * b.y);
};


hit_record Face::GetIntersection(const ray& r) const {
  hit_record hit_record;
  hit_record.t = kInf;
  hit_record.material_id = -1;

  if (!r.is_shadow && dot(r.direction(), normal) > .0) {
        return hit_record;
  }

  float x = r.direction().x;
  float y = r.direction().y;
  float z = r.direction().z;


  vec3 p1 = v0_vector;
  vec3 p2 = v1_vector;
  vec3 p3 = v2_vector;
  float one = (p1.x-p2.x);
  float ten = (p1.z-p3.z);
  float eleven = (p1.y-p3.y);
  float twelve = (p1.z-r.origin().z);
  float thirteen = (p1.z-p2.z);
  float two = (eleven*z-ten*y);
  float three = (p1.x-p3.x);
  float nine = (p1.y-p2.y);
  float four = (nine*z-thirteen*y);
  float five = (nine*ten-eleven*thirteen);
  float six = (p1.x-r.origin().x);
  float eight = (p1.y-r.origin().y);
  float seven = (eight*z-twelve*y);



  float det_A_inv = 1.0f / ((one*two)-(three*four)+(x*five));

  float beta = ((six*two)-(three*seven)+(x*(eight*ten-eleven*twelve)))* det_A_inv;


  if(beta < 0.f || beta > 1.0f )
  {
   return hit_record;
  }

  float gamma = ((one*seven)-(six*four)+(x*(nine*twelve-eight*thirteen))) * det_A_inv;

  if( gamma < 0.f || (beta+gamma > 1.0f))
  {
    return hit_record;
  }

  float t = ((one*(eleven*twelve-ten*eight))
              -(three*(nine*twelve-thirteen*eight))
              +(six*five))* det_A_inv;

  hit_record.t = t;
  hit_record.normal = normal;
  hit_record.material_id = material_id;
  hit_record.obj = this;
  return hit_record;
};



hit_record Sphere::GetIntersection(const ray& r) const {
  hit_record hit_record;
  hit_record.material_id = -1;
  hit_record.t = std::numeric_limits<float>::infinity();

  vec3 oc = (r.origin() - center);

  float a = dot(r.direction(),r.direction());
  float b = 2*dot(r.direction(),(r.origin() - center));
  float c = dot(oc,oc) - (radius * radius);

  float delta = (b*b) - (4*a*c);

  if(delta < 0){
      return hit_record;
  }

  else{
      float t1 = ((-1)*b + sqrt(delta))/(2*a);
      float t2 = ((-1)*b - sqrt(delta))/(2*a);

      hit_record.t = fmin(t1, t2);
      hit_record.material_id = material_id;
      hit_record.normal = getnormal(hit_record.t, r);
      hit_record.obj = this;

      return hit_record;
  }
};

void Sphere::Initialize() {
  const vec3 rad_vec = vec3(radius, radius, radius);
  const vec3 min_c = center - rad_vec;
  const vec3 max_c = center + rad_vec;
  bounding_box.min_corner = min_c;
  bounding_box.max_corner = max_c;
};

void Face::CalculateNormal() {

      ba = v0_vector - v1_vector;
      ca = v0_vector - v2_vector;

      normal = unit_vector(cross(ba,ca));

      vec3 min_c = v0_vector;
      vec3 max_c = v0_vector;

      min_c.x = fmin(min_c.x, v1_vector.x);
      min_c.y = fmin(min_c.y, v1_vector.y);
      min_c.z = fmin(min_c.z, v1_vector.z);

      max_c.x = fmax(max_c.x, v1_vector.x);
      max_c.y = fmax(max_c.y, v1_vector.y);
      max_c.z = fmax(max_c.z, v1_vector.z);

      min_c.x = fmin(min_c.x, v2_vector.x);
      min_c.y = fmin(min_c.y, v2_vector.y);
      min_c.z = fmin(min_c.z, v2_vector.z);

      max_c.x = fmax(max_c.x, v2_vector.x);
      max_c.y = fmax(max_c.y, v2_vector.y);
      max_c.z = fmax(max_c.z, v2_vector.z);
      bounding_box.min_corner = min_c;
      bounding_box.max_corner = max_c;
};
