#include <iostream>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"
#include "triangle.h"

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
    int x0 = t0.x;
    int y0 = t0.y;
    int x1 = t1.x;
    int y1 = t1.y;
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;

        if (error2>dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;

        if (error2>dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

void line_sweeping_lower_triangle_by_border(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    int total_height = t2.y - t0.y;
    int segment_height = t1.y - t0.y + 1; //cannot be 0
    for (int y = t0.y; y <= t1.y; ++y)
    {
        //we look for proportion of the current y coordinate related to (t0, t2) and (t0, t1)
        float current_height = (y - t0.y);
        float alpha = current_height / total_height;
        float beta = current_height / segment_height;

        //we want to x for current y as (1. - beta) * t0 + beta * t2 (e.g. segment)
        //but we do not define '*' for float so we rewrite as t0 + (t2 - t0) * beta
        Vec2i segP = t0 + (t1 - t0) * beta; //t0 * (1. - beta) + t1 * beta;
        
        Vec2i totalP = t0 + (t2 - t0) * alpha; //t0 * (1. - alpha) + t2 * alpha;

        //!!!TODO: for some reason rewriting formula for P converts float ans to int(ans) - 1
        //Vec2i false_totalP = t0 * (1. - alpha) + t2 * alpha;
        //if (!(totalP == check_totalP))
        //{
        //    std::cout << "!!!!REWRITING POINT FORMULA GONE WRONG!!!!!!";
        //    std::cout << alpha << '\n';
        //    std::cout << t0 << t2;
        //    std::cout << totalP;
        //     std::cout << check_totalP;
        //     exit(1);
        //}
        //need to swap because we found left and right bounds for x and now just paint each point (x_j: x_lb->rb, y_i)
        if (segP.x > totalP.x) std::swap(segP, totalP);

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            image.set(x, y, color);
        }
    }
}

void line_sweeping_lower_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image, TGAColor color, int** zbuffer)
{
    int total_height = t2.y - t0.y;
    int segment_height = t1.y - t0.y + 1; //cannot be 0
    for (int y = t0.y; y <= t1.y; ++y)
    {
        //we look for proportion of the current y coordinate related to (t0, t2) and (t0, t1)
        float current_height = (y - t0.y);
        float alpha = current_height / total_height;
        float beta = current_height / segment_height;

        //we want to find x for current y as (1. - beta) * t0 + beta * t2 (e.g. segment)
        //but we do not define '*' for float so we rewrite as t0 + (t2 - t0) * beta
        Vec3i segP = t0 + Vec3f(t1 - t0) * beta; //t0 * (1. - beta) + t1 * beta;
        
        Vec3i totalP = t0 + Vec3f(t2 - t0) * alpha; //t0 * (1. - alpha) + t2 * alpha;

        if (segP.x > totalP.x) std::swap(segP, totalP);

        float total_width = totalP.x - segP.x;

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            float phi = segP.x == totalP.x ? 1 : (float) (x - segP.x) / total_width;
            Vec3i curP = Vec3f(segP) + Vec3f(totalP - segP) * phi;
            if (zbuffer[curP.x][y] < curP.z) {
                image.set(curP.x, y, color);
                zbuffer[curP.x][y] = curP.z;
            }
        }
    }
}


void line_sweeping_lower_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int** zbuffer, float intensity)
{
    int total_height = t2.y - t0.y;
    int segment_height = t1.y - t0.y + 1; //cannot be 0
    for (int y = t0.y; y <= t1.y; ++y)
    {
        //we look for proportion of the current y coordinate related to (t0, t2) and (t0, t1)
        float current_height = (y - t0.y);
        float alpha = current_height / total_height;
        float beta = current_height / segment_height;

        //we want to find x for current y as (1. - beta) * t0 + beta * t2 (e.g. segment)
        //but we do not define '*' for float so we rewrite as t0 + (t2 - t0) * beta
        Vec3i segP = t0 + Vec3f(t1 - t0) * beta; //t0 * (1. - beta) + t1 * beta;
        Vec3i totalP = t0 + Vec3f(t2 - t0) * alpha; //t0 * (1. - alpha) + t2 * alpha;
        
        Vec2i segUV = vt0 + (vt1 - vt0) * beta;
        Vec2i totalUV = vt0 + (vt2 - vt0) * alpha;

        if (segP.x > totalP.x) std::swap(segP, totalP);

        float total_width = totalP.x - segP.x;

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            float phi = segP.x == totalP.x ? 1 : (float) (x - segP.x) / total_width;
            Vec3i curP = Vec3f(segP) + Vec3f(totalP - segP) * phi;
            Vec2i uvP = segUV + (totalUV - segUV) * phi;
            TGAColor color = model->diffuse(uvP, intensity);
            if (zbuffer[curP.x][y] < curP.z) {
                image.set(curP.x, y, color);
                zbuffer[curP.x][y] = curP.z;
            }
        }
    }
}


