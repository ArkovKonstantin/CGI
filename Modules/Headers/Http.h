#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

class HTTP{
public:
    HTTP(char **env);
    vector<string> getPostData();
    string getHeader(const string& name);
private:
    string data;
    string method;
    string pathInfo;
    map<string, string> postData;
    map<string, string> headers;
    string urlDecode(string str);
    map<string, string> setHeaders(char **env);
    void processRequestBody();
};