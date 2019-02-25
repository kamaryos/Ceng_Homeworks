#ifndef SHAPESH
#define SHAPESH
#include "vec3.h"

struct Face
{
    int v0_id;
    int v1_id;
    int v2_id;
    Face()
    Face(int v0, int v1, int v2): v0_id(v0),v1_id(v1),v2_id(v2){}
};


struct Material
{
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        vec3 mirror;
        float phong_exponent;
};


class Shapes{
public:
  int object_type;
  int material_id;
  Shapes() {}
};

class Sphere : public Shapes{
public:
  int center_vertex_id;
  float radius;
  Sphere(){}
  Sphere(int material_id, int center_vertex_id, float radius) : object_type(1),material_id(material_id), center_vertex_id(center_vertex_id), radius(radius) {}
  Vec4f hit_sphere(const ray& r, const vec3& center, float radius);
};

class Triangle : public Shapes{
public:
  Face indices;
  Triangle()
  Triangle(int material_id, Face indices) : object_type(2), material_id(material_id){
    indices = Face(indices.v0_id,indices.v1_id,indices.v2_id);
    }
  Vec4f hit_triangle(const ray& r, const &vec3 p1, const &vec3 p2, const &vec3 p3);
};

class Mesh : public Shapes{
public:
  std::vector<Face> faces;
  Mesh()
  Mesh(std::vector<Face> faces):object_type(3), faces(vector<Face> n_faces(faces)){} // Is this possible and the class architecture is ok or not!?

};

void t_min(float t, float t1, float t2){
  if(t1<t2){t=t1;}
  else if(t2<t1){t=t2;}
  else if(t1==t2){t=t1;}
};


Vec4f Sphere::hit_sphere(const ray& r, const vec3& center, float radius){ // if ray intersects, return intersection point, if not 0,0,0,-1

  vec3 oc = (r.origin() - center);

  float a = dot(r.direction(),r.direction());
  float b = 2*dot(r.direction(),(r.direction() - center));
  float c = dot(oc,oc) - (radius * radius);

  float delta = (b*b) - (4*a*c);
  float t = 0;

  if(delta < 0){
      //Find a better way to return this part
        Vec4f result(0,0,0,-1);
        return result;
  }

  else{

      float t1 = ((-1)*b + sqrt(delta))/(2*a);
      float t2 = ((-1)*b - sqrt(delta))/(2*a);
      t_min(t,t1,t2);
  }

  //Vec3 --> Vec4f function need to be added

  Vec4f result (r.origin()+t*r.direction(),t);
  return result;
};

Vec4f Triangle::hit_triangle(const ray& r, const &vec3 p1, const &vec3 p2, const &vec3 p3){

  


};






#endif
