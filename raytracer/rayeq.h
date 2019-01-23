#ifndef __RAY__EQUATION__
#define __RAY__EQUATION___

#define PI 3.14

#include <iostream>
#include <cmath>
#include <string>
#include "parser.h"
#include "ppm.h"
#include "vecop.h"

namespace rayeq
{

  parser::Vec3f diffuse_of_point(const parser::Vec3f &light_position, const parser::Vec3f &light_intensity, const parser::Vec3f& normal,
      const parser::Vec3f &diffuse, const parser::Vec3f &intersection_position){
      parser::Vec3f result; // result RGB values.
      parser::Vec3f light_vector = vecop::subVec3f(light_position, intersection_position); // Vector from intersection point to light source.
      float lengthOfLightVector = vecop::lengthOfVector(light_vector);
      float cos_alpha = vecop::dotProduct(light_vector, normal) / (lengthOfLightVector);
      if (cos_alpha<0){
          result.x=0;
          result.y=0;
          result.z=0;
          return result;
      }
      else{
          result.x = (light_intensity.x * diffuse.x * cos_alpha) / (lengthOfLightVector * lengthOfLightVector);
          result.y = (light_intensity.y * diffuse.y * cos_alpha) / (lengthOfLightVector * lengthOfLightVector);
          result.z = (light_intensity.z * diffuse.z * cos_alpha) / (lengthOfLightVector * lengthOfLightVector);
          return result;
      }
  };

  parser::Vec3f specular_of_point(const parser::Vec3f& light_position,const parser::Vec3f& light_intensity, const parser::Vec3f& normal,const  parser::Vec3f& specular,
      const  parser::Vec3f& intersection_position, const parser::Vec3f& rayOrigin, float phong_exponent){
    parser::Vec3f result; // result RGB values.

      parser::Vec3f light_vector = vecop::subVec3f(light_position, intersection_position); // Vector from intersection point to light source.
      float lengthOfLightVector = vecop::lengthOfVector(light_vector);
      light_vector = vecop::scaler_division(light_vector, lengthOfLightVector); // Make it unit

      parser::Vec3f w_0 = vecop::subVec3f(rayOrigin, intersection_position);
      float lengthOfw_0 = vecop::lengthOfVector(w_0);
      w_0 = vecop::scaler_division(w_0, lengthOfw_0); // Make it unit

      parser::Vec3f half_vector = vecop::addVec3f(light_vector, w_0); // Half vector
      float lengthOfHalfVector = vecop::lengthOfVector(half_vector);
      half_vector = vecop::scaler_division(half_vector, lengthOfHalfVector); // Make it unit

      float cos_alpha = vecop::dotProduct(normal, half_vector);

      if(cos_alpha<0){
        result.x = 0;
        result.y = 0;
        result.z = 0;
        return result;
      }
      else{
        result.x = (light_intensity.x * specular.x * pow(cos_alpha, phong_exponent)) / (lengthOfLightVector * lengthOfLightVector);
        result.y = (light_intensity.y * specular.y * pow(cos_alpha, phong_exponent)) / (lengthOfLightVector * lengthOfLightVector);
        result.z = (light_intensity.z * specular.z * pow(cos_alpha, phong_exponent)) / (lengthOfLightVector * lengthOfLightVector);
          return result;
      }
  };


  /*
  std::map<string,double> total_times_of_scopes;
  struct ProfileScope{
      ProfileScope(const char* name)
      {
          start_time = get_time();
      }

      ~ProfileScope()
      {
          double end_time = get_time();
          total_times_of_scopes[scope_name] += end_time - start_time;
          //printf("%s : %f /n", scope_name.c_str(), end_time - start_time);
      }

      double start_time;
      string scope_name;
  };*/

  bool is_object_between(const parser::Vec3f &ray_origin, const parser::Vec3f& light_position, const parser::Scene& scene){
      int number_of_spheres = scene.spheres.size();
      int number_of_triangles = scene.triangles.size();
      int number_of_meshes = scene.meshes.size();

      //ProfileScope scope("is_object_between");

      parser::Vec3f ray_direction = vecop::subVec3f(light_position, ray_origin);

      for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
          parser::Vec4f temp = intersect::rayIntersectsSphere(ray_origin, ray_direction, scene.vertex_data[scene.spheres[j].center_vertex_id-1], scene.spheres[j].radius);
          if((temp.w > scene.shadow_ray_epsilon) && (temp.w < 1)){return true;}
      }
      for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
          parser::Vec4f temp = intersect::rayIntersectsTriangle(scene.vertex_data[scene.triangles[t].indices.v0_id-1], scene.vertex_data[scene.triangles[t].indices.v1_id-1], scene.vertex_data[scene.triangles[t].indices.v2_id-1], ray_direction, ray_origin);
          if((temp.w > scene.shadow_ray_epsilon) && (temp.w < 1)){return true;}
      }
      for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
          parser::Vec4f1i temp = intersect::rayIntersectsMesh(scene.meshes[m].faces, scene.vertex_data, ray_direction, ray_origin, scene.shadow_ray_epsilon);
          if((temp.w > scene.shadow_ray_epsilon) && (temp.w < 1)){return true;}
      }
      return false;
  };



