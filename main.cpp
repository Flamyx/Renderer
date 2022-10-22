#include <vector>
#include <cmath>
#include <random>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "triangle.h"

Model *model = NULL;
const int width  = 800;
const int height = 800;

TGAColor get_random_tga_color() {
		std::random_device rd;
		//Mersen Twister on random seed
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni (0, 255);
		
		TGAColor rand(uni(rng), uni(rng), uni(rng), 255);
		return rand;
}

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else model = new Model("./obj/african_head.obj");
    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < model->nfaces(); ++i)
    {   
        std::vector<int> face = model->face(i);
        //world coordinates
        Vec3f v0 = model->vert(face[0]);
        Vec3f v1 = model->vert(face[1]);
        Vec3f v2 = model->vert(face[2]);

        TGAColor color;
        //Vector product of two 3D vectors is a vector that is perpendicular to their surface!
        Vec3f A = v1 - v0, B = v2 - v0;
        Vec3f normal = (A ^ B).normalize();
        //Light Intensity is a dot product of normal with camera-to-triangle vector (vector of light)
        Vec3f light_dir (0, 0, -1);
        float intensity = light_dir * normal;
        if (intensity > 0) {
        color = TGAColor(255*intensity, 255*intensity,255*intensity, 255);
        }
        else continue;

        triangle(v0, v1, v2, image, color, width, height);
       //break;
    }
    
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
