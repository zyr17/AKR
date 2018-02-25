#pragma once
#include "data.h"
#include "ann.h"
#include "heaphash.h"
namespace old {
	data::result trueway(const std::vector<data::mappoint> &mappoints, const data::query &query);
	data::result greedyway(const std::vector<data::mappoint> &mappoints, const data::query &query);
}