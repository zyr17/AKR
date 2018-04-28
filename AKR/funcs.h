#pragma once
#include "data.h"
#include "ann.h"
#include "heaphash.h"
#include "taskfinish.h"
template <class T> class funcs {
public:
	static data::result getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, int nowend, double nowbest);
	static std::vector<int> getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, double nowbest);
	static std::vector<std::vector<int>> getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static std::vector<std::vector<int>> filtneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, const geo::point endpoint, double nowbest);
	static data::result exactway1(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result exactway2(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result naivegreedyway(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result naivegreedywayplus(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result bettergreedyway(const std::vector<data::mappoint> &mappoints, const data::query &query);
	static data::result bettergreedywayplus(const std::vector<data::mappoint> &mappoints, const data::query &query);
};
template <class T> data::result funcs<T>::getdetail(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &allneedpoints, int nowend, double nowbest){
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
		if (nowmin < nowdetail.res.reslength){ 
			delete &nowdetail;
			continue; 
		}
		hh.insert(nowdetail);
		if (nowdetail.res.reslength > nowbest){
			delete &nowdetail;
			continue;
		}
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
			delete &nowdetail;
			break;
		}
		delete &nowdetail;
	}
	for (auto i : heap)
		delete i;
	printf("heapcount %d\n", heapcount);
	return tres;
}
template <class T> std::vector<int> funcs<T>::getendpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, double nowbest){
	std::vector<int> res;
	auto nowbest2 = nowbest * nowbest;
	for (auto &mappoint : mappoints){
		if (T::getenddis2(query, mappoint.p) >= nowbest2)
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
template <class T> std::vector<std::vector<int>> funcs<T>::getneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query){
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
template <class T> std::vector<std::vector<int>> funcs<T>::filtneedpoints(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<std::vector<int>> &needpoints, const geo::point endpoint, double nowbest){
	std::vector<std::vector<int>> res;
	for (auto &i : needpoints){
		std::vector<int> tvec;
		for (auto j : i)
			if (T::getminpassdis(query, endpoint, mappoints[j].p) < nowbest)
				tvec.push_back(j);
		res.push_back(tvec);
	}
	return res;
}
template <class T> data::result funcs<T>::exactway1(const std::vector<data::mappoint> &mappoints, const data::query &query){
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
template <class T> data::result funcs<T>::exactway2(const std::vector<data::mappoint> &mappoints, const data::query &query){
	data::result res = naivegreedyway(mappoints, query);
	//low:全部直接连，理论最小值 up:目前最优解的值
	double llow = 0, &lup = res.reslength;
	auto endpoints = getendpoints(mappoints, query, res.reslength);
	auto allneedpoints = getneedpoints(mappoints, query);
	LARGE_INTEGER litmp;
	QueryPerformanceFrequency(&litmp);
	auto dfFreq = litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	auto start = litmp.QuadPart;
	ann::ann<T> ann(mappoints, query);
	for (;;){
		//printf("--------------new endpoint---------------\n");
		int nowend = ann.nextsmallest(res.reslength);
		if (nowend == -1) break;
		llow = T::getenddis(query, mappoints[nowend].p);
		if (llow > lup) break;
		auto needpoints = filtneedpoints(mappoints, query, allneedpoints, mappoints[nowend].p, lup);
		bool flag = 0;
		for (auto &i : needpoints)
			if (!i.size()) flag = 1;
		if (flag) continue;
		taskfinish::taskfinish<T> taskfinish(mappoints, query, needpoints, nowend);
		std::vector<int> belong;
		belong.resize(query.needcategory.size());
		for (;;){
			data::result nowres = taskfinish.get(belong, lup);
			if (nowres.reslength < lup){
				res = nowres;
				//max下，两者相等那么已经有最优解可以不继续枚举；avg下需要中途点完全没有绕路，基本无效但是基本没有代价
				if (lup == llow)
					goto End;
			}
			bool flag = 0;
			for (int i = query.needcategory.size() - 1; i >= 0; i--){
				if (belong[i] < query.start.size() - 1){
					belong[i]++;
					flag = 1;
					break;
				}
				belong[i] = 0;
			}
			if (!flag)
				break;
		}
	}
End:;
	QueryPerformanceCounter(&litmp);
	res.time = (litmp.QuadPart - start) * 1000000 / dfFreq;
	return res;
}
template <class T> data::result funcs<T>::naivegreedyway(const std::vector<data::mappoint> &mappoints, const data::query &query){
	auto endpoints = getendpoints(mappoints, query, 1e100);
	auto needpoints = getneedpoints(mappoints, query);
	LARGE_INTEGER litmp;
	QueryPerformanceFrequency(&litmp);
	auto dfFreq = litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	auto start = litmp.QuadPart;
	auto res = T::naivegreedy(mappoints, query, endpoints, needpoints);
	QueryPerformanceCounter(&litmp);
	res.time = (litmp.QuadPart - start) * 1000000 / dfFreq;
	return res;
}
template <class T> data::result funcs<T>::naivegreedywayplus(const std::vector<data::mappoint> &mappoints, const data::query &query){
	auto endpoints = getendpoints(mappoints, query, 1e100);
	auto needpoints = getneedpoints(mappoints, query);
	LARGE_INTEGER litmp;
	QueryPerformanceFrequency(&litmp);
	auto dfFreq = litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	auto start = litmp.QuadPart;
	auto res = T::naivegreedyplus(mappoints, query, endpoints, needpoints);
	QueryPerformanceCounter(&litmp);
	res.time = (litmp.QuadPart - start) * 1000000 / dfFreq;
	return res;
}
template <class T> data::result funcs<T>::bettergreedyway(const std::vector<data::mappoint> &mappoints, const data::query &query){
	auto endpoints = getendpoints(mappoints, query, 1e100);
	auto needpoints = getneedpoints(mappoints, query);
	LARGE_INTEGER litmp;
	QueryPerformanceFrequency(&litmp);
	auto dfFreq = litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	auto start = litmp.QuadPart;
	auto res = T::bettergreedy(mappoints, query, endpoints, needpoints);
	QueryPerformanceCounter(&litmp);
	res.time = (litmp.QuadPart - start) * 1000000 / dfFreq;
	return res;
}
template <class T> data::result funcs<T>::bettergreedywayplus(const std::vector<data::mappoint> &mappoints, const data::query &query){
	auto endpoints = getendpoints(mappoints, query, 1e100);
	auto needpoints = getneedpoints(mappoints, query);
	LARGE_INTEGER litmp;
	QueryPerformanceFrequency(&litmp);
	auto dfFreq = litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	auto start = litmp.QuadPart;
	auto res = T::bettergreedyplus(mappoints, query, endpoints, needpoints);
	QueryPerformanceCounter(&litmp);
	res.time = (litmp.QuadPart - start) * 1000000 / dfFreq;
	return res;
}