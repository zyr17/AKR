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
		for (auto &i : inquery.start){
			double t = (i - center).len();
			if (centerr < t)
				centerr = t;
			if (minr > t)
				minr = t;
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
	int ann::nextsmallest(double nowbest){
		int nowbesti = -1;
		for (int i = 0; i < endorder.size(); i++){
			if (enddis[i] == -2) continue;
			if ((mappoints[endorder[i]].p - center).len() >= nowbest + minr) break;
			if (enddis[i] == -1){
				double td = 0;
				for (auto &j : query.start){
					double t = (j - mappoints[endorder[i]].p).len();
					if (t > td)
						td = t;
				}
				enddis[i] = td;
			}
			if (enddis[i] < nowbest){
				nowbest = enddis[i];
				nowbesti = i;
			}
		}
		if (nowbest == 1e100)
			return -1;
		enddis[nowbesti] = -2;
		return nowbesti == -1 ? -1 : endorder[nowbesti];
	}
}