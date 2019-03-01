#ifndef CAMERAH
#define CAMERAH
#include "vec.h"
#include "shapes.h"


struct Camera
{
        vec3 position;
        vec3 gaze;
        vec3 up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
        Camera(){}
        Camera(vec3 position, vec3 gaze,vec3 up,Vec4f near_plane,int image_width,int image_height,std::string image_name)
        :position(position), gaze(gaze),up(up),near_plane(near_plane),image_width(image_width),image_height(image_height),image_name(image_name){}
        static vec3 locateFirstPixel(const Camera& camera , int i, vec3 plane_side_vector, float width_pixel);
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 mirror;
    vec3 transparency;
    float phong_exponent;
    float refraction_index;

};

struct PointLight
{
        vec3 position;
        vec3 intensity;
        PointLight(){}
        PointLight(vec3 position, vec3 intensity) : position(position), intensity(intensity){}
        static vec3 diffuse_shading(const PointLight& point_light, const vec3& normal,
              const vec3 &diffuse, const vec3 &intersection_position);
        static vec3 specular_shading(const PointLight& point_light, const vec3& normal,const  vec3& specular,
            const  vec3& intersection_position, const vec3& rayOrigin, float phong_exponent);
};



vec3 PointLight::diffuse_shading(const PointLight& point_light, const vec3& normal,
      const vec3 &diffuse, const vec3 &intersection_position){

      vec3 light_vector = (point_light.position - intersection_position); // Vector from intersection point to light source.
      float cos_alpha = dot(light_vector, normal) / (light_vector.length());
      if (cos_alpha<0){
          return vec3(0,0,0);
      }
      else{
          float temp = cos_alpha * (1.0f / (light_vector.squared_length()));
          return point_light.intensity*diffuse*temp;
      }
};

vec3 PointLight::specular_shading(const PointLight& point_light, const vec3& normal,const  vec3& specular,
    const  vec3& intersection_position, const vec3& rayOrigin, float phong_exponent){

    vec3 light_vector = unit_vector(point_light.position - intersection_position); // Vector from intersection point to light source.
    vec3 w_0 = unit_vector(rayOrigin - intersection_position);
    vec3 half_vector = unit_vector(light_vector + w_0); // Half vector
    float cos_alpha = dot(normal, half_vector);

    if(cos_alpha<0){
      return vec3(0,0,0);
    }
    else{
      float temp = pow(cos_alpha, phong_exponent) / (light_vector.squared_length()) ;
      return point_light.intensity*specular*temp;
    }
};


vec3 Camera::locateFirstPixel(const Camera& camera , int i, vec3 plane_side_vector, float width_pixel){
    vec3 m = camera.position + camera.gaze * camera.near_distance ;
    return vec3(m + (plane_side_vector * (abs(camera.near_plane.x) - width_pixel/2)) + (camera.up * (camera.near_plane.w - width_pixel/2)) - (i * camera.up * width_pixel)) ;

};



#endif
