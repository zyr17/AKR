#include "test.h"
namespace test {
	void testheaphash(){
		int catenum = 5;
		int startnum = 3;
		heaphash::heaphash hh(catenum);
		data::detaildata detail;
		detail.category.resize(catenum);
		detail.res.lines.resize(startnum);

		detail.category[0] = 1;
		detail.category[3] = 1;
		detail.category[4] = 1;
		detail.res.reslength = 10.5;
		detail.res.lines[0].res.push_back(1);
		detail.res.lines[1].res.push_back(100);
		volatile auto res = hh.find(detail);
		printf("%f\n", res);
		hh.insert(detail);
		res = hh.find(detail);
		printf("%f\n", res);
		auto detail2 = detail;
		detail2.category[4] = 0;
		res = hh.find(detail2);
		printf("%f\n", res);
		detail2.res.reslength = 11.5;
		hh.insert(detail2);
		res = hh.find(detail2);
		printf("%f\n", res);
		detail2.res.reslength = 9.5;
		hh.insert(detail2);
		res = hh.find(detail2);
		printf("%f\n", res);
		detail2.res.lines[0].res.push_back(100);
		detail2.res.lines[1].res.push_back(1);
		res = hh.find(detail2);
		printf("%f\n", res);
		detail2.res.lines[0].res.pop_back();
		detail2.res.lines[0].res.pop_back();
		res = hh.find(detail2);
		printf("%f\n", res);
		hh.insert(detail2);
	}
}