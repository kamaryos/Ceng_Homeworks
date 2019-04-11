#ifndef SHAPESH
#define SHAPESH
#include <vector>
#include <limits>
#include "bvh.h"


class ray;

//TODO:
//is_object_between() shadow_ray_epsilon check !!
//object struct find a solution for the ray eq object intersection


float Determinant(vec3 a, vec3 b, vec3 c);



struct Face : public Object{

    vec3 v0_vector;
    vec3 v1_vector;
    vec3 v2_vector;

    int material_id;

    vec3 normal;

    Face() {}
    Face(vec3 v0, vec3 v1, vec3 v2): v0_vector(v0),v1_vector(v1),v2_vector(v2){}
//
    // inline vec3 normal() {
    //   return unit_vector(cross(v0_vector - v1_vector,v0_vector - v2_vector));
    // }

    void CalculateNormal() ;

    hit_record GetIntersection(const ray& r) const;
    const BoundingBox GetBoundingBox() const { return bounding_box; }

private:
  BoundingBox bounding_box;
  vec3 ba;
  vec3 ca;
};


struct Sphere : public Object{
  // int object_type = 1;
  // int center_vertex_id;
  vec3 center;
  int material_id;
  float radius;
  // float shadow_ray_epsilon;
  // Sphere(){}
  // Sphere(int material_id, vec3 center, float radius) : material_id(material_id), center(center), radius(radius) {}
  // static Vec4f hit_sphere(const ray& r, const Sphere& sphere);
  // static aabb bounding_box_sphere(const Sphere& sphere);

  vec3 getnormal(float t,const ray& r) const{
    return unit_vector(r.point_at_parameter(t) - center);
  }


  hit_record GetIntersection(const ray& r) const;
  void Initialize();

  const BoundingBox GetBoundingBox() const { return bounding_box; }

  BoundingBox bounding_box;
};

class Triangle {
public:
  // int object_type = 2;
  int material_id;
  Face indices;
  // Triangle(){}
  // Triangle(int material_id, vec3 v0, vec3 v1, vec3 v2) : material_id(material_id){
  //   indices = Face(v0,v1,v2);
  //   }
  // Triangle(int material_id, Face indices) : material_id(material_id){
  //   indices = Face(indices.v0_vector,indices.v1_vector,indices.v2_vector);
  //   }
  // static hit_record hit_triangle(const ray& r, const Triangle& triangle);
};

class Mesh {
public:
  // int object_type = 3;
  int material_id;
  std::vector<Face> faces;
  // float shadow_ray_epsilon;
  // Mesh(){}
  // Mesh(std::vector<Triangle> triangles, float shadow_ray_epsilon): triangles(triangles),shadow_ray_epsilon(shadow_ray_epsilon){} // Is this possible and the class architecture is ok or not!?
  //
  // static hit_record hit_mesh(const ray& r, const Mesh& mesh);
};

// class Object{
// public:
//   int object_type;
//   int material_id;
//   Sphere sphere;
//   Triangle triangle;
// };
//
// bool is_object_between(const vec3 &ray_origin, const vec3& light_position, const std::vector<Mesh>& meshes, const std::vector<Triangle>& triangles, const std::vector<Sphere>& spheres,const float& sre );

#endif
