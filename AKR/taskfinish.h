#pragma once
#include <vector>
#include <set>
#include "data.h"
#include "geo.h"
namespace taskfinish{
	class onelinehash {
	private:
		int left, right;
		std::vector<std::vector<data::oneline*>> min;
	public:
		onelinehash(int categorynumber);
		void insert(data::oneline *number);
		data::oneline* findmin(int category) const;
		data::oneline* operator[](int category) const;
	};
	struct onestart{
		std::vector<data::oneline*> heap;
		std::set<unsigned long long> hash;
		onelinehash pre;
		onestart(int categorynumber) : pre(categorynumber) {}
	};
	class taskfinish{
	private:
		data::query query;
		const std::vector<data::mappoint> *mappoints;
		const std::vector<std::vector<int>> *needpoints;
		int m;
		std::vector<onestart> starts;
		static void addtores(data::result &res, data::oneline *line);
		void lineappend(geo::point start, data::oneline *line, int p);
	public:
		taskfinish(const std::vector<data::mappoint> &inmappoints, const data::query &inquery, const std::vector<std::vector<int>> &inneedpoints, int inm);
		data::result get(const std::vector<int> &belong, double llow, double lup);
	};
}