#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

class DataBase{
public:
    DataBase(const string& fp);
    void add(vector<string> data);
    void del(string id);
    vector<vector<string>> readAll();
private:
    string filePath;
    char delimiter = ':';
};