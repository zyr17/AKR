#pragma once
#include <vector>
#include <set>
#include "data.h"
#include "geo.h"
namespace taskfinish{
	//TODO 销毁时释放new oneline的内存
	class onelinehash {
	private:
		int left, right;
		std::vector<std::vector<data::oneline*>> min;
	public:
		onelinehash(int categorynumber);
		void insert(data::oneline *number);
		data::oneline* findmin(int category) const;
		data::oneline* operator[](int category) const;
		~onelinehash() {
			for (auto &i : min)
				for (auto j : i)
					delete j;
		}
	};
	struct onestart{
		std::vector<data::oneline*> heap;
		std::set<unsigned long long> hash;
		onelinehash pre;
		onestart(int categorynumber) : pre(categorynumber) {}
	};
	template <class T> class taskfinish{
	private:
		data::query query;
		const std::vector<data::mappoint> *mappoints;
		const std::vector<std::vector<int>> *needpoints;
		int m;
		std::vector<onestart> starts;
		//static void addtores(data::result &res, data::oneline *line);
		void lineappend(geo::point start, data::oneline *line, int p);
		//bool smallthanlup(double res, double line, double lup);
	public:
		taskfinish(const std::vector<data::mappoint> &inmappoints, const data::query &inquery, const std::vector<std::vector<int>> &inneedpoints, int inm);
		data::result get(const std::vector<int> &belong, double lup);
		~taskfinish() {
			for (auto &i : starts)
				for (auto &j : i.heap)
					delete j;
		}
	};
	template <class T> taskfinish<T>::taskfinish(const std::vector<data::mappoint> &inmappoints, const data::query &inquery, const std::vector<std::vector<int>> &inneedpoints, int inm){
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
	/*template <class T> void taskfinish<T>::addtores(data::result &res, data::oneline *line){
		res.lines.push_back(*line);
		if (res.reslength < line->length)
			res.reslength = line->length;
	}*/
	template <class T> void taskfinish<T>::lineappend(geo::point start, data::oneline *line, int p){
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
	/*template <class T> bool taskfinish<T>::smallthanlup(double res, double line, double lup){
		return (res > line ? res : line) < lup;
	}*/
	template <class T> data::result taskfinish<T>::get(const std::vector<int> &belong, double lup){
		//bool printend = 0;
		data::result res;
		res.endpoint = m;
		res.reslength = 0;
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
				T::addtores(res, preget);
				//if (preget->length > llow)
				//	llow = preget->length;
			}
			else{
				for (int count = 0;; count++){
					//if (count % 100000 == 0 && count) printf("%d %d %d %d\n", i, count, hash.size(), heap.size());
					//if (count > 99999) printend = 1;
					if (!heap.size()){
						res.reslength = 1e100;
						return res;
					}
					auto cmp = [&](data::oneline* x, data::oneline* y) { return x->length > y->length; };
					auto nowline = heap[0];
					std::pop_heap(heap.begin(), heap.end(), cmp);
					heap.pop_back();
					bool terminate = 0;
					//if (nowline->length >= lup){
					if (!T::smallthanlup(res.reslength, nowline->length, lup)){
						res.reslength = 1e100;
						terminate = 1;
					}
					auto hashed = ((nowline->res.size() ? nowline->res.back() + 1 : 0) << query.needcategory.size()) + nowline->category;
					if (hash.find(hashed) != hash.end()){
						delete nowline;
						continue;
					}
					else hash.insert(hashed);
					if (pre[nowline->category] == nullptr){
						pre.insert(nowline);
						//if (nowline->length > llow)
						//	llow = nowline->length;
						if ((nowline->category & needcate[i]) == needcate[i]){
							terminate = 1;
							T::addtores(res, nowline);
						}
					}
					for (int need = 0; need < query.needcategory.size(); need++){
						if (!((1 << need) & nowline->category)){
							for (auto p : (*needpoints)[need]){
								auto copy = new data::oneline(*nowline);
								lineappend(query.start[i], copy, p);
								//当该条路径过长，单条超出lup时不必加入堆。avg下预测不会有效果。由于考虑单条将res量置零不然会导致堆枚举不完整
								if (T::smallthanlup(0, copy->length, lup)){
									heap.push_back(copy);
									std::push_heap(heap.begin(), heap.end(), cmp);
									//在oneline加入heap时便检查是不是可用于构造最优解。avg无法采用此剪枝，简单测试效果不佳故舍弃
									/*if (copy->length < llow && (copy->category & needcate[i]) == needcate[i] && !terminate){
										terminate = 1;
										T::addtores(res, copy);
									}*/
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
		//if (printend)
		//	printf("--------------get end---------------\n");
		return res;
	}
}