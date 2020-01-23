#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
// std :: hex 
using namespace std;

class HTTP{
public:
    HTTP(char **env){
        headers = setHeaders(env);
        if (headers["REQUEST_METHOD"] == "POST"){
            processRequestBody();
        }
    }
    vector<string> getPostData(){
        return postData;
    }
    string getHeader(const string& name){
        return headers[name];
    }
    
private:
    string data;
    string method;
    string pathInfo;
    vector<string> postData;
    map<string, string> headers;

    string urlDecode(string str){
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
    vector<string> split(const string& data){
        vector<string> postData;
        string p;
        bool flag;
        for (auto ch : data){
            if (ch == '='){
                flag = true;
                continue;
            }else if(ch == '&'){
                flag = false;
                postData.push_back(urlDecode(p));
                p = "";
            }
            if (flag == true){
                p += ch;
            }
        }
        return postData;
    }
    string strip(string s, char ch = ' '){
        bool sFlag = true;
        bool eFlag = true;
        int sIdx = 0;
        int eIdx = s.size();
        while(sFlag != false || eFlag != false){
            if(sFlag == true && s[sIdx] == ch){
                sIdx++;
            }else{
                sFlag = false;
            }
            if(eFlag == true && s[eIdx - 1] == ch){
                eIdx--;
            }else{
                eFlag = false;
            }
        }
        return s.substr(sIdx, eIdx-sIdx);
    }
    map<string,string> split2(string s, char d1, char d2){
        map<string, string> splitedData;
        istringstream stream1(s);
        string line;
        while(getline(stream1, line, d2)){
            line = strip(line);
            istringstream stream2(line);
            string key;
            string val;
            getline(stream2, key, d1);
            getline(stream2, val, d1);
            splitedData[key] = val;
        }
        return splitedData;
    }   
    
    map<string, string> setHeaders(char **env){
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
    void processRequestBody(){
        string contentType = headers["CONTENT_TYPE"];
        if (contentType.find("multipart/form-data") != -1){
            // Загрузка файла
            string line;
            getline(cin, line);
            string boundary = line.erase(line.size()-1);
            while (getline(cin, line)){
                string disposition = line.substr(32, line.size()-33);
                map<string, string> m = split2(disposition, '=', ';');
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
            data += '&';
            postData = split(data); // TODO split2
        }    
    }
};

class DataBase{
public:
    DataBase(const string& fp){
        filePath = fp;
    }
    void add(vector<string> data){
        vector<vector<string>> dbData = readAll();
        string record;
        if (dbData.size() == 0){
            record = '1' + delimiter;
        }else{
            istringstream ss(dbData[dbData.size()-1][0]);
            int last_id;
            ss >> last_id;
            last_id++;
            string s = to_string(last_id);
            record = s + delimiter;
        }
        for (int i = 0; i < data.size(); ++i){
            record += data[i];            
            if (i < data.size()-1){
                record += delimiter;
            }
        }
        ofstream output(filePath, ios::app);
        output << record << endl;
        
    }
    void del(string id){
        vector<vector<string>> dbData = readAll();
        string record;
        ofstream output(filePath);
        for (const auto& row:dbData){
            if (row[0] != id){
                for (int i = 0; i < row.size(); ++i){
                    record += row[i];
                    if (i < row.size()-1){
                        record += delimiter;
                    }       
                }
                output << record << endl;
                record.clear();
            }
        }
    }
    vector<vector<string>> readAll(){
        ifstream input(filePath);
        string row;
        vector<vector<string>> data;
        while (getline(input, row)){
            istringstream rowStream(row);
            string col;
            vector<string> rowArr; 
            while (getline(rowStream, col, delimiter)){
                rowArr.push_back(col);
            }
            data.push_back(rowArr);
        }
        return data;

    }
    
private:
    string filePath;
    char delimiter = ':';
};

class Application{
public:
    Application(HTTP req){
        string route = req.getHeader("PATH_INFO");
        string method = req.getHeader("REQUEST_METHOD");
        if (method == "POST"){
            DataBase db("db");
            vector<string> postData = req.getPostData();
            if (route == "/add"){    
                db.add(postData);
                render("index.html");
            }else if(route == "/del"){
                db.del(postData[0]);
                render("index.html");
            }else if(route == "/upload"){
                // загрузка файлов
                // string line;
                // ofstream output("file.txt");
                // while(getline(cin , line)){
                //     output << line << endl;
                // }
                // output.close();
                render("uploadFiles.html"); 
            }
        }else if(method == "GET"){
            if (route == "/upload"){
                render("uploadFiles.html");
            }else if (route == ""){
                render("index.html");
            }else if (route == "/files"){
                // TODO Список файлов
            }
        }
        // cout << req.getPathInfo() << endl;
        for (auto el : req.getPostData()){
            cout << el << endl;
        }
    } 
private:
    void fun(const string& line, bool& first){
        DataBase db("db");
        DbData = db.readAll();
        if (line == "                    </tr>" && first == true){
                    first = false;
                    if (DbData.empty()){
                        cout << "DataBase is empty" << endl;
                    }else{
                        for (const auto& row: DbData){
                            cout << "<form action='http://localhost/cgi-bin/main.cgi/del' method='POST'>" << endl;
                            cout << "<tr>" << endl;
                            for (const auto& col : row){
                                cout << "<td>" << col << "</td>" << endl;
                            }
                            cout << "<td style='border: none;'><input type='submit' value='Del'></td>" << endl;
                            cout << "<td style='display:none;'><input type='text' name='p' value='"<< row[0] << "'></td>" << endl;
                            cout << "</tr>" << endl;
                            cout << "</form>" << endl;

                        }
                    }
                }

    }
    void render(const string& filename){
        ifstream input(filename);
        string line;
        bool first = true;
        cout << "Content-type:text/html\r\n\r\n";
        if (input){
            while(getline(input, line)){
                cout << line << endl;
                if (filename == "index.html"){
                    fun(line, first);
                }
            }
        }

    }
    vector<vector<string>> DbData;
};


int main(int argc, char **argv, char** env){
    HTTP req(env);
    Application myApp(req);
    return -1;
}