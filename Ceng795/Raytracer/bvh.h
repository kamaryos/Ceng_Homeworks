#ifndef _BOUNDING_VOLUME_HIERARCHY_
#define _BOUNDING_VOLUME_HIERARCHY_

#include "vec.h"
#include <vector>
#include "ray.h"





class BoundingBox {
 public:
  BoundingBox()
      : min_corner(vec3(kInf, kInf, kInf)),
        max_corner(vec3(-kInf, -kInf, -kInf)) {}
  BoundingBox(const vec3& min_c, const vec3& max_c)
      : min_corner(min_c), max_corner(max_c) {}

  float DoesIntersect(const ray& ray) const;
  void Expand(const BoundingBox& boudning_box);
  int GetMaxDimension() const;
  vec3 GetExtent() const;
  vec3 GetCenter() const;

  vec3 min_corner;
  vec3 max_corner;
  vec3 delta;
  vec3 center;
};

class Object {
 public:
  virtual hit_record GetIntersection(const ray& ray) const = 0;
  virtual const BoundingBox GetBoundingBox() const = 0;
};


struct Node {
  BoundingBox bounding_box;
  Node* left;
  Node* right;
  int start;
  int end;
};




class BoundingVolumeHierarchy {
 public:
  BoundingVolumeHierarchy(std::vector<Object*>* objects);
  hit_record GetIntersection(const ray& r, const Object* hit_obj) const;
  bool GetIntersection(const ray& r, float tmax, const Object* hit_obj) const;

 private:
  void build(Node* cur, int left, int right);
  void GetIntersection(const ray& r, Node* cur, hit_record& hit_record,
                       const Object* hit_obj) const;
  void GetIntersection(const ray& r, Node* cur, float tmax, float& tmin,
                       const Object* hit_obj) const;

  std::vector<Object*>* objects_;
  Node* tree_;
};

#endif
