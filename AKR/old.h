#pragma once
#include "data.h"
#include "ann.h"
#include "heaphash.h"
template <class T> class old {
public:
	static data::result getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, int nowend, double nowbest);
	static std::vector<int> getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static std::vector<std::vector<int>> getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result trueway(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result greedyway(const std::vector<data::mappoint> &mappoints, const data::query &query);
};
template <class T> data::result old<T>::getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, int nowend, double nowbest){
	int needtot = 0;
	for (auto &i : needpoints)
		needtot += i.size();
	printf("needtot %d   ", needtot);
	geo::point endpoint = mappoints[nowend].p;
	data::result tres;
	tres.reslength = 1e100;
	std::vector<data::detaildata*> heap;
	heap.reserve(1000000);
	data::detaildata *tdetail = new data::detaildata;
	tdetail->category.resize(query.needcategory.size());
	tdetail->res.reslength = 0;
	tdetail->res.endpoint = nowend;
	for (auto i : query.start){
		std::vector<int> tvec;
		tdetail->res.res.push_back(tvec);
		double tdouble = (i - mappoints[nowend].p).len();
		tdetail->res.length.push_back(tdouble);
		T::updatereslength(*tdetail, 0, tdouble);
	}
	heap.push_back(tdetail);
	int heapcount = 0;
	heaphash::heaphash hh(query.needcategory.size());
	auto cmp = [&](data::detaildata *x, data::detaildata *y) { return x->res.reslength > y->res.reslength; };
	for (; heap.size(); heapcount++){
		auto &nowdetail = *heap[0];
		/*
		printf("%f | ", nowdetail.res.reslength);
		for (int i = 0; i < nowdetail.category.size(); i++)
		printf("%d ", nowdetail.category[i] ? 1 : 0);
		printf("| ");
		for (int i = 0; i < nowdetail.res.res.size(); i++)
		printf("%d ", nowdetail.res.res[i].size() != 0 ? nowdetail.res.res[i][nowdetail.res.res[i].size() - 1] : -1);
		printf("\n");
		*/
		std::pop_heap(heap.begin(), heap.end(), cmp);
		heap.pop_back();
		double nowmin = hh.find(nowdetail);
		if (nowmin < nowdetail.res.reslength) continue;
		hh.insert(nowdetail);
		if (nowdetail.res.reslength > nowbest)
			continue;
		bool haszero = 0;
		for (int i = 0; i < needpoints.size(); i++)
			if (nowdetail.category[i] == 0){
				haszero = 1;
				for (auto j : needpoints[i])
					for (int k = 0; k < query.start.size(); k++){
						auto adddetail = T::tryadddetail(mappoints, query, nowdetail, endpoint, nowbest, j, k);
						if (adddetail == nullptr) continue;
						heap.push_back(adddetail);
						std::push_heap(heap.begin(), heap.end(), cmp);
					}
			}
		if (!haszero){
			tres = nowdetail.res;
			break;
		}
	}
	printf("heapcount %d\n", heapcount);
	return tres;
}
template <class T> std::vector<int> old<T>::getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query){
	std::vector<int> res;
	for (auto &mappoint : mappoints){
		unsigned long long ull = 1LL << query.endcategory.size();
		for (auto i : query.endcategory){
			for (auto j : mappoint.category)
				if (i == j){
					ull /= 2;
					break;
				}
		}
		if (ull == 1) res.push_back(&mappoint - &mappoints[0]);
	}
	return res;
}
template <class T> std::vector<std::vector<int>> old<T>::getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query){
	std::vector<std::vector<int>> res;
	std::vector<int> tmp;
	for (int i = 0; i < query.needcategory.size(); i++)
		res.push_back(tmp);
	for (int i = 0; i < mappoints.size(); i++)
		for (int j = 0; j < query.needcategory.size(); j++)
			for (auto k : mappoints[i].category)
				if (query.needcategory[j] == k){
					res[j].push_back(i);
					break;
				}
	return res;
}
template <class T> data::result old<T>::trueway(const std::vector<data::mappoint> &mappoints, const data::query &query){
	auto endpoints = getendpoints(mappoints, query);
	auto needpoints = getneedpoints(mappoints, query);
	data::result res;
	res.reslength = 1e100;
	ann::ann<T> ann(mappoints, query);
	for (;;){
		int nowend = ann.nextsmallest(res.reslength);
		if (nowend == -1) break;
		auto tres = getdetail(mappoints, query, needpoints, nowend, res.reslength);
		if (tres.reslength < res.reslength)
			res = tres;
	}
	return res;
}
template <class T> data::result old<T>::greedyway(const std::vector<data::mappoint> &mappoints, const data::query &query){
	data::result res;
	res.reslength = 0;
	geo::point center = geo::findcircle(query.start);
	auto endpoints = getendpoints(mappoints, query);
	auto needpoints = getneedpoints(mappoints, query);
	if (endpoints.size() == 0){
		res.reslength = 1e100;
		return res;
	}
	auto endpointi = endpoints[0];
	for (auto i : endpoints){
		auto p = mappoints[i].p;
		if ((p - center).len() < (mappoints[endpointi].p - center).len())
			endpointi = i;
	}
	res.endpoint = endpointi;
	auto endpoint = mappoints[endpointi].p;
	for (auto i : query.start){
		auto tnum = (i - endpoint).len();
		res.length.push_back(tnum);
		if (res.reslength < tnum)
			res.reslength = tnum;
		std::vector<int> v;
		res.res.push_back(v);
	}
	std::vector<int> needpoint;
	for (int i = 0; i < needpoints.size(); i++){
		if (needpoints[i].size() == 0){
			res.reslength = 1e100;
			return res;
		}
		auto tp = needpoints[i][0];
		for (auto j : needpoints[i]){
			if ((mappoints[j].p - center).len() < (mappoints[tp].p - center).len())
				tp = j;
		}
		needpoint.push_back(tp);
	}
	std::vector<bool> doneneedpoint;
	for (auto i : needpoint)
		doneneedpoint.push_back(0);
	for (;;){
		int nowmin = 0;
		for (int i = 1; i < res.length.size(); i++)
			if (res.length[i] < res.length[nowmin])
				nowmin = i;
		auto prev = res.res[nowmin].size() == 0 ? query.start[nowmin] : mappoints[res.res[nowmin][res.res[nowmin].size() - 1]].p;
		int minnext = -1;
		double minnextlen = 1e100;
		for (int i = 0; i < needpoint.size(); i++){
			if (doneneedpoint[i]) continue;
			auto nowlen = (mappoints[needpoint[i]].p - prev).len() + (mappoints[needpoint[i]].p - endpoint).len();
			if (nowlen < minnextlen){
				minnextlen = nowlen;
				minnext = i;
			}
		}
		if (minnext == -1){
			for (auto i : doneneedpoint)
				if (i == 0){
					res.reslength = 1e100;
					return res;
				}
			return res;
		}
		res.length[nowmin] -= (prev - endpoint).len();
		res.length[nowmin] += minnextlen;
		if (res.length[nowmin] > res.reslength)
			res.reslength = res.length[nowmin];
		res.res[nowmin].push_back(needpoint[minnext]);
		for (int i = 0; i < needpoint.size(); i++){
			for (auto j : mappoints[needpoint[minnext]].category)
				if (query.needcategory[i] == j)
					doneneedpoint[i] = 1;
		}
	}
}