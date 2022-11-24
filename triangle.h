#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void line_sweeping_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, 
                                        Vec2i vt0, Vec2i vt1, Vec2i vt2, 
                                        float* intensity, TGAImage &image, Model* model, 
                                        int *zbuffer, int width, int height);

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, 
              Vec2i vt0, Vec2i vt1, Vec2i vt2, 
              float* intensity, TGAImage &image, Model* model, 
              int *zbuffer, int width, int height);