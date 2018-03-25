#include <iostream>
#include <map>
#include <ctime>
#include <direct.h>
#include "init.h"
#include "funcs.h"
#include "test.h"
#include "maxavg.h"
#if 1
#define DATAFOLDER "../Data/gaode/"
#define RANDOMNUM 1000
#define MAPPOINTFILE "small.txt"
#else
#define DATAFOLDER "../Data/naive/"
#define RANDOMNUM 8
#define MAPPOINTFILE "data.txt"
#endif
typedef maxavg::avgclass USEDCLASS;
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
		data::result oldtrueres = funcs<USEDCLASS>::exactway2(mappoints, query);
		sprintf(buffer, DATAFOLDER "/random/result/%06d.txt", i);
		oldtrueres.write(buffer, &query);
		trueclock += clock() - startclock;
		startclock = clock();
		data::result oldgreedyres = funcs<USEDCLASS>::naivegreedywayplus(mappoints, query);
		//printf("%f %f\n", oldtrueres.reslength, oldgreedyres.reslength);
		if (oldtrueres.reslength > oldgreedyres.reslength * (1 + 1e-8) + 1e-8){
			printf("greedy wrong in %d\n", i);
			for (;;);
		}
		greedyclock += clock() - startclock;
		greedypoint += oldgreedyres.reslength / oldtrueres.reslength;
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
	//map数据格式：一行一个点，坐标x, y，语义数量k，若干语义空格分隔，语义中间无空格，最长100
	auto mappoints = init::initmappoints(DATAFOLDER MAPPOINTFILE, words2num);

	auto randomed = init::getqueries(DATAFOLDER "random/query", words2num);

	double trueclock, greedyclock, greedypoint;
	multitests(RANDOMNUM, trueclock, greedyclock, greedypoint, mappoints, words2num, randomed);
	FILE *f = fopen(DATAFOLDER "res.txt", "w");
	fprintf(f, "%f %f %f\n", trueclock, greedyclock, greedypoint);
	fclose(f);
	/*
	//query数据格式：起点数量，起点x, y，终点语义数量，终点语义，经过语义数量，经过语义
	//auto query = init::initquery(DATAFOLDER "query.txt", words2num);
	auto query = init::randomquery(10, 1, 5, mappoints, words2num);
	data::result oldtrueres = old::trueway(mappoints, query);
	data::result oldgreedyres = old::greedyway(mappoints, query);
	auto res = oldtrueres;
	*/
}