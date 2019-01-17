#include <iostream>
#include <cmath>
#include <string>
#include "parser.h"
#include "ppm.h"

using namespace std;

typedef unsigned char RGB[3];


float locateFirstPixel_x(parser::Vec3f position, parser::Vec3f gaze, parser::Vec3f up, parser::Vec4f near_plane, float near_distance, int image_width, int image_height , int i, float plane_side_vector_x, float width_pixel){
    float m = position.x + (gaze.x * near_distance); //
    float pixel_position_x = (m + (plane_side_vector_x * (abs(near_plane.x) - width_pixel/2)) + (up.x * (near_plane.w - width_pixel/2))) - (i * up.x * width_pixel);
    return pixel_position_x;
};

float locateFirstPixel_y(parser::Vec3f position, parser::Vec3f gaze, parser::Vec3f up, parser::Vec4f near_plane, float near_distance, int image_width, int image_height , int i, float plane_side_vector_y, float width_pixel){
    float m = position.y + (gaze.y * near_distance); //
    float pixel_position_y = (m + (plane_side_vector_y * (abs(near_plane.x) - width_pixel/2)) + (up.y * (near_plane.w - width_pixel/2))) - (i * up.y * width_pixel);
    return pixel_position_y;
};

float locateFirstPixel_z(parser::Vec3f position, parser::Vec3f gaze, parser::Vec3f up, parser::Vec4f near_plane, float near_distance, int image_width, int image_height , int i, float plane_side_vector_z, float width_pixel){
    float m = position.z + (gaze.z * near_distance); //
    float pixel_position_z = (m + (plane_side_vector_z * (abs(near_plane.x) - width_pixel/2)) + (up.z * (near_plane.w - width_pixel/2))) - (i * up.z * width_pixel);
    return pixel_position_z;
};

