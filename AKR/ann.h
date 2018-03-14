#pragma once
#include <vector>
#include <ctime>
#include "data.h"
namespace ann {
	bool iscover(const std::vector<int> &, const std::vector<int> &);
	//TODO read paper
	//每次调用返回一个未返回过的ANN结果。使用方法2，每次计算中途点缓存结果减少重复计算
	template <class T> class ann {
		std::vector<int> mindistance, endorder;
		std::vector<double> enddis;
		std::vector<data::mappoint> mappoints;
		data::query query;
		double centerr, minr, sigmar;
		geo::point center;
	public:
		ann(const std::vector<data::mappoint> &inmappoints, const data::query &inquery);
		//static bool outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar);
		//static double getenddis(const data::query &query, const geo::point &p);
		int nextsmallest(double nowbest = 1e100);
	};
	template <class T> ann<T>::ann(const std::vector<data::mappoint> &inmappoints, const data::query &inquery){
		auto stc = clock();
		mappoints = inmappoints;
		query = inquery;
		center = geo::findcircle(inquery.start);
		//printf("ann::findcircle %lld\n", clock() - stc);
		centerr = 0;
		minr = 1e100;
		sigmar = 0;
		for (auto &i : inquery.start){
			double t = (i - center).len();
			if (centerr < t)
				centerr = t;
			if (minr > t)
				minr = t;
			sigmar += t;
		}
		endorder.clear();
		for (int i = 0; i < inmappoints.size(); i++)
			if (iscover(inquery.endcategory, inmappoints[i].category))
				endorder.push_back(i);
		//printf("ann::endorder %lld\nendordernum %d\n", clock() - stc, endorder.size());
		std::sort(endorder.begin(), endorder.end(), [&](int x, int y){ return (inmappoints[x].p - center).len() < (inmappoints[y].p - center).len(); });
		//printf("ann::sort %lld\n", clock() - stc);
		enddis.clear();
		for (auto &i : endorder)
			enddis.push_back(-1);
	}
	template <class T> int ann<T>::nextsmallest(double nowbest){
		int nowbesti = -1;
		for (int i = 0; i < endorder.size(); i++){
			if (enddis[i] == -2) continue;
			if (T::outcheck(mappoints[endorder[i]].p, center, nowbest, minr, sigmar)) break;
			if (enddis[i] == -1)
				enddis[i] = T::getenddis(query, mappoints[endorder[i]].p);
			if (enddis[i] < nowbest){
				nowbest = enddis[i];
				nowbesti = i;
			}
		}
		if (nowbesti == -1)
			return -1;
		enddis[nowbesti] = -2;
		return nowbesti == -1 ? -1 : endorder[nowbesti];
	}
}