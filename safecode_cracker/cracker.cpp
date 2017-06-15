#include <cstdio>
#include <queue>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "bitmap_image.hpp"
#include "compare_data.hpp"
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

//int file_id[CMP_DATA_SIZE] = {8, 44, 10, 27, 11, 19, 21, 2, 4, 15, 56, 5, 14, 16, 1, 22};
//int label[CMP_DATA_SIZE] =   {3, 3,  4,   4,  5,  5,  5, 6, 6,  7,  7, 8, 8,   8, 9, 9};
vector<DigitMeta> dm;

inline bool is_white(rgb_t color){
    return (color.red & color.green & color.blue) == 255;
}

DigitMeta bfs(Image& img, uint w, uint h, uint x, uint y, uint& size){
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    queue<Point> que;
    que.push(make_pair(x, y));
    uint minX = 1e5, minY = 1e5, maxX = 0, maxY = 0;
    visited[y][x] = true;
    while(!que.empty()){
        size ++ ;
        auto p = que.front(); que.pop();
        for(int i = 0 ;i < 4 ; i ++){
            uint x = p.first + dx[i];
            uint y = p.second + dy[i];
            rgb_t color;
            img.get_pixel(x, y, color);
            if(0 <= x && x < w && 0 <= y && y < h && !visited[y][x] && !is_white(color)){
                visited[y][x] = true;
                que.push(make_pair(x, y));
                if(minX > x)minX = x;
                if(minY > y)minY = y;
                if(maxX < x)maxX = x;
                if(maxY < y)maxY = y;
            }
        }
    }
    return {minX, minY, maxX, maxY};
}
void fill_white(Image& img, uint w, uint h, uint x, uint y){
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    queue<Point> que;
    que.push(make_pair(x, y));
    visited[y][x] = false;
    while(!que.empty()){
        auto p = que.front(); que.pop();
        for(int i = 0 ;i < 4 ; i ++){
            uint x = p.first + dx[i];
            uint y = p.second + dy[i];
            rgb_t color;
            img.get_pixel(x, y, color);
            if(0 <= x && x < w && 0 <= y && y < h && !is_white(color)){
                img.set_pixel(x, y, (rgb_t){255, 255, 255});
                visited[y][x] = false;
                que.push(make_pair(x, y));
            }
        }
    }
}

void preprocess(vector<Digit>& digits, char* filename){
    Image image(filename);
    if (!image)exit(1);
    stringstream s;
    const unsigned int height = image.height();
    const unsigned int width  = image.width();

    memset(visited, 0, sizeof(visited));
    for (uint y = 0; y < height; y++){
        for (uint x = 0; x < width; x++){
            rgb_t color;
            uint size = 0;
            image.get_pixel(x, y, color);

            if (!is_white(color) && !visited[y][x]){
                DigitMeta _dm = bfs(image, width, height, x, y, size);
                if(size >= 20)dm.push_back(_dm);
                else fill_white(image, width, height, x, y);
            }
        }
    }
    sort(dm.begin(), dm.end(), [](const DigitMeta& a, const DigitMeta &b){return a.minX < b.minX;});
    
    for(auto& d: dm){
        Digit digit;
        memset(digit.bitmap, 0, sizeof(digit.bitmap));
        for(uint y = 0 ; y <= d.maxY - d.minY ; y++)
            for(uint x = 0 ; x <= d.maxX - d.minX ; x++)
                digit.bitmap[y][x] = visited[d.minY + y][d.minX + x];
        digits.push_back(digit);
    }
}

void load_compare_data(vector<pair<Digit, int>>& compare_data){

    for(int i = 0 ;i < CMP_DATA_SIZE; i ++){
        Digit d;
        memcpy(d.bitmap, compare_bmp[i], 17*17);
        compare_data.push_back(make_pair(d, label[i]));
    }
}

void dump_vector_to_images(){
    vector<Image> p_images;
    stringstream s;
    for(auto& d: dm){
        Image img(OUTPUT_SIZE, OUTPUT_SIZE);
        img.set_all_channels(255, 255, 255);
        for(uint y = 0 ; y <= d.maxY - d.minY ; y++)
            for(uint x = 0 ; x <= d.maxX - d.minX ; x++)
                if(visited[d.minY + y][d.minX + x])
                    img.set_pixel(x, y, (rgb_t){0,0,0});
        p_images.push_back(img);
    }
    string fname;
    for(uint i = 0; i < p_images.size() ; i ++){
        s << i << ".bmp";
        p_images[i].save_image(s.str());
        s.str("");
    }
}
int recognize(vector<Digit>& digits, vector<pair<Digit, int>>& compare_data){
    int ret = 0;
    for(auto &digit: digits){
        int d = 0;
        double prob_mx = 0;
        for(auto& data: compare_data){
            double shift_mx = 0;
            for(int dx = -2 ;dx <= 2 ; dx ++){
                for(int dy = -2 ;dy <= 2 ; dy ++){
                    int cnt = 0, total = 0;
                    for(int y = 0 ;y < OUTPUT_SIZE; y ++){
                        for(int x = 0 ;x < OUTPUT_SIZE; x ++){
                            if(0 <= y + dy && y + dy < OUTPUT_SIZE && 0 <= x + dx && x + dx <= OUTPUT_SIZE){
                                total ++;
                                cnt += (digit.bitmap[y+dy][x+dx] == data.first.bitmap[y][x]);
                            }
                        }
                    }
                    double m = (double)cnt/total;
                    if(m > shift_mx)shift_mx = m;
                }
            }
            if(shift_mx > prob_mx){
                prob_mx = shift_mx;
                d = data.second;
            }
        }
        ret = ret * 10 + d;
    }
    return ret;
}

int main(int argc, char ** argv){
    if(argc < 2)return 1;
    vector<Digit> digits;
    vector<pair<Digit, int>> compare_data; 
    preprocess(digits, argv[1]);
    load_compare_data(compare_data);
    int safecode = recognize(digits, compare_data);
    cout << safecode;
}

