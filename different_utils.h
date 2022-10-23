#include <random>
#include "tgaimage.h"

TGAColor get_random_tga_color() {
		std::random_device rd;
		//Mersen Twister on random seed
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni (0, 255);
		
		TGAColor rand(uni(rng), uni(rng), uni(rng), 255);
		return rand;
}