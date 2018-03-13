#pragma once
#include "data.h"
#include "ann.h"
#include "heaphash.h"
class old {
public:
	static inline void updatereslength(data::detaildata &detail, double olddata, double newdata);
	static inline data::detaildata tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint);
	static data::result getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, int nowend, double nowbest);
	static std::vector<int> getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static std::vector<std::vector<int>> getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result trueway(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result greedyway(const std::vector<data::mappoint> &mappoints, const data::query &query);
};