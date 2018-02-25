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
	std::vector<data::mappoint> initmappoints(std::string filename, std::map<std::string, int> &words2num);
	data::query initquery(std::string filename, std::map<std::string, int> &words2num);
	data::query randomquery(int startpointnum, int endcategorynum, int needcategorynum, std::vector<data::mappoint> &mappoints, std::map<std::string, int> &words2num);
	void getfiles(const char* lpPath, std::vector<std::string> &fileList);
	std::vector<data::query> getqueries(std::string foldername, std::map<std::string, int> &words2num);
}