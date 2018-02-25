#include "init.h"
namespace init {
	std::vector<std::string> num2words;
	std::vector<data::mappoint> initmappoints(std::string filename, std::map<std::string, int> &words2num){
		FILE *f = fopen(filename.c_str(), "r");
		std::vector<data::mappoint> res;
		double x, y;
		int k;
		while (~fscanf(f, "%lf%lf%d", &x, &y, &k)){
			data::mappoint tmp(x, y);
			for (; k--; ){
				char buffer[100] = { 0 };
				fscanf(f, "%s", buffer);
				std::string name = buffer;
				if (words2num.find(name) == words2num.end()){
					int nownum = words2num.size();
					words2num[name] = nownum;
					num2words.push_back(name);
				}
				tmp.category.push_back(words2num[name]);
			}
			res.push_back(tmp);
			//printf("%d\n", res.size());
		}
		fclose(f);
		return res;
	}

	data::query initquery(std::string filename, std::map<std::string, int> &words2num){
		FILE *f = fopen(filename.c_str(), "r");
		data::query res;
		int k;
		for (fscanf(f, "%d", &k); k--; ){
			double x, y;
			fscanf(f, "%lf%lf", &x, &y);
			res.start.push_back(geo::point(x, y));
		}
		char buffer[100] = { 0 };
		for (fscanf(f, "%d", &k); k--; ){
			fscanf(f, "%s", buffer);
			std::string str(buffer);
			if (words2num.find(str) != words2num.end())
				res.endcategory.push_back(words2num[str]);
			else res.endcategory.push_back(-1);
		}
		for (fscanf(f, "%d", &k); k--; ){
			fscanf(f, "%s", buffer);
			std::string str(buffer);
			int tnum = -1;
			if (words2num.find(str) != words2num.end())
				tnum = words2num[str];
			bool inend = 0;
			for (auto i : res.endcategory)
				if (i == tnum) inend = 1;
			if (!inend) res.needcategory.push_back(tnum);
		}
		return res;
	}
	data::query randomquery(int startpointnum, int endcategorynum, int needcategorynum, std::vector<data::mappoint> &mappoints, std::map<std::string, int>& words2num)
	{
		data::query res;
		double minx = 1e100, miny = 1e100, maxx = - 1e100, maxy = - 1e100;
		for (auto i : mappoints){
			if (minx > i.p.x) minx = i.p.x;
			if (miny > i.p.y) miny = i.p.y;
			if (maxx < i.p.x) maxx = i.p.x;
			if (maxy < i.p.y) maxy = i.p.y;
		}
		double delx = maxx - minx, dely = maxy - miny;
		for (int i = startpointnum; i--; ){
			double x = minx + delx * rand() / RAND_MAX, y = miny + dely * rand() / RAND_MAX;
			res.start.push_back(geo::point(x, y));
		}
		assert(words2num.size() <= RAND_MAX);
		for (int i = endcategorynum; i--; )
			res.endcategory.push_back(rand() % words2num.size());
		for (int i = needcategorynum; i--; )
			res.needcategory.push_back(rand() % words2num.size());
		return res;
	}

	void getfiles(const char* lpPath, std::vector<std::string> &fileList) {
		char szFind[MAX_PATH];
		WIN32_FIND_DATA FindFileData;

		strcpy(szFind, lpPath);
		strcat(szFind, "/*.*");
		wchar_t wszFind[MAX_PATH] = {0};
		MultiByteToWideChar(CP_ACP, 0, szFind, strlen(szFind), wszFind, MultiByteToWideChar(CP_ACP, 0, szFind, strlen(szFind), NULL, 0));
		HANDLE hFind = ::FindFirstFile(wszFind, &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind)    return;

		while (true)
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (FindFileData.cFileName[0] != '.')
				{
					char szFile[MAX_PATH] = {0};
					strcpy(szFile, lpPath);
					strcat(szFile, "/");
					strcat(szFile, (char*)(FindFileData.cFileName));
					getfiles(szFile, fileList);
				}
			}
			else
			{
				//std::cout << FindFileData.cFileName << std::endl;  
				char fn[MAX_PATH] = {0};
				WideCharToMultiByte(CP_ACP, 0, FindFileData.cFileName, wcslen(FindFileData.cFileName), fn, WideCharToMultiByte(CP_ACP, 0, FindFileData.cFileName, wcslen(FindFileData.cFileName), NULL, 0, NULL, NULL), NULL, NULL);
				fileList.push_back(fn);
			}
			if (!FindNextFile(hFind, &FindFileData))    break;
		}
		FindClose(hFind);
	}

	std::vector<data::query> getqueries(std::string foldername, std::map<std::string, int> &words2num){
		std::vector<std::string> files;
		getfiles(foldername.c_str(), files);
		std::vector<data::query> res;
		for (auto i : files)
			res.push_back(initquery((foldername + "/" + i).c_str(), words2num));
		return res;
	}
}