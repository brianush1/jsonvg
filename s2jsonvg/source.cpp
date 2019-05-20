#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

namespace status_code {
	constexpr int success = 0;
}

std::string encode(float* point) {
	std::ostringstream result;
	result << "[" << point[0] << ", " << point[1] << "]";
	return result.str();
}

bool collinear(float* a, float* b, float* c) {
	double slope1 = (a[1] - b[1]) / (a[0] - b[0]);
	double slope2 = (b[1] - c[1]) / (b[0] - c[0]);
	double slope3 = (a[1] - c[1]) / (a[0] - c[0]);
	double maxabsdif1 = abs(slope1 - slope2);
	double maxabsdif2 = abs(slope1 - slope3);
	double maxabsdif3 = abs(slope2 - slope3);
	double maxabsdif = maxabsdif1;
	if (maxabsdif2 > maxabsdif) maxabsdif = maxabsdif2;
	if (maxabsdif3 > maxabsdif) maxabsdif = maxabsdif3;
	if (maxabsdif < 0.002) {
		return true;
	}
	else {
		return false;
	}
}

int main(int argc, char** argv) {
	std::vector<std::string> args;
	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	if (args.size() != 2) {
		std::cout << "Usage: s2jsonvg input.svg output.jsonvg" << std::endl;
		return status_code::success;
	}
	std::ostringstream result;
	auto svg_image = nsvgParseFromFile(args[0].c_str(), "px", 96.0f);
	result << "jsonvg(0){\n";
	result << "\t\"width\": " << svg_image->width << ",\n";
	result << "\t\"height\": " << svg_image->height << ",\n";
	result << "\t\"content\": [\n";
	auto first = true;
	for (auto shape = svg_image->shapes; shape != nullptr; shape = shape->next) {
		for (auto path = shape->paths; path != nullptr; path = path->next) {
			if (first) {
				first = false;
			}
			else {
				result << ",\n";
			}
			result << "\t\t{\n";
			result << "\t\t\t\"segments\": [\n";
			auto points = path->pts;
			for (auto i = 0; i < path->npts - 1; i += 3) {
				auto p = &points[i * 2];
				if (i > 0) result << ",\n";
				bool is_line = collinear(&p[0], &p[2], &p[4])
					&& collinear(&p[2], &p[4], &p[6])
					&& collinear(&p[0], &p[2], &p[6])
					&& collinear(&p[0], &p[4], &p[6]);
				if (is_line) {
					result << "\t\t\t\t[" << encode(&p[0]) << ", " << encode(&p[6]) << "]";
				}
				else {
					result << "\t\t\t\t[" << encode(&p[0])
						<< ", " << encode(&p[2])
						<< ", " << encode(&p[4])
						<< ", " << encode(&p[6]) << "]";
				}
			}
			result << "\n\t\t\t],\n";
			result << "\t\t\t\"fill\": {\n";
			if (shape->fill.type == NSVG_PAINT_COLOR) {
				result << "\t\t\t\t\"type\": \"solid\",\n";
				uint32_t color = shape->fill.color;
				uint8_t red = color & 0xFF;
				uint8_t green = (color >> 8) & 0xFF;
				uint8_t blue = (color >> 16) & 0xFF;
				uint8_t alpha = (color >> 24) & 0xFF;
				auto r = (double)red / 255;
				auto g = (double)green / 255;
				auto b = (double)blue / 255;
				auto a = (double)alpha / 255;
				result << "\t\t\t\t\"color\": [" << r << ", " << g
					<< ", " << b << ", " << a << "]\n";
			}
			else {
				result << "\t\t\t\t\"type\": \"none\"\n";
			}
			result << "\t\t\t},\n";
			result << "\t\t\t\"stroke\": {\n";
			if (shape->stroke.type == NSVG_PAINT_COLOR) {
				result << "\t\t\t\t\"type\": \"solid\",\n";
				result << "\t\t\t\t\"location\": \"edge\",\n";
				auto join = "miter";
				switch (shape->strokeLineJoin) {
				case NSVG_JOIN_MITER:
					join = "miter";
					break;
				case NSVG_JOIN_BEVEL:
					join = "bevel";
					break;
				case NSVG_JOIN_ROUND:
					join = "round";
					break;
				}
				result << "\t\t\t\t\"join\": \"" << join << "\",\n";
				auto cap = "sharp";
				switch (shape->strokeLineCap) {
				case NSVG_CAP_BUTT:
				case NSVG_CAP_SQUARE:
					cap = "sharp";
					break;
				case NSVG_CAP_ROUND:
					cap = "round";
					break;
				}
				result << "\t\t\t\t\"cap\": \"" << cap << "\",\n";
				result << "\t\t\t\t\"weight\": \"" << shape->strokeWidth << "\",\n";
				uint32_t color = shape->stroke.color;
				uint8_t red = color & 0xFF;
				uint8_t green = (color >> 8) & 0xFF;
				uint8_t blue = (color >> 16) & 0xFF;
				uint8_t alpha = (color >> 24) & 0xFF;
				auto r = (double)red / 255;
				auto g = (double)green / 255;
				auto b = (double)blue / 255;
				auto a = (double)alpha / 255;
				result << "\t\t\t\t\"color\": [" << r << ", " << g
					<< ", " << b << ", " << a << "]\n";
			}
			else {
				result << "\t\t\t\t\"type\": \"none\"\n";
			}
			result << "\t\t\t}\n";
			result << "\t\t}";
		}
	}
	result << "\n\t]\n}";
	std::ofstream file(args[1]);
	file << result.str();
	return status_code::success;
}