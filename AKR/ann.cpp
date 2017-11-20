#include "ann.h"
#include "geo.h"
namespace ann {
	bool iscover(const std::vector<int> &small, const std::vector<int> &big) {
		for (auto i : small) {
			bool flag = 0;
			for (auto j : big)
				if (i == j) {
					flag = 1;
					break;
				}
			if (!flag) return 0;
		}
		return 1;
	}
	ann::ann(const std::vector<data::mappoint> &inmappoints, const data::query &inquery) {
		mappoints = inmappoints;
		query = inquery;
		center = geo::findcircle(inquery.start);
		centerr = 0;
		for (auto &i : inquery.start) {
			double t = (i - center).len();
			if (centerr < t)
				centerr = t;
		}
		endorder.clear();
		for (int i = 0; i < inmappoints.size(); i++)
			if (iscover(inquery.endcategory, inmappoints[i].category))
				endorder.push_back(i);
		std::sort(endorder.begin(), endorder.end(), [=](int x, int y) { return (inmappoints[x].p - center).len() < (inmappoints[y].p - center).len(); });
		enddis.clear();
		for (auto &i : endorder)
			enddis.push_back(-1);
	}
	int ann::nextsmallest() {
		double nowbest = 1e100;
		int nowbesti = -1;
		for (int i = 0; i < endorder.size(); i++) {
			if (enddis[i] == -2) continue;
			if ((mappoints[endorder[i]].p - center).len() >= nowbest + centerr) break;
			if (enddis[i] == -1) {
				double td = 0;
				for (auto &j : query.start) {
					double t = (j - mappoints[endorder[i]].p).len();
					if (t > td)
						td = t;
				}
				enddis[i] = td;
			}
			if (enddis[i] < nowbest) {
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