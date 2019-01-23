#ifndef __VECTOR__OPERATIONS__
#define __VECTOR__OPERATIONS__

#include <iostream>
#include <cmath>
#include <string>
#include "parser.h"

namespace vecop
{
  //Functions
  float dotProduct(const parser::Vec3f& first, const parser::Vec3f& second){ // Return the dot product of 2 vectors.
  	float result = (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
  	return result;
  };

  float lengthOfVector(const parser::Vec3f& vector){ // Return length of the vector.
      float result = sqrt(((vector.x)*(vector.x)) + ((vector.y)*(vector.y)) + ((vector.z)*(vector.z)));
      return result;
  };

  parser::Vec3f subVec3f(const parser::Vec3f& first, const parser::Vec3f& second){ // Return first vector - second vector.
  	parser::Vec3f result;
  	result.x = first.x - second.x;
  	result.y = first.y - second.y;
  	result.z = first.z - second.z;
  	return result;
  };

  parser::Vec3f addVec3f(const parser::Vec3f& first,const parser::Vec3f& second){ // Return first vector + second vector.
  	parser::Vec3f result;
  	result.x = first.x + second.x;
  	result.y = first.y + second.y;
  	result.z = first.z + second.z;
  	return result;
  };

  parser::Vec3f scaler_division(const parser::Vec3f& vector, float i){
  	parser::Vec3f result;
  	result.x = vector.x / i;
  	result.y = vector.y / i;
  	result.z = vector.z / i;
  	return result;
  };

  parser::Vec3f scaler_multiplication(const parser::Vec3f& vector, float i){
      parser::Vec3f result;
      result.x = vector.x * i;
      result.y = vector.y * i;
      result.z = vector.z * i;
      return result;
  };

  parser::Vec3f multiplyVec3f(const parser::Vec3f& first, const parser::Vec3f& second){ // Return first vector * second vector.
      parser::Vec3f result;
      result.x = first.x * second.x;
      result.y = first.y * second.y;
      result.z = first.z * second.z;
      return result;
  };

  parser::Vec3f crossProduct(const parser::Vec3f& first, const parser::Vec3f& second){ // Return the dot product of 2 vectors.
  	parser::Vec3f result ;
    result.x = first.y*second.z - first.z*second.y;
    result.y = first.z*second.x - first.x*second.z;
    result.z = first.x*second.y - first.y*second.x;
    return result;
  };

  parser::Vec3i convert_float_to_int(const parser::Vec3f& vector){
      parser::Vec3i result;
      result.x = llround(vector.x);
      result.y = llround(vector.y);
      result.z = llround(vector.z);
      return result;
  };

  parser::Vec3f normalize(const parser::Vec3f vector){
    parser::Vec3f result;
    float length = 1.0f / (lengthOfVector(vector));
    result.x = vector.x * length;
    result.y = vector.y * length;
    result.z = vector.z * length;
    return result;
  };

}

#endif