//   float compute_ambient_factor(const parser::Vec3f& ray_origin0, const parser::Vec3f& normal,const parser::Scene& scene){

//       parser::Vec3f ray_origin = vecop::addVec3f(ray_origin0, vecop::scaler_multiplication(normal, scene.shadow_ray_epsilon));

//       int ray_count = 15;
//       float dxsquare = 1.0f / (float)(ray_count * ray_count);
//       float dx = 1.0f / (float)ray_count;

//       int number_of_spheres = scene.spheres.size();
//       int number_of_triangles = scene.triangles.size();
//       int number_of_meshes = scene.meshes.size();

//       parser::Vec4f intersection_point_4; // intersection point with t (w=t)
//       intersection_point_4.w = 0;
//       parser::Object object; // object which we intersect
//       parser::Vec3f intersection_point; // intersection point without t

//       float ambient_factor = 0;

//       for(int i = 0; i < ray_count; i++)
//       {
//           for(int j = 0; j < ray_count; j++)
//           {
//               float x = PI * i * dx;
//               float y = PI * j * dx * 2;

//               float azimuthal = 2 * PI *j * dx;
//               float sin2_zenith = i * dx;
//               float sin_zenith = sqrt(sin2_zenith);

//               parser::Vec3f ray_direction;

//               ray_direction.x = sin_zenith*cos(azimuthal);
//               ray_direction.y = sin_zenith*sin(azimuthal);
//               ray_direction.z = sqrt(1 - sin2_zenith);

//               if(vecop::dotProduct(normal, ray_direction) < 0)
//               {
//                   ray_direction =  vecop::scaler_multiplication(ray_direction, -1);
//               }

//                 for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
//                   parser::Vec4f temp = intersect::rayIntersectsSphere(ray_origin, ray_direction, scene.vertex_data[scene.spheres[j].center_vertex_id-1], scene.spheres[j].radius);
//                   if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w) ){
//                       intersection_point_4 = temp;
//                       object.object_type = 1; // Sphere
//                       object.material_id = scene.spheres[j].material_id;
//                       object.center_vertex_id = scene.spheres[j].center_vertex_id;
//                       object.radius = scene.spheres[j].radius;
//                   }
//               }

//               for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
//                   parser::Vec4f temp = intersect::rayIntersectsTriangle(scene.vertex_data[scene.triangles[t].indices.v0_id-1], scene.vertex_data[scene.triangles[t].indices.v1_id-1], scene.vertex_data[scene.triangles[t].indices.v2_id-1], ray_direction, ray_origin);
//                   if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w)){
//                       intersection_point_4 = temp;
//                       object.object_type = 2; // Triangle
//                       object.material_id = scene.triangles[t].material_id;
//                       object.indices     = scene.triangles[t].indices;
//                   }
//               }

//               for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
//                   parser::Vec4f1i temp = intersect::rayIntersectsMesh(scene.meshes[m].faces, scene.vertex_data, ray_direction, ray_origin, scene.shadow_ray_epsilon);
//                   if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || (temp.w < intersection_point_4.w))) {
//                       intersection_point_4.x = temp.x;
//                       intersection_point_4.y = temp.y;
//                       intersection_point_4.z = temp.z;
//                       intersection_point_4.w = temp.w;
//                       object.object_type = 3; // Mesh
//                       object.material_id = scene.meshes[m].material_id;
//                       object.indices     = scene.meshes[m].faces[temp.i];
//                   }
//               }

//               if(intersection_point_4.w == 0 || intersection_point_4.w > 1)
//               { // If ray intersects with an object
//                   float dP = vecop::dotProduct(ray_direction,normal) ;
//                   float lP = 1.0f / (vecop::lengthOfVector(ray_direction)*vecop::lengthOfVector(normal) ) ;


//                   ambient_factor += (dxsquare * dP * lP );
//               }
//           }
//       }

