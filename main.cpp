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
        if (route == "/add"){
            DataBase db("db");
            vector<string> postData = req.getPostData();
            db.add(postData);
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
                        cout << "<tr>" << endl;
                            for (const auto& col : row){
                                cout << "<td>" << col << "</td>" << endl;
                            }
                        }
                        cout << "</tr>" << endl;
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