#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec2f> vert_textures_;
	std::vector<Vec3f> vert_normals_;
	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int> > face_textures_;
	TGAImage diffuse_map_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f vert_normal(int i);
	std::vector<int> face(int idx);
	void load_diffuse_map(std::string filename);
	TGAColor diffuse(Vec2i uv);
	Vec2i get_texture_uv (int face_idx, int vert_idx);
	TGAImage get_diffuse_map ();
};

#endif //__MODEL_H__
