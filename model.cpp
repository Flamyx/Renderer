#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            //the line from .obj file is vertice coordinates
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f vt;
            for (int i = 0; i < 2; ++i) iss >> vt[i];
            vert_textures_.push_back(vt);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            for (int i=0;i<3;i++) iss >> n[i];
            vert_normals_.push_back(n);
        }
        else if (!line.compare(0, 2, "f ")) {
            //the line from .obj encodes face (triangle) with 3 vertice's indexes
            std::vector<int> f;
            std::vector<int> vt;
            int itrash, idx, vt_idx;
            iss >> trash;
            //each Face is coded as v1/vt1/rand v2/vt2/rand v3/vt3/rand
            while (iss >> idx >> trash >> vt_idx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                vt_idx--;
                f.push_back(idx);
                vt.push_back(vt_idx);
            }
            faces_.push_back(f);
            face_textures_.push_back(vt);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::vert_normal(int i) {
    return vert_normals_[i];
}

void Model::load_diffuse_map(std::string filename) {
    diffuse_map_.read_tga_file(filename.c_str());
    diffuse_map_.flip_vertically();
}

TGAImage Model::get_diffuse_map() {
    return diffuse_map_;
}

TGAColor Model::diffuse(Vec2i uv, float intensity) {
    TGAColor text = diffuse_map_.get(uv.x, uv.y);
    text = TGAColor(text.r*intensity, text.g*intensity, text.b*intensity, 255);
    return text;
}

Vec2i Model::get_texture_uv(int face_idx, int vert_idx) {
    int vt_idx = face_textures_[face_idx][vert_idx];
    Vec2f uv = vert_textures_[vt_idx];
    return Vec2i(uv.x * diffuse_map_.get_width(), uv.y * diffuse_map_.get_height());

}