//       return ambient_factor;
//   };


  parser::Vec3f generate_ray(const parser::Vec3f& ray_origin, const parser::Vec3f& ray_direction, const parser::Scene& scene, int max_recursion_depth){
      parser::Vec3f result;
      int number_of_spheres = scene.spheres.size();
      int number_of_triangles = scene.triangles.size();
      int number_of_meshes = scene.meshes.size();
      parser::Vec4f intersection_point_4; // intersection point with t (w=t)
      intersection_point_4.w = 0;
      parser::Object object; // object which we intersect
      parser::Vec3f intersection_point; // intersection point without t

      for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
          parser::Vec4f temp = intersect::rayIntersectsSphere(ray_origin, ray_direction, scene.vertex_data[scene.spheres[j].center_vertex_id-1], scene.spheres[j].radius);
          if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w) ){
              intersection_point_4 = temp;
              object.object_type = 1; // Sphere
              object.material_id = scene.spheres[j].material_id;
              object.center_vertex_id = scene.spheres[j].center_vertex_id;
              object.radius = scene.spheres[j].radius;
          }
      }

      for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
          parser::Vec4f temp = intersect::rayIntersectsTriangle(scene.vertex_data[scene.triangles[t].indices.v0_id-1], scene.vertex_data[scene.triangles[t].indices.v1_id-1], scene.vertex_data[scene.triangles[t].indices.v2_id-1], ray_direction, ray_origin);
          if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w)){
              intersection_point_4 = temp;
              object.object_type = 2; // Triangle
              object.material_id = scene.triangles[t].material_id;
              object.indices     = scene.triangles[t].indices;
          }
      }

      for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
          parser::Vec4f1i temp = intersect::rayIntersectsMesh(scene.meshes[m].faces, scene.vertex_data, ray_direction, ray_origin, scene.shadow_ray_epsilon);
          if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || (temp.w < intersection_point_4.w))) {
              intersection_point_4.x = temp.x;
              intersection_point_4.y = temp.y;
              intersection_point_4.z = temp.z;
              intersection_point_4.w = temp.w;
              object.object_type = 3; // Mesh
              object.material_id = scene.meshes[m].material_id;
              object.indices     = scene.meshes[m].faces[temp.i];
          }
      }

      if(intersection_point_4.w != 0) { // If ray intersects with an object
          intersection_point.x = intersection_point_4.x;
          intersection_point.y = intersection_point_4.y;
          intersection_point.z = intersection_point_4.z;
          parser::Vec3f normal;
          if(object.object_type == 1){
              normal = vecop::subVec3f(intersection_point, scene.vertex_data[object.center_vertex_id-1]); // Normal vector of intersection point (for spheres)
              normal = vecop::scaler_division(normal, vecop::lengthOfVector(normal)); // make it  unit
          }
          else if(object.object_type == 2 || object.object_type == 3 ){
              normal = vecop::crossProduct((vecop::subVec3f(scene.vertex_data[object.indices.v1_id-1],scene.vertex_data[object.indices.v0_id-1])),(vecop::subVec3f(scene.vertex_data[object.indices.v2_id-1],scene.vertex_data[object.indices.v0_id-1])));
              normal = vecop::scaler_division(normal,vecop::lengthOfVector(normal)); // make it  unit
          }

  //          float ambient_factor = compute_ambient_factor(intersection_point, normal, scene);
  //          parser::Vec3f color;
  //          color.x = 255.0f;
  //          color.y = 255.0f;
  //          color.z = 255.0f;
  //          parser::Vec3f ambient_light = vecop::scaler_multiplication(scene.ambient_light, ambient_factor);
  //          return ambient_light;

          if(max_recursion_depth==0){ // No reflectance.
              result = vecop::multiplyVec3f(scene.ambient_light, scene.materials[object.material_id-1].ambient);
              for(auto p_light : scene.point_lights){
                  if(is_object_between(intersection_point, p_light.position, scene) == true){
                      break;
                  }
                  else {
                      parser::Vec3f diffuse = diffuse_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
                      parser::Vec3f specular = specular_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].specular, intersection_point, ray_origin, scene.materials[object.material_id-1].phong_exponent);
                      parser::Vec3f shading;
                      shading.x = diffuse.x + specular.x;
                      shading.y = diffuse.y + specular.y;
                      shading.z = diffuse.z + specular.z;
                      result = vecop::addVec3f(shading,result);
                  }
              }
              return result;
          }

          else if(max_recursion_depth>0){ // Hit and reflect.
              result = vecop::multiplyVec3f(scene.ambient_light, scene.materials[object.material_id-1].ambient);
              for(auto p_light : scene.point_lights){
                  if(is_object_between(intersection_point, p_light.position, scene) == true){
                      break;
                  }
                  else {
                      parser::Vec3f diffuse = diffuse_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
                      parser::Vec3f specular = specular_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].specular, intersection_point, ray_origin, scene.materials[object.material_id-1].phong_exponent);
                      parser::Vec3f shading = vecop::addVec3f (diffuse, specular);
                      result = vecop::addVec3f(shading, result);
                  }
              }
              parser::Vec3f ray_direction_normalized = vecop::scaler_division(ray_direction, vecop::lengthOfVector(ray_direction));
              float temp = vecop::dotProduct(normal, vecop::scaler_multiplication(ray_direction_normalized,-1));
              parser::Vec3f new_ray_direction = vecop::addVec3f(ray_direction_normalized, vecop::scaler_multiplication(vecop::scaler_multiplication(normal,2),temp));
              return vecop::addVec3f(result, vecop::multiplyVec3f(generate_ray(intersection_point, new_ray_direction, scene, max_recursion_depth-1), scene.materials[object.material_id-1].mirror));
          }
      }

      else{ // If ray doesn't intersect set result to background color.
          result.x = scene.background_color.x;
          result.y = scene.background_color.y;
          result.z = scene.background_color.z;
          return result;
      }
    }


}

#endif
