#ifndef SHAPESH
#define SHAPESH
#include <vector>
#include "vec.h"

class ray;

//TODO:
//is_object_between() shadow_ray_epsilon check !!
//object struct find a solution for the ray eq object intersection

struct Face
{
    int v0_id;
    int v1_id;
    int v2_id;

    vec3 v0_vector;
    vec3 v1_vector;
    vec3 v2_vector;

    vec3 normal = unit_vector(cross(v1_vector-v0_vector,v2_vector-v0_vector));

    Face() {}
    Face(vec3 v0, vec3 v1, vec3 v2): v0_vector(v0),v1_vector(v1),v2_vector(v2){}
    Face(vec3 v0, vec3 v1, vec3 v2, vec3 normal): v0_vector(v0),v1_vector(v1),v2_vector(v2),normal(normal){}
};


class Shapes{
public:
  int object_type;
  int material_id;
};

class Sphere : public Shapes{
public:
  int object_type = 1;
  int center_vertex_id;
  vec3 center;
  int material_id;
  float radius;
  Sphere(){}
  Sphere(int material_id, vec3 center, float radius) : material_id(material_id), center(center), radius(radius) {}
  static bool hit_sphere_bool(const ray& r, const Sphere& sphere);
  static Vec4f hit_sphere(const ray& r, const Sphere& sphere);
};

class Triangle : public Shapes{
public:
  int object_type = 2;
  int material_id;
  Face indices;
  Triangle(){}
  Triangle(int material_id, Face indices) : material_id(material_id){
    indices = Face(indices.v0_vector,indices.v1_vector,indices.v2_vector,indices.normal);
    }
  static bool hit_triangle_bool(const ray& r, const Triangle& triangle);
  static Vec4f hit_triangle(const ray& r, const Triangle& triangle);
};

class Mesh : public Triangle {
public:
  int object_type = 3;
  int material_id;
  std::vector<Face> faces;
  float shadow_ray_epsilon;
  Mesh(){}
  Mesh(std::vector<Face> faces, float shadow_ray_epsilon): faces(faces),shadow_ray_epsilon(shadow_ray_epsilon){} // Is this possible and the class architecture is ok or not!?
  static bool hit_mesh_bool(const ray& r, const Mesh& mesh);
  static Vec4f1i hit_mesh(const ray& r, const Mesh& mesh);
};

class Object{
public:
  int object_type;
  int material_id;
  Sphere sphere;
  Triangle triangle;
};

bool is_object_between(const vec3 &ray_origin, const vec3& light_position, const std::vector<Mesh>& meshes, const std::vector<Triangle>& triangles, const std::vector<Sphere>& spheres);

#endif
