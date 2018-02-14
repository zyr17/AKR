#include "init.h"
namespace init {
	std::vector<std::string> num2words;
	std::vector<data::mappoint> initmappoints(std::string filename, std::map<std::string, int> &words2num){
		FILE *f = fopen(filename.c_str(), "r");
		std::vector<data::mappoint> res;
		double x, y;
		int k;
		while (~fscanf(f, "%lf%lf%d", &x, &y, &k)){
			data::mappoint tmp(x, y);
			for (; k--; ){
				char buffer[100] = { 0 };
				fscanf(f, "%s", buffer);
				std::string name = buffer;
				if (words2num.find(name) == words2num.end()){
					int nownum = words2num.size();
					words2num[name] = nownum;
					num2words.push_back(name);
				}
				tmp.category.push_back(words2num[name]);
			}
			res.push_back(tmp);
			//printf("%d\n", res.size());
		}
		fclose(f);
		return res;
	}

	data::query initquery(std::string filename, std::map<std::string, int> &words2num){
		FILE *f = fopen(filename.c_str(), "r");
		data::query res;
		int k;
		for (fscanf(f, "%d", &k); k--; ){
			double x, y;
			fscanf(f, "%lf%lf", &x, &y);
			res.start.push_back(geo::point(x, y));
		}
		char buffer[100] = { 0 };
		for (fscanf(f, "%d", &k); k--; ){
			fscanf(f, "%s", buffer);
			std::string str(buffer);
			if (words2num.find(str) != words2num.end())
				res.endcategory.push_back(words2num[str]);
			else res.endcategory.push_back(-1);
		}
		for (fscanf(f, "%d", &k); k--; ){
			fscanf(f, "%s", buffer);
			std::string str(buffer);
			int tnum = -1;
			if (words2num.find(str) != words2num.end())
				tnum = words2num[str];
			bool inend = 0;
			for (auto i : res.endcategory)
				if (i == tnum) inend = 1;
			if (!inend) res.needcategory.push_back(tnum);
		}
		return res;
	}
	data::query randomquery(int startpointnum, int endcategorynum, int needcategorynum, std::vector<data::mappoint> &mappoints, std::map<std::string, int>& words2num)
	{
		data::query res;
		double minx = 1e100, miny = 1e100, maxx = - 1e100, maxy = - 1e100;
		for (auto i : mappoints){
			if (minx > i.p.x) minx = i.p.x;
			if (miny > i.p.y) miny = i.p.y;
			if (maxx < i.p.x) maxx = i.p.x;
			if (maxy < i.p.y) maxy = i.p.y;
		}
		double delx = maxx - minx, dely = maxy - miny;
		for (int i = startpointnum; i--; ){
			double x = minx + delx * rand() / RAND_MAX, y = miny + dely * rand() / RAND_MAX;
			res.start.push_back(geo::point(x, y));
		}
		assert(words2num.size() <= RAND_MAX);
		for (int i = endcategorynum; i--; )
			res.endcategory.push_back(rand() % words2num.size());
		for (int i = needcategorynum; i--; )
			res.needcategory.push_back(rand() % words2num.size());
		return res;
	}
	void writequery(std::string filename, data::query &query){
		FILE *f = fopen(filename.c_str(), "w");
		fprintf(f, "%d\n", query.start.size());
		for (auto i : query.start)
			fprintf(f, "%f %f\n", i.x, i.y);
		fprintf(f, "%d\n", query.endcategory.size());
		for (auto i : query.endcategory)
			fprintf(f, "%s\n", num2words[i]);
		fprintf(f, "%d\n", query.needcategory.size());
		for (auto i : query.needcategory)
			fprintf(f, "%s\n", num2words[i]);
		fclose(f);
	}
}