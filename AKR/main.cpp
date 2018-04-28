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
#define RANDOMNUM 100
#define MAPPOINTFILE "format.txt"
#else
#define DATAFOLDER "../Data/naive/"
#define RANDOMNUM 8
#define MAPPOINTFILE "data.txt"
#endif
typedef maxavg::maxclass USEDCLASS;
void multitests(int times, double &trueclock, std::vector<double> &greedyclock, std::vector<double> &greedypoint, std::vector<data::mappoint> &mappoints, std::map<std::string, int> &words2num, std::vector<data::query> &randomed, bool forcerandom = false){
	trueclock = 0;
	greedyclock.clear();
	greedypoint.clear();
	//0 = naive, 1 = naive+, 2=better, 3 = better+
	greedyclock.resize(4);
	greedypoint.resize(4);
	for (int i = 1; i <= times; i++){
		char buffer[1000] = { 0 };
		printf("multitest doing %d\n", i);
		data::query query;
		if (randomed.size() >= i && !forcerandom){
			query = randomed[i - 1];
		}
		else{
			query = init::randomquery(4, 5, 6, 4, 5, 0.09, mappoints, words2num);
			sprintf(buffer, DATAFOLDER "/random/query/%06d.txt", i);
			query.write(buffer);
		}
		data::result exactres = funcs<USEDCLASS>::exactway2(mappoints, query);
		/*
		data::result exactres;
		exactres.reslength = 0.01;
		exactres.time = 1;
		*/
		sprintf(buffer, DATAFOLDER "/random/result/%06d.txt", i);
		exactres.write(buffer, &query);
		trueclock += exactres.time;
		data::result naivegreedyres = funcs<USEDCLASS>::naivegreedyway(mappoints, query);
		greedyclock[0] += naivegreedyres.time;
		data::result naivegreedyplusres = funcs<USEDCLASS>::naivegreedywayplus(mappoints, query);
		greedyclock[1] += naivegreedyplusres.time;
		data::result bettergreedyres = funcs<USEDCLASS>::bettergreedyway(mappoints, query);
		greedyclock[2] += bettergreedyres.time;
		data::result bettergreedyplusres = funcs<USEDCLASS>::bettergreedywayplus(mappoints, query);
		greedyclock[3] += bettergreedyplusres.time;
		printf("time res: %d %d %d %d %d\n", exactres.time, naivegreedyres.time, naivegreedyplusres.time, bettergreedyres.time, bettergreedyplusres.time);
		/*
		if (exactres.reslength > naivegreedyplusres.reslength * (1 + 1e-8) + 1e-8){
			printf("naivegreedyplus wrong in %d\n", i);
			for (;;);
		}
		if (exactres.reslength > bettergreedyplusres.reslength * (1 + 1e-8) + 1e-8){
			printf("bettergreedyplus wrong in %d\n", i);
			for (;;);
		}
		*/
		/*if (bettergreedyres.reslength > naivegreedyres.reslength * (1 + 1e-8) + 1e-8){
			printf("bettergreedy worse than naivegreedy in %d\n", i);
			for (;;);
		}*/
		greedypoint[0] += naivegreedyres.reslength / exactres.reslength;
		greedypoint[1] += naivegreedyplusres.reslength / exactres.reslength;
		greedypoint[2] += bettergreedyres.reslength / exactres.reslength;
		greedypoint[3] += bettergreedyplusres.reslength / exactres.reslength;
	}
	trueclock /= times;
	for (int i = 0; i < 4; i++) {
		greedypoint[i] /= times;
		greedyclock[i] /= times;
	}
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

	double trueclock;
	std::vector<double> greedyclock, greedypoint;
	multitests(RANDOMNUM, trueclock, greedyclock, greedypoint, mappoints, words2num, randomed);
	FILE *f = fopen(DATAFOLDER "random/res.txt", "w");
	fprintf(f, "true: %f 1.000000\nnaivegreedy: %f %f\nnaivegreedyplus: %f %f\nbettergreedy: %f %f\nbettergreedyplus: %f %f\n", trueclock, greedyclock[0], greedypoint[0], greedyclock[1], greedypoint[1], greedyclock[2], greedypoint[2], greedyclock[3], greedypoint[3]);
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