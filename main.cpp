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

Model *model = NULL;
using json = nlohmann::json;

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = (up^z).normalize();
    Vec3f y = (z^x).normalize();
    Matrix res = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}

Matrix viewport(int x, int y, int w, int h, int depth) {
    
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f; // w/2
    m[1][3] = y+h/2.f; // h / 2
    m[2][3] = depth/2.f; // d / 2

    m[0][0] = w/2.f; // 3/8 w
    m[1][1] = h/2.f; // 3/8 h
    m[2][2] = depth/2.f;
    return m;
}

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
    const int camera_z_pos = cfg["camera_z_pos"];
    std::string diffuse_map_path = cfg["diffuse_map_path"];

    model->load_diffuse_map(diffuse_map_path);

    int* zbuffer = new int[width*height];
    for (int i = 0; i < width*height; ++i) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir = Vec3f(1,-1,1).normalize();
    Vec3f eye(1,1,camera_z_pos);
    Vec3f center(0,0,0);
    //Vec3f light_dir (0, 0, 1);
    //Vec3f camera(0,0,camera_z_pos);
    Matrix ModelView  = lookat(eye, center, Vec3f(0,1,0));
    Matrix Projection = Matrix::identity(4);
    Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth);
    Projection[3][2] = -1.f/(eye - center).norm();


    for (int i = 0; i < model->nfaces(); ++i)
    {   
        std::vector<int> face = model->face(i);
        Vec3f world_coords[3];
        Vec3i screen_coords[3];
        float intensity[3];
        for (int j = 0; j < 3; ++ j) {
            //world coordinates
            world_coords[j] = model->vert(face[j]);
            //project screen coords
            screen_coords[j] = Vec3f(ViewPort*Projection*ModelView*Matrix(world_coords[j]));
            //get light intensity
            Vec3f norm = model->vert_normal(face[j]);
            intensity[j] = norm * light_dir;
        }

        std::vector<Vec2i> vts;
        for (int vt_idx = 0; vt_idx < 3; ++vt_idx) {
            Vec2i vt = model->get_texture_uv(i, vt_idx);
            vts.push_back(vt);
        }
        triangle(screen_coords[0], screen_coords[1], screen_coords[2], vts[0], vts[1], vts[2], intensity, image, model, zbuffer, width, height);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
