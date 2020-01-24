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
    string getCookie(const string& key);
private:
    string data;
    string method;
    string pathInfo;
    map<string, string> cookie;
    map<string, string> postData;
    map<string, string> headers;
    string urlDecode(string str);
    map<string, string> setHeaders(char **env);
    map<string, string> setCookie(const string& data);
    void processRequestBody();
};