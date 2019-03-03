#include "shapes.h"
#include "ray.h"



Vec4f Sphere::hit_sphere(const ray& r, const Sphere& sphere){ // if ray intersects, return intersection point, if not 0,0,0,-1

  vec3 oc = (r.origin() - sphere.center);

  float a = dot(r.direction(),r.direction());
  float b = 2*dot(r.direction(),(r.origin() - sphere.center));
  float c = dot(oc,oc) - (sphere.radius * sphere.radius);

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
      if(t1<t2){t=t1;}
      else if(t2<t1){t=t2;}
      else if(t1==t2){t=t1;}
      Vec4f result(r.point_at_parameter(t),t);
      return result;
  }
};

Vec4f Triangle::hit_triangle(const ray& r, const Triangle& triangle){

  float x = r.direction().x;
  float y = r.direction().y;
  float z = r.direction().z;
  vec3 p1 = triangle.indices.v0_vector;
  vec3 p2 = triangle.indices.v1_vector;
  vec3 p3 = triangle.indices.v2_vector;
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

  if(beta < 0 || beta > 1.0f)
  {
   Vec4f result(0,0,0,-1);
   return result;
  }

  float gamma = ((one*seven)-(six*four)+(x*(nine*twelve-eight*thirteen))) * det_A_inv;

  if( gamma < 0 || (beta+gamma > 1))
  {
    Vec4f result(0,0,0,-1);
    return result;
  }
  else{
    float t = ((one*(eleven*twelve-ten*eight))
              -(three*(nine*twelve-thirteen*eight))
              +(six*five))* det_A_inv;

    Vec4f result(r.point_at_parameter(t),t);
    return result;
  }
};

Vec4f1i Mesh::hit_mesh(const ray& r, const Mesh& mesh){
    int size_mesh = mesh.faces.size();
    Vec4f1i result(0,0,0,-1,0);

    for(int i = 0 ; i < size_mesh ; i++){
        Vec4f temp = Triangle::hit_triangle(r,Triangle(mesh.material_id,mesh.faces[i]));
        if((temp.w > mesh.shadow_ray_epsilon) && ((result.w == -1) || (temp.w < result.w))){
            result = Vec4f1i(temp,i);
        }
    }
    return result;
};

//Need to be decided about the implementation
bool is_object_between(const vec3 &ray_origin, const vec3& light_position, const std::vector<Mesh>& meshes, const std::vector<Triangle>& triangles, const std::vector<Sphere>& spheres,const float& sre ){
    int number_of_spheres = spheres.size();
    int number_of_triangles = triangles.size();
    int number_of_meshes = meshes.size();
    //ProfileScope scope("is_object_between");

    ray r(ray_origin,light_position-ray_origin);

    for(int s=0; s < number_of_spheres; s++){ // Check intersections with spheres.
        Vec4f temp = Sphere::hit_sphere(r,spheres[s]);
        if((temp.w > sre) && (temp.w < 1)){return true;}
    }
    for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
        Vec4f temp = Triangle::hit_triangle(r,triangles[t]);
        if((temp.w > sre) && (temp.w < 1)){return true;}
    }
    for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
        Vec4f1i temp = Mesh::hit_mesh(r,meshes[m]);
        if((temp.w > sre) && (temp.w < 1)){return true;}
    }
    return false;
};
