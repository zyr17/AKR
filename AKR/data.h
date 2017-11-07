#pragma once
#include <vector>
#include "geo.h"
namespace data {
	struct mappoint {
		std::vector<int> category;
		geo::point p;
		mappoint() {}
		mappoint(double x, double y) {
			p.x = x;
			p.y = y;
		}
	};
	struct query {
		std::vector<geo::point> start;
		std::vector<int> endcategory;
		std::vector<int> needcategory;
	};
	struct result {
		double maxlength;
		std::vector<double> length;
		std::vector<std::vector<int>> res;
	};
	struct detaildata {
		result res;
		std::vector<bool> category;
		bool operator< (const detaildata &k) const {
			return res.length < k.res.length;
		}
	};
}