#ifndef __PARSER__
#define __PARSER__

#include <string>
#include <vector>
#include "parser.h"
#include "tinyxml2.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "vec.h"

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.


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

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };

    struct Triangle
    {
        int material_id;
        Face indices;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Object
    {
    	int object_type; //    1 = sphere     2 = Triangle     3 = Mesh
    	int material_id; // For all
    	int center_vertex_id; // if sphere
        float radius; // if sphere
        Face indices; // if Triangle
        std::vector<Face> faces; // If Mesh
    };

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

        //Functions
        void loadFromXml(const std::string& filepath)
        {
            tinyxml2::XMLDocument file;
            std::stringstream stream;

            auto res = file.LoadFile(filepath.c_str());
            if (res)
            {
                throw std::runtime_error("Error: The xml file cannot be loaded.");
            }

            auto root = file.FirstChild();
            if (!root)
            {
                throw std::runtime_error("Error: Root is not found.");
            }

            //Get BackgroundColor
            auto element = root->FirstChildElement("BackgroundColor");
            if (element)
            {
                stream << element->GetText() << std::endl;
            }
            else
            {
                stream << "0 0 0" << std::endl;
            }
            stream >> background_color.x >> background_color.y >> background_color.z;

            //Get ShadowRayEpsilon
            element = root->FirstChildElement("ShadowRayEpsilon");
            if (element)
            {
                stream << element->GetText() << std::endl;
            }
            else
            {
                stream << "0.001" << std::endl;
            }
            stream >> shadow_ray_epsilon;

            //Get MaxRecursionDepth
            element = root->FirstChildElement("MaxRecursionDepth");
            if (element)
            {
                stream << element->GetText() << std::endl;
            }
            else
            {
                stream << "0" << std::endl;
            }
            stream >> max_recursion_depth;

            //Get Cameras
            element = root->FirstChildElement("Cameras");
            element = element->FirstChildElement("Camera");
            Camera camera;
            while (element)
            {
                auto child = element->FirstChildElement("Position");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("Gaze");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("Up");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("NearPlane");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("NearDistance");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("ImageResolution");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("ImageName");
                stream << child->GetText() << std::endl;

                stream >> camera.position.x >> camera.position.y >> camera.position.z;
                stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
                stream >> camera.up.x >> camera.up.y >> camera.up.z;
                stream >> camera.near_plane.x >> camera.near_plane.y >> camera.near_plane.z >> camera.near_plane.w;
                stream >> camera.near_distance;
                stream >> camera.image_width >> camera.image_height;
                stream >> camera.image_name;

                cameras.push_back(camera);
                element = element->NextSiblingElement("Camera");
            }

            //Get Lights
            element = root->FirstChildElement("Lights");
            auto child = element->FirstChildElement("AmbientLight");
            stream << child->GetText() << std::endl;
            stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
            element = element->FirstChildElement("PointLight");
            PointLight point_light;
            while (element)
            {
                child = element->FirstChildElement("Position");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("Intensity");
                stream << child->GetText() << std::endl;

                stream >> point_light.position.x >> point_light.position.y >> point_light.position.z;
                stream >> point_light.intensity.x >> point_light.intensity.y >> point_light.intensity.z;

                point_lights.push_back(point_light);
                element = element->NextSiblingElement("PointLight");
            }

            //Get Materials
            element = root->FirstChildElement("Materials");
            element = element->FirstChildElement("Material");
            Material material;
            while (element)
            {
                child = element->FirstChildElement("AmbientReflectance");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("DiffuseReflectance");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("SpecularReflectance");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("MirrorReflectance");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("PhongExponent");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("Transparency");
                stream << child->GetText() << std::endl;
                child = element->FirstChildElement("RefractionIndex");
                stream << child->GetText() << std::endl;


                stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
                stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
                stream >> material.specular.x >> material.specular.y >> material.specular.z;
                stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
                stream >> material.phong_exponent;
                stream >> material.transparency.x >> material.transparency.y >> material.transparency.z;
                stream >> material.refraction_index;

                materials.push_back(material);
                element = element->NextSiblingElement("Material");
            }

            //Get VertexData
            element = root->FirstChildElement("VertexData");
            stream << element->GetText() << std::endl;
            vec3 vertex;
            while (!(stream >> vertex.x).eof())
            {
                stream >> vertex.y >> vertex.z;
                vertex_data.push_back(vertex);
            }
            stream.clear();

            //Get Meshes
            element = root->FirstChildElement("Objects");
            element = element->FirstChildElement("Mesh");
            Mesh mesh;
            while (element)
            {
                child = element->FirstChildElement("Material");
                stream << child->GetText() << std::endl;
                stream >> mesh.material_id;

                child = element->FirstChildElement("Faces");
                stream << child->GetText() << std::endl;
                Face face;
                while (!(stream >> face.v0_id).eof())
                {
                    stream >> face.v1_id >> face.v2_id;
                    mesh.faces.push_back(face);
                }
                stream.clear();

                meshes.push_back(mesh);
                mesh.faces.clear();
                element = element->NextSiblingElement("Mesh");
            }
            stream.clear();

            //Get Triangles
            element = root->FirstChildElement("Objects");
            element = element->FirstChildElement("Triangle");
            Triangle triangle;
            while (element)
            {
                child = element->FirstChildElement("Material");
                stream << child->GetText() << std::endl;
                stream >> triangle.material_id;

                child = element->FirstChildElement("Indices");
                stream << child->GetText() << std::endl;
                stream >> triangle.indices.v0_id >> triangle.indices.v1_id >> triangle.indices.v2_id;

                triangles.push_back(triangle);
                element = element->NextSiblingElement("Triangle");
            }

            //Get Spheres
            element = root->FirstChildElement("Objects");
            element = element->FirstChildElement("Sphere");
            Sphere sphere;
            while (element)
            {
                child = element->FirstChildElement("Material");
                stream << child->GetText() << std::endl;
                stream >> sphere.material_id;

                child = element->FirstChildElement("Center");
                stream << child->GetText() << std::endl;
                stream >> sphere.center_vertex_id;

                child = element->FirstChildElement("Radius");
                stream << child->GetText() << std::endl;
                stream >> sphere.radius;

                spheres.push_back(sphere);
                element = element->NextSiblingElement("Sphere");
            }
        }

    };
}

#endif