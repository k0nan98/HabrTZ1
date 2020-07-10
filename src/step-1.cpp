// step-1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include<cstdio>
#include <windows.h>
#include <fstream>
#include <string>
#include <ctime>
#include <filesystem>
using namespace std;

std::ofstream myfile;
std::string workingdir()
{
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf) + '\\';
}
void charChanger(string _patch) {
	std::fstream fs(_patch, std::fstream::in | std::fstream::out);
	if (fs.is_open()) {
		while (!fs.eof()) {
			if (fs.get() == '\r') {
				fs.seekp((fs.tellp() - static_cast<std::streampos>(1)));
				fs.put('\n');
				fs.seekp(fs.tellp());
			}
		}
		fs.close();
	}
	else {

		std::cout << "Faild to open" << '\n';
	}
}
string parseSensorFile(string line, tm fileT, string patch) { //patch = path to work directory

	try {
		
			string str = line;
			tm tm1, tm2;
			int afterdot, afterdot2, mustBeOne, polarAngle, azimuthAngle;
			float e;
			sscanf(str.c_str(), "%4d-%2d-%2d %2d:%2d:%2d.%3d -> %4d-%2d-%2d %2d:%2d:%2d.%3d -> %*2c-%*2d-%*1c; %*5d-%*2d:%*1d:%f:%*1d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday,
				&tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec, &afterdot, &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday,
				&tm2.tm_hour, &tm2.tm_min, &tm2.tm_sec, &afterdot2, &e);
			return to_string(tm2.tm_year) + "-" + to_string(tm2.tm_mon) + "-" + to_string(tm2.tm_mday) + " " + to_string(tm2.tm_hour) + ":" + to_string(tm2.tm_min) + ":" + to_string(tm2.tm_sec) +"," +to_string(e);
			//cout << tm1.tm_year << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
		

	}
	catch (exception e) {

		cout << "Error in data format";
	}
	return 0;
}


bool sameFile = false;
string parse(string line, tm fileT, string time, string dir) { //dir = path to work directory
	if (sameFile == false) {
		myfile << "motor time,polar angle,azimuth angle,sensor time,E\n";
		sameFile = true;
	}
	try {

			string str = line;
			tm tm1;
			int afterdot, mustBeOne, polarAngle, azimuthAngle;
			sscanf(str.c_str(), "%4d-%2d-%2d %2d:%2d:%2d.%3d -> %1d:%3d:%3d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday,
				&tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec, &afterdot, &mustBeOne, &polarAngle, &azimuthAngle);

			if (mustBeOne == 1) {
				tm result = tm();
				result.tm_year = tm1.tm_year - fileT.tm_year;
				result.tm_mon = tm1.tm_mon - fileT.tm_mon;
				result.tm_mday = tm1.tm_mday - fileT.tm_mday;
				result.tm_hour = tm1.tm_hour - fileT.tm_hour;
				result.tm_min = tm1.tm_min - fileT.tm_min;
				result.tm_sec = tm1.tm_sec - fileT.tm_sec;
				string fres = to_string(result.tm_year)+ "-" + to_string(result.tm_mon)+ "-" + to_string(result.tm_mday)+" " + to_string(result.tm_hour)+":" + to_string(result.tm_min)+":" + to_string(result.tm_sec)+","+to_string(polarAngle-500)+","+ to_string(azimuthAngle - 500);
				

				return fres;

			}
			//cout << tm1.tm_year << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
		
	}
	catch (exception e) {

		cout << "Error in data format";
	}
	return "";
		

}

void readFile(string _patch, tm fileT, string fileTime, string workDir) {
	sameFile = false;

	string s; // сюда будем класть считанные строки
	ifstream file(_patch); // файл из которого читаем (для линукс путь будет выглядеть по другому)
	charChanger(_patch);
	string s2; // сюда будем класть считанные строки
	ifstream file2(workDir + fileTime + " sensor data.log"); // файл из которого читаем (для линукс путь будет выглядеть по другому)
	charChanger(workDir + fileTime + " sensor data.log");
	while (getline(file, s) && getline(file2, s2)) { // пока не достигнут конец файла класть очередную строку в переменную (s)
		
		cout << s << endl; // выводим на экран

		string a = parse(s, fileT, fileTime, workDir);
		if (a.length() > 1) {
			myfile << a << "," << parseSensorFile(s2, fileT, workDir) << "\n";
		}
		myfile.flush();

	}

	file.close(); // обязательно закрываем файл что бы не повредить его
	file2.close(); // обязательно закрываем файл что бы не повредить его

}
void findAllFiles(string _patch)
{
	string patch = _patch + "\\*.*";
	wstring stemp = wstring(patch.begin(), patch.end());
	WIN32_FIND_DATA data;
	LPCWSTR lpatch = stemp.c_str();

	HANDLE h = FindFirstFileW(lpatch, &data);

	if (h != INVALID_HANDLE_VALUE) //Если директория существует
	{
		do //Перебор имён
		{
			char* nPtr = new char[lstrlen(data.cFileName) + 1];
			for (int i = 0; i < lstrlen(data.cFileName); i++)
				nPtr[i] = char(data.cFileName[i]);

			nPtr[lstrlen(data.cFileName)] = '\0';
			cout << nPtr << endl; // Вывод имени файла
			string filename = nPtr; //+1 Костыль
			if (filename.size() >= 14) {
				if (filename.find("sensor data") != std::string::npos) {

					return; //break on sensor data files
				}
				tm fileT;
				//20200626155830 motor position.log
				string subname;
				for (int i = 0; i < 14; i++) {
					subname += nPtr[i];
				}
				sscanf(nPtr, "%4d%2d%2d%2d%2d%2d", &fileT.tm_year, &fileT.tm_mon, &fileT.tm_mday,
					&fileT.tm_hour, &fileT.tm_min, &fileT.tm_sec);

				
				string fileTime = subname;

				myfile.open(_patch + "\\" + fileTime + ".csv");

				readFile(_patch + "\\" + nPtr, fileT, fileTime, _patch+"\\");  //Чтение из файла

				myfile.flush();
				myfile.close();

				
			}
		} while (FindNextFile(h, &data)); //Перебор имён
	}
	else
		cout << "Error: No such folder." << endl;

	FindClose(h);

}


int main() {
	string _folder = workingdir();
	//string _folder = "D:\\222\\111";

	findAllFiles(_folder);

	return 0;

}

