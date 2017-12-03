#include <iostream>
#include <map>
#include <direct.h>
#include "init.h"
#include "old.h"
#define DATAFOLDER "../Data/gaode/"
int main() {
	char buffer[1000];
	_getcwd(buffer, 1000);
	printf("%s", buffer);
	std::map<std::string, int> words2num;
	//map���ݸ�ʽ��һ��һ���㣬����x, y����������k����������ո�ָ��������м��޿ո��100
	auto mappoints = init::initmappoints(DATAFOLDER "small.txt", words2num);
	//query���ݸ�ʽ��������������x, y���յ������������յ����壬����������������������
	auto query = init::initquery(DATAFOLDER "query.txt", words2num);
	data::result oldtrueres = old::trueway(mappoints, query);
	data::result oldgreedyres = old::greedyway(mappoints, query);
	auto res = oldtrueres;
}