#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <experimental/filesystem>
#include "utils.h"
// std :: hex 
using namespace std;
namespace fs = std::experimental::filesystem;


class HTTP{
public:
    HTTP(char **env){
        headers = setHeaders(env);
        if (headers["REQUEST_METHOD"] == "POST"){
            processRequestBody();
        }
    }
    vector<string> getPostData(){
        vector<string> vals;
        for (const auto& el: postData){
            vals.push_back(urlDecode(el.second));
        }
        return vals;
    }
    string getHeader(const string& name){
        return headers[name];
    }
    
private:
    string data;
    string method;
    string pathInfo;
    map<string, string> postData;
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
                render("users.html");
            }else if(route == "/delUser"){
                db.del(postData[0]);
                render("users.html");
            }else if(route == "/upload"){
                // загрузка файлов
                // string line;
                // ofstream output("file.txt");
                // while(getline(cin , line)){
                //     output << line << endl;
                // }
                // output.close();
                render("files.html"); 
            }else if(route == "/delFile"){
                // TODO
            }
        }else if(method == "GET"){
            if (route == "/upload"){
                render("files.html");
            }else if (route == ""){
                render("users.html");
            }else if (route == "/files"){
                // TODO Список файлов
                cout << "Content-type:text/html\r\n\r\n";
                for(auto& p: fs::directory_iterator("files")){
                    cout << p << " ";
                    try {
                        cout << fs::file_size(p) <<" bytes"<< "<br>"; // attempt to get size of a directory
                    }catch(fs::filesystem_error& e) {
                        cout << e.what() << "<br>";
                    }   
                }
            }
        }
        // cout << req.getPathInfo() << endl;
        // for (auto el : req.getPostData()){
        //     cout << el << endl;
        // }
    } 
private:
    void renderUsers(const string& line, bool& first){
        DataBase db("db");
        DbData = db.readAll();
        if (line == "            </tr>" && first == true){
                    first = false;
                    if (DbData.empty()){
                        cout << "DataBase is empty" << endl;
                    }else{
                        for (const auto& row: DbData){
                            cout << "<form action='http://localhost/cgi-bin/main.cgi/delUser' method='POST'>" << endl;
                            cout << "<tr>" << endl;
                            for (const auto& col : row){
                                cout << "<td>" << col << "</td>" << endl;
                            }
                            cout << "<td style='border: none;'><input type='submit' value='Del'></td>" << endl;
                            cout << "<td style='display:none;'><input type='text' name='p4' value='"<< row[0] << "'></td>" << endl;
                            cout << "</tr>" << endl;
                            cout << "</form>" << endl;

                        }
                    }
                }
    }
    void renderFiles(const string& line, bool& first){
        bool empty = true;
        if (line == "            </tr>" && first == true){
            for (auto& p : fs::directory_iterator(filesDir)){
                empty = false;
                cout << "<form action='http://localhost/cgi-bin/main.cgi/delFile' method='POST'>" << endl;
                cout << "<tr>" << endl;
                cout << "<td>" << strip(p.path(), '\"').substr(filesDir.size()+1) << "</td>" << endl;
                cout << "<td>" << fs::file_size(p) << " bytes" << "</td>" << endl;
                cout << "<td style='border: none;'><input type='submit' value='Del'></td>" << endl;
                cout << "<td style='display:none;'><input type='text' name='p' value='"<< p << "'></td>" << endl;
                cout << "</tr>" << endl;
                cout << "</form>" << endl;

            }
            first = false;
            if (empty == true){
                cout << "</table><h4>Dir is empty</h4>";
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
                if (filename == "users.html"){
                    renderUsers(line, first);
                }else if(filename == "files.html"){
                    renderFiles(line, first);
                }
            }
        }

    }
    vector<vector<string>> DbData;
    string filesDir = "files";
};


int main(int argc, char **argv, char** env){
    HTTP req(env);
    Application myApp(req);
    return -1;
}