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

};

struct PointLight
{
        vec3 position;
        vec3 intensity;
};


#endif
