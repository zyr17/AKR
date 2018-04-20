#pragma once
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>
#include <Windows.h>
#include "data.h"
#include "init.h"
namespace init {
	extern std::vector<std::string> num2words;
	extern std::vector<int> numstime;
	const int limit[] = { 0, 1, 3, 10, 30, 100, 300, 1000, 3000, 10000, 30000, 100000, 300000, 1000000, 3000000 };
	extern std::vector<std::vector<int>> numunderlimit;
	std::vector<data::mappoint> initmappoints(std::string filename, std::map<std::string, int> &words2num);
	data::query initquery(std::string filename, std::map<std::string, int> &words2num);
	data::query randomquery(int startpointnum, int endlimit, int needcategorynum, int minlimit, int maxlimit, double cover, std::vector<data::mappoint> &mappoints, std::map<std::string, int> &words2num);
	void getfiles(const char* lpPath, std::vector<std::string> &fileList);
	std::vector<data::query> getqueries(std::string foldername, std::map<std::string, int> &words2num);
}