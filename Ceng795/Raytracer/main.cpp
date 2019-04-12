#include <iostream>
#include <fstream>
#include "vec.h"
#include "ray.h"
#include "shapes.h"
#include "camera.h"
#include "parser.h"
#include "scene.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int clamp(int rgb) { return ((rgb > 255) ? 255 : rgb);}

void initialize(const std::string& filepath){

  SceneRenderer scene_renderer(filepath.c_str());

  const Camera& camera =  scene_renderer.Cameras()[0];


  const int width  = camera.image_width;
  const int height = camera.image_height;
  Vec3i *pixels = new Vec3i[width * height];

  scene_renderer.SetUpScene(camera);

  scene_renderer.RenderImage(camera,pixels,0,height,width);


  unsigned char* image = new unsigned char[width * height * 3];

   int idx = 0;
   for (int i = 0; i < width; i++) {
     for (int j = 0; j < height; j++) {
       const Vec3i pixel = pixels[i * height + j];
       image[idx++] = clamp(pixel.x);
       image[idx++] = clamp(pixel.y);
       image[idx++] = clamp(pixel.z);
     }
   }

  delete[] pixels;

	stbi_write_png(camera.image_name.c_str(),width,height,3,image,width*sizeof(char)*3);
  delete[] image;
}


int main(int argc, char* argv[]) {

	initialize(argv[1]);
	return 0;

}
