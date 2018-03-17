#pragma once
#include "geo.h"
#include "data.h"
namespace maxavg{
	class maxclass{
	public:
		static bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n);
		static double getenddis(const data::query &query, const geo::point &p);
		static void updatereslength(data::detaildata &detail, double data);
		static data::detaildata* tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
		static data::result naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
	};
	class avgclass{
	public:
		static bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n);
		static double getenddis(const data::query &query, const geo::point &p);
		static void updatereslength(data::detaildata &detail, double data);
		static data::detaildata* tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
		static data::result naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
	};
}