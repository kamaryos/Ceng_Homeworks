#ifndef CAMERAH
#define CAMERAH
#include "vec.h"


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


bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted);
vec3 reflect(const vec3& v, const vec3& n);


#endif