void line_sweeping_higher_triangle_by_border(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    int total_height = t2.y - t0.y;
    int segment_height = t2.y - t1.y + 1; //cannot be 0
    for (int y = t1.y; y <= t2.y; ++y)
    {
        //we look for proportion of the current y coordinate related to (t0, t2) and !(t1, t2)! rather than (t0, t1) as in lower
        float current_height1 = y - t1.y;
        float current_height2 = y - t0.y;
        float alpha = current_height2 / total_height;
        float beta = current_height1 / segment_height;

        Vec2i segP = t1 + (t2 - t1) * beta; //t1 * (1. - beta) + t2 * beta;
        Vec2i totalP = t0 + (t2 - t0) * alpha; //t0 * (1. - alpha) + t2 * alpha;
        //need to swap because we found left and right bounds for x and now just paint each point (x_j: x_lb->rb, y_i)
        if (segP.x > totalP.x) std::swap(segP, totalP);

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            image.set(x, y, color);
        }
    }
}


void line_sweeping_higher_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image, TGAColor color, int** zbuffer)
{
    int total_height = t2.y - t0.y;
    int segment_height = t2.y - t1.y + 1; //cannot be 0
    for (int y = t1.y; y <= t2.y; ++y)
    {
        //we look for proportion of the current y coordinate related to (t0, t2) and !(t1, t2)! rather than (t0, t1) as in lower
        float current_height1 = y - t1.y;
        float current_height2 = y - t0.y;
        float alpha = current_height2 / total_height;
        float beta = current_height1 / segment_height;

        Vec3i segP = t1 + Vec3f(t2 - t1) * beta; //t1 * (1. - beta) + t2 * beta;
        Vec3i totalP = t0 + Vec3f(t2 - t0) * alpha; //t0 * (1. - alpha) + t2 * alpha;
        //need to swap because we found left and right bounds for x and now just paint each point (x_j: x_lb->rb, y_i)
        if (segP.x > totalP.x) std::swap(segP, totalP);

        float total_width = (totalP.x - segP.x);

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            //need to smh get z-coord for cur x -> compute proportion as phi make new point as segP + (totP - segP) * phi
            float phi = segP.x == totalP.x ? 1 : (float) (x - segP.x) / total_width;
            Vec3i curP = Vec3f(segP) + Vec3f(totalP - segP) * phi;
            if (zbuffer[curP.x][y] < curP.z) {
                image.set(curP.x, y, color);
                zbuffer[curP.x][y] = curP.z;
            }
        }
    }
}


void line_sweeping_higher_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int** zbuffer, float intensity)
{
    int total_height = t2.y - t0.y;
    int segment_height = t2.y - t1.y + 1; //cannot be 0
    for (int y = t1.y; y <= t2.y; ++y)
    {
        //we look for proportion of the current y coordinate related to (t0, t2) and !(t1, t2)! rather than (t0, t1) as in lower
        float current_height1 = y - t1.y;
        float current_height2 = y - t0.y;
        float alpha = current_height2 / total_height;
        float beta = current_height1 / segment_height;

        Vec3i segP = t1 + Vec3f(t2 - t1) * beta; //t1 * (1. - beta) + t2 * beta;
        Vec3i totalP = t0 + Vec3f(t2 - t0) * alpha; //t0 * (1. - alpha) + t2 * alpha;
        Vec2i segUV = vt1 + (vt2 - vt1) * beta;
        Vec2i totalUV = vt0 + (vt2 - vt0) * alpha;
        //need to swap because we found left and right bounds for x and now just paint each point (x_j: x_lb->rb, y_i)
        if (segP.x > totalP.x) std::swap(segP, totalP);

        float total_width = (totalP.x - segP.x);

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            //need to smh get z-coord for cur x -> compute proportion as phi make new point as segP + (totP - segP) * phi
            float phi = segP.x == totalP.x ? 1 : (float) (x - segP.x) / total_width;
            Vec3i curP = Vec3f(segP) + Vec3f(totalP - segP) * phi;
            Vec2i uvP = segUV + (totalUV - segUV) * phi;
            TGAColor color = model->diffuse(uvP, intensity);
            if (zbuffer[curP.x][y] < curP.z) {
                image.set(curP.x, y, color);
                zbuffer[curP.x][y] = curP.z;
            }
        }
    }
}


