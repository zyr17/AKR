#include <iostream>
#include <map>
#include "init.h"
#include "old.h"
int main() {
	std::map<std::string, int> words2num;
	//map数据格式：一行一个点，坐标x, y，语义数量k，若干语义空格分隔，语义中间无空格，最长100
	auto mappoints = init::initmappoints("", words2num);
	//query数据格式：起点数量，起点x, y，终点语义数量，终点语义，经过语义数量，经过语义
	auto query = init::initquery("", words2num);
	data::result oldtrueres = old::trueway(mappoints, query);
	data::result oldgreedyres = old::greedyway(mappoints, query);

}