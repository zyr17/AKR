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
}