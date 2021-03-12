
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <list>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "c:\Users\belyn\uimages\stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "c:\Users\belyn\uimages\stb_image_write.h"
}

using namespace std;

class Timer
{
private:

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

uint8_t* cut2x(uint8_t* data, int x, int y, int n) {
	int width = x / 2;
	int height = y / 2;
	uint8_t* cutdata = new uint8_t[width * height * n];
	int i = 0;
	int j = 0;
	while (i < x * y * n) {
		int k = 0;
		int s = 0;
		while (k < x * n) {
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


float u(float x, float xj, float xj1, float uj, float uj1) {
	return (uj1 - uj) * (x - xj) / (xj1 - xj) + uj;
}

void linear_parallel_enlarge(uint8_t *cutdata, int width, int height,
									int n, int part,  uint8_t *data) {
	
	int j = width * height * n * part;
	int i = width * height * n / 4 * part; // width * height * n / 4, width * height * n / 2, width * height * n * 3 / 4,
	int end_i = i + width * height * n / 4; // width * height * n / 2, width * height * n * 3 / 4, width * height * n,
	while (i < end_i) {
		int k = 0;
		int s = 0;
		while (k < width * n) {
			data [j + s] = cutdata[i + k];
			data[j + s + 1] = cutdata[i + k + 1];
			data[j + s + 2] = cutdata[i + k + 2];
			k += 3;
			s += 6;
		}
		i += width * n;
		j += 4 * width * n;
	}
	for (int channel = 0; channel < n; ++channel) {
		i = width * height * n * part;
		end_i = i +  width * height * n;
		while (i < end_i) {
			int j = n;
			while (j < 2 * width * n - 3) {
				float res = abs(u(j, j - 1, j + 1,
					data[i + j - n + channel],
					data[i + j + n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
				j += n * 2;
			}
			if (j == 2 * width * n - 3) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + j - n + channel],
					data[i + j - 2 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
			}

			i += width * 4 * n;
		}
	}
	for (int channel = 0; channel < n; ++channel) {
		int actual_width = 2 * width * n;
		i = 0;
		while (i <= actual_width - 1) {
			j = (part * 2) * height / 4 + 1;
			//j = 1, 2 * height / 4 + 1, 4 * height / 4 + 1, 6 * height / 4 + 1
			while (j < (part + 1) * 2 * height / 4 - 1) {
				float res = abs(u(j, j - 1, j + 1,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j + 1) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				j += 2;
			}
			if (j == (part + 1) * 2 * height / 4 - 1) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j - 2) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				
			}
			i += n;
			
		}
		
	}

}


uint8_t* enlarge_2x(uint8_t* cutdata, int width, int height, int n) {	// �������� 
	uint8_t* data = new uint8_t[width * 2 * height * 2 * n];
	int i = 0;
	int j = 0;
	while (i < width * height * n) { 
		int k = 0;
		int s = 0;
		while (k < width * n) {
			data[j + s] = cutdata[i + k];
			data[j + s + 1] = cutdata[i + k + 1];
			data[j + s + 2] = cutdata[i + k + 2];
			k += 3;
			s += 6;
		}
		i += width * n;
		j += 4 * width * n;
	}
	for (int channel = 0; channel < n; ++channel) {
		int i = 0;
		while (i < width * height * 4 * n) {
			int j = n;
			while (j < width * 2 * n - 3) {
				float res = abs(u(j, j - 1, j + 1,
					data[i + j - n + channel],
					data[i + j + n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
				j += n * 2;
			}
			if (j == width * 2 * n - 3) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + j - n + channel],
					data[i + j - 2 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
			}
			i += width * 4 * n;
		}
	}
	for (int channel = 0; channel < n; ++channel) {
		int actual_width = width * n * 2;
		i = 0;
		while (i < actual_width - 1) {
			int j = 1;
			while (j < height * 2 - 1) {
				float res = abs(u(j, j - 1, j + 1,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j + 1) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				j += 2;
			}
			if (j == height * 2 - 1) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j - 2) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
			}
			i += n;
		}
	}
	return data;
}

float A(float x, float xj, float xj1, float xj2) {
	if (xj <= x && x <= xj1) {
		return (x - xj1) / (xj - xj2);
	}
	cout << "segmentation error";
	return -1;
}

float B(float x, float xj, float xj1, float xj2) {
	if (xj <= x && x <= xj1) {
		return (x - xj) / (xj1 - xj2);
	}
	cout << "segmentation error";
	return -1;
}

float C(float x, float xj, float xj1, float xj2) {
	if (xj <= x && x <= xj1) {
		return (x - xj2) / (xj - xj1);
	}
	cout << "segmentation error";
	return -1;
}

float left_u(float x, float xj, float xj1, float xj2, float uj, float uj1, float uj2) {
	float a = A(x, xj, xj1, xj2);
	float b = B(x, xj, xj1, xj2);
	float c = C(x, xj, xj1, xj2);
	return uj * a * c - uj1 * b * c + uj2 * a * b;
}

float D(float x, float xj, float xj1, float xj_) {
	if (xj <= x && x <= xj1) {
		return (x - xj1) / (xj - xj_);
	}
	cout << "segmentation error";
	return -1;
}

float E(float x, float xj, float xj1, float xj_) {
	if (xj <= x && x <= xj1) {
		return (x - xj) / (xj1 - xj_);
	}
	cout << "segmentation error";
	return -1;
}

float F(float x, float xj, float xj1, float xj_) {
	if (xj <= x && x <= xj1) {
		return (x - xj_) / (xj - xj1);
	}
	cout << "segmentation error";
	return -1;
}

float right_u(float x, float xj, float xj1, float xj_, float uj, float uj1, float uj_) {
	float d = D(x, xj, xj1, xj_);
	float e = E(x, xj, xj1, xj_);
	float f = F(x, xj, xj1, xj_);
	return uj * d * f - uj1 * f * e + uj_ * d * e;
}

void third_order_parallel_enlarge(uint8_t* cutdata, int width, int height,
	int n, int part, uint8_t* data) {
	int j = width * height * n * part;
	int i = width * height * n / 4 * part;
	int end_i = i + width * height * n / 4;
	while (i < end_i) {
		int k = 0;
		int s = 0;
		while (k < width * n) {
			data[j + s] = cutdata[i + k];
			data[j + s + 1] = cutdata[i + k + 1];
			data[j + s + 2] = cutdata[i + k + 2];
			k += 3;
			s += 6;
		}
		i += width * n;
		j += 4 * width * n;
	}
	for (int channel = 0; channel < n; ++channel) {
		i = width * height * n * part;
		end_i = i + width * height * n;
		while (i < end_i) {
			int j = n;
			while (j < width * n) {
				float res = abs(left_u(j, j - 1, j + 1, j + 2,
					data[i + j - n + channel],
					data[i + j + n + channel],
					data[i + j + 3 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
				j += n * 2;
			}
			while (j < width * 2 * n - 3) {
				float res = abs(right_u(j, j - 1, j + 1, j - 2,
					data[i + j - n + channel],
					data[i + j + n + channel],
					data[i + j - 3 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
				j += n * 2;
			}
			if (j == 2 * width * n - 3) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + j - n + channel],
					data[i + j - 2 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
			}
			i += width * 4 * n;
		}
	}
	for (int channel = 0; channel < n; ++channel) {
		int actual_width = width * n * 2;
		i = 0;
		while (i < actual_width - 1) {
			j = (part * 2) * height / 4 + 1;
			while (j < ((part + 1) * 2) * height / 8) {
				float res = abs(left_u(j, j - 1, j + 1, j + 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j + 1) + channel],
					data[i + actual_width * (j + 3) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				j += 2;
			}
			while (j < ((part + 1) * 2) * height / 4 - 1) {
				float res = abs(right_u(j, j - 1, j + 1, j - 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j + 1) + channel],
					data[i + actual_width * (j - 3) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				j += 2;
			}
			if (j == (part + 1) * 2 * height / 4 - 1) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j - 2) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
			}
			i += n;
		}
	}
}

uint8_t* enlarge_parallel(uint8_t* cutdata, int width, int height, int n, int method) {
	uint8_t* data = new uint8_t[width * 2 * height * 2 * n];
	vector<uint8_t*> data_parts;
	vector<thread> threads;
	for (int i = 0; i < 4; ++i) {
		if (method == 1) {
			threads.push_back(thread(linear_parallel_enlarge, ref(cutdata),
				width, height, n, i, ref(data)));
		}
		if (method == 2) {
			threads.push_back(thread(third_order_parallel_enlarge, ref(cutdata),
				width, height, n, i, ref(data)));
		}
	}
	for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
	return data;
}

uint8_t* enlarge2x_third_order(uint8_t* cutdata, int width, int height, int n) {
	uint8_t* data = new uint8_t[width * 2 * height * 2 * n];
	int i = 0;
	int j = 0;
	while (i < width * height * n) { // ��������������� ��������� �������
		int k = 0;
		int s = 0;
		while (k < width * n) {
			data[j + s] = cutdata[i + k];
			data[j + s + 1] = cutdata[i + k + 1];
			data[j + s + 2] = cutdata[i + k + 2];
			k += 3;
			s += 6;
		}
		i += width * n;
		j += 4 * width * n;
	}
	for (int channel = 0; channel < n; ++channel) {
		int i = 0;
		while (i < width * height * 4 * n) {
			int j = n;
			while (j < width * n) {
				float res = abs(left_u(j, j - 1, j + 1, j + 2,
					data[i + j - n + channel],
					data[i + j + n + channel],
					data[i + j + 3 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
				j += n * 2;
			}
			while (j < width * 2 * n - 3) {
				float res = abs(right_u(j, j - 1, j + 1, j - 2,
					data[i + j - n + channel],
					data[i + j + n + channel],
					data[i + j - 3 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
				j += n * 2;
			}
			if (j == 2 * width * n - 3) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + j - n + channel],
					data[i + j - 2 * n + channel]));
				if (res > 255) res = 255;
				data[i + j + channel] = (uint8_t)(res);
			}
			i += width * 4 * n;
		}
	}
	for (int channel = 0; channel < n; ++channel) {
		int actual_width = width * n * 2;
		i = 0;
		while (i < actual_width - n) {
			int j = 1;
			while (j < height) {
				float res = abs(left_u(j, j - 1, j + 1, j + 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j + 1) + channel],
					data[i + actual_width * (j + 3) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				j += 2;
			}
			while (j < height * 2 - 1) {
				float res = abs(right_u(j, j - 1, j + 1, j - 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j + 1) + channel],
					data[i + actual_width * (j - 3) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
				j += 2;
			}
			if (j == height * 2 - 1) {
				float res = abs(u(j, j - 1, j - 2,
					data[i + actual_width * (j - 1) + channel],
					data[i + actual_width * (j - 2) + channel]));
				if (res > 255) res = 255;
				data[i + actual_width * j + channel] = (uint8_t)(res);
			}
			i += n;
		}

	}
	return data;
}

void test() {
	
	int x, y, n;std::string filename = "C:\\Users\\belyn\\uimages\\vincent-van-gogh_road-with-cypresses-1890.jpg";
	unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 0);
	cout << x << " " << y << " " << n << endl;
	uint8_t* dataint = (uint8_t*)data;
	uint8_t* cutdata = cut2x(data, x, y, n);
	int width = x / 2;
	int height = y / 2;
	ofstream outf("simple_timefile.txt");
	for (int i = 0; i < 5; ++i) {
		Timer t1;
		uint8_t* enlarged_data = enlarge_2x(cutdata, x / 2, y / 2, n);
		outf << "time taken for enlarging with linear splines: " << t1.elapsed() << endl; // 0.29, 0.55, 0.3, 0.31, 0.51
		//stbi_write_jpg("simple_linear.jpg", x, y, 3, enlarged_data, 100);
		Timer t2;
		uint8_t* enlarged_data2 = enlarge2x_third_order(cutdata, x / 2, y / 2, n);
		outf << "time taken for enlarging with square splines: " << t2.elapsed() << endl; // 0.74, 1.21, 0.98, 0.92, 0.69  ~ 0.908
		//stbi_write_jpg("simple_square.jpg", width * 2, height * 2, 3, enlarged_data2, 100);
	}
}

void thread_test() {
	std::string filename = "C:\\Users\\belyn\\uimages\\vincent-van-gogh_road-with-cypresses-1890.jpg";
	int x, y, n;
	unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, 0);
	cout << x << " " << y << " " << n << endl;
	uint8_t* dataint = (uint8_t*)data;
	uint8_t* cutdata = cut2x(data, x, y, n);
	int width = x / 2;
	int height = y / 2;

	ofstream outf("parallel_timefile.txt");
	for (int i = 0; i < 5; ++i) {
		Timer t1;
		uint8_t* enlarged_data = enlarge_parallel(cutdata, x / 2, y / 2, n, 1);
		outf << "time taken for parallel enlarging with linear splines: " << t1.elapsed() << endl; // 0.29, 0.55, 0.3, 0.31, 0.51
		//stbi_write_jpg("parallel_linear.jpg", x, y, 3, enlarged_data, 100);
		Timer t2;
		uint8_t* enlarged_data2 = enlarge_parallel(cutdata, x / 2, y / 2, n, 2);
		outf << "time taken for parallel enlarging with square splines: " << t2.elapsed() << endl; // 0.29, 0.55, 0.3, 0.31, 0.51
		//stbi_write_jpg("parallel_square.jpg", x, y, 3, enlarged_data2, 100);
	}
}

void count_mistake(string original) {
	int x, y, n;
	unsigned char *data = stbi_load(original.c_str(), &x, &y, &n, 0);
	cout << x << " " << y << " " << n << endl;
	uint8_t* orig = (uint8_t*)data;
	
	ofstream outf("outfile.txt");
	vector<string> files = { "simple_linear.jpg", "simple_square.jpg", "parallel_linear.jpg", "parallel_square.jpg" };
	for (auto enl : files) {
		outf << enl << endl;
		int x1, y1, n1;
		unsigned char *data1 = stbi_load(enl.c_str(), &x1, &y1, &n1, 0);
		//cout << x << " " << y << " " << n << endl;
		uint8_t* enlarged = (uint8_t*)data1;
		uint8_t* mistake = new uint8_t[x * y * n];

		for (int channel = 0; channel < n; ++channel) {
			int i = 0;
			while (i < x * n * y) {
				int j = 0;
				while (j < x * n) {
					int res = (int)orig[i + j + channel] - (int)enlarged[i + j + channel];
					if (res < 0) { res = 0; }
					mistake[i + j + channel] = (uint8_t)res;
					j += n;
				}
				i += x * n;
			}
		}
		string name = "mistake_.jpg";
		name.insert(8, enl);
		stbi_write_jpg(name.c_str(), x, y, n, mistake, 100);

		int* string_mistakes = new int[n * x / 30]; // ������ �� ������ � ������ ������� �� 1/10 ������
		int* row_mistakes = new int[n * x / 60];	// ����� ������ �������, ������� �������������
		for (int channel = 0; channel < n; ++channel) {
			int i = x * n;	// ������, ������� ����������������� �� ��������
			int j = x * n * 2; // ������, � ������� ����������������� �������
			int k = 0;

			while (k < x * n / 30) {
				string_mistakes[k + channel] = abs((int)orig[i + k + channel] - (int)enlarged[i + k + channel]);
				k += n;
			}
			k = 0;
			int s = 3;
			while (k < n * x / 60) {
				row_mistakes[k + channel] = abs((int)orig[i + s + channel] - (int)enlarged[i + s + channel]);
				k += n;
				s += 2 * n;
			}
		}
		float string_average_red_mistake, string_average_green_mistake, string_average_blue_mistake;
		int sum1 = 0, sum2 = 0, sum3 = 0;
		for (int i = 0; i < x * n / 30; i += n) {
			sum1 += string_mistakes[i];
			sum2 += string_mistakes[i + 1];
			sum3 += string_mistakes[i + 2];
		}
		string_average_red_mistake = (float)sum1 / ((float)x / 10);
		string_average_green_mistake = (float)sum2 / ((float)x / 10);
		string_average_blue_mistake = (float)sum3 / ((float)x / 10);
		float string_pixel_mistake = string_average_red_mistake + string_average_green_mistake + string_average_blue_mistake;
		float row_average_red_mistake, row_average_green_mistake, row_average_blue_mistake;
		sum1 = 0, sum2 = 0, sum3 = 0;
		for (int i = 0; i < x * n / 60; i += n) {
			sum1 += row_mistakes[i];
			sum2 += row_mistakes[i + 1];
			sum3 += row_mistakes[i + 2];
		}
		row_average_red_mistake = (float)sum1 / ((float)x / 20);
		row_average_green_mistake = (float)sum2 / ((float)x / 20);
		row_average_blue_mistake = (float)sum3 / ((float)x / 20);

		float row_pixel_mistake = row_average_red_mistake + row_average_green_mistake + row_average_blue_mistake;
		outf << "string_average_red_mistake, string_average_green_mistake, string_average_blue_mistake: " <<
			string_average_red_mistake << " " << string_average_green_mistake << " " << string_average_blue_mistake << endl;
		outf << "string_pixel_mistake: " << string_pixel_mistake << endl;
		outf << "row_average_red_mistake, row_average_green_mistake, row_average_blue_mistake: " <<
			row_average_red_mistake << " " << row_average_green_mistake << " " << row_average_blue_mistake << endl;
		outf << "row_pixel_mistake: " << row_pixel_mistake << endl;
	}
}


int main()
{
	test();
	thread_test();
	string original = "vincent-van-gogh_road-with-cypresses-1890.jpg";	
	//count_mistake(original);
	
	return 0;
}
