#include "old.h"
namespace old {
	data::result getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, int nowend, double nowbest) {
		geo::point endpoint = mappoints[nowend].p;
		data::result tres;
		tres.maxlength = 1e100;
		std::vector<data::detaildata> heap;
		data::detaildata tdetail;
		tdetail.category.resize(query.needcategory.size());
		tdetail.res.maxlength = 0;
		tdetail.res.endpoint = nowend;
		for (auto i : query.start) {
			std::vector<int> tvec;
			tdetail.res.res.push_back(tvec);
			double tdouble = (i - mappoints[nowend].p).len();
			tdetail.res.length.push_back(tdouble);
			if (tdouble > tdetail.res.maxlength)
				tdetail.res.maxlength = tdouble;
		}
		heap.push_back(tdetail);
		for (; heap.size(); ) {
			auto nowdetail = heap[0];
			std::pop_heap(heap.begin(), heap.end());
			heap.pop_back();
			if (nowdetail.res.maxlength > nowbest)
				continue;
			bool haszero = 0;
			for (int i = 0; i < needpoints.size(); i ++ )
				if (nowdetail.category[i] == 0) {
					haszero = 1;
					for (auto j : needpoints[i]) 
						for (int k = 0; k < query.start.size(); k++) {
							geo::point p = query.start[k];
							if (nowdetail.res.res[k].size() != 0)
								p = mappoints[*(--nowdetail.res.res[k].end())].p;
							geo::point midpoint = mappoints[j].p;
							double nowlength = nowdetail.res.length[k] - (p - endpoint).len() + (p - midpoint).len() + (midpoint - endpoint).len();
							if (nowlength > nowbest) continue;
							auto adddetail = nowdetail;
							adddetail.res.length[k] = nowlength;
							if (nowlength > adddetail.res.maxlength)
								adddetail.res.maxlength = nowlength;
							adddetail.res.res[k].push_back(j);
							for (int detailnum = 0; detailnum < adddetail.category.size(); detailnum++)
								if (adddetail.category[detailnum] == 0)
									for (auto have : mappoints[j].category)
										if (have == query.needcategory[detailnum]) {
											adddetail.category[detailnum] = 1;
											break;
										}
							heap.push_back(adddetail);
							std::push_heap(heap.begin(), heap.end());
						}
				}
			if (!haszero) return nowdetail.res;
		}
		return tres;
	}
	std::vector<int> getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query) {
		std::vector<int> res;
		for (auto &mappoint : mappoints) {
			unsigned long long ull = 1LL << query.endcategory.size();
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
		for (int i = 0; i < mappoints.size(); i++)
			for (int j = 0; j < query.needcategory.size(); j++)
				for (auto k : mappoints[i].category)
					if (query.needcategory[j] == k) {
						res[j].push_back(i);
						break;
					}
		return res;
	}
	data::result trueway(const std::vector<data::mappoint> &mappoints, const data::query &query) {
		auto endpoints = getendpoints(mappoints, query);
		auto needpoints = getneedpoints(mappoints, query);
		data::result res;
		res.maxlength = 1e100;
		ann::ann ann(mappoints, query);
		for (; ; ) {
			int nowend = ann.nextsmallest();
			if (nowend == -1) break;
			auto tres = getdetail(mappoints, query, needpoints, nowend, res.maxlength);
			if (tres.maxlength < res.maxlength)
				res = tres;
		}
		return res;
	}
	data::result greedyway(const std::vector<data::mappoint> &mappoints, const data::query &query) {
		data::result res;
		res.maxlength = 0;
		geo::point center = geo::findcircle(query.start);
		auto endpoints = getendpoints(mappoints, query);
		auto needpoints = getneedpoints(mappoints, query);
		auto endpointi = endpoints[0];
		for (auto i : endpoints) {
			auto p = mappoints[i].p;
			if ((p - center).len() < (mappoints[endpointi].p - center).len())
				endpointi = i;
		}
		res.endpoint = endpointi;
		auto endpoint = mappoints[endpointi].p;
		for (auto i : query.start) {
			auto tnum = (i - endpoint).len();
			res.length.push_back(tnum);
			if (res.maxlength < tnum)
				res.maxlength = tnum;
			std::vector<int> v;
			res.res.push_back(v);
		}
		std::vector<int> needpoint;
		for (int i = 0; i < needpoints.size(); i++) {
			auto tp = needpoints[i][0];
			for (auto j : needpoints[i]) {
				if ((mappoints[j].p - center).len() < (mappoints[tp].p - center).len())
					tp = j;
			}
			needpoint.push_back(tp);
		}
		std::vector<bool> doneneedpoint;
		for (auto i : needpoint)
			doneneedpoint.push_back(0);
		//need step 3
		for (; ; ) {
			int nowmin = 0;
			for (int i = 1; i < res.length.size(); i++)
				if (res.length[i] < res.length[nowmin])
					nowmin = i;
			auto prev = res.res[nowmin].size() == 0 ? query.start[nowmin] : mappoints[res.res[nowmin][res.res[nowmin].size() - 1]].p;
			res.length[nowmin] -= (prev - endpoint).len();
			int minnext = -1;
			double minnextlen = 1e100;
			for (int i = 0; i < needpoint.size(); i++) {
				if (doneneedpoint[i]) continue;
				auto nowlen = (mappoints[needpoint[i]].p - prev).len() + (mappoints[needpoint[i]].p - endpoint).len();
				if (nowlen < minnextlen) {
					minnextlen = nowlen;
					minnext = i;
				}
			}
			if (minnext == -1) {
				for (auto i : doneneedpoint)
					if (i == 0) {
						res.maxlength = 1e100;
						return res;
					}
				return res;
			}
			res.length[nowmin] += minnextlen;
			if (res.length[nowmin] > res.maxlength)
				res.maxlength = res.length[nowmin];
			res.res[nowmin].push_back(needpoint[minnext]);
			for (int i = 0; i < needpoint.size(); i++) {
				for (auto j : mappoints[needpoint[nowmin]].category)
					if (query.needcategory[i] == j)
						doneneedpoint[i] = 1;
			}
		}
	}
}