float dotProduct(parser::Vec3f first, parser::Vec3f second){ // Return the dot product of 2 vectors.
	float result = (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
	return result;
};

float lengthOfVector(parser::Vec3f vector){ // Return length of the vector.
    float result = sqrt(((vector.x)*(vector.x)) + ((vector.y)*(vector.y)) + ((vector.z)*(vector.z)));
    return result;
};

parser::Vec3f subVec3f(parser::Vec3f first, parser::Vec3f second){ // Return first vector - second vector.
	parser::Vec3f result;
	result.x = first.x - second.x;
	result.y = first.y - second.y;
	result.z = first.z - second.z;
	return result;
};

parser::Vec3f addVec3f(parser::Vec3f first, parser::Vec3f second){ // Return first vector + second vector.
	parser::Vec3f result;
	result.x = first.x + second.x;
	result.y = first.y + second.y;
	result.z = first.z + second.z;
	return result;
};

parser::Vec3i addVec3i(parser::Vec3i first, parser::Vec3i second){ // Return first vector + second vector.
    parser::Vec3i result;
    result.x = first.x + second.x;
    result.y = first.y + second.y;
    result.z = first.z + second.z;
    return result;
};

parser::Vec3f scaler_division(parser::Vec3f vector, float i){
	parser::Vec3f result;
	result.x = vector.x / i;
	result.y = vector.y / i;
	result.z = vector.z / i;
	return result;
};

parser::Vec3f scaler_multiplication(parser::Vec3f vector, float i){
    parser::Vec3f result;
    result.x = vector.x * i;
    result.y = vector.y * i;
    result.z = vector.z * i;
    return result;
};

parser::Vec3f multiplyVec3f(parser::Vec3f first, parser::Vec3f second){ // Return first vector * second vector.
    parser::Vec3f result;
    result.x = first.x * second.x;
    result.y = first.y * second.y;
    result.z = first.z * second.z;
    return result;
};

parser::Vec3f multiplyVec3if(parser::Vec3i first, parser::Vec3f second){ // Return first vector * second vector.
    parser::Vec3f result;
    result.x = first.x * second.x;
    result.y = first.y * second.y;
    result.z = first.z * second.z;
    return result;
};

parser::Vec3f crossProduct(parser::Vec3f first, parser::Vec3f second){ // Return the dot product of 2 vectors.
	parser::Vec3f result ;
  result.x = first.y*second.z - first.z*second.y;
  result.y = first.z*second.x - first.x*second.z;
  result.z = first.x*second.y - first.y*second.x;
  return result;
};

parser::Vec3i convert_float_to_int(parser::Vec3f vector){
    parser::Vec3i result;
    result.x = llround(vector.x);
    result.y = llround(vector.y);
    result.z = llround(vector.z);
    return result;
};

parser::Vec3f diffuse_of_point(parser::Vec3f light_position, parser::Vec3f light_intensity, parser::Vec3f normal, parser::Vec3f diffuse, parser::Vec3f intersection_position){
    parser::Vec3f result; // result RGB values.
    parser::Vec3f light_vector = subVec3f(light_position, intersection_position); // Vector from intersection point to light source.
    float lengthOfLightVector = lengthOfVector(light_vector);
    float cos_alpha = dotProduct(light_vector, normal) / (lengthOfLightVector * lengthOfVector(normal));
    if (cos_alpha<0){
        result.x=0;
        result.y=0;
        result.z=0;
        return result;
    }
    else{
        result.x = light_intensity.x * diffuse.x * cos_alpha / (lengthOfLightVector * lengthOfLightVector);
        result.y = light_intensity.y * diffuse.y * cos_alpha / (lengthOfLightVector * lengthOfLightVector);
        result.z = light_intensity.z * diffuse.z * cos_alpha / (lengthOfLightVector * lengthOfLightVector);
        if(result.x>255){result.x=255;}
        if(result.y>255){result.y=255;}
        if(result.z>255){result.z=255;}
        return result;
    }
};

parser::Vec3f specular_of_point(parser::Vec3f light_position, parser::Vec3f light_intensity, parser::Vec3f normal, parser::Vec3f specular, parser::Vec3f intersection_position, parser::Vec3f rayOrigin, float phong_exponent){
	parser::Vec3f result; // result RGB values.
    parser::Vec3f light_vector = subVec3f(light_position, intersection_position); // Vector from intersection point to light source.
    float lengthOfLightVector = lengthOfVector(light_vector);
    parser::Vec3f half_vector = scaler_division(addVec3f(light_vector, subVec3f(rayOrigin,intersection_position)), lengthOfVector(addVec3f(light_vector,subVec3f(rayOrigin, intersection_position))));
    float cos_alpha = dotProduct(normal, half_vector) / (lengthOfVector(half_vector) * lengthOfVector(normal));
    if(cos_alpha<0){
    	result.x = 0;
    	result.y = 0;
    	result.z = 0;
    	return result;
    }
    else{
    	result.x = (light_intensity.x * specular.x * pow(cos_alpha, phong_exponent)) / (lengthOfLightVector * lengthOfLightVector);
    	//cout << phong_exponent << endl;
    	result.y = (light_intensity.y * specular.y * pow(cos_alpha, phong_exponent)) / (lengthOfLightVector * lengthOfLightVector);
    	result.z = (light_intensity.z * specular.z * pow(cos_alpha, phong_exponent)) / (lengthOfLightVector * lengthOfLightVector);
    	if(result.x>255){result.x=255;}
        if(result.y>255){result.y=255;}
        if(result.z>255){result.z=255;}

        return result;
    }
};

parser::Vec4f rayIntersectsSphere(parser::Vec3f rayOrigin, parser::Vec3f rayDirection, parser::Vec3f sphereCenter, float sphereRadius){ // if ray intersects, return intersection point, if not 0,0,0,-1
	parser::Vec4f result;
  float a = dotProduct(rayDirection,rayDirection);
	float b = 2*dotProduct(rayDirection,(subVec3f(rayOrigin, sphereCenter)));
	float c = dotProduct(subVec3f(rayOrigin, sphereCenter),subVec3f(rayOrigin, sphereCenter))-(sphereRadius * sphereRadius);
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

parser::Vec4f rayIntersectsTriangle(parser::Vec3f point_a, parser::Vec3f point_b, parser::Vec3f point_c, parser::Vec3f rayDirection, parser::Vec3f rayOrigin){// check whether ray intersects with triangle
  float det_A = ((point_a.x-point_b.x)*((point_a.y-point_c.y)*rayDirection.z-(point_a.z-point_c.z)*rayDirection.y))
               -((point_a.x-point_c.x)*((point_a.y-point_b.y)*rayDirection.z-(point_a.z-point_b.z)*rayDirection.y))
               +(rayDirection.x*((point_a.y-point_b.y)*(point_a.z-point_c.z)-(point_a.y-point_c.y)*(point_a.z-point_b.z)));

  float beta  = (((point_a.x-rayOrigin.x)*((point_a.y-point_c.y)*rayDirection.z-(point_a.z-point_c.z)*rayDirection.y))
               -((point_a.x-point_c.x)*((point_a.y-rayOrigin.y)*rayDirection.z-(point_a.z-rayOrigin.z)*rayDirection.y))
               +(rayDirection.x*((point_a.y-rayOrigin.y)*(point_a.z-point_c.z)-(point_a.y-point_c.y)*(point_a.z-rayOrigin.z))))/det_A;

  float gamma = (((point_a.x-point_b.x)*((point_a.y-rayOrigin.y)*rayDirection.z-(point_a.z-rayOrigin.z)*rayDirection.y))
               -((point_a.x-rayOrigin.x)*((point_a.y-point_b.y)*rayDirection.z-(point_a.z-point_b.z)*rayDirection.y))
               +(rayDirection.x*((point_a.y-point_b.y)*(point_a.z-rayOrigin.z)-(point_a.y-rayOrigin.y)*(point_a.z-point_b.z))))/det_A;

  parser::Vec4f result;

  if((beta < 0) || (gamma < 0) || (beta+gamma > 1)){
    result.x = 0;
    result.y = 0;
    result.z = 0;
    result.w = -1;
    return result;
  }
  else{
   float t     = (((point_a.x-point_b.x)*((point_a.y-point_c.y)*(point_a.z-rayOrigin.z)-(point_a.z-point_c.z)*(point_a.y-rayOrigin.y)))
                 -((point_a.x-point_c.x)*((point_a.y-point_b.y)*(point_a.z-rayOrigin.z)-(point_a.z-point_b.z)*(point_a.y-rayOrigin.y)))
                 +((point_a.x-rayOrigin.x)*((point_a.y-point_b.y)*(point_a.z-point_c.z)-(point_a.y-point_c.y)*(point_a.z-point_b.z))))/det_A;

   result.x = rayOrigin.x + t*rayDirection.x;
   result.y = rayOrigin.y + t*rayDirection.y;
   result.z = rayOrigin.z + t*rayDirection.z;
   result.w = t;
   return result;
  }
};

parser::Vec4f1i rayIntersectsMesh(std::vector<parser::Face> faces, std::vector<parser::Vec3f> vertex_data, parser::Vec3f rayDirection, parser::Vec3f rayOrigin){
    int size_mesh = faces.size();
    parser::Vec4f1i result;
    result.x = 0;
    result.y = 0;
    result.z = 0;
    result.w = -1;
    result.i = 0;
    for(int i = 0 ; i < size_mesh ; i++){
        parser::Vec4f temp = rayIntersectsTriangle(vertex_data[faces[i].v0_id-1],vertex_data[faces[i].v1_id-1],vertex_data[faces[i].v2_id-1],rayDirection,rayOrigin);
        if((temp.w!=-1) && ((result.w == -1) || (temp.w < result.w))){
            result.x = temp.x;
            result.y = temp.y;
            result.z = temp.z;
            result.w = temp.w;
            result.i = i;
        }
    }
    return result;
};

parser::Vec4f1i rayIntersectsMesh_2(std::vector<parser::Face> faces, std::vector<parser::Vec3f> vertex_data, parser::Vec3f rayDirection, parser::Vec3f rayOrigin, float shadow_ray_epsilon){
    int size_mesh = faces.size();
    parser::Vec4f1i result;
    result.x = 0;
    result.y = 0;
    result.z = 0;
    result.w = -1;
    result.i = 0;
    for(int i = 0 ; i < size_mesh ; i++){
        parser::Vec4f temp = rayIntersectsTriangle(vertex_data[faces[i].v0_id-1],vertex_data[faces[i].v1_id-1],vertex_data[faces[i].v2_id-1],rayDirection,rayOrigin);
        if((temp.w > shadow_ray_epsilon) && (temp.w < 1)){
            result.x = temp.x;
            result.y = temp.y;
            result.z = temp.z;
            result.w = temp.w;
            result.i = i;
            break;
        }
    }
    return result;
};

bool is_object_between(parser::Vec3f ray_origin, parser::Vec3f light_position, parser::Scene scene){
    int number_of_spheres = scene.spheres.size();
    int number_of_triangles = scene.triangles.size();
    int number_of_meshes = scene.meshes.size();

    parser::Vec3f ray_direction = subVec3f(light_position, ray_origin);

    for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
        parser::Vec4f temp = rayIntersectsSphere(ray_origin, ray_direction, scene.vertex_data[scene.spheres[j].center_vertex_id-1], scene.spheres[j].radius);
        if((temp.w > scene.shadow_ray_epsilon) && (temp.w < 1)){return true;}
    }
    for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
        parser::Vec4f temp = rayIntersectsTriangle(scene.vertex_data[scene.triangles[t].indices.v0_id-1], scene.vertex_data[scene.triangles[t].indices.v1_id-1], scene.vertex_data[scene.triangles[t].indices.v2_id-1], ray_direction, ray_origin);
        if((temp.w > scene.shadow_ray_epsilon) && (temp.w < 1)){return true;}
    }
    for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
        parser::Vec4f1i temp = rayIntersectsMesh_2(scene.meshes[m].faces, scene.vertex_data, ray_direction, ray_origin, scene.shadow_ray_epsilon);
        if((temp.w > scene.shadow_ray_epsilon) && (temp.w < 1)){return true;}
    }
    return false;
};

