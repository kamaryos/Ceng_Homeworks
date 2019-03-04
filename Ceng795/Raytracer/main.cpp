#include <iostream>
#include <fstream>
#include "vec.h"
#include "ray.h"
#include "shapes.h"
#include "camera.h"
#include "parser.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

vec3 color(const ray& r){
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5*(unit_direction.y + 1.0);
	return (1.0 - t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
}

void initialize(const std::string& filepath){

    Scene scene;
    scene.loadFromXml(filepath); // Read the xml file into the scene


    Camera camera = scene.cameras[0];
    int w = camera.image_width;  // set w = # of pixels in width
    int h = camera.image_height; // set h = # of pixels in height

    unsigned char* image = new unsigned char [w * h * 3]; // image array
    vec3 plane_side_vector = unit_vector(cross(camera.up,camera.gaze)) ; // plane_side_vector = UP vector of image plane(same with cameras UP vector) X GAZE vector

    float width_pixel = (abs(camera.near_plane.x) + abs(camera.near_plane.y)) / camera.image_width; // Width of one pixel

    vec3 ray_origin = camera.position;

    for(int i = 0; i<h; i++){ // For loops to set pixels left to right line by line
				vec3 firstPixel = Camera::locateFirstPixel(camera,i,plane_side_vector,width_pixel);
				for(int k = 0; k<w; k++){
						float kw = (k * width_pixel);
						vec3 current_pixel = firstPixel - plane_side_vector*kw ;

            vec3 ray_direction = current_pixel - camera.position;
						ray r(ray_origin,ray_direction);

            Vec3i shading_int(ray::generate_ray(r,scene,scene.max_recursion_depth));

            if(shading_int.x > 255){image[((3*i)*w) + (3*k)] = 255;}
            else{image[((3*i)*w) + (3*k)] = shading_int.x;}
            if(shading_int.y > 255){image[((3*i)*w) + ((3*k)+1)] = 255;}
            else{image[((3*i)*w) + ((3*k)+1)] = shading_int.y;}
            if(shading_int.z > 255){image[((3*i)*w) + ((3*k)+2)] = 255;}
            else{image[((3*i)*w) + ((3*k)+2)] = shading_int.z;}
        }
    }

		stbi_write_png(camera.image_name.c_str(),w,h,3,image,w*sizeof(char)*3);
}


int main(int argc, char* argv[]) {

	initialize(argv[1]);
	return 0;

}
