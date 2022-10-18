#include <iostream>
#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
const int width = 200;
const int height = 200;
Model *model = NULL;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
    int x0 = t0.u;
    int y0 = t0.v;
    int x1 = t1.u;
    int y1 = t1.v;
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
        Vec2i segP = t0 + (t1 - t0) * beta;
        
        Vec2i totalP = t0 + (t2 - t0) * alpha;

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

        Vec2i segP = t1 + (t2 - t1) * beta;
        Vec2i totalP = t0 + (t2 - t0) * alpha;
        //need to swap because we found left and right bounds for x and now just paint each point (x_j: x_lb->rb, y_i)
        if (segP.x > totalP.x) std::swap(segP, totalP);

        for (int x = segP.x; x <= totalP.x; ++x)
        {
            image.set(x, y, color);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    //draw filled triangle with line sweeping algo
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t1.y > t2.y) std::swap(t1, t2);
    if (t0.y > t1.y) std::swap(t0, t1);

    //separate triangle by horizontal line from t1 
    //draw lower triangle first then the above one

    line_sweeping_lower_triangle_by_border(t0, t1, t2, image, color);
    //change color and fill the higher part fo the triangle
    color.distort_rgb_vals(80);
    line_sweeping_higher_triangle_by_border(t0, t1, t2, image, color);
}

int main(int argc, char** argv)
{
    if (argc == 2) model = new Model(argv[1]);
    else model = new Model("obj/african_head.obj");

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    TGAImage image = TGAImage(width, height, TGAImage::RGB);

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, blue);
    image.flip_vertically();
    image.write_tga_file("triangles.tga");
    delete model;
    return 0;
}