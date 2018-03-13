#include "ann.h"
#include "geo.h"
namespace ann {
	bool iscover(const std::vector<int> &small, const std::vector<int> &big){
		for (auto i : small){
			bool flag = 0;
			for (auto j : big)
				if (i == j){
					flag = 1;
					break;
				}
			if (!flag) return 0;
		}
		return 1;
	}
}