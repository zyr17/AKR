#include <iostream>
#include <map>
#include "init.h"
#include "old.h"
int main() {
	std::map<std::string, int> words2num;
	//map���ݸ�ʽ��һ��һ���㣬����x, y����������k����������ո�ָ��������м��޿ո��100
	auto mappoints = init::initmappoints("", words2num);
	//query���ݸ�ʽ��������������x, y���յ������������յ����壬����������������������
	auto query = init::initquery("", words2num);
	data::result oldtrueres = old::trueway(mappoints, query);
	data::result oldgreedyres = old::greedyway(mappoints, query);

}