#ifndef PARSERH
#define PARSERH

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "shapes.h"
#include "camera.h"


//Notice that all the structures are as simple as possible
//so that you are not enforced to adopt any style or design.
struct Scene
{
public:
    //Data
    vec3 background_color;
    float shadow_ray_epsilon;
    float intersection_test_epsilon;
    int max_recursion_depth;
    std::vector<Camera> cameras;
    vec3 ambient_light;
    std::vector<PointLight> point_lights;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;
    std::vector<vec3> vertex_data;

    void loadFromXml(const std::string& filepath);
};





#endif
