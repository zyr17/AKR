#pragma once
#include "geo.h"
#include "data.h"
namespace maxavg{
	class maxclass{
	public:
		static bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar);
		static double getenddis(const data::query &query, const geo::point &p);
		static void updatereslength(data::detaildata &detail, double olddata, double newdata);
		static data::detaildata tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
	};
	class avgclass{
	public:

	};
}