parser::Vec3i generate_ray(parser::Vec3f ray_origin, parser::Vec3f ray_direction, parser::Scene scene, int max_recursion_depth){
    parser::Vec3i result;
    int number_of_spheres = scene.spheres.size();
    int number_of_triangles = scene.triangles.size();
    int number_of_meshes = scene.meshes.size();
    parser::Vec4f intersection_point_4; // intersection point with t (w=t)
    intersection_point_4.w = 0;
    parser::Object object; // object which we intersect
    parser::Vec3f intersection_point; // intersection point without t

    for(int j=0; j<number_of_spheres; j++){ // Check intersections with spheres.
        parser::Vec4f temp = rayIntersectsSphere(ray_origin, ray_direction, scene.vertex_data[scene.spheres[j].center_vertex_id-1], scene.spheres[j].radius);
        if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && (!(intersection_point_4.w) || temp.w < intersection_point_4.w) ){
            intersection_point_4 = temp;
            object.object_type = 1;
            object.material_id = scene.spheres[j].material_id;
            object.center_vertex_id = scene.spheres[j].center_vertex_id;
            object.radius = scene.spheres[j].radius;
        }
    }

    for(int t=0; t<number_of_triangles; t++){ // // Check intersections with triangles.
        parser::Vec4f temp = rayIntersectsTriangle(scene.vertex_data[scene.triangles[t].indices.v0_id-1], scene.vertex_data[scene.triangles[t].indices.v1_id-1], scene.vertex_data[scene.triangles[t].indices.v2_id-1], ray_direction, ray_origin);
        if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && (!(intersection_point_4.w) || temp.w < intersection_point_4.w)){
            intersection_point_4 = temp;
            object.object_type = 2; // Triangle
            object.material_id = scene.triangles[t].material_id;
            object.indices     = scene.triangles[t].indices;
        }
    }

    for(int m=0; m<number_of_meshes; m++){ // Check intersections with meshes.
        parser::Vec4f1i temp = rayIntersectsMesh(scene.meshes[m].faces, scene.vertex_data, ray_direction, ray_origin);
        if((temp.w != -1) && (temp.w > scene.shadow_ray_epsilon) && (!(intersection_point_4.w) || (temp.w < intersection_point_4.w))) {
            intersection_point_4.x = temp.x;
            intersection_point_4.y = temp.y;
            intersection_point_4.z = temp.z;
            intersection_point_4.w = temp.w;
            object.object_type = 3; // Mesh
            object.material_id = scene.meshes[m].material_id;
            object.indices     = scene.meshes[m].faces[temp.i];
        }
    }

    if(intersection_point_4.w != 0) { // If ray intersects with an object
        intersection_point.x = intersection_point_4.x;
        intersection_point.y = intersection_point_4.y;
        intersection_point.z = intersection_point_4.z;
        parser::Vec3f normal;
        if(object.object_type == 1){
            normal = subVec3f(intersection_point, scene.vertex_data[object.center_vertex_id-1]); // Normal vector of intersection point (for spheres)
            normal = scaler_division(normal,lengthOfVector(normal)); // make it  unit
        }
        else if(object.object_type == 2 || object.object_type == 3 ){
            normal = crossProduct((subVec3f(scene.vertex_data[object.indices.v1_id-1],scene.vertex_data[object.indices.v0_id-1])),(subVec3f(scene.vertex_data[object.indices.v2_id-1],scene.vertex_data[object.indices.v0_id-1])));
            normal = scaler_division(normal,lengthOfVector(normal)); // make it  unit
        }

        if(max_recursion_depth==0){ // No reflectance.
            result = convert_float_to_int(multiplyVec3f(scene.ambient_light, scene.materials[object.material_id-1].ambient));
            for(auto p_light : scene.point_lights){
                if(is_object_between(intersection_point, p_light.position, scene) == true){
                    break;
                }
                else {
                    parser::Vec3f diffuse = diffuse_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
                    parser::Vec3f specular = specular_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].specular, intersection_point, ray_origin, scene.materials[object.material_id-1].phong_exponent);
                    parser::Vec3f shading;
                    shading.x = diffuse.x + specular.x;
                    shading.y = diffuse.y + specular.y;
                    shading.z = diffuse.z + specular.z;
                    parser::Vec3i shading_int = convert_float_to_int(shading);
                    result = addVec3i(shading_int,result);
                }
            }
            return result;
        }

        else if(max_recursion_depth>0){ // Hit and reflect.
            result = convert_float_to_int(multiplyVec3f(scene.ambient_light, scene.materials[object.material_id-1].ambient));
            for(auto p_light : scene.point_lights){
                if(is_object_between(intersection_point, p_light.position, scene) == true){
                    break;
                }
                else {
                    parser::Vec3f diffuse = diffuse_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].diffuse, intersection_point);
                    parser::Vec3f specular = specular_of_point(p_light.position, p_light.intensity, normal, scene.materials[object.material_id-1].specular, intersection_point, ray_origin, scene.materials[object.material_id-1].phong_exponent);
                    parser::Vec3f shading;
                    shading.x = diffuse.x + specular.x;
                    shading.y = diffuse.y + specular.y;
                    shading.z = diffuse.z + specular.z;
                    parser::Vec3i shading_int = convert_float_to_int(shading);
                    result = addVec3i(shading_int, result);
                }
            }
            float temp = dotProduct(normal,scaler_multiplication(ray_direction,-1));
            parser::Vec3f new_ray_direction = addVec3f(ray_direction,scaler_multiplication(scaler_multiplication(normal,2),temp));
            return addVec3i(result, convert_float_to_int(multiplyVec3if(generate_ray(intersection_point, new_ray_direction, scene, max_recursion_depth-1), scene.materials[object.material_id-1].mirror)));
        }
    }

    else{ // If ray doesn't intersect set result to background color.
        result = scene.background_color;
        return result;
    }
}

