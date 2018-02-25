#include "heaphash.h"
namespace heaphash {
	hashvalue::hashvalue(int categorynumber){
		left = categorynumber / 2;
		right = categorynumber - left;
		for (int i = 1 << left; i--;){
			std::vector<double> vec;
			vec.resize(1 << right);
			memset(&(vec[0]), 0x70, sizeof(double) * (1 << right));
			min.push_back(vec);
		}
	}
	void hashvalue::insert(std::vector<bool> &categoryvec, double number){
		assert(categoryvec.size() == left + right);
		int lnum = 0, rnum = 0;
		for (int i = 0; i < left; i++)
			lnum = lnum * 2 + (categoryvec[i] == true);
		for (int i = left; i < left + right; i++)
			rnum = rnum * 2 + (categoryvec[i] == true);
		for (int i = lnum;; i = (i - 1) & lnum){
			if (min[i][rnum] > number)
				min[i][rnum] = number;
			if (!i) break;
		}
	}
	double hashvalue::findmin(std::vector<bool> &categoryvec) const{
		assert(categoryvec.size() == left + right);
		//printf("%x\n", this);
		int lnum = 0, rnum = 0;
		for (int i = 0; i < left; i++)
			lnum = lnum * 2 + (categoryvec[i] == true);
		for (int i = left; i < left + right; i++)
			rnum = rnum * 2 + (categoryvec[i] == true);
		//printf("%d %d %d %d\n", left, right, lnum, rnum);
		double res = 1e100;
		rnum ^= (1 << right) - 1;
		for (int i = rnum;; i = (i - 1) & rnum){
			int update = i ^ ((1 << right) - 1);
			if (min[lnum][update] < res)
				res = min[lnum][update];
			if (!i) break;
		}
		return res;
	}
	void heaphash::insert(data::detaildata &detail){
		unsigned long long hash1 = 0, hash2 = 0;
		std::vector<int> points;
		for (int i = 0; i < detail.res.res.size(); i++){
			int last = detail.res.res[i].size() != 0 ? detail.res.res[i][detail.res.res[i].size() - 1] : multi1 - 1 - i;
			points.push_back(last);
		}
		std::sort(points.begin(), points.end());
		for (auto i : points){
			hash1 = (hash1 + i) * multi1;
			hash2 = (hash2 + i) * multi2;
		}
		for (;; hash1++){
			auto i = map.find(hash1);
			if (i != map.end() && i->second.hash != hash2){
				printf("[WARN]in heaphash::insert, hash1 conflict!\n");
				continue;
			}
			if (i == map.end()){
				hashvalue hv(catenum);
				hv.hash = hash2;
				map.insert(std::make_pair(hash1, hv));
			}
			map[hash1].insert(detail.category, detail.res.maxlength);
			break;
		}
	}
	double heaphash::find(data::detaildata &detail){
		unsigned long long hash1 = 0, hash2 = 0;
		std::vector<int> points;
		for (int i = 0; i < detail.res.res.size(); i++){
			int last = detail.res.res[i].size() != 0 ? detail.res.res[i][detail.res.res[i].size() - 1] : multi1 - 1 - i;
			points.push_back(last);
		}
		std::sort(points.begin(), points.end());
		for (auto i : points){
			hash1 = (hash1 + i) * multi1;
			hash2 = (hash2 + i) * multi2;
		}
		for (;; hash1++){
			auto i = map.find(hash1);
			if (i == map.end()) break;
			if (i->second.hash != hash2){
				printf("[WARN]in heaphash::find, hash1 conflict!\n");
				continue;
			}
			//printf("\n%x\n", *i);
			//printf("%x\n", &(i->second));
			return i->second.findmin(detail.category);
		}
		return 1e100;
	}
	heaphash::heaphash(int categorynumber){
		catenum = categorynumber;
	}
}