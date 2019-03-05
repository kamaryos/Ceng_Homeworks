#ifndef PARSERH
#define PARSERH

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "tinyxml2.h"
#include "shapes.h"
#include "camera.h"


//Notice that all the structures are as simple as possible
//so that you are not enforced to adopt any style or design.
struct Scene
{
public:
    //Data
    Vec3i background_color;
    float shadow_ray_epsilon;
    int max_recursion_depth;
    std::vector<Camera> cameras;
    vec3 ambient_light;
    std::vector<PointLight> point_lights;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;


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
          bool mr = false;
          bool pe = false;
          bool tr = false;
          bool ri = false;

          child = element->FirstChildElement("AmbientReflectance");
          stream << child->GetText() << std::endl;
          child = element->FirstChildElement("DiffuseReflectance");
          stream << child->GetText() << std::endl;
          child = element->FirstChildElement("SpecularReflectance");
          stream << child->GetText() << std::endl;
          child = element->FirstChildElement("MirrorReflectance");
          if(child != NULL) { mr = true; stream << child->GetText() << std::endl;}
          child = element->FirstChildElement("PhongExponent");
          if(child != NULL) { pe = true; stream << child->GetText() << std::endl;}
          child = element->FirstChildElement("Transparency");
          if(child != NULL) { tr = true; stream << child->GetText() << std::endl;}
          child = element->FirstChildElement("RefractionIndex");
          if(child != NULL) { ri = true; stream << child->GetText() << std::endl;}


          stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
          stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
          stream >> material.specular.x >> material.specular.y >> material.specular.z;
          if(mr == true) {stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;}
          if(pe == true) {stream >> material.phong_exponent;}
          if(tr == true) {stream >> material.transparency.x >> material.transparency.y >> material.transparency.z;}
          if(ri == true) {stream >> material.refraction_index;}
          materials.push_back(material);
          element = element->NextSiblingElement("Material");
      }
      std::vector<vec3> vertex_data;
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
	  int v0_id;int v1_id;int v2_id; 
          while (!(stream >> v0_id).eof())
          {
              stream >> v1_id >> v2_id;
              face.v0_vector = vertex_data[v0_id - 1];
              face.v1_vector = vertex_data[v1_id - 1];
              face.v2_vector = vertex_data[v2_id - 1];

              //face.normal =  unit_vector(cross(v1,v2));
              mesh.faces.push_back(face);
          }
          stream.clear();
          mesh.shadow_ray_epsilon = shadow_ray_epsilon;
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
	  int v0_id;int v1_id;int v2_id; 
          stream >> v0_id >> v1_id >> v2_id;

          triangle.indices.v0_vector = vertex_data[v0_id - 1];
          triangle.indices.v1_vector = vertex_data[v1_id - 1];
          triangle.indices.v2_vector = vertex_data[v2_id - 1];

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
          sphere.center = vertex_data[sphere.center_vertex_id-1];

          child = element->FirstChildElement("Radius");
          stream << child->GetText() << std::endl;
          stream >> sphere.radius;

          spheres.push_back(sphere);
          element = element->NextSiblingElement("Sphere");
      }
    }

};





#endif
