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

Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
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

    int** zbuffer = new int*[width];
    for (int i = 0; i < width; ++i) {
        zbuffer[i] = new int[height];
        for (int  j = 0; j < height; ++j) zbuffer[i][j] = std::numeric_limits<int>::min();
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir (0, 0, 1);
    Vec3f camera(0,0,camera_z_pos);

    Matrix Projection = Matrix::identity(4);
    Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth);
    Projection[3][2] = -1.f/camera.z;


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
        
        float intensity = normal * light_dir;
        if (intensity <= 0) continue;

        //project screen coords
        v0 = m2v(ViewPort * Projection * v2m(v0));
        v1 = m2v(ViewPort * Projection * v2m(v1));
        v2 = m2v(ViewPort * Projection * v2m(v2));

        std::vector<Vec2i> vts;
        for (int vt_idx = 0; vt_idx < 3; ++vt_idx) {
            Vec2i vt = model->get_texture_uv(i, vt_idx);
            vts.push_back(vt);
         }
        
        triangle(v0, v1, v2, vts[0], vts[1], vts[2], image, model, zbuffer, intensity);
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
