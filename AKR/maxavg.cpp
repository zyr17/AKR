#include "maxavg.h"
namespace maxavg{
	bool maxclass::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar){
		return (pointnum - center).len() >= nowbest + minr;
	}
	double maxclass::getenddis(const data::query &query, const geo::point &p){
		double res = 0;
		for (auto &j : query.start){
			double t = (j - p).len();
			if (t > res)
				res = t;
		}
		return res;
	}
	inline void maxclass::updatereslength(data::detaildata &detail, double olddata, double newdata) {
		if (newdata > detail.res.reslength)
			detail.res.reslength = newdata;
	}

	inline data::detaildata maxclass::tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint) {
		geo::point p = query.start[startpoint];
		if (nowdetail.res.res[startpoint].size() != 0)
			p = mappoints[*(--nowdetail.res.res[startpoint].end())].p;
		geo::point midpoint = mappoints[addpoint].p;
		double nowlength = nowdetail.res.length[startpoint] - (p - endpoint).len() + (p - midpoint).len() + (midpoint - endpoint).len();
		if (nowlength > nowbest){
			data::detaildata res;
			res.res.reslength = 1e100;
			return res;
		}
		auto adddetail = nowdetail;
		auto oldlength = adddetail.res.length[startpoint];
		adddetail.res.length[startpoint] = nowlength;
		updatereslength(adddetail, oldlength, nowlength);
		adddetail.res.res[startpoint].push_back(addpoint);
		for (int detailnum = 0; detailnum < adddetail.category.size(); detailnum++)
			if (adddetail.category[detailnum] == 0)
				for (auto have : mappoints[addpoint].category)
					if (have == query.needcategory[detailnum]){
						adddetail.category[detailnum] = 1;
						break;
					}
		return adddetail;
	}
}