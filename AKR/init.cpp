#include "init.h"
namespace init {
	std::vector<data::mappoint> init::initmappoints(std::string filename, std::map<std::string, int> words2num) {
		FILE *f = fopen(filename.c_str(), "r");
		std::vector<data::mappoint> res;
		double x, y;
		int k;
		while (~fscanf(f, "%lf%lf%d", &x, &y, &k)) {
			data::mappoint tmp(x, y);
			for (; k--; ) {
				char buffer[100] = { 0 };
				fscanf(f, "%s", buffer);
				std::string name = buffer;
				if (words2num.find(name) == words2num.end()) {
					int nownum = words2num.size();
					words2num[name] = nownum;
				}
				tmp.category.push_back(words2num[name]);
			}
			res.push_back(tmp);
		}
		fclose(f);
	}

	data::query init::initquery(std::string filename, std::map<std::string, int> words2num) {
		FILE *f = fopen(filename.c_str(), "r");
		data::query res;
		int k;
		for (fscanf(f, "%d", &k); k--; ) {
			double x, y;
			fscanf(f, "%lf%lf", &x, &y);
			res.start.push_back(geo::point(x, y));
		}
		char buffer[100] = { 0 };
		for (fscanf(f, "%d", &k); k--; ) {
			fscanf(f, "%s", buffer);
			std::string str(buffer);
			if (words2num.find(str) != words2num.end())
				res.endcategory.push_back(words2num[str]);
			else res.endcategory.push_back(-1);
		}
		for (fscanf(f, "%d", &k); k--; ) {
			fscanf(f, "%s", buffer);
			std::string str(buffer);
			if (words2num.find(str) != words2num.end())
				res.needcategory.push_back(words2num[str]);
			else res.needcategory.push_back(-1);
		}
		return res;
	}
}