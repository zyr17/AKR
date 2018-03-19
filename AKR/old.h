#pragma once
#include "data.h"
#include "ann.h"
#include "heaphash.h"
template <class T> class old {
public:
	static data::result getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, int nowend, double nowbest);
	static std::vector<int> getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, double nowbest);
	static std::vector<std::vector<int>> getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static std::vector<std::vector<int>> old<T>::filtneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, const geo::point endpoint, double nowbest);
	static data::result exactway1(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result naivegreedyway(const std::vector<data::mappoint> &mappoints, const data::query &query);
};
template <class T> data::result old<T>::getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &allneedpoints, int nowend, double nowbest){
	auto needpoints = filtneedpoints(mappoints, query, allneedpoints, mappoints[nowend].p, nowbest);
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
	for (int i = 0; i < query.needcategory.size(); i++)
		for (auto j : mappoints[nowend].category)
			if (query.needcategory[i] == j)
				tdetail->category[i] = 1;
	tdetail->res.reslength = 0;
	tdetail->res.endpoint = nowend;
	tdetail->res.lines.resize(query.start.size());
	for (int i = 0; i < query.start.size(); i++){
		double tdouble = (query.start[i] - mappoints[nowend].p).len();
		tdetail->res.lines[i].length = tdouble;
		T::updatereslength(*tdetail, tdouble);
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
template <class T> std::vector<int> old<T>::getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, double nowbest){
	std::vector<int> res;
	for (auto &mappoint : mappoints){
		if (T::getenddis(query, mappoint.p) >= nowbest)
			continue;
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
template <class T> std::vector<std::vector<int>> old<T>::filtneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, const geo::point endpoint, double nowbest){
	std::vector<std::vector<int>> res;
	for (auto i : needpoints){
		std::vector<int> tvec;
		for (auto j : i)
			if (T::getminpassdis(query, endpoint, mappoints[j].p) < nowbest)
				tvec.push_back(j);
		res.push_back(tvec);
	}
	return res;
}
template <class T> data::result old<T>::exactway1(const std::vector<data::mappoint> &mappoints, const data::query &query){
	data::result res = naivegreedyway(mappoints, query);
	auto endpoints = getendpoints(mappoints, query, res.reslength);
	auto needpoints = getneedpoints(mappoints, query);
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
template <class T> data::result old<T>::naivegreedyway(const std::vector<data::mappoint> &mappoints, const data::query &query){
	auto endpoints = getendpoints(mappoints, query, 1e100);
	auto needpoints = getneedpoints(mappoints, query);
	return T::naivegreedy(mappoints, query, endpoints, needpoints);
}