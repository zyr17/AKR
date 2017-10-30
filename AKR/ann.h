#pragma once
#include <vector>
#include "data.h"
namespace ann {
	//TODO read paper
	//ÿ�ε��÷���һ��δ���ع���ANN�����ʹ�÷���2��ÿ�μ�����;�㻺���������ظ�����
	struct ann {
		std::vector<int> mindistance, endorder;
		std::vector<double> enddis;
		std::vector<data::mappoint> mappoints;
		data::query query;
		double centerr;
		geo::point center;
		ann(std::vector<data::mappoint> &inmappoints, data::query &inquery);
		int nextsmallest();
	};
}