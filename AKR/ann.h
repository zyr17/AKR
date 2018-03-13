#pragma once
#include <vector>
#include <ctime>
#include "data.h"
namespace ann {
	//TODO read paper
	//每次调用返回一个未返回过的ANN结果。使用方法2，每次计算中途点缓存结果减少重复计算
	class ann {
		std::vector<int> mindistance, endorder;
		std::vector<double> enddis;
		std::vector<data::mappoint> mappoints;
		data::query query;
		double centerr, minr, sigmar;
		geo::point center;
	public:
		ann(const std::vector<data::mappoint> &inmappoints, const data::query &inquery);
		static inline bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar);
		static inline double getenddis(const data::query &query, const geo::point &p);
		int nextsmallest(double nowbest = 1e100);
	};
}