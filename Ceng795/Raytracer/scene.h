
#ifndef SCENERENDERH
#define SCENERENDERH

#include "bvh.h"
#include "parser.h"

class Camera;
class Object;
class ray;

class SceneRenderer {
 private:
  vec3 q, usu, vsv;
  Scene scene_;
  std::vector<Object*> objects_;
  BoundingVolumeHierarchy* bounding_volume_hierarchy;

  const vec3 TraceRay(const ray& r, int depth,
                               const Object* hit_obj) const;
  const vec3 CalculateS(int i, int j) const;
  const Vec3i RenderPixel(int i, int j,
                                  const Camera& camera) const;

 public:
  SceneRenderer(const char* scene_path);

  void SetUpScene(const Camera& camera);
  const std::vector<Camera>& Cameras() const { return scene_.cameras; }

  void RenderImage(const Camera& camera, Vec3i* result,
                   const int min_height, const int max_height,
                   const int width) const;
};

#endif
