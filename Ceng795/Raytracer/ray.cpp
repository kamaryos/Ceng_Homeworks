#include "ray.h"
#include "camera.h"
#include "shapes.h"
#include "parser.h"

//TODO: Sphere intersection is wrong!!
//Only triangle intersection is working

vec3 ray::generate_ray(const ray& r, const Scene& scene, int max_recursion_depth){
      vec3 result;
      int number_of_spheres = scene.spheres.size();
      int number_of_triangles = scene.triangles.size();
      int number_of_meshes = scene.meshes.size();
      Vec4f intersection_point_4; // intersection point with t (w=t)
      intersection_point_4.w = 0;
      Object object; // object which we intersect
      vec3 intersection_point; // intersection point without t

      for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
          Vec4f temp = Sphere::hit_sphere(r, scene.spheres[j]);
          if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w) ){
              intersection_point_4 = temp;
              object.object_type = 1;
              object.material_id = scene.spheres[j].material_id;
              object.sphere = scene.spheres[j];

          }
      }

      for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
          Vec4f temp = Triangle::hit_triangle(r, scene.triangles[t]);
          if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || temp.w < intersection_point_4.w)){
              intersection_point_4 = temp;
              object.object_type = 2;
              object.material_id = scene.triangles[t].material_id;
              object.triangle = scene.triangles[t]; // Triangle
          }
      }

      for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
          Vec4f1i temp = Mesh::hit_mesh(r,scene.meshes[m]);
          if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && ((intersection_point_4.w==0) || (temp.w < intersection_point_4.w))) {
              intersection_point_4.x = temp.x;
              intersection_point_4.y = temp.y;
              intersection_point_4.z = temp.z;
              intersection_point_4.w = temp.w;
              Triangle triangle1(scene.meshes[m].material_id,scene.meshes[m].faces[temp.i]); // Mesh
              object.object_type = 2;
              object.triangle = triangle1;
              std::cout<<"Mesh intersected!"<<std::endl;
          }
      }

      if(intersection_point_4.w != 0) { // If ray intersects with an object
          intersection_point.x = intersection_point_4.x;
          intersection_point.y = intersection_point_4.y;
          intersection_point.z = intersection_point_4.z;
          vec3 normal;

          std::cout<<object.object_type<< std::endl;
          if(object.object_type == 1){
              //const Sphere* sphere  = static_cast<const Sphere*>(object);
              normal = unit_vector(intersection_point - object.sphere.center); // Normal vector of intersection point (for spheres)

          }
          else if(object.object_type == 2){
              //const Triangle* triangle = static_cast<const Triangle*>(object);
              normal = object.triangle.indices.normal();
          }

          if(max_recursion_depth==0){ // Not> reflectance.
              result = (scene.ambient_light * scene.materials[object.material_id-1].ambient);
              for(auto p_light : scene.point_lights){
                  if(is_object_between(intersection_point, p_light.position, scene.meshes, scene.triangles, scene.spheres,scene.shadow_ray_epsilon) == true){
                      continue;
                  }
                  else {
                      vec3 diffuse = PointLight::diffuse_shading(p_light, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
                      vec3 specular = PointLight::specular_shading(p_light, normal, scene.materials[object.material_id-1].specular, intersection_point, r.origin(), scene.materials[object.material_id-1].phong_exponent);
                      result += diffuse + specular ;
                  }
              }
              return result;
          }

          else if(max_recursion_depth>0){ // Hit and reflect.


              result = (scene.ambient_light * scene.materials[object.material_id-1].ambient);

              for(auto p_light : scene.point_lights){
                  if(is_object_between(intersection_point, p_light.position, scene.meshes, scene.triangles, scene.spheres,scene.shadow_ray_epsilon) == true){
                      continue;
                  }
                  else {
                      vec3 diffuse = PointLight::diffuse_shading(p_light, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
                      vec3 specular = PointLight::specular_shading(p_light, normal, scene.materials[object.material_id-1].specular, intersection_point, r.origin(), scene.materials[object.material_id-1].phong_exponent);
                      result += diffuse + specular;
                  }
              }
              vec3 ray_direction_normalized = (r.direction() / (r.direction().length()));
              float temp = dot(normal,(-1)*(ray_direction_normalized));
              vec3 new_ray_direction = (ray_direction_normalized + 2*temp*normal);
              return (result + generate_ray(ray(intersection_point,new_ray_direction), scene, max_recursion_depth-1) * scene.materials[object.material_id-1].mirror);
          }
      }

      else{ // If ray doesn't intersect set result to background color.
          result.x = scene.background_color.x;
          result.y = scene.background_color.y;
          result.z = scene.background_color.z;
          return result;
      }
}
