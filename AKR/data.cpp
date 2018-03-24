#include "data.h"
namespace data {
	void query::write(std::string filename){
		FILE *f = fopen(filename.c_str(), "w");
		fprintf(f, writestr().c_str());
		fclose(f);
	}
	std::string query::writestr(){
		std::string res;
		auto &q = *this;
		char buffer[1000] = { 0 };
		sprintf(buffer, "%d\n", q.start.size());
		res += buffer;
		for (auto i : q.start){
			sprintf(buffer, "%f %f\n", i.x, i.y);
			res += buffer;
		}
		sprintf(buffer, "%d\n", q.endcategory.size());
		res += buffer;
		for (auto i : q.endcategory){
			res += i == -1 ? "Unknown Keyword" : init::num2words[i];
			res += "\n";
		}
		sprintf(buffer, "%d\n", q.needcategory.size());
		res += buffer;
		for (auto i : q.needcategory){
			res += i == -1 ? "Unknown Keyword" : init::num2words[i];
			res += "\n";
		}
		return res;
	}
	void result::write(std::string filename, query *q){
		FILE *f = fopen(filename.c_str(), "w");
		if (q != nullptr){
			fprintf(f, "query data\n----------------\n");
			fprintf(f, q->writestr().c_str());
			fprintf(f, "----------------\n");
		}
		fprintf(f, writestr().c_str());
		fclose(f);
	}
	//endpoint res均为int，对应mappoints中的点。数据量大不作查找
	std::string result::writestr(){
		//TODO mappoints内容输出，以及相关数据进行验证
		std::string resstr;
		char buffer[1000] = { 0 };
		sprintf(buffer, "reslength: %.6f\n", reslength);
		resstr += buffer;
		sprintf(buffer, "endpoint: %d\n", endpoint);
		resstr += buffer;
		for (int i = 0; i < lines.size(); i++){
			sprintf(buffer, "route ID %d, length: %.6f\nroute:", i, lines[i].length);
			resstr += buffer;
			for (auto j : lines[i].res){
				sprintf(buffer, " %d", j);
				resstr += buffer;
			}
			resstr += "\n";
		}
		return resstr;
	}
}