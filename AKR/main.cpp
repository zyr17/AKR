#include <iostream>
#include <map>
#include <ctime>
#include <direct.h>
#include "init.h"
#include "old.h"
#include "test.h"
#define DATAFOLDER "../Data/gaode/"
void multitests(int times, double &trueclock, double &greedyclock, double &greedypoint, std::vector<data::mappoint> &mappoints, std::map<std::string, int> &words2num, std::vector<data::query> &randomed, bool forcerandom = false){
	trueclock = greedyclock = greedypoint = 0;
	for (int i = 1; i <= times; i++){
		char buffer[1000] = { 0 };
		printf("multitest doing %d\n", i);
		data::query query;
		if (randomed.size() >= i && !forcerandom){
			query = randomed[i - 1];
		}
		else{
			query = init::randomquery(4, 1, 4, mappoints, words2num);
			sprintf(buffer, DATAFOLDER "/random/query/%06d.txt", i);
			query.write(buffer);
		}
		int startclock = clock();
		data::result oldtrueres = old::trueway(mappoints, query);
		sprintf(buffer, DATAFOLDER "/random/result/%06d.txt", i);
		oldtrueres.write(buffer, &query);
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
int main(){
	srand(unsigned(time(NULL)));
	char buffer[1000];
	_getcwd(buffer, 1000);
	printf("%s", buffer);
	std::map<std::string, int> words2num;
	//map���ݸ�ʽ��һ��һ���㣬����x, y����������k����������ո�ָ��������м��޿ո��100
	auto mappoints = init::initmappoints(DATAFOLDER "small.txt", words2num);

	auto randomed = init::getqueries(DATAFOLDER "random/query", words2num);

	double trueclock, greedyclock, greedypoint;
	multitests(100, trueclock, greedyclock, greedypoint, mappoints, words2num, randomed);
	FILE *f = fopen(DATAFOLDER "res.txt", "w");
	fprintf(f, "%f %f %f\n", trueclock, greedyclock, greedypoint);
	fclose(f);
	/*
	//query���ݸ�ʽ��������������x, y���յ������������յ����壬����������������������
	//auto query = init::initquery(DATAFOLDER "query.txt", words2num);
	auto query = init::randomquery(10, 1, 5, mappoints, words2num);
	data::result oldtrueres = old::trueway(mappoints, query);
	data::result oldgreedyres = old::greedyway(mappoints, query);
	auto res = oldtrueres;
	*/
}