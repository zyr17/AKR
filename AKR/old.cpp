#include "old.h"
namespace old {
	std::vector<int> getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query) {
		std::vector<int> res;
		for (auto &mappoint : mappoints) {
			unsigned long long ull = 1 << query.endcategory.size();
			for (auto i : query.endcategory) {
				for (auto j : mappoint.category)
					if (i == j) {
						ull /= 2;
						break;
					}
			}
			if (ull == 1) res.push_back(&mappoint - &mappoints[0]);
		}
		return res;
	}
	std::vector<std::vector<int>> getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query) {
		std::vector<std::vector<int>> res;
		std::vector<int> tmp;
		for (int i = 0; i < query.needcategory.size(); i++)
			res.push_back(tmp);
	}
	data::result trueway(const std::vector<data::mappoint> &mappoints, const data::query &query) {
		auto endpoints = getendpoints(mappoints, query);
		auto needpoints = getneedpoints(mappoints, query);
	}
	data::result greedyway(const std::vector<data::mappoint> &mappoints, const data::query &query) {

	}
}