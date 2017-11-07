#pragma once
#include <vector>
#include <map>
#include <iostream>
#include "data.h"
#include "init.h"
namespace init {
	std::vector<data::mappoint> initmappoints(std::string filename, std::map<std::string, int> words2num);
	data::query initquery(std::string filename, std::map<std::string, int> words2num);
}