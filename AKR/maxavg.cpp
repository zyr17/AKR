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
	void maxclass::updatereslength(data::detaildata &detail, double data) {
		if (data > detail.res.reslength)
			detail.res.reslength = data;
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
		updatereslength(adddetail, nowlength);
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
	data::result maxclass::naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 0;
		geo::point center = geo::findcircle(query.start);
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

	bool avgclass::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n){
		return n * (pointnum - center).len() >= nowbest + sigmar;
	}
	double avgclass::getenddis(const data::query &query, const geo::point &p){
		double res = 0;
		for (auto &j : query.start)
			res += (j - p).len();
		return res;
	}
	void avgclass::updatereslength(data::detaildata &detail, double data) {
		detail.res.reslength += data;
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
		updatereslength(adddetail, deltalength);
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
	data::result avgclass::naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 0;
		geo::point center = geo::findcircle(query.start);
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
			res.reslength += tnum;
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
			int minline = -1;
			int minnext = -1;
			double minnextlen = 1e100;
			for (int j = 0; j < needpoint.size(); j++){
				if (doneneedpoint[j]) continue;
				for (int i = 0; i < res.length.size(); i++){
					auto p = res.res[i].size() == 0 ? query.start[i] : mappoints[res.res[i][res.res[i].size() - 1]].p;
					auto deltadis = (mappoints[needpoint[j]].p - endpoint).len() + (mappoints[needpoint[j]].p - p).len() - (p - center).len();
					if (deltadis < minnextlen){
						minnextlen = deltadis;
						minline = i;
						minnext = j;
					}
				}
			}
			if (minline == -1){
				for (auto i : doneneedpoint)
					if (i == 0){
						res.reslength = 1e100;
						return res;
					}
				return res;
			}
			res.length[minline] += minnextlen;
			res.reslength += minnextlen;
			res.res[minline].push_back(needpoint[minnext]);
			for (int i = 0; i < needpoint.size(); i++){
				for (auto j : mappoints[needpoint[minnext]].category)
					if (query.needcategory[i] == j)
						doneneedpoint[i] = 1;
			}
		}
	}
}