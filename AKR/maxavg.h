#pragma once
#include "geo.h"
#include "data.h"
#include "ann.h"
namespace maxavg{
	struct heapunit{
		int p1, p2, pmid, linenum;
		double length;
		static heapunit makeheapunit(const std::vector<data::mappoint> &mappoints, int p1, int p2, int pmid, int linenum, const geo::point &startpoint);
		bool operator< (heapunit k) const{
			return length > k.length;
		}
	};
	class maxclass{
	public:
		static bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n);
		static double getenddis2(const data::query &query, const geo::point &p);
		static double getenddis(const data::query &query, const geo::point &p);
		static void updatereslength(data::detaildata &detail, double data);
		static data::detaildata* tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
		static data::result onenaivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints);
		static data::result naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static data::result naivegreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static data::result onebettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints);
		static data::result bettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static data::result bettergreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static double getminpassdis(const data::query &query, geo::point endpoint, geo::point midpoint);
		static void addtores(data::result &res, data::oneline *line);
		static bool smallthanlup(double res, double line, double lup);
	};
	class avgclass{
	public:
		static bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n);
		static double getenddis2(const data::query &query, const geo::point &p);
		static double getenddis(const data::query &query, const geo::point &p);
		static void updatereslength(data::detaildata &detail, double data);
		static data::detaildata* tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
		static data::result onenaivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints);
		static data::result naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static data::result naivegreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static data::result onebettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints);
		static data::result bettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static data::result bettergreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints);
		static double getminpassdis(const data::query &query, geo::point endpoint, geo::point midpoint);
		static void addtores(data::result &res, data::oneline *line);
		static bool smallthanlup(double res, double line, double lup);
	};
}