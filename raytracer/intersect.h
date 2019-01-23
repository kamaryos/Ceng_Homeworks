#ifndef __RAY__INTERSECTION__
#define __RAY__INTERSECTION___

#include <iostream>
#include <cmath>
#include <string>
#include "parser.h"
#include "ppm.h"
#include "vecop.h"


namespace intersect
{

  parser::Vec4f rayIntersectsSphere(const parser::Vec3f& rayOrigin, const parser::Vec3f& rayDirection, const parser::Vec3f& sphereCenter, float sphereRadius){ // if ray intersects, return intersection point, if not 0,0,0,-1
  	parser::Vec4f result;
      float a = vecop::dotProduct(rayDirection,rayDirection);
  	float b = 2*vecop::dotProduct(rayDirection,(vecop::subVec3f(rayOrigin, sphereCenter)));
  	float c = vecop::dotProduct(vecop::subVec3f(rayOrigin, sphereCenter),vecop::subVec3f(rayOrigin, sphereCenter))-(sphereRadius * sphereRadius);
      float delta = (b*b) - (4*a*c);
      float t = 0;
  	if(delta < 0){
          result.x = 0;
          result.y = 0;
          result.z = 0;
          result.w = -1;
          return result;
      }
      else{
          float t1 = ((-1)*b + sqrt(delta))/(2*a);
          float t2 = ((-1)*b - sqrt(delta))/(2*a);
          if(t1<t2){t=t1;}
          else if(t2<t1){t=t2;}
          else if(t1==t2){t=t1;}
      }
      result.x = rayOrigin.x + t*rayDirection.x;
      result.y = rayOrigin.y + t*rayDirection.y;
      result.z = rayOrigin.z + t*rayDirection.z;
      result.w = t;
      return result;
  };

  parser::Vec4f rayIntersectsTriangle(const parser::Vec3f& point_a, const parser::Vec3f& point_b, const parser::Vec3f& point_c,const parser::Vec3f& rayDirection, const parser::Vec3f& rayOrigin){// check whether ray intersects with triangle
      float x = rayDirection.x;
      float y = rayDirection.y;
      float z = rayDirection.z;
      float one = (point_a.x-point_b.x);
      float ten = (point_a.z-point_c.z);
      float eleven = (point_a.y-point_c.y);
      float twelve = (point_a.z-rayOrigin.z);
      float thirteen = (point_a.z-point_b.z);
      float two = (eleven*z-ten*y);
      float three = (point_a.x-point_c.x);
      float nine = (point_a.y-point_b.y);
      float four = (nine*z-thirteen*y);
      float five = (nine*ten-eleven*thirteen);
      float six = (point_a.x-rayOrigin.x);
      float eight = (point_a.y-rayOrigin.y);
      float seven = (eight*z-twelve*y);

      float det_A = (one*two)
                 -(three*four)
                 +(x*five);

      float det_A_inv = 1.0f / det_A;

      float beta  = ((six*two)
                 -(three*seven)
                 +(x*(eight*ten-eleven*twelve)))* det_A_inv;


      parser::Vec4f result;

      if(beta < 0.f || beta > 1.0f)
      {
          result.x = 0;
          result.y = 0;
          result.z = 0;
          result.w = -1;
          return result;
      }

      float gamma = ((one*seven)
                 -(six*four)
                 +(x*(nine*twelve-eight*thirteen))) * det_A_inv;

      if( gamma < 0 || (beta+gamma > 1))
      {
          result.x = 0;
          result.y = 0;
          result.z = 0;
          result.w = -1;
          return result;
      }
      else{
          float t = ((one*(eleven*twelve-ten*eight))
                    -(three*(nine*twelve-thirteen*eight))
                    +(six*five))* det_A_inv;

          result.x = rayOrigin.x + t*x;
          result.y = rayOrigin.y + t*y;
          result.z = rayOrigin.z + t*z;
          result.w = t;
          return result;
      }
  };

  parser::Vec4f1i rayIntersectsMesh(const std::vector<parser::Face>& faces, const std::vector<parser::Vec3f> &vertex_data, const parser::Vec3f &rayDirection,
      const parser::Vec3f& rayOrigin, float shadow_ray_epsilon){
      int size_mesh = faces.size();
      parser::Vec4f1i result;
      result.x = 0;
      result.y = 0;
      result.z = 0;
      result.w = -1;
      result.i = 0;
      for(int i = 0 ; i < size_mesh ; i++){
          parser::Vec4f temp = rayIntersectsTriangle(vertex_data[faces[i].v0_id-1],vertex_data[faces[i].v1_id-1],vertex_data[faces[i].v2_id-1],rayDirection,rayOrigin);
          if((temp.w > shadow_ray_epsilon) && ((result.w == -1) || (temp.w < result.w))){
              result.x = temp.x;
              result.y = temp.y;
              result.z = temp.z;
              result.w = temp.w;
              result.i = i;
          }
      }
      return result;
  };

}

#endif
