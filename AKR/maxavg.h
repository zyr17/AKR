#pragma once
#include "geo.h"
#include "data.h"
namespace maxavg{
	class maxclass{
	public:
		inline bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar);
		inline double getenddis(const data::query &query, const geo::point &p);
		inline void updatereslength(data::detaildata &detail, double olddata, double newdata);
		inline data::detaildata tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
	};
	class avgclass{
	public:

	};
}