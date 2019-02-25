#ifndef CAMERAH
#define CAMERAH
#include "vec3.h"

class Camera
{
public:
        vec3 position;
        vec3 gaze;
        vec3 up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
        
};

class PointLight
{
public:
        vec3 position;
        vec3 intensity;
};

#endif
