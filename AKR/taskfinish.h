#pragma once
#include <vector>
#include <set>
#include "data.h"
#include "geo.h"
namespace taskfinish{
	struct onestart{
		std::vector<data::oneline*> heap;
		std::set<unsigned long long> hash;
		std::vector<data::oneline*> pre;
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
	class onelinehash {
	private:
		int left, right;
		std::vector<std::vector<data::oneline*>> min;
	public:
		onelinehash(int categorynumber);
		void insert(int category, data::oneline *number);
		data::oneline* findmin(int category) const;
	};
}