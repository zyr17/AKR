#include <iostream>
#include <map>
#include <ctime>
#include <direct.h>
#include "init.h"
#include "old.h"
#define DATAFOLDER "../Data/gaode/"
void multitests(int times, double &trueclock, double &greedyclock, double &greedypoint, std::vector<data::mappoint> &mappoints, std::map<std::string, int> &words2num) {
	trueclock = greedyclock = greedypoint = 0;
	for (int i = 1; i <= times; i++) {
		printf("multitest doing %d\n", i);
		auto query = init::randomquery(5, 1, 5, mappoints, words2num);
		int startclock = clock();
		data::result oldtrueres = old::trueway(mappoints, query);
		trueclock += clock() - startclock;
		startclock = clock();
		data::result oldgreedyres = old::greedyway(mappoints, query);
		greedyclock += clock() - startclock;
		greedypoint += oldgreedyres.maxlength / oldtrueres.maxlength;
	}
	trueclock /= times;
	greedyclock /= times;
	greedypoint /= times;
}
int main() {
	srand(unsigned(time(NULL)));
	char buffer[1000];
	_getcwd(buffer, 1000);
	printf("%s", buffer);
	std::map<std::string, int> words2num;
	//map���ݸ�ʽ��һ��һ���㣬����x, y����������k����������ո�ָ��������м��޿ո��100
	auto mappoints = init::initmappoints(DATAFOLDER "small.txt", words2num);
	//query���ݸ�ʽ��������������x, y���յ������������յ����壬����������������������
	double trueclock, greedyclock, greedypoint;
	multitests(100, trueclock, greedyclock, greedypoint, mappoints, words2num);

	/*
	//auto query = init::initquery(DATAFOLDER "query.txt", words2num);
	auto query = init::randomquery(10, 1, 5, mappoints, words2num);
	data::result oldtrueres = old::trueway(mappoints, query);
	data::result oldgreedyres = old::greedyway(mappoints, query);
	auto res = oldtrueres;
	*/
}