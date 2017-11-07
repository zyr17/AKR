#pragma once
#include <algorithm>
#include <vector>
namespace geo {
	struct point {
		double x, y;
		point() {}
		point(double x, double y) : x(x), y(y) {}
		point operator- (const point &b) const {
			return point(x - b.x, y - b.y);
		}
		double len2() const {
			return x * x + y * y;
		}
		double len() const {
			return sqrt(len2());
		}
	};
	point findcircle(std::vector<point> pt);
}