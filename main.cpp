#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdlib.h>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "triangle.h"
#include "different_utils.h"

Model *model = NULL;
using json = nlohmann::json;

int main(int argc, char** argv) {
    //Configurable with .json - simple and effective!
    std::ifstream f(argv[1]);
    json cfg = json::parse(f);
    //a bit of manipulation to convert str to const char*
    std::string s_path = cfg["object_dir"];
    const char *path = s_path.c_str();
    model = new Model(path);

    const int width  = cfg["width"];
    const int height = cfg["height"];
    const int depth = cfg["depth"];
    bool clown_shading = cfg["clown_shading"];

    int** zbuffer = new int*[width];
    for (int i = 0; i < width; ++i) {
        zbuffer[i] = new int[height];
        for (int  j = 0; j < height; ++j) zbuffer[i][j] = std::numeric_limits<int>::min();
    }

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
        // !!! Because I'm going from A to B light direction is from (0, 0, 1) if B to A (B ^ A) then (0, 0, -1) !!!
        Vec3f normal = (A ^ B).normalize();
        //Light Intensity is a dot product of normal with camera-to-triangle vector (vector of light)
        Vec3f light_dir (0, 0, 1);
        float intensity = light_dir * normal;
        if (clown_shading) color = get_random_tga_color();
        else if (intensity > 0) {
        color = TGAColor(255*intensity, 255*intensity,255*intensity, 255);
        }
        else continue;

        triangle(v0, v1, v2, image, color, width, height, depth, zbuffer);
       //break;
    }
    
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
