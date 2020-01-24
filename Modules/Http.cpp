#include "Headers/Http.h"
#include "../utils.h"

HTTP::HTTP(char **env){
    headers = setHeaders(env);
    cookie = setCookie(headers["HTTP_COOKIE"]);
    if (headers["REQUEST_METHOD"] == "POST"){
        processRequestBody();
    }
}

vector<string> HTTP::getPostData(){
    vector<string> vals;
    for (const auto& el: postData){
        vals.push_back(urlDecode(el.second));
    }
    return vals;
}

string HTTP::getHeader(const string& name){
    return headers[name];
}

string HTTP::getCookie(const string& key){
    return cookie[key];
} 

string HTTP::urlDecode(string str){
    string ret;
    char ch;
    int i, ii, len = str.length();
    for (i=0; i < len; i++){
        if(str[i] != '%'){
            if(str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }else{
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}

map<string, string> HTTP::setHeaders(char **env){
    map<string,string> headers;
    ofstream output("requestHeaders.text");
    while (*env){
        string s = *env++;
        output << s << endl;
        string key = s.substr(0, s.find('='));
        string val = s.substr(s.find('=')+1);
        headers[key] = val;
    }
    output.close();
    return headers;
}
map<string, string> HTTP::setCookie(const string& data){
    map<string, string> cookie;
    if (data != ""){
        cookie = split(data, '=', ';'); 
    }
    return cookie;
}

void HTTP::processRequestBody(){
    string contentType = headers["CONTENT_TYPE"];
    if (contentType.find("multipart/form-data") != -1){
        // Загрузка файла
        string line;
        getline(cin, line);
        string boundary = line.erase(line.size()-1);
        while (getline(cin, line)){
            string disposition = line.substr(32, line.size()-33);
            map<string, string> m = split(disposition, '=', ';');
            string fn = strip(m["filename"], '\"');
            if (fn != ""){
                string path = "./files/" + fn;
                ofstream output(path, ios_base::binary);
                getline(cin, line);
                string cType = line; //TODO mime type
                getline(cin, line); // empty string
                getline(cin, line); // body
                while(line.find(boundary) == -1){
                    output << line << endl;
                    getline(cin, line);
                }
                output.close();
            }else{
                while(line.find(boundary) == -1){
                    getline(cin, line);
                }
            }
        } 
    }else{
        cin >> data;
        postData = split(data, '=', '&'); // TODO split2
    }    
}