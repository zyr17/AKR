#pragma once
#include <vector>
#include "data.h"
namespace ann {
	//TODO read paper
	//ÿ�ε��÷���һ��δ���ع���ANN�����ʹ�÷���2��ÿ�μ�����;�㻺���������ظ�����
	class ann {
		std::vector<int> mindistance, endorder;
		std::vector<double> enddis;
		std::vector<data::mappoint> mappoints;
		data::query query;
		double centerr, minr;
		geo::point center;
	public:
		ann(const std::vector<data::mappoint> &inmappoints, const data::query &inquery);
		int nextsmallest();
	};
}