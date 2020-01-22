#include <iostream>
#include <string>
#include <map>
using namespace std;

map<string, string> fun(char** env){
    map<string,string> headers;
    while (*env){
        string s = *env++;
        string key = s.substr(0, s.find('='));
        string val = s.substr(s.find('=')+1);
        headers[key] = val;
    }
    return headers;
}

int main(int argc, char **argv, char** env)
{
    cout << "Content-type:text/html\r\n\r\n";
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>CGI Envrionment Variables</title>\n";
    cout << "</head>\n";
    cout << "<body>\n";
    // cout << *env << "<br/>";
    map<string, string> headers = fun(env);
        // cout << *env++ << "<br/>";
    cout << headers["abc"] << endl;
    cout << "</body>\n";
    cout << "</html>\n";

    return -1;
}