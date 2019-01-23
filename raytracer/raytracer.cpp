#include <iostream>
#include <cmath>
#include <string>
#include "parser.h"
#include "ppm.h"
#include "vecop.h"
#include "intersect.h"
#include "rayeq.h"

using namespace std;

typedef unsigned char RGB[3];




float locateFirstPixel_x(const parser::Vec3f& position, const parser::Vec3f& gaze, const parser::Vec3f up, const parser::Vec4f& near_plane, float near_distance, int image_width, int image_height , int i, float plane_side_vector_x, float width_pixel){
    float m = position.x + (gaze.x * near_distance); //
    float pixel_position_x = (m + (plane_side_vector_x * (abs(near_plane.x) - width_pixel/2)) + (up.x * (near_plane.w - width_pixel/2))) - (i * up.x * width_pixel);
    return pixel_position_x;
};

float locateFirstPixel_y(const parser::Vec3f& position, const parser::Vec3f& gaze, const parser::Vec3f& up, const parser::Vec4f& near_plane, float near_distance, int image_width, int image_height , int i, float plane_side_vector_y, float width_pixel){
    float m = position.y + (gaze.y * near_distance); //
    float pixel_position_y = (m + (plane_side_vector_y * (abs(near_plane.x) - width_pixel/2)) + (up.y * (near_plane.w - width_pixel/2))) - (i * up.y * width_pixel);
    return pixel_position_y;
};

float locateFirstPixel_z(const parser::Vec3f& position, const parser::Vec3f& gaze, const parser::Vec3f& up, const parser::Vec4f& near_plane, float near_distance, int image_width, int image_height , int i, float plane_side_vector_z, float width_pixel){
    float m = position.z + (gaze.z * near_distance); //
    float pixel_position_z = (m + (plane_side_vector_z * (abs(near_plane.x) - width_pixel/2)) + (up.z * (near_plane.w - width_pixel/2))) - (i * up.z * width_pixel);
    return pixel_position_z;
};



unsigned char* initialize(){

    parser::Scene scene;
    scene.loadFromXml("simple.xml"); // Read the xml file into the scene


    for(auto c : scene.cameras){
        parser::Camera camera = c;
        int w = camera.image_width;  // set w = # of pixels in width
        int h = camera.image_height; // set h = # of pixels in height

        unsigned char* image = new unsigned char [w * h * 3]; // image array
        parser::Vec3f plane_side_vector ; // plane_side_vector = UP vector of image plane(same with cameras UP vector) X GAZE vector
        plane_side_vector.x = (camera.up.y * camera.gaze.z) - (camera.up.z * camera.gaze.y);
        plane_side_vector.y = (camera.up.z * camera.gaze.x) - (camera.up.x * camera.gaze.z);
        plane_side_vector.z = (camera.up.x * camera.gaze.y) - (camera.up.y * camera.gaze.x);
        plane_side_vector = vecop::normalize(plane_side_vector);
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
                parser::Vec3f ray_direction = vecop::subVec3f(current_pixel, camera.position);
                parser::Vec3i shading_int = vecop::convert_float_to_int(rayeq::generate_ray(ray_origin, ray_direction,scene, scene.max_recursion_depth));

                if(shading_int.x > 255){image[((3*i)*w) + (3*k)] = 255;}
                else{image[((3*i)*w) + (3*k)] = shading_int.x;}
                if(shading_int.y > 255){image[((3*i)*w) + ((3*k)+1)] = 255;}
                else{image[((3*i)*w) + ((3*k)+1)] = shading_int.y;}
                if(shading_int.z > 255){image[((3*i)*w) + ((3*k)+2)] = 255;}
                else{image[((3*i)*w) + ((3*k)+2)] = shading_int.z;}
            }
        }
        //write_ppm((camera.image_name).c_str(), image, w, h);
        return image;
    }
}

int main(){
  unsigned char* image = initialize();



  //write_ppm("simple.txt", image, 800, 800);

  return 0;
}
