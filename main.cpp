#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// std :: hex 
using namespace std;

class HTTP{
public:
    HTTP(){
        method = getenv("REQUEST_METHOD");
        if (method == "POST"){
            cin >> data;
            data += '&';
            postData = split(data);
            pathInfo = getenv("PATH_INFO");
        }
    }
    string getPathInfo(){
        return pathInfo;
    }
    vector<string> getPostData(){
        return postData;
    }
    string getMethod(){
        return method;
    }
    
private:
    string data;
    string method;
    string pathInfo;
    vector<string> postData;

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
                postData.push_back(p);
                p = "";
            }
            if (flag == true){
                p += ch;
            }
        }
        return postData;
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
    Application(){
        HTTP req;
        string route;
        route = req.getPathInfo();
        string method = req.getMethod();
        if (method == "POST"){
            DataBase db("db");
            vector<string> postData = req.getPostData();
            if (route == "/add"){    
                db.add(postData);
            }else if(route == "/del"){
                db.del(postData[0]);
            }
        }
        render("index.html");
        for (auto el : req.getPostData()){
            cout << el << endl;
        }
    } 
private:
    void render(const string& filename){
        DataBase db("db");
        DbData = db.readAll();
        ifstream input(filename);
        string line;
        bool first = true;
        cout << "Content-type:text/html\r\n\r\n";
        if (input){
            while(getline(input, line)){
                cout << line << endl;
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
        }

    }
    vector<vector<string>> DbData;
};


int main(){
    Application myApp;
    return -1;
}