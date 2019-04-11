#include "ray.h"
#include "camera.h"
#include "shapes.h"
#include "parser.h"

//
// bool ray::refraction(const ray& r_in, const vec3& normal, float refraction_index, vec3& refracted, float r_o){
//   vec3 outward_normal;
//   float ni_over_nt;
//
//   float cosine;
//   float inObject = false;
//   if (dot(r_in.direction(), normal) > 0) {
//       outward_normal = -normal;
//       ni_over_nt = refraction_index;
//       cosine = dot(r_in.direction(), normal) ;
//       inObject = true;
//
//   }
//   else {
//       outward_normal = normal;
//       ni_over_nt = 1.0 / refraction_index;
//       cosine = -dot(r_in.direction(), normal) ;
//       inObject = false;
//   }
//
//   if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
//     r_o = schlick(cosine, ni_over_nt);
//   else // total reflection
//     r_o = 1.0;
//
//   return inObject;
//
// }
//
//
//
// vec3 ray::generate_ray(const ray& r, const Scene& scene, int max_recursion_depth){
//       vec3 result;
//       int number_of_spheres = scene.spheres.size();
//       int number_of_triangles = scene.triangles.size();
//       int number_of_meshes = scene.meshes.size();
//       Vec4f intersection_point_4; // intersection point with t (w=t)
//       intersection_point_4.w = 0;
//       Object object; // object which we intersect
//       vec3 intersection_point; // intersection point without t
//
//       for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
//           Vec4f temp = Sphere::hit_sphere(r, scene.spheres[j]);
//           if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w) ){
//               intersection_point_4 = temp;
//               object.object_type = 1;
//               object.material_id = scene.spheres[j].material_id;
//               object.sphere = scene.spheres[j];
//
//           }
//       }
//
//       for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
//           hit_record temp = Triangle::hit_triangle(r, scene.triangles[t]);
//           if((temp.t != -1) && (temp.t > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.t < intersection_point_4.w)){
//               intersection_point_4 = Vec4f(temp.p,temp.t);
//               object.object_type = 2;
//               object.material_id = scene.triangles[t].material_id;
//               object.triangle = scene.triangles[t]; // Triangle
//           }
//       }
//
//       for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
//           hit_record temp = Mesh::hit_mesh(r,scene.meshes[m]);
//           if((temp.t != -1) && (temp.t > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || (temp.t < intersection_point_4.w))) {
//               intersection_point_4 = Vec4f(temp.p,temp.t);
//
//               object.object_type = 3; // Mesh
// 	            object.material_id = scene.meshes[m].material_id;
//               object.triangle = scene.meshes[m].triangles[temp.mesh_no];
//           }
//       }
//
//       if(intersection_point_4.w != 0) { // If ray intersects with an object
//           intersection_point.x = intersection_point_4.x;
//           intersection_point.y = intersection_point_4.y;
//           intersection_point.z = intersection_point_4.z;
//           vec3 normal;
//           if(object.object_type == 1){
//               //const Sphere* sphere  = static_cast<const Sphere*>(object);
//               normal = unit_vector(intersection_point - object.sphere.center); // Normal vector of intersection point (for spheres)
//           }
//           else if(object.object_type == 2 || object.object_type == 3){
//               //const Triangle* triangle = static_cast<const Triangle*>(object);
//               normal = object.triangle.indices.normal();
//           }
//
//           if(max_recursion_depth==0){ // Not reflectance.
//               result = (scene.ambient_light * scene.materials[object.material_id-1].ambient);
//               for(auto p_light : scene.point_lights){
//                   if(is_object_between(intersection_point, p_light.position, scene.meshes, scene.triangles, scene.spheres,scene.shadow_ray_epsilon) == true){
//                       continue;
//                   }
//                   else {
//                       vec3 diffuse = PointLight::diffuse_shading(p_light, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
//                       vec3 specular = PointLight::specular_shading(p_light, normal, scene.materials[object.material_id-1].specular, intersection_point, r.origin(), scene.materials[object.material_id-1].phong_exponent);
//                       result += diffuse + specular ;
//                   }
//               }
//               return result;
//           }
//
//           else if(max_recursion_depth>0){ // Hit and reflect.
//
//
//               result = (scene.ambient_light * scene.materials[object.material_id-1].ambient);
//
//               for(auto p_light : scene.point_lights){
//                   if(is_object_between(intersection_point, p_light.position, scene.meshes, scene.triangles, scene.spheres,scene.shadow_ray_epsilon) == true){
//                       continue;
//                   }
//                   else {
//                       vec3 diffuse = PointLight::diffuse_shading(p_light, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
//                       vec3 specular = PointLight::specular_shading(p_light, normal, scene.materials[object.material_id-1].specular, intersection_point, r.origin(), scene.materials[object.material_id-1].phong_exponent);
//                       result += diffuse + specular;
//                   }
//               }
//
//
//               float refraction_index = scene.materials[object.material_id-1].refraction_index;
//
//               if(refraction_index == 0){
//                 vec3 reflected = reflect(unit_vector(r.direction()), normal);
//                 vec3 reflected_color =  generate_ray(ray(intersection_point,reflected), scene, max_recursion_depth-1) ;
//                 return result + reflected_color* scene.materials[object.material_id-1].mirror ;
//               }
//
//               vec3 refracted ;
//               float kr ;
//               vec3 new_normal;
//               bool inObject = refraction(r,normal,refraction_index,refracted,kr);
//               vec3 attenuation_1 = power(scene.materials[object.material_id-1].transparency,1+intersection_point_4.w);
//               vec3 attenuation_2 = power(scene.materials[object.material_id-1].transparency,intersection_point_4.w);
//
//
//               if(inObject == true){
//                 vec3 reflected = reflect(unit_vector(r.direction()), normal);
//                 vec3 reflected_color =  generate_ray(ray(intersection_point,reflected,inObject), scene, max_recursion_depth-1) ;
//                 vec3 refracted_color =  generate_ray(ray(intersection_point,refracted,inObject), scene, max_recursion_depth-1) ;
//                 return (result + attenuation_1*(kr*reflected_color  + (1-kr)*refracted_color));
//               }
//               else{
//                 vec3 reflected = reflect(unit_vector(r.direction()), (-1)*normal);
//                 vec3 reflected_color =  attenuation_2*generate_ray(ray(intersection_point,reflected,inObject), scene, max_recursion_depth-1) ;
//                 vec3 refracted_color =  generate_ray(ray(intersection_point,refracted,inObject), scene, max_recursion_depth-1) ;
//                 return (result + attenuation_1*(kr*reflected_color  + (1-kr)*refracted_color));
//               }
//
//
//           }
//       }
//
//       else{ // If ray doesn't intersect set result to background color.
//           result.x = scene.background_color.x;
//           result.y = scene.background_color.y;
//           result.z = scene.background_color.z;
//           return result;
//       }
// }
