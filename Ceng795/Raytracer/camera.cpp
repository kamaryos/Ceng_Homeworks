#include "camera.h"


vec3 PointLight::diffuse_shading(const PointLight& point_light, const vec3& normal,
      const vec3 &diffuse, const vec3 &intersection_position){

      vec3 light_vector = (point_light.position - intersection_position); // Vector from intersection point to light source.
      float cos_alpha = std::max(0.f,dot(light_vector, normal) / (light_vector.length()));

      float temp = cos_alpha * (1.0f / (light_vector.squared_length()));
      return point_light.intensity*diffuse*temp;

};

vec3 PointLight::specular_shading(const PointLight& point_light, const vec3& normal,const  vec3& specular,
    const  vec3& intersection_position, const vec3& rayOrigin, float phong_exponent){

    vec3 light_vector = (point_light.position - intersection_position); // Vector from intersection point to light source.
    vec3 w_0 = (rayOrigin - intersection_position);
    vec3 half_vector = unit_vector(light_vector + w_0); // Half vector

    float cos_alpha = std::max(0.f,dot(normal, half_vector));

    float temp = pow(cos_alpha, phong_exponent) * (1.0f / (light_vector.squared_length())) ;
    return point_light.intensity*specular*temp;
};


vec3 Camera::locateFirstPixel(const Camera& camera , int i, vec3 plane_side_vector, float width_pixel){
    vec3 m = camera.position + camera.gaze * camera.near_distance ;
    return vec3(m + (plane_side_vector * (abs(camera.near_plane.x) - width_pixel/2)) + (camera.up * (camera.near_plane.w - width_pixel/2)) - (i * camera.up * width_pixel)) ;

};


bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv + n*dt) - n*sqrt(discriminant);
        return true;
    }
    else
        return false;
};

vec3 reflect(const vec3& v, const vec3& n) {
     return v - 2*dot(v,n)*n;
};
