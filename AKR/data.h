#pragma once
#include <vector>
#include "geo.h"
namespace init {
	extern std::vector<std::string> num2words;
}
namespace data {
	struct mappoint {
		std::vector<int> category;
		geo::point p;
		mappoint(){}
		mappoint(double x, double y){
			p.x = x;
			p.y = y;
		}
	};
	struct query {
		std::vector<geo::point> start;
		std::vector<int> endcategory;
		std::vector<int> needcategory;
		void write(std::string filename);
		std::string writestr();
	};
	struct result {
		double reslength;
		int endpoint;
		std::vector<double> length;
		std::vector<std::vector<int>> res;
		void write(std::string filename, query *query = nullptr);
		std::string writestr();
	};
	struct detaildata {
		result res;
		std::vector<bool> category;
	};
}