int main(int argc, char* argv[]){
    parser::Scene scene;
    scene.loadFromXml(argv[1]); // Read the xml file into the scene
    for(auto c : scene.cameras){
        parser::Camera camera = c;
        int w = camera.image_width;  // set w = # of pixels in width
        int h = camera.image_height; // set h = # of pixels in height

        unsigned char* image = new unsigned char [w * h * 3]; // image array
        parser::Vec3f plane_side_vector ; // plane_side_vector = UP vector of image plane(same with cameras UP vector) X GAZE vector
        plane_side_vector.x = (camera.up.y * camera.gaze.z) - (camera.up.z * camera.gaze.y);
        plane_side_vector.y = (camera.up.z * camera.gaze.x) - (camera.up.x * camera.gaze.z);
        plane_side_vector.z = (camera.up.x * camera.gaze.y) - (camera.up.y * camera.gaze.x);
        float width_pixel = (abs(camera.near_plane.x) + abs(camera.near_plane.y)) / camera.image_width; // Width of one pixel

        parser::Vec3f ray_origin = camera.position;

        for(int i = 0; i<h; i++){ // For loops to set pixels left to right line by line
            float firstPixel_x = locateFirstPixel_x(camera.position, camera.gaze, camera.up, camera.near_plane, camera.near_distance,w,h,i,plane_side_vector.x, width_pixel); // get first pixel location (x) of the line
            float firstPixel_y = locateFirstPixel_y(camera.position, camera.gaze, camera.up, camera.near_plane, camera.near_distance,w,h,i,plane_side_vector.y, width_pixel); // get first pixel location (y) of the line
            float firstPixel_z = locateFirstPixel_z(camera.position, camera.gaze, camera.up, camera.near_plane, camera.near_distance,w,h,i,plane_side_vector.z, width_pixel); // get first pixel location (z) of the line
            for(int k = 0; k<w; k++){
              	parser::Vec3f current_pixel ;
                current_pixel.x = firstPixel_x - (plane_side_vector.x * (k * width_pixel));
                current_pixel.y = firstPixel_y - (plane_side_vector.y * (k * width_pixel));
                current_pixel.z = firstPixel_z - (plane_side_vector.z * (k * width_pixel));
                parser::Vec3f ray_direction = subVec3f(current_pixel,camera.position);
                parser::Vec3i shading_int = generate_ray(ray_origin, ray_direction,scene, scene.max_recursion_depth);

                if(shading_int.x > 255){image[((3*i)*w) + (3*k)] = 255;}
                else{image[((3*i)*w) + (3*k)] = shading_int.x;}
                if(shading_int.y > 255){image[((3*i)*w) + ((3*k)+1)] = 255;}
                else{image[((3*i)*w) + ((3*k)+1)] = shading_int.y;}
                if(shading_int.z > 255){image[((3*i)*w) + ((3*k)+2)] = 255;}
                else{image[((3*i)*w) + ((3*k)+2)] = shading_int.z;} 
            }
        }
        write_ppm((camera.image_name).c_str(), image, w, h);
    }
}
