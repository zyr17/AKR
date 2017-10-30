#pragma once
#include <vector>
#include <map>
#include <iostream>
#include "data.h"
namespace init {
	std::vector<data::mappoint> init::initmappoints(std::string filename, std::map<std::string, int> words2num);
	data::query init::initquery(std::string filename, std::map<std::string, int> words2num);
}