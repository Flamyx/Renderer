#include "tgaimage.h"
#include "geometry.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color);

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image, TGAColor color, int** zbuffer);

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

void triangle(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, int width, int height);

void triangle(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, int width, int height, int depth, int** zbuffer);