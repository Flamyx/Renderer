#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

void triangle(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, int width, int height);

void triangle(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, int width, int height, int depth, int** zbuffer);

void triangle(Vec3f t0, Vec3f t1, Vec3f t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int width, int height, int depth, int** zbuffer, float intensity);

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int** zbuffer, float intensity);