void line_sweeping_triangle_by_border(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int** zbuffer, float intensity)
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

        Vec2i segUV = lower_part ? vt0 + (vt1 - vt0) * beta : vt1 + (vt2 - vt1) * beta;
        Vec2i totalUV = vt0 + (vt2 - vt0) * alpha;

        if (segP.x > totalP.x) std::swap(segP, totalP);

        float total_width = totalP.x - segP.x;

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            float phi = segP.x == totalP.x ? 1 : (float) (x - segP.x) / total_width;
            Vec3i curP = Vec3f(segP) + Vec3f(totalP - segP) * phi;
            Vec2i uvP = segUV + (totalUV - segUV) * phi;
            TGAColor color = model->diffuse(uvP, intensity);
            if (zbuffer[curP.x][curP.y] < curP.z) {
                image.set(curP.x, curP.y, color);
                zbuffer[curP.x][curP.y] = curP.z;
            }
        }
    }
}


void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    //degenerate triangle excluded
    if (t0.y==t1.y && t0.y==t2.y) return;
    //draw filled triangle with line sweeping algo
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t1.y > t2.y) std::swap(t1, t2);
    if (t0.y > t1.y) std::swap(t0, t1);

    //separate triangle by horizontal line from t1 
    //draw lower triangle first then the above one

    line_sweeping_lower_triangle_by_border(t0, t1, t2, image, color);
    //change color (optional for checking) and fill the higher part fo the triangle
    //color.distort_rgb_vals(80);
    line_sweeping_higher_triangle_by_border(t0, t1, t2, image, color);
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image, TGAColor color, int** zbuffer) {
    //degenerate triangle excluded
    if (t0.y==t1.y && t0.y==t2.y) return;
    //draw filled triangle with line sweeping algo
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t1.y > t2.y) std::swap(t1, t2);
    if (t0.y > t1.y) std::swap(t0, t1);

    //separate triangle by horizontal line from t1 
    //draw lower triangle first then the above one

    line_sweeping_lower_triangle_by_border(t0, t1, t2, image, color, zbuffer);
    //change color (optional for checking) and fill the higher part fo the triangle
    //color.distort_rgb_vals(80);
    line_sweeping_higher_triangle_by_border(t0, t1, t2, image, color, zbuffer);
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int** zbuffer, float intensity) {
    //degenerate triangle excluded
    if (t0.y==t1.y && t0.y==t2.y) return;
    //draw filled triangle with line sweeping algo
    if (t0.y > t1.y) { std::swap(t0, t1); std::swap(vt0, vt1); }
    if (t1.y > t2.y) { std::swap(t1, t2); std::swap(vt1, vt2); }
    if (t0.y > t1.y) { std::swap(t0, t1); std::swap(vt0, vt1); }

    //separate triangle by horizontal line from t1 
    //draw lower triangle first then the above one

    line_sweeping_triangle_by_border(t0, t1, t2, vt0, vt1, vt2, image, model, zbuffer, intensity);
}


void triangle(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, int width, int height) {
    //convert vertices to 2-D and paint triangle with color fill
    Vec2i v0 ((t0.x + 1.) * width / 2., (t0.y + 1.) * height / 2.);
    Vec2i v1 ((t1.x + 1.) * width / 2., (t1.y + 1.) * height / 2.);
    Vec2i v2 ((t2.x + 1.) * width / 2., (t2.y + 1.) * height / 2.);

    triangle(v0, v1, v2, image, color);
}


void triangle(Vec3f t0, Vec3f t1, Vec3f t2, TGAImage &image, TGAColor color, int width, int height, int depth, int** zbuffer) {
    //convert world coords to screen coords with depth perception through z-buffering
    Vec3i v0 ((t0.x + 1.) * width / 2, (t0.y + 1.) * height / 2, (t0.z + 1.) * depth / 2);
    Vec3i v1 ((t1.x + 1.) * width / 2, (t1.y + 1.) * height / 2, (t1.z + 1.) * depth / 2);
    Vec3i v2 ((t2.x + 1.) * width / 2, (t2.y + 1.) * height / 2, (t2.z + 1.) * depth / 2);

    triangle(v0, v1, v2, image, color, zbuffer);
}


void triangle(Vec3f t0, Vec3f t1, Vec3f t2, Vec2i vt0, Vec2i vt1, Vec2i vt2, TGAImage &image, Model* model, int width, int height, int depth, int** zbuffer, float intensity) {
     //convert world coords to screen coords with depth perception through z-buffering and take color of textures from diffuse map
    Vec3i v0 ((t0.x + 1.) * width / 2, (t0.y + 1.) * height / 2, (t0.z + 1.) * depth / 2);
    Vec3i v1 ((t1.x + 1.) * width / 2, (t1.y + 1.) * height / 2, (t1.z + 1.) * depth / 2);
    Vec3i v2 ((t2.x + 1.) * width / 2, (t2.y + 1.) * height / 2, (t2.z + 1.) * depth / 2);
    triangle(v0, v1, v2, vt0, vt1, vt2, image, model, zbuffer, intensity);
}
