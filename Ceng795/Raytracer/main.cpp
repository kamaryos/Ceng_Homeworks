#include <iostream>
#include <fstream>
#include "vec.h"
#include "ray.h"
#include "shapes.h"
#include "camera.h"
#include "parser.h"

vec3 color(const ray& r){
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5*(unit_direction.y + 1.0);
	return (1.0 - t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
}
//
// unsigned char* initialize(){
//
//     parser::Scene scene;
//     scene.loadFromXml("simple.xml"); // Read the xml file into the scene
//
//
//     for(auto c : scene.cameras){
//         Camera camera = c;
//         int w = camera.image_width;  // set w = # of pixels in width
//         int h = camera.image_height; // set h = # of pixels in height
//
//         unsigned char* image = new unsigned char [w * h * 3]; // image array
//         vec3 plane_side_vector = unit_vector(cross(camera.up,camera.gaze)) ; // plane_side_vector = UP vector of image plane(same with cameras UP vector) X GAZE vector
//
//         float width_pixel = (abs(camera.near_plane.x) + abs(camera.near_plane.y)) / camera.image_width; // Width of one pixel
//
//         vec3 ray_origin = camera.position;
//
//         for(int i = 0; i<h; i++){ // For loops to set pixels left to right line by line
// 						vec3 firstPixel = Camera::locateFirstPixel(camera,i,plane_side_vector,width_pixel);
// 						for(int k = 0; k<w; k++){
// 								float kw = (k * width_pixel);
// 								vec3 current_pixel = firstPixel - plane_side_vector*kw ;
//
//                 vec3 ray_direction = current_pixel - camera.position;
//
//                 Vec3i shading_int(ray_direction);
//
//                 if(shading_int.x > 255){image[((3*i)*w) + (3*k)] = 255;}
//                 else{image[((3*i)*w) + (3*k)] = shading_int.x;}
//                 if(shading_int.y > 255){image[((3*i)*w) + ((3*k)+1)] = 255;}
//                 else{image[((3*i)*w) + ((3*k)+1)] = shading_int.y;}
//                 if(shading_int.z > 255){image[((3*i)*w) + ((3*k)+2)] = 255;}
//                 else{image[((3*i)*w) + ((3*k)+2)] = shading_int.z;}
//             }
//         }
//         //write_ppm((camera.image_name).c_str(), image, w, h);
//         return image;
//     }
// }


int main() {

	int nx = 1024;
	int ny = 1024;

	std::ofstream file;
	file.open("main.ppm");
	file << "P3\n" << nx << " " << ny << "\n255\n";
	vec3 lower_left_corner(-2.0,-1.0,-1.0);
	vec3 horizontal(4.0,0.0,0.0);
	vec3 vertical(0.0,2.0,0.0);
	vec3 origin(0.0,0.0,0.0);

	Face face(horizontal,vertical,lower_left_corner);
	Triangle triangle(1,face);
	for(int j = ny-1 ; j >=0 ; j--) {
		for(int i = 0 ; i < nx ; i++){

			float u = float(i)/float(nx);
			float v = float(j)/float(ny);
			ray r(origin,lower_left_corner+u*horizontal+v*vertical);
			vec3 col  = color(r);
			int  ir = int(255.99*col.x);
			int  ig = int(255.99*col.y);
			int  ib = int(255.99*col.x);
			file << ir << " " << ig << " " << ib << "\n";
		}
	}

	file.close();

}
