#include <cstdio>
#include <queue>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "bitmap_image.hpp"
using namespace std;

#define IMAGE_HEIGHT    50
#define IMAGE_WIDTH     100
#define OUTPUT_SIZE     17
#define SHIFT           6
#define CMP_DATA_SIZE   16

typedef unsigned int uint;
typedef struct { uint minX, minY, maxX, maxY; } DigitMeta;
typedef struct { bool bitmap[OUTPUT_SIZE][OUTPUT_SIZE];} Digit;
typedef pair<uint, uint> Point;
typedef bitmap_image Image;

bool visited[IMAGE_HEIGHT][IMAGE_WIDTH];
const string TEST_DIR = "test_data/";
const string FEED = "safecode_feed/";

int file_id[CMP_DATA_SIZE] = {8, 44, 10, 27, 11, 19, 21, 2, 4, 15, 56, 5, 14, 16, 1, 22};
int label[CMP_DATA_SIZE] =   {3, 3,  4,   4,  5,  5,  5, 6, 6,  7,  7, 8, 8,   8, 9, 9};
vector<DigitMeta> dm;

inline bool is_white(rgb_t color){
    return (color.red & color.green & color.blue) == 255;
}
void load_compare_data(){
    fstream f("meta", fstream::out);
    f << "bool compare_data[16][17][17] = {";
    for(int i = 0 ;i < CMP_DATA_SIZE; i ++){
        stringstream s;
        s << FEED << file_id[i] << ".bmp";
        Image image(s.str());
        s.str("");
        if (!image)exit(1);
        const unsigned int height = image.height();
        const unsigned int width  = image.width();
        f << "{";
        for (uint y = 0; y < height; y++){
            f << "{";
            for (uint x = 0; x < width; x++){
                rgb_t color;
                image.get_pixel(x, y, color);
                if(x)f << ", ";
                f << !is_white(color);
            }
            f << "}";
            if(y != height-1)f << ", ";
        }
        f << "}";
        if(i != CMP_DATA_SIZE-1)f << ", ";
    }
    f << "};";
}
int main(int argc, char ** argv){
    load_compare_data();
}

