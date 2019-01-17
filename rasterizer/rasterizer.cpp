#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>


Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;



/*
    Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++){
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;
        }
    }
}

/*
    Transformations, culling, rasterization are done here.
    You can define helper functions inside this file (rasterizer.cpp) only.
    Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/

Vec3 divideVec3WithScalar(Vec3 v, double c) {
    Vec3 result;
    result.x = v.x / c;
    result.y = v.y / c;
    result.z = v.z / c;

    return result;
}

void multiplyMatrixWithScalar(double r[4], double m[4], double c) {
    r[0] = m[0] * c;
    r[1] = m[1] * c;
    r[2] = m[2] * c;
    r[3] = m[3] * c;
}

Vec3 translation(Vec3 input, Translation translation){
    Vec3 r;
    r.x = input.x + translation.tx;
    r.y = input.y + translation.ty;
    r.z = input.z + translation.tz;
    r.colorId = input.colorId;
    return r;
}

Vec3 rotation(Vec3 input, Rotation rotation){
    double x = rotation.ux;
    double y = rotation.uy;
    double z = rotation.uz;
    double x_k = rotation.ux*rotation.ux;
    double y_k = rotation.uy*rotation.uy;
    double z_k = rotation.uz*rotation.uz;
    double L_div = 1.0f / (x_k+y_k+z_k);
    double L_sq = sqrt(x_k+y_k+z_k);
    double angle = (rotation.angle*3.14159)/180;
    double c = cos(angle);
    double s = sin(angle);
    double k[4][4] = {{(x_k +(y_k+z_k)*c)*L_div , (x*y*(1-c) - z*L_sq*s )*L_div , (x*z*(1-c) + y*L_sq*s )*L_div , 0},
                      {(x*y*(1-c) + z*L_sq*s )*L_div , (y_k +(x_k+z_k)*c)*L_div , (y*z*(1-c) - x*L_sq*s )*L_div , 0},
                      {(x*z*(1-c) - y*L_sq*s )*L_div , (y*z*(1-c) + x*L_sq*s )*L_div , (z_k +(x_k+y_k)*c)*L_div , 0},
                      {0 , 0 , 0 , 1}
                     };
    double v[4] = {input.x,input.y,input.z,1};
    double r[4];  multiplyMatrixWithVec4d(r,k,v);

    Vec3 result;
    result.x = r[0];
    result.y = r[1];
    result.z = r[2];
    result.colorId = input.colorId;

    return result;
}

Vec3 scale(Vec3 input, Scaling scaling){
    Vec3 r;
    r.x = input.x * scaling.sx;
    r.y = input.y * scaling.sy;
    r.z = input.z * scaling.sz;
    r.colorId = input.colorId;
    return r;
}

Vec3 view_transformation(double world_point[4], Camera cam){
    double temp1[4];
    double temp2[4];
    double temp3[4];
    double temp4[4];
    Vec3 transformed_point;
    // Camera Transformation Matrix
    double cam_transformation[4][4] = { {cam.u.x, cam.u.y, cam.u.z, (-1)*(cam.u.x*cam.pos.x + cam.u.y*cam.pos.y + cam.u.z*cam.pos.z)},
                                        {cam.v.x, cam.v.y, cam.v.z, (-1)*(cam.v.x*cam.pos.x + cam.v.y*cam.pos.y + cam.v.z*cam.pos.z)},
                                        {cam.w.x, cam.w.y, cam.w.z, (-1)*(cam.w.x*cam.pos.x + cam.w.y*cam.pos.y + cam.w.z*cam.pos.z)},
                                        {0,0,0,1}
                                      };
    // Perspective Projection Matrix
    double pers_projection[4][4]    = { {(2*cam.n)/(cam.r-cam.l), 0 , (cam.r+cam.l)/(cam.r-cam.l), 0},
                                        {0, (2*cam.n)/(cam.t-cam.b), (cam.t+cam.b)/(cam.t-cam.b), 0},
                                        {0, 0, (-1)*((cam.f + cam.n) / (cam.f - cam.n)), (-1)*((2*cam.f*cam.n)/(cam.f-cam.n))},
                                        {0,0,-1,0}
                                      };
    // Viewport Transformation Matrix

    double view_transformation[4][4] = {{(double(cam.sizeX)/2),0,0,((double(cam.sizeX)-1)/2)},
                                        {0,(double(cam.sizeY)/2),0,((double(cam.sizeY)-1)/2)},
                                        {0,0,0.5,0.5},
                                        {0,0,0,1}
                                       };
    multiplyMatrixWithVec4d(temp1,cam_transformation,world_point);
    multiplyMatrixWithVec4d(temp2,pers_projection,temp1);
    double div = 1.0f/temp2[3];
    multiplyMatrixWithScalar(temp3,temp2,div);
    multiplyMatrixWithVec4d(temp4,view_transformation,temp3);

    transformed_point.x = temp4[0];
    transformed_point.y = temp4[1];
    transformed_point.z = temp4[2];
    return transformed_point;
}

void draw(Vec3 first, Vec3 second){
    double slope = (second.y-first.y)/(second.x-first.x);
    int x1 = (int)first.x;
    int y1 = (int)first.y;
    Color color1 = colors[first.colorId];

    int x2 = (int)second.x;
    int y2 = (int)second.y;
    Color color2 = colors[second.colorId];

    double diff_r = abs(color1.r - color2.r);
    double diff_g = abs(color1.g - color2.g);
    double diff_b = abs(color1.b - color2.b);

    if(slope == 0){ //  If SLOPE = 0
        if(x2 > x1){
            double ratio_r = (diff_r) / (x2 - x1);
            double ratio_g = (diff_g) / (x2 - x1);
            double ratio_b = (diff_b) / (x2 - x1);
            for(int i = 0; i <= (x2-x1); i++){
                if(color1.r < color2.r){image[x1+i][y1].r = color1.r + (i * ratio_r);} // RED
                else{image[x1+i][y1].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1+i][y1].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1+i][y1].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1+i][y1].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1+i][y1].b = color1.b - (i * ratio_b);}
            }
        }
        else if(x2 < x1){
            double ratio_r = (diff_r) / (x1 - x2);
            double ratio_g = (diff_g) / (x1 - x2);
            double ratio_b = (diff_b) / (x1 - x2);
            for(int i = 0; i <= (x1-x2); i++){
                if(color1.r < color2.r){image[x1-i][y1].r = color1.r + (i * ratio_r);} // RED
                else{image[x1-i][y1].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1-i][y1].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1-i][y1].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1-i][y1].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1-i][y1].b = color1.b - (i * ratio_b);}
            }
        }
    }
    else if(x1 == x2){ // If SLOPE = INF
        if(y2 > y1){
            double ratio_r = (diff_r) / (y2 - y1);
            double ratio_g = (diff_g) / (y2 - y1);
            double ratio_b = (diff_b) / (y2 - y1);
            for(int i = 0; i <= (y2-y1); i++){
                if(color1.r < color2.r){image[x1][y1+i].r = color1.r + (i * ratio_r);} // RED
                else{image[x1][y1+i].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1][y1+i].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1][y1+i].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1][y1+i].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1][y1+i].b = color1.b - (i * ratio_b);}
            }
        }
        else if(y2 < y1){
            double ratio_r = (diff_r) / (y1 - y2);
            double ratio_g = (diff_g) / (y1 - y2);
            double ratio_b = (diff_b) / (y1 - y2);
            for(int i = 0; i <= (y1-y2); i++){
                if(color1.r < color2.r){image[x1][y1-i].r = color1.r + (i * ratio_r);} // RED
                else{image[x1][y1-i].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1][y1-i].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1][y1-i].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1][y1-i].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1][y1-i].b = color1.b - (i * ratio_b);}
            }
        }
    }
    else if(slope > 0 && slope <= 1){ // If  0 < SLOPE <= 1
        if(y2 > y1){
            int y = y1;
            int d = 2*(y1-y2)+(x2-x1);
            double ratio_r = (diff_r) / (x2 - x1);
            double ratio_g = (diff_g) / (x2 - x1);
            double ratio_b = (diff_b) / (x2 - x1);
            for(int i = 0; i <= (x2-x1); i++){
                if(color1.r < color2.r){image[x1+i][y].r = color1.r + (i * ratio_r);} // RED
                else{image[x1+i][y].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1+i][y].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1+i][y].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1+i][y].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1+i][y].b = color1.b - (i * ratio_b);}

                if(d<0){ // CHOOSE NE
                    y += 1;
                    d += 2 * ((y1-y2) + (x2-x1));
                }
                else{ // CHOOSE E
                    d += 2 * (y1 - y2);
                }
            }
        }
        else if(y2 < y1){
            int y = y1;
            int d = 2*(y2-y1)+(x1-x2);
            double ratio_r = (diff_r) / (x1 - x2);
            double ratio_g = (diff_g) / (x1 - x2);
            double ratio_b = (diff_b) / (x1 - x2);
            for(int i = 0; i <= (x1-x2); i++){
                if(color1.r < color2.r){image[x1-i][y].r = color1.r + (i * ratio_r);} // RED
                else{image[x1-i][y].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1-i][y].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1-i][y].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1-i][y].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1-i][y].b = color1.b - (i * ratio_b);}

                if(d<0){ // CHOOSE SW
                    y -= 1;
                    d += 2 * ((y2-y1) + (x1-x2));
                }
                else{ // CHOOSE W
                    d += 2 * (y2 - y1);
                }
            }
        }
    }
    else if(slope > 1){ // If 1 < SLOPE < INFINITY
        if(x2 > x1){
            int x = x1;
            int d = 2 * (x2-x1) + (y1-y2);
            double ratio_r = (diff_r) / (y2 - y1);
            double ratio_g = (diff_g) / (y2 - y1);
            double ratio_b = (diff_b) / (y2 - y1);
            for(int i = 0; i <= (y2-y1); i++){
                if(color1.r < color2.r){image[x][y1+i].r = color1.r + (i * ratio_r);} // RED
                else{image[x][y1+i].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x][y1+i].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x][y1+i].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x][y1+i].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x][y1+i].b = color1.b - (i * ratio_b);}
                if(d<0){ // CHOOSE N
                    d += 2 * (x2 - x1);
                }
                else{ // CHOOSE NE
                    x += 1 ;
                    d += 2 * ((x2-x1) + (y1-y2));
                }
            }
        }
        else if(x2 < x1){
            int x = x1;
            int d = 2 * (x1-x2) + (y2-y1);
            double ratio_r = (diff_r) / (y1 - y2);
            double ratio_g = (diff_g) / (y1 - y2);
            double ratio_b = (diff_b) / (y1 - y2);
            for(int i = 0; i <= (y1-y2); i++){
                if(color1.r < color2.r){image[x][y1-i].r = color1.r + (i * ratio_r);} // RED
                else{image[x][y1-i].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x][y1-i].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x][y1-i].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x][y1-i].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x][y1-i].b = color1.b - (i * ratio_b);}
                if(d<0){ // CHOOSE S
                    d += 2 * (x1 - x2);
                }
                else{ // CHOOSE SW
                    x -= 1;
                    d += 2 * ((x1-x2) + (y2-y1));
                }
            }
        } 
    }
    else if(slope >= (-1) && slope < 0){ // If  -1 <= SLOPE < 0
        if(x2 > x1){
            int y = y1;
            int d = 2 * (y1 - y2) + (x1 - x2);
            double ratio_r = (diff_r) / (x2 - x1);
            double ratio_g = (diff_g) / (x2 - x1);
            double ratio_b = (diff_b) / (x2 - x1);
            for(int i = 0; i <= (x2-x1); i++){
                if(color1.r < color2.r){image[x1+i][y].r = color1.r + (i * ratio_r);} // RED
                else{image[x1+i][y].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1+i][y].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1+i][y].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1+i][y].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1+i][y].b = color1.b - (i * ratio_b);}

                if(d<0){ // CHOOSE E
                    d += 2 * (y1 - y2);
                }
                else{ // CHOOSE SE
                    y -= 1;
                    d += 2 * ((y1 - y2) + (x1 - x2));
                }
            }
        }
        else if(x2 < x1){
            int y = y1;
            int d = 2 * (y2-y1) + (x2-x1);
            double ratio_r = (diff_r) / (x1 - x2);
            double ratio_g = (diff_g) / (x1 - x2);
            double ratio_b = (diff_b) / (x1 - x2);
            for(int i = 0; i <= (x1-x2); i++){
                if(color1.r < color2.r){image[x1-i][y].r = color1.r + (i * ratio_r);} // RED
                else{image[x1-i][y].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x1-i][y].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x1-i][y].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x1-i][y].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x1-i][y].b = color1.b - (i * ratio_b);}

                if(d<0){ // CHOOSE W
                    d += 2 * (y2 - y1);
                }
                else{ // CHOOSE NW
                    y += 1;
                    d += 2 * ((y2-y1) + (x2-x1));
                }
            }
        }
    }
    else if(slope < (-1)){ // If SLOPE < -1
        if(x2 > x1){
            int x = x1;
            int d = 2 * (x1-x2) + (y1-y2);
            double ratio_r = (diff_r) / (y1 - y2);
            double ratio_g = (diff_g) / (y1 - y2);
            double ratio_b = (diff_b) / (y1 - y2);
            for(int i = 0; i <= (y1-y2); i++){
                if(color1.r < color2.r){image[x][y1-i].r = color1.r + (i * ratio_r);} // RED
                else{image[x][y1-i].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x][y1-i].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x][y1-i].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x][y1-i].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x][y1-i].b = color1.b - (i * ratio_b);}

                if(d<0){ // CHOOSE SE
                    x += 1;
                    d += 2 * ((x1-x2) + (y1-y2));
                }
                else{ // CHOOSE S
                    d += 2 * (x1 - x2);
                }
            }
        }
        else if(x2 < x1){
            int x = x1;
            int d = 2 * (x2-x1) + (y2-y1);
            double ratio_r = (diff_r) / (y2 - y1);
            double ratio_g = (diff_g) / (y2 - y1);
            double ratio_b = (diff_b) / (y2 - y1);
            for(int i = 0; i <= (y2-y1); i++){
                if(color1.r < color2.r){image[x][y1+i].r = color1.r + (i * ratio_r);} // RED
                else{image[x][y1+i].r = color1.r - (i * ratio_r);}

                if(color1.g < color2.g){image[x][y1+i].g = color1.g + (i * ratio_g);} // GREEN
                else{image[x][y1+i].g = color1.g - (i * ratio_g);}

                if(color1.b < color2.b){image[x][y1+i].b = color1.b + (i * ratio_b);} // BLUE
                else{image[x][y1+i].b = color1.b - (i * ratio_b);}

                if(d<0){ // CHOOSE NW
                    x -= 1;
                    d += 2 * ((x2-x1) + (y2-y1));
                }
                else{ // CHOOSE N
                    d += 2 * (x2 - x1);
                }
            }
        }
    }
}

void midpoint(Vec3 first, Vec3 second, Vec3 third){
    draw(first, second); // Line between first and second
    draw(second, third); // Line between second and third
    draw(third, first);  // Line between third and first
}

bool backface_culling(Vec3 first, Vec3 second, Vec3 third, Vec3 v){

    Vec3 first_vector; 
    first_vector.x = second.x - first.x ; 
    first_vector.y = second.y - first.y ; 
    first_vector.z = second.z - first.z ;

    Vec3 second_vector;
    second_vector.x = third.x - first.x ; 
    second_vector.y = third.y - first.y ; 
    second_vector.z = third.z - first.z ;

    Vec3 normal_vector = crossProductVec3(first_vector,second_vector);
    Vec3 normalize_normal = normalizeVec3(normal_vector);

    int backface_culling = dotProductVec3(normalize_normal,v);

    if( backface_culling >= 0.005){
        return false;
    }
    else{
        return true;
    }
}

double triangle_rasterization(Vec3 first, Vec3 second, Vec3 third, Vec3 point){

  int function_1 = point.x*(first.y-second.y) + point.y*(second.x-first.x) + first.x*second.y - first.y*second.x;

  double function_2_div = 1.0f / (third.x*(first.y-second.y) + third.y*(second.x-first.x) + first.x*second.y - first.y*second.x);

  return (function_1 * function_2_div) ;
}

Color color_make(double alpha, double beta, double gamma, Color color1, Color color2, Color color3){

  double new_r = (alpha*color1.r + beta*color2.r + gamma*color3.r);
  double new_g = (alpha*color1.g + beta*color2.g + gamma*color3.g);
  double new_b = (alpha*color1.b + beta*color2.b + gamma*color3.b);

  Color color;

  color.r = new_r ; color.g = new_g ; color.b = new_b ;

  return color ;
}

void forwardRenderingPipeline(Camera cam) {
    for(auto modell : models){

        int num_triangle = modell.numberOfTriangles;
        int num_transformation = modell.numberOfTransformations;

        for(int i=0;i<num_triangle;i++){
            Vec3 point1 = vertices[modell.triangles[i].vertexIds[0]];
            Vec3 point2 = vertices[modell.triangles[i].vertexIds[1]];
            Vec3 point3 = vertices[modell.triangles[i].vertexIds[2]];

            for(int k=0; k<num_transformation; k++){
                if(modell.transformationTypes[k]=='t'){
                    point1 = translation(point1,translations[modell.transformationIDs[k]]);
                    point2 = translation(point2,translations[modell.transformationIDs[k]]);
                    point3 = translation(point3,translations[modell.transformationIDs[k]]);
                }
                else if(modell.transformationTypes[k]=='s'){
                    point1 = scale(point1,scalings[modell.transformationIDs[k]]);
                    point2 = scale(point2,scalings[modell.transformationIDs[k]]);
                    point3 = scale(point3,scalings[modell.transformationIDs[k]]);
                }
                else if(modell.transformationTypes[k]=='r'){
                    point1 = rotation(point1,rotations[modell.transformationIDs[k]]);
                    point2 = rotation(point2,rotations[modell.transformationIDs[k]]);
                    point3 = rotation(point3,rotations[modell.transformationIDs[k]]);
                }
            }

            if(backfaceCullingSetting == 1){
                Vec3 temp = addVec3(point1,point2);
                Vec3 temp2 = addVec3(temp,point3);
                Vec3 mid = divideVec3WithScalar(temp2, 3);
                Vec3 v = subtractVec3(mid,cam.pos);
                // Vec3 normal_v = normalizeVec3(v);
                bool flag = backface_culling(point1,point2,point3,v);
                if(flag == false){
                    continue;
                }
            }
            // Find first point's x,y values on the image plane and set the image array
            double world_point1[4];
            world_point1[0] = point1.x;
            world_point1[1] = point1.y;
            world_point1[2] = point1.z;
            world_point1[3] = 1;
            Vec3 result1 = view_transformation(world_point1,cam);
            result1.x = (round(result1.x));
            result1.y = (round(result1.y));
            result1.colorId = point1.colorId;

            // Find second point's x,y values on the image plane and set the image array
            double world_point2[4];
            world_point2[0] = point2.x;
            world_point2[1] = point2.y;
            world_point2[2] = point2.z;
            world_point2[3] = 1;
            Vec3 result2 = view_transformation(world_point2,cam);
            result2.x = (round(result2.x));
            result2.y = (round(result2.y));
            result2.colorId = point2.colorId;

            // Find third point's x,y values on the image plane and set the image array
            double world_point3[4];
            world_point3[0] = point3.x;
            world_point3[1] = point3.y;
            world_point3[2] = point3.z;
            world_point3[3] = 1;
            Vec3 result3 = view_transformation(world_point3,cam);
            result3.x = (round(result3.x));
            result3.y = (round(result3.y));
            result3.colorId = point3.colorId;

            if(modell.type == 0 ){ // Midpoint Algorithm
                midpoint(result1,result2,result3);
            }

            else{ // Triangle rasterization
                int x_min = std::min(result1.x,std::min(result2.x,result3.x));
                int x_max = std::max(result1.x,std::max(result2.x,result3.x));
                int y_min = std::min(result1.y,std::min(result2.y,result3.y));
                int y_max = std::max(result1.y,std::max(result2.y,result3.y));
                for(int j = y_min; j < y_max ; j++){
                    for(int i = x_min ; i < x_max ; i++){
                        Vec3 point;
                        point.x = i; point.y = j;
                        double alpha = triangle_rasterization(result2,result3,result1,point);
                        double beta  = triangle_rasterization(result3,result1,result2,point);
                        double gamma = triangle_rasterization(result1,result2,result3,point);
                        if(alpha >= 0 && beta >= 0 && gamma >= 0){
                            Color color = color_make(alpha,beta,gamma,colors[result1.colorId],colors[result2.colorId],colors[result3.colorId]);
                            image[(int)point.x][(int)point.y].r = color.r;
                            image[(int)point.x][(int)point.y].g = color.g;
                            image[(int)point.x][(int)point.y].b = color.b;
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
            for (j = 0; j < cameras[i].sizeX; j++) {
                delete image[j];
            }

            delete[] image;
        }

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}
