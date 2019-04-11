
#include "scene.h"
#include "camera.h"
#include "shapes.h"
#include "ray.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

#include <chrono>
#include <ctime>



namespace {

bool NotZero(const vec3 vec) { return vec.x != 0 || vec.y != 0 || vec.z != 0; }

}  // namespace

const vec3 SceneRenderer::CalculateS(int i, int j) const {
  return q + usu * (i + .5) - vsv * (j + .5);
}

const vec3 SceneRenderer::TraceRay(const ray& r, int depth,
                                    const Object* hit_obj = nullptr) const {
  vec3 color = scene_.background_color;
  const hit_record hit_record =
      bounding_volume_hierarchy->GetIntersection(r, hit_obj);
  const int material_id = hit_record.material_id ;

  if (material_id != -1) {

    const vec3 origin = r.origin();
    const vec3 direction = r.direction();
    const vec3 intersection_point = origin + direction * hit_record.t;
    const vec3 normal = hit_record.normal;
    const Material material = scene_.materials[material_id];
    color = scene_.ambient_light*(material.ambient);

    for (const PointLight& light : scene_.point_lights) {
      // Shadow check
      const vec3 wi = light.position - intersection_point;
      const vec3 wi_normal = unit_vector(wi);
      const ray shadow_ray(intersection_point + wi_normal * scene_.shadow_ray_epsilon, wi_normal,
      true);
      if (bounding_volume_hierarchy->GetIntersection(
              shadow_ray, wi.length() - scene_.shadow_ray_epsilon,
              hit_record.obj)) {
        continue;
      }
      const float r_square = dot(wi, wi);
      const vec3 intensity = light.intensity / r_square;

      // Diffuse light
      const float cos_theta = dot(wi_normal, normal);
      const float cos_thetap = cos_theta > 0. ? cos_theta : 0.;
      color += (material.diffuse * cos_thetap)*(intensity);

      // Specular light
      const vec3 h = unit_vector(wi_normal - direction);
      const float cos_alpha = dot(normal,h);
      const float cos_alphap = cos_alpha > 0. ? cos_alpha : 0.;
      color += (material.specular * pow(cos_alphap, material.phong_exponent))
                   *(intensity);
    }
    // Specular reflection
    if (depth > 0 && NotZero(material.mirror)) {
      const vec3 wi =
          unit_vector(direction + normal * -2 * dot(direction,normal));
      const ray reflection_ray(intersection_point + wi * scene_.shadow_ray_epsilon, wi, false);
      color += TraceRay(reflection_ray, depth - 1, hit_record.obj)
                   *(material.mirror);
    }
  }
  return color;
}

const Vec3i SceneRenderer::RenderPixel(int i, int j,
                                       const Camera& camera) const {
  const vec3 origin = camera.position;
  const vec3 direction = unit_vector(CalculateS(i, j) - origin);
  const ray ray(origin, direction, false);
  return Vec3i(SceneRenderer::TraceRay(ray, scene_.max_recursion_depth));
}

void SceneRenderer::RenderImage(const Camera& camera, Vec3i* result,
                                const int min_height, const int max_height,
                                const int width) const {
  auto start = std::chrono::system_clock::now();
  for (int j = min_height; j < max_height; j++) {
    for (int i = 0; i < width; i++) {
      result[j * width + i] = RenderPixel(i, j, camera);
    }
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout<< "Ray Tracing time: " << elapsed_seconds.count() << "s\n";
}

SceneRenderer::SceneRenderer(const char* scene_path) {

  auto start1 = std::chrono::system_clock::now();
  scene_.loadFromXml(scene_path);
  auto end1 = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end1-start1;
  std::cout<< "Parsed time: " << elapsed_seconds.count() << "s\n";


  for (Triangle& obj : scene_.triangles) {
    objects_.push_back(&obj.indices);
  }
  for (Sphere& obj : scene_.spheres) {
    objects_.push_back(&obj);
  }
  for (Mesh& mesh : scene_.meshes) {
    for (Face& obj : mesh.faces) {
      objects_.push_back(&obj);
    }
  }
  auto start2 = std::chrono::system_clock::now();
  bounding_volume_hierarchy = new BoundingVolumeHierarchy(&objects_);
  auto end2 = std::chrono::system_clock::now();
  elapsed_seconds = end2-start2;
  std::cout<< "BVH build time: " << elapsed_seconds.count() << "s\n";
}

void SceneRenderer::SetUpScene(const Camera& camera) {
  const Vec4f view_plane = camera.near_plane;
  const vec3 gaze = unit_vector(camera.gaze);
  const float dist = camera.near_distance;
  const float l = view_plane.x;
  const float r = view_plane.y;
  const float b = view_plane.z;
  const float t = view_plane.w;
  const vec3 u = unit_vector(cross(gaze,camera.up));
  const vec3 v = cross(u,gaze);
  const vec3 m = camera.position + gaze * dist;
  q = m + u * l + v * t;
  usu = u * (r - l) / camera.image_width;
  vsv = v * (t - b) / camera.image_height;
}
