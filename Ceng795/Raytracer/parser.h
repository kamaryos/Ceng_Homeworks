#ifndef __PARSER__
#define __PARSER__

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "tinyxml2.h"
#include "vec.h"
#include "camera.h"
#include "shapes.h"


namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        vec3 ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<vec3> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        void loadFromXml(const std::string& filepath);

    };
}

#endif
