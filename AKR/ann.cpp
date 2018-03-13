#include "ann.h"
#include "geo.h"
namespace ann {
	bool iscover(const std::vector<int> &small, const std::vector<int> &big){
		for (auto i : small){
			bool flag = 0;
			for (auto j : big)
				if (i == j){
					flag = 1;
					break;
				}
			if (!flag) return 0;
		}
		return 1;
	}
	ann::ann(const std::vector<data::mappoint> &inmappoints, const data::query &inquery){
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
	inline bool ann::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar){
		return (pointnum - center).len() >= nowbest + minr;
	}
	inline double ann::getenddis(const data::query &query, const geo::point &p){
		double res = 0;
		for (auto &j : query.start){
			double t = (j - p).len();
			if (t > res)
				res = t;
		}
		return res;
	}
	int ann::nextsmallest(double nowbest){
		int nowbesti = -1;
		for (int i = 0; i < endorder.size(); i++){
			if (enddis[i] == -2) continue;
			if (outcheck(mappoints[endorder[i]].p, center, nowbest, minr, sigmar)) break;
			if (enddis[i] == -1)
				enddis[i] = getenddis(query, mappoints[endorder[i]].p);
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