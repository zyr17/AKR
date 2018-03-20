#include "taskfinish.h"
namespace taskfinish{
	onelinehash::onelinehash(int categorynumber){
		left = categorynumber / 2;
		right = categorynumber - left;
		for (int i = 1 << left; i--;){
			std::vector<data::oneline*> vec;
			vec.resize(1 << right);
			min.push_back(vec);
		}
	}
	void onelinehash::insert(data::oneline *number){
		int category = number->category;
		int lnum = category & ((1 << left) - 1), rnum = category >> left;
		for (int i = lnum;; i = (i - 1) & lnum){
			if (min[i][rnum] == nullptr)
				min[i][rnum] = new data::oneline(*number);
			if (!i) break;
		}
	}
	data::oneline* onelinehash::findmin(int category) const{
		//printf("%x\n", this);
		int lnum = category & ((1 << left) - 1), rnum = category >> left;
		//printf("%d %d %d %d\n", left, right, lnum, rnum);
		data::oneline *res = nullptr;
		rnum ^= (1 << right) - 1;
		for (int i = rnum;; i = (i - 1) & rnum){
			int update = i ^ ((1 << right) - 1);
			if (min[lnum][update] != nullptr && (res == nullptr || min[lnum][update]->length < res->length))
				res = min[lnum][update];
			if (!i) break;
		}
		return res;
	}
	data::oneline* onelinehash::operator[](int category) const{
		return findmin(category);
	}

	taskfinish::taskfinish(const std::vector<data::mappoint> &inmappoints, const data::query &inquery, const std::vector<std::vector<int>> &inneedpoints, int inm){
		query = inquery;
		mappoints = &inmappoints;
		needpoints = &inneedpoints;
		m = inm;
		auto o = onestart(query.needcategory.size());
		starts.resize(query.start.size(), o);
		for (int num = 0; num < query.start.size(); num++){
			auto &i = starts[num];
			auto o = new data::oneline;
			o->category = 0;
			o->length = (inmappoints[m].p - query.start[num]).len();
			i.heap.push_back(o);
		}
	}
	void taskfinish::addtores(data::result &res, data::oneline *line){
		res.lines.push_back(*line);
		if (res.reslength < line->length)
			res.reslength = line->length;
	}
	void taskfinish::lineappend(geo::point start, data::oneline *line, int p){
		geo::point endp = line->res.size() ? (*mappoints)[line->res.back()].p : start;
		line->length -= (endp - (*mappoints)[m].p).len();
		line->length += (endp - (*mappoints)[p].p).len() + ((*mappoints)[p].p - (*mappoints)[m].p).len();
		//TODO 可以预处理每个点的类别压位
		for (int i = 0; i < query.needcategory.size(); i++)
			for (auto j : (*mappoints)[p].category)
				if (query.needcategory[i] == j)
					line->category |= 1 << i;
		line->res.push_back(p);
	}
	data::result taskfinish::get(const std::vector<int> &belong, double llow, double lup){
		data::result res;
		res.endpoint = m;
		std::vector<int> needcate;
		needcate.resize(query.start.size());
		for (int i = 0; i < belong.size(); i++)
			needcate[belong[i]] |= 1 << i;
		for (int i = 0; i < starts.size(); i++){
			auto &start = starts[i];
			auto &heap = start.heap;
			auto &hash = start.hash;
			auto &pre = start.pre;
			if (pre[needcate[i]] != nullptr){
				auto preget = pre[needcate[i]];
				addtores(res, preget);
				if (preget->length > llow)
					llow = preget->length;
			}
			else{
				for (;;){
					if (!heap.size()){
						res.reslength = 1e100;
						return res;
					}
					auto cmp = [&](data::oneline* x, data::oneline* y) { return x->length > y->length; };
					auto nowline = heap[0];
					std::pop_heap(heap.begin(), heap.end(), cmp);
					heap.pop_back();
					if (nowline->length >= lup){
						res.reslength = 1e100;
						return res;
					}
					auto hashed = ((nowline->res.size() ? nowline->res.back() + 1 : 0) << query.needcategory.size()) + nowline->category;
					if (hash.find(hashed) != hash.end()){
						delete nowline;
						continue;
					}
					else hash.insert(hashed);
					bool terminate = 0;
					if (pre[nowline->category] == nullptr){
						//TODO 最好有heaphash类似操作?
						pre.insert(nowline);
						if (nowline->length > llow)
							llow = nowline->length;
						if ((nowline->category & needcate[i]) == needcate[i]){
							terminate = 1;
							addtores(res, nowline);
						}
					}
					for (int need = 0; need < query.needcategory.size(); need++){
						if (!((1 << need) & nowline->category)){
							for (auto p : (*needpoints)[need]){
								auto copy = new data::oneline(*nowline);
								lineappend(query.start[i], copy, p);
								if (copy->length < lup){
									heap.push_back(copy);
									std::push_heap(heap.begin(), heap.end(), cmp);
									if (copy->length < llow && (copy->category & needcate[i]) == needcate[i] && !terminate){
										terminate = 1;
										addtores(res, copy);
									}
								}
								else delete copy;
							}
						}
					}
					delete nowline;
					if (terminate) break;
				}
			}
		}
		return res;
	}
}