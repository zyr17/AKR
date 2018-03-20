#pragma once
#include <map>
#include <algorithm>
#include <cassert>
#include "data.h"
namespace heaphash {
	class hashvalue {
	private:
		int left, right;
		std::vector<std::vector<double>> min;
	public:
		unsigned long long hash;
		hashvalue(int categorynumber);
		hashvalue(){}
		void insert(std::vector<bool> &categoryvec, double number);
		double findmin(std::vector<bool> &categoryvec) const;
	};
	class heaphash {
	private:
		const unsigned long long multi1 = 1999993, multi2 = 2199979;
		int catenum;
		std::map<unsigned long long, hashvalue> map;
	public:
		void insert(data::detaildata &detail);
		double find(data::detaildata &detail);
		heaphash(int categorynumber);
	};
}