#include "maxavg.h"
namespace maxavg{
	heapunit heapunit::makeheapunit(const std::vector<data::mappoint> &mappoints, int p1, int p2, int pmid, int linenum, const geo::point &startpoint){
		heapunit hu;
		hu.p1 = p1;
		hu.p2 = p2;
		hu.pmid = pmid;
		hu.linenum = linenum;
		geo::point start = p1 == -1 ? startpoint : mappoints[p1].p;
		geo::point end = mappoints[p2].p, mid = mappoints[pmid].p;
		auto part1 = (start - mid).len(), part2 = (end - mid).len(), old = (start - end).len();
		hu.length = part1 + part2 - old;
		return hu;
	}
	bool maxclass::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n){
		return (pointnum - center).len() >= nowbest + minr;
	}
	double maxclass::getenddis2(const data::query &query, const geo::point &p){
		double res = 0;
		for (auto &j : query.start){
			double t = (j - p).len2();
			if (t > res)
				res = t;
		}
		return res;
	}
	double maxclass::getenddis(const data::query &query, const geo::point &p) {
		return sqrt(getenddis2(query, p));
	}
	void maxclass::updatereslength(data::detaildata &detail, double data) {
		if (data > detail.res.reslength)
			detail.res.reslength = data;
	}
	data::detaildata* maxclass::tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint) {
		geo::point p = query.start[startpoint];
		if (nowdetail.res.lines[startpoint].res.size() != 0)
			p = mappoints[nowdetail.res.lines[startpoint].res.back()].p;
		geo::point midpoint = mappoints[addpoint].p;
		double nowlength = nowdetail.res.lines[startpoint].length - (p - endpoint).len() + (p - midpoint).len() + (midpoint - endpoint).len();
		if (nowlength > nowbest)
			return nullptr;
		auto adddetailptr = new data::detaildata(nowdetail);
		auto &adddetail = *adddetailptr;
		adddetail.res.lines[startpoint].length = nowlength;
		updatereslength(adddetail, nowlength);
		adddetail.res.lines[startpoint].res.push_back(addpoint);
		for (int detailnum = 0; detailnum < adddetail.category.size(); detailnum++)
			if (adddetail.category[detailnum] == 0)
				for (auto have : mappoints[addpoint].category)
					if (have == query.needcategory[detailnum]){
						adddetail.category[detailnum] = 1;
						break;
					}
		return adddetailptr;
	}
	data::result maxclass::onenaivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 0;
		geo::point center = geo::findcircle(query.start);
		res.endpoint = endpointi;
		res.lines.resize(query.start.size());
		auto endpoint = mappoints[endpointi].p;
		for (int i = 0; i < query.start.size(); i++){
			auto tnum = (query.start[i] - endpoint).len();
			res.lines[i].length = tnum;
			if (res.reslength < tnum)
				res.reslength = tnum;
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
			for (int i = 1; i < res.lines.size(); i++)
				if (res.lines[i].length < res.lines[nowmin].length)
					nowmin = i;
			auto prev = res.lines[nowmin].res.size() == 0 ? query.start[nowmin] : mappoints[res.lines[nowmin].res.back()].p;
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
			res.lines[nowmin].length -= (prev - endpoint).len();
			res.lines[nowmin].length += minnextlen;
			if (res.lines[nowmin].length > res.reslength)
				res.reslength = res.lines[nowmin].length;
			res.lines[nowmin].res.push_back(needpoint[minnext]);
			for (int i = 0; i < needpoint.size(); i++){
				for (auto j : mappoints[needpoint[minnext]].category)
					if (query.needcategory[i] == j)
						doneneedpoint[i] = 1;
			}
		}
	}
	data::result maxclass::naivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
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
		return onenaivegreedy(mappoints, query, endpointi, needpoints);
	}
	data::result maxclass::naivegreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
		ann::ann<maxclass> ann(mappoints, query);
		for (;;){
			auto endpointi = ann.nextsmallest(res.reslength);
			if (endpointi < 0) break;
			auto tres = onenaivegreedy(mappoints, query, endpointi, needpoints);
			if (tres.reslength < res.reslength)
				res = tres;
		}
		return res;
	}
	data::result maxclass::onebettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 0;
		res.endpoint = endpointi;
		auto endpoint = mappoints[endpointi].p;
		std::vector<std::vector<heapunit>> heaps;
		for (int i = 0; i < query.start.size(); i++){
			data::oneline one;
			one.category = 0;
			one.length = (query.start[i] - endpoint).len();
			res.lines.push_back(one);
			heaps.push_back(std::vector<heapunit>());
			if (one.length > res.reslength)
				res.reslength = one.length;
			for (auto &j : needpoints){
				for (auto k : j)
					heaps[i].push_back(heapunit::makeheapunit(mappoints, -1, endpointi, k, res.lines.size() - 1, query.start[i]));
				std::make_heap(heaps[i].begin(), heaps[i].end());
			}
		}
		unsigned long long totcate = 0;
		for (;;){
			int nowmin = 0;
			for (int i = 0; i < res.lines.size(); i++)
				if (res.lines[nowmin].length > res.lines[i].length)
					nowmin = i;
			auto &heap = heaps[nowmin];
			auto &line = res.lines[nowmin];
			for (;;){
				if (heap.size() == 0){
					res.reslength = 1e100;
					return res;
				}
				int mid = heap[0].pmid, start = heap[0].p1, end = heap[0].p2;
				double delta = heap[0].length;
				bool exist = start == -1 ? (line.res.size() == 0 ? end == endpointi : end == line.res[0]) : 0;
				for (int i = 0; i < line.res.size(); i++)
					if (line.res[i] == start)
						exist = end == (i + 1 == line.res.size() ? endpointi : line.res[i + 1]);
				std::pop_heap(heap.begin(), heap.end());
				heap.pop_back();
				if (!exist) continue;
				unsigned long long nowcate = 0;
				for (int i = 0; i < query.needcategory.size(); i++)
					for (auto j : mappoints[mid].category)
						if (query.needcategory[i] == j)
							nowcate |= 1 << i;
				if (nowcate == (totcate & nowcate)) continue;
				line.res.push_back(mid);
				line.length += delta;
				totcate |= nowcate;
				line.category |= nowcate;
				if (line.length > res.reslength)
					res.reslength = line.length;
				for (int i = line.res.size() - 1;; i--)
					if (i && line.res[i - 1] != start){
						line.res[i] = line.res[i - 1];
						line.res[i - 1] = mid;
					}
					else{
						for (int i = 0; i < needpoints.size(); i++)
							if (!((1 << i) & totcate))
								for (auto j : needpoints[i]){
									heap.push_back(heapunit::makeheapunit(mappoints, start, mid, j, nowmin, query.start[nowmin]));
									std::push_heap(heap.begin(), heap.end());
									heap.push_back(heapunit::makeheapunit(mappoints, mid, end, j, nowmin, query.start[nowmin]));
									std::push_heap(heap.begin(), heap.end());
								}
						break;
					}
				break;
			}
			if (totcate + 1 == 1 << query.needcategory.size()) return res;
		}
	}
	data::result maxclass::bettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
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
		return onebettergreedy(mappoints, query, endpointi, needpoints);
	}
	data::result maxclass::bettergreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
		ann::ann<maxclass> ann(mappoints, query);
		for (;;){
			auto endpointi = ann.nextsmallest(res.reslength);
			if (endpointi < 0) break;
			auto tres = onebettergreedy(mappoints, query, endpointi, needpoints);
			if (tres.reslength < res.reslength)
				res = tres;
		}
		return res;
	}
	double maxclass::getminpassdis(const data::query &query, geo::point endpoint, geo::point midpoint){
		double res = 1e100;
		for (auto i : query.start){
			double now = (i - midpoint).len();
			if (now < res)
				res = now;
		}
		return res + (midpoint - endpoint).len();
	}
	void maxclass::addtores(data::result &res, data::oneline *line){
		res.lines.push_back(*line);
		if (res.reslength < line->length)
			res.reslength = line->length;
	}
	bool maxclass::smallthanlup(double res, double line, double lup){
		return (res > line ? res : line) < lup;
	}

	bool avgclass::outcheck(const geo::point &pointnum, const geo::point &center, double nowbest, double minr, double sigmar, int n){
		return n * (pointnum - center).len() >= nowbest + sigmar;
	}
	double avgclass::getenddis2(const data::query &query, const geo::point &p){
		double res = 0;
		for (auto &j : query.start)
			res += (j - p).len2();
		return res;
	}
	double avgclass::getenddis(const data::query &query, const geo::point &p) {
		return sqrt(getenddis2(query, p));
	}
	void avgclass::updatereslength(data::detaildata &detail, double data) {
		detail.res.reslength += data;
	}
	data::detaildata* avgclass::tryadddetail(const std::vector<data::mappoint> &mappoints, const data::query &query, data::detaildata &nowdetail, geo::point endpoint, double nowbest, int addpoint, int startpoint) {
		geo::point p = query.start[startpoint];
		if (nowdetail.res.lines[startpoint].res.size() != 0)
			p = mappoints[nowdetail.res.lines[startpoint].res.back()].p;
		geo::point midpoint = mappoints[addpoint].p;
		double deltalength = (p - midpoint).len() + (midpoint - endpoint).len() - (p - endpoint).len();
		if (nowdetail.res.reslength + deltalength > nowbest)
			return nullptr;
		auto adddetailptr = new data::detaildata(nowdetail);
		auto &adddetail = *adddetailptr;
		auto oldlength = adddetail.res.lines[startpoint].length;
		adddetail.res.lines[startpoint].length += deltalength;
		updatereslength(adddetail, deltalength);
		adddetail.res.lines[startpoint].res.push_back(addpoint);
		for (int detailnum = 0; detailnum < adddetail.category.size(); detailnum++)
			if (adddetail.category[detailnum] == 0)
				for (auto have : mappoints[addpoint].category)
					if (have == query.needcategory[detailnum]){
						adddetail.category[detailnum] = 1;
						break;
					}
		return adddetailptr;
	}
	data::result avgclass::onenaivegreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 0;
		geo::point center = geo::findcircle(query.start);
		res.endpoint = endpointi;
		res.lines.resize(query.start.size());
		auto endpoint = mappoints[endpointi].p;
		for (int i = 0; i < query.start.size(); i++){
			auto tnum = (query.start[i] - endpoint).len();
			res.lines[i].length = tnum;
			res.reslength += tnum;
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
				for (int i = 0; i < res.lines.size(); i++){
					auto p = res.lines[i].res.size() == 0 ? query.start[i] : mappoints[res.lines[i].res.back()].p;
					auto deltadis = (mappoints[needpoint[j]].p - endpoint).len() + (mappoints[needpoint[j]].p - p).len() - (p - endpoint).len();
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
			res.lines[minline].length += minnextlen;
			res.reslength += minnextlen;
			res.lines[minline].res.push_back(needpoint[minnext]);
			for (int i = 0; i < needpoint.size(); i++){
				for (auto j : mappoints[needpoint[minnext]].category)
					if (query.needcategory[i] == j)
						doneneedpoint[i] = 1;
			}
		}
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
		return onenaivegreedy(mappoints, query, endpointi, needpoints);
	}
	data::result avgclass::naivegreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
		ann::ann<avgclass> ann(mappoints, query);
		for (;;){
			auto endpointi = ann.nextsmallest(res.reslength);
			if (endpointi < 0) break;
			auto tres = onenaivegreedy(mappoints, query, endpointi, needpoints);
			if (tres.reslength < res.reslength)
				res = tres;
		}
		return res;
	}
	data::result avgclass::onebettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, int endpointi, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 0;
		res.endpoint = endpointi;
		auto endpoint = mappoints[endpointi].p;
		std::vector<heapunit> heap;
		for (int i = 0; i < query.start.size(); i++){
			data::oneline one;
			one.category = 0;
			one.length = (query.start[i] - endpoint).len();
			res.lines.push_back(one);
			res.reslength += one.length;
			for (auto &j : needpoints)
				for (auto k : j)
					heap.push_back(heapunit::makeheapunit(mappoints, -1, endpointi, k, res.lines.size() - 1, query.start[i]));
		}
		std::make_heap(heap.begin(), heap.end());
		unsigned long long totcate = 0;
		for (;;){
			if (heap.size() == 0){
				res.reslength = 1e100;
				return res;
			}
			int mid = heap[0].pmid, start = heap[0].p1, end = heap[0].p2, linenum = heap[0].linenum;
			double delta = heap[0].length;
			auto &line = res.lines[linenum];
			bool exist = start == -1 ? (line.res.size() == 0 ? end == endpointi : end == line.res[0]) : 0;
			for (int i = 0; i < line.res.size(); i++)
				if (line.res[i] == start)
					exist = end == (i + 1 == line.res.size() ? endpointi : line.res[i + 1]);
			std::pop_heap(heap.begin(), heap.end());
			heap.pop_back();
			if (!exist) continue;
			unsigned long long nowcate = 0;
			for (int i = 0; i < query.needcategory.size(); i++)
				for (auto j : mappoints[mid].category)
					if (query.needcategory[i] == j)
						nowcate |= 1 << i;
			if (nowcate == (totcate & nowcate)) continue;
			line.res.push_back(mid);
			line.length += delta;
			totcate |= nowcate;
			line.category |= nowcate;
			res.reslength += delta;
			for (int i = line.res.size() - 1;; i--)
				if (i && line.res[i - 1] != start){
					line.res[i] = line.res[i - 1];
					line.res[i - 1] = mid;
				}
				else{
					for (int i = 0; i < needpoints.size(); i++)
						if (!((1 << i) & totcate))
							for (auto j : needpoints[i]){
								heap.push_back(heapunit::makeheapunit(mappoints, start, mid, j, linenum, query.start[linenum]));
								std::push_heap(heap.begin(), heap.end());
								heap.push_back(heapunit::makeheapunit(mappoints, mid, end, j, linenum, query.start[linenum]));
								std::push_heap(heap.begin(), heap.end());
							}
					break;
				}
			if (totcate + 1 == 1 << query.needcategory.size()) return res;
		}
	}
	data::result avgclass::bettergreedy(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
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
		return onebettergreedy(mappoints, query, endpointi, needpoints);
	}
	data::result avgclass::bettergreedyplus(const std::vector<data::mappoint> &mappoints, const data::query &query, const std::vector<int> &endpoints, const std::vector<std::vector<int>> &needpoints){
		data::result res;
		res.reslength = 1e100;
		ann::ann<avgclass> ann(mappoints, query);
		for (;;){
			auto endpointi = ann.nextsmallest(res.reslength);
			if (endpointi < 0) break;
			auto tres = onebettergreedy(mappoints, query, endpointi, needpoints);
			if (tres.reslength < res.reslength)
				res = tres;
		}
		return res;
	}
	double avgclass::getminpassdis(const data::query &query, geo::point endpoint, geo::point midpoint){
		double res = 1e100, tot = 0;
		for (auto i : query.start)
			tot += (i - endpoint).len();
		for (auto i : query.start){
			double now = tot - (i - endpoint).len() + (i - midpoint).len() + (midpoint - endpoint).len();
			if (now < res)
				res = now;
		}
		return res;
	}
	void avgclass::addtores(data::result &res, data::oneline *line){
		res.lines.push_back(*line);
		res.reslength += line->length;
	}
	bool avgclass::smallthanlup(double res, double line, double lup){
		return res + line < lup;
	}
}