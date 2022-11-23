#include <iostream>
#include <vector>
#include "triangle.h"

void line_sweeping_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, float* intensity, TGAImage &image, Model* model, int* zbuffer, int width, int height)
{
    int total_height = t2.y - t0.y;
    for (int y=0; y <= total_height; ++y)
    {
        bool lower_part = y <= t1.y - t0.y && t0.y != t1.y ;
        int segment_height = lower_part? t1.y - t0.y: t2.y - t1.y;
        float alpha = (float) y / total_height;
        float beta = (float) (y - (lower_part ? 0 : t1.y - t0.y)) / segment_height;
        //std::cout << "lower part is " << lower_part << std::endl;
        Vec3i segP = lower_part ? t0 + Vec3f(t1 - t0) * beta : t1 + Vec3f(t2 - t1) * beta;
        Vec3i totalP = t0 + Vec3f(t2 - t0) * alpha;

        float seg_intensity = lower_part ? intensity[0] + (intensity[1] - intensity[0]) * beta : intensity[1] + (intensity[2] - intensity[1]) * beta;
        float total_intensity = intensity[0] + (intensity[2] - intensity[0]) * alpha;

        if (segP.x > totalP.x) { std::swap(segP, totalP); std::swap(seg_intensity, total_intensity); }

        float total_width = totalP.x - segP.x;

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            float phi = segP.x == totalP.x ? 1 : (float) (x - segP.x) / total_width;
            Vec3i curP = Vec3f(segP) + Vec3f(totalP - segP) * phi;
            float cur_intensity = seg_intensity + (total_intensity - seg_intensity) * phi;

            int idx = curP.x+curP.y*width;

            if (curP.x>=width||curP.y>=height||curP.x<0||curP.y<0) continue;

            if (zbuffer[idx] < curP.z) {
                image.set(curP.x, curP.y, TGAColor(255, 255, 255)*cur_intensity);
                zbuffer[idx] = curP.z;
            }
        }
    }
}


void triangle(Vec3i t0, Vec3i t1, Vec3i t2, float* intensity, TGAImage &image, Model* model, int *zbuffer, int width, int height) {
    //degenerate triangle excluded
    if (t0.y==t1.y && t0.y==t2.y) return;
    //draw filled triangle with line sweeping algo
    if (t0.y > t1.y) { std::swap(t0, t1); std::swap(intensity[0], intensity[1]); }
    if (t1.y > t2.y) { std::swap(t1, t2); std::swap(intensity[1], intensity[2]); }
    if (t0.y > t1.y) { std::swap(t0, t1); std::swap(intensity[0], intensity[1]); }

    //separate triangle by horizontal line from t1 
    //draw lower triangle first then the above one

    line_sweeping_triangle_by_border(t0, t1, t2, intensity, image, model, zbuffer, width, height);
}
