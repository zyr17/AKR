#include "maxavg.h"
namespace maxavg{
	bool maxclass::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n){
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
	void maxclass::updatereslength(data::detaildata &detail, double olddata, double newdata) {
		if (newdata > detail.res.reslength)
			detail.res.reslength = newdata;
	}
	data::detaildata* maxclass::tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint) {
		geo::point p = query.start[startpoint];
		if (nowdetail.res.res[startpoint].size() != 0)
			p = mappoints[*(--nowdetail.res.res[startpoint].end())].p;
		geo::point midpoint = mappoints[addpoint].p;
		double nowlength = nowdetail.res.length[startpoint] - (p - endpoint).len() + (p - midpoint).len() + (midpoint - endpoint).len();
		if (nowlength > nowbest)
			return nullptr;
		auto adddetailptr = new data::detaildata(nowdetail);
		auto &adddetail = *adddetailptr;
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
		return adddetailptr;
	}

	bool avgclass::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n){
		return n * (pointnum - center).len() >= nowbest + sigmar;
	}
	double avgclass::getenddis(const data::query &query, const geo::point &p){
		double res = 0;
		for (auto &j : query.start)
			res += (j - p).len();
		return res;
	}
	void avgclass::updatereslength(data::detaildata &detail, double olddata, double newdata) {
		detail.res.reslength += newdata - olddata;
	}
	data::detaildata* avgclass::tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint) {
		geo::point p = query.start[startpoint];
		if (nowdetail.res.res[startpoint].size() != 0)
			p = mappoints[*(--nowdetail.res.res[startpoint].end())].p;
		geo::point midpoint = mappoints[addpoint].p;
		double deltalength = (p - midpoint).len() + (midpoint - endpoint).len() - (p - endpoint).len();
		if (nowdetail.res.reslength + deltalength > nowbest)
			return nullptr;
		auto adddetailptr = new data::detaildata(nowdetail);
		auto &adddetail = *adddetailptr;
		auto oldlength = adddetail.res.length[startpoint];
		adddetail.res.length[startpoint] += deltalength;
		updatereslength(adddetail, oldlength, deltalength);
		adddetail.res.res[startpoint].push_back(addpoint);
		for (int detailnum = 0; detailnum < adddetail.category.size(); detailnum++)
			if (adddetail.category[detailnum] == 0)
				for (auto have : mappoints[addpoint].category)
					if (have == query.needcategory[detailnum]){
						adddetail.category[detailnum] = 1;
						break;
					}
		return adddetailptr;
	}
}