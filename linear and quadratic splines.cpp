#include <iostream>
#include<memory>
#include <list>
#include <chrono>
#include <sstream>
#include <string>

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
}

using namespace std;

class Timer
{
private:
    // Псевдонимы типов используются для удобного доступа к вложенным типам
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_t> m_beg;

public:
    Timer() : m_beg(clock_t::now())
    {
    }

    void reset()
    {
        m_beg = clock_t::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
};

uint8_t* cut2x(uint8_t* data, int x, int y, int n){
    int width = x / 2;
    int height = y / 2;
    uint8_t* cutdata = new uint8_t[width * height * n];
    int i = 0;
    int j = 0;
    while(i < x * y * n){
        int k = 0;
        int s = 0;
        while(k < x * n){
            cutdata[j + s] = data[i + k];
            cutdata[j + s + 1] = data[i + k + 1];
            cutdata[j + s + 2] = data[i + k + 2];
            k += 6;
            s += 3;
        }
        i += 2 * x * n;
        j += width * n;
    }
    return cutdata;
}


float u(float x, float xj, float xj1, float uj, float uj1){
    return (uj1 - uj) * (x - xj) / (xj1 - xj) + uj;
}

uint8_t* enlarge_2x(uint8_t* cutdata, int width, int height, int n){
    uint8_t* data = new uint8_t[width * 2 * height * 2 * n];
    int i = 0;
    int j = 0;
    while(i < width * height * n){ // восстанавливаем имеющиеся пиксели
        int k = 0;
        int s = 0;
        while(k < width * n){
            data[j + s] = cutdata[i + k];
            data[j + s + 1] = cutdata[i + k + 1];
            data[j + s + 2] = cutdata[i + k + 2];
            k += 3;
            s += 6;
        }
        i += width * n;
        j += 4 * width * n;
    }
    for(int channel = 0; channel < n; ++channel){
        int i = 0;
        while(i < width * height * 4 * n){
            int j = n;
            while(j < width * 2 * n - 3){
                float res = abs(u(j, j-1, j+1,
                                  data[i + j - n + channel],
                                  data[i + j + n + channel]));
                if( res > 255) res = 255;
                data[i + j + channel] = (uint8_t)(res);
                j += n * 2;
            }
            i += width * 4 * n;
        }
    }
    for(int channel = 0; channel < n; ++channel){
        int actual_width = width * n * 2;
        i = 0;
        while(i < actual_width - n){
            int j = 1;
            while(j < height * 2 - 1){
                float res = abs(u(j, j - 1, j + 1,
                                  data[i + actual_width * (j - 1) + channel],
                                  data[i + actual_width * (j + 1) + channel]));
                if(res > 255) res = 255;
                data[i + actual_width * j + channel] = (uint8_t)(res);
                j += 2;
            }
            i += n;
        }
    }
    return data;
}

float A(float x, float xj, float xj1, float xj2){
    if(xj <= x && x <= xj1){
        return (x - xj1) / (xj - xj2);
    }
    cout << "segmentation error";
    return -1;
}

float B(float x, float xj, float xj1, float xj2){
    if(xj <= x && x <= xj1){
        return (x - xj) / (xj1 - xj2);
    }
    cout << "segmentation error";
    return -1;
}

float C(float x, float xj, float xj1, float xj2){
    if(xj <= x && x <= xj1){
        return (x - xj2) / (xj - xj1);
    }
    cout << "segmentation error";
    return -1;
}

float left_u(float x, float xj, float xj1, float xj2, float uj, float uj1, float uj2){
    float a = A(x, xj, xj1, xj2);
    float b = B(x, xj, xj1, xj2);
    float c = C(x, xj, xj1, xj2);
    return uj * a * c - uj1 * b * c + uj2 * a * b;
}

float D(float x, float xj, float xj1, float xj_){
    if(xj <= x && x <= xj1){
        return (x - xj1) / (xj - xj_);
    }
    cout << "segmentation error";
    return -1;
}

float E(float x, float xj, float xj1, float xj_){
    if(xj <= x && x <= xj1){
        return (x - xj) / (xj1 - xj_);
    }
    cout << "segmentation error";
    return -1;
}

float F(float x, float xj, float xj1, float xj_){
    if(xj <= x && x <= xj1){
        return (x - xj_) / (xj - xj1);
    }
    cout << "segmentation error";
    return -1;
}

float right_u(float x, float xj, float xj1, float xj_, float uj, float uj1, float uj_){
    float d = D(x, xj, xj1, xj_);
    float e = E(x, xj, xj1, xj_);
    float f = F(x, xj, xj1, xj_);
    return uj * d * f - uj1 * f * e + uj_ * d * e;
}

uint8_t* enlarge2x_third_order(uint8_t* cutdata, int width, int height, int n){
    uint8_t* data = new uint8_t[width * 2 * height * 2 * n];
    int i = 0;
    int j = 0;
    while(i < width * height * n){ // восстанавливаем имеющиеся пиксели
        int k = 0;
        int s = 0;
        while(k < width * n){
            data[j + s] = cutdata[i + k];
            data[j + s + 1] = cutdata[i + k + 1];
            data[j + s + 2] = cutdata[i + k + 2];
            k += 3;
            s += 6;
        }
        i += width * n;
        j += 4 * width * n;
    }
    for(int channel = 0; channel < n; ++channel){
        int i = 0;
        while(i < width * height * 4 * n){
            int j = n;
            while(j < width * n){
                float res = abs(left_u(j, j - 1, j + 1, j + 2,
                                       data[i + j - n + channel],
                                       data[i + j + n + channel],
                                       data[i + j + 3 * n + channel]));
                if(res > 255) res = 255;
                data[i + j + channel]  = (uint8_t)(res);
                j += n * 2;
            }
            while(j < width * 2 * n - 3){
                float res = abs(right_u(j, j - 1, j + 1, j - 2,
                                        data[i + j - n + channel],
                                        data[i + j + n + channel],
                                        data[i + j - 3 * n + channel]));
                if(res > 255) res = 255;
                data[i + j + channel]  = (uint8_t)(res);
                j += n * 2;

            }
            i += width * 4 * n;
        }
    }
    for(int channel = 0; channel < n; ++channel){
        int actual_width = width * n * 2;
        i = 0;
        while(i < actual_width - n){
            int j = 1;
            while(j < height){
                float res = abs(left_u(j, j - 1, j + 1, j + 2,
                                       data[i + actual_width * (j - 1) + channel],
                                       data[i + actual_width * (j + 1) + channel],
                                       data[i + actual_width * (j + 3) + channel]));
                if(res > 255) res = 255;
                data[i + actual_width * j + channel] = (uint8_t)(res);
                j += 2;
            }
            while(j < height * 2 - 1){
                float res = abs(right_u(j, j - 1, j + 1, j - 2,
                                        data[i + actual_width * (j - 1) + channel],
                                        data[i + actual_width * (j + 1) + channel],
                                        data[i + actual_width * (j - 3) + channel]));
                if(res > 255) res = 255;
                data[i + actual_width * j + channel] = (uint8_t)(res);
                j += 2;
            }
            i += n;
        }

    }
    return data;
}

int main(int argc,char **argv)
{
#define CHANNEL_NUM 3
    std::string filename = "C:\\Users\\belyn\\uimages\\vincent-van-gogh_road-with-cypresses-1890.jpg";
    string filename2 = "C:\\Users\\belyn\\uimages\\monkey_teeth.jpg";

    // ... x = width, y = height, n = # 8-bit components per pixel
    int x,y,n;
    Timer t1;
    unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 0);
    cout<<x<<" "<<y<<" "<<n<<endl;
    uint8_t* dataint = (uint8_t*)data;
//    cout<< +dataint[288 + 9 + 0]<<" "<< +dataint[288 + 9 + 1]<<" "<< +dataint[288 + 9 + 2]<<endl;
    uint8_t* cutdata = cut2x(data, x, y, n);
    int width = x / 2;
    int height = y / 2;
//    stbi_write_jpg("cutimg.jpg", x / 2, y / 2, 3, cutdata, 100);
    Timer t2;
    uint8_t* enlarged_data = enlarge_2x(cutdata, x / 2, y / 2, n);
    cout<<"time taken for enlarging with linear splines: "<< t2.elapsed()<< endl; // 0.29, 0.55, 0.3, 0.31, 0.51
    stbi_write_jpg("enlargedimg1.jpg", x, y, 3, enlarged_data, 100);
//    uint8_t* enlarged_data2 = enlarge2x_third_order(cutdata, x / 2, y / 2, n);
//    cout<<"time taken for enlarging with square splines: "<< t2.elapsed()<< endl; // 0.74, 1.21, 0.98, 0.92, 0.69  ~ 0.908
//    stbi_write_jpg("enlarged correct road.jpg", width * 2, height * 2, 3, enlarged_data2, 100);
    cout<<"time taken for the whole process: " << t1.elapsed()<<endl; // 3.07, 4.03, 3.45, 3.1, 3.16



}
