#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <experimental/filesystem>
#include "utils.h"
#include "Modules/Headers/DataBase.h"
#include "Modules/Headers/Http.h"
#include "Modules/Headers/Session.h"



using namespace std;
namespace fs = std::experimental::filesystem;

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
            }else if(route == "/files"){
                render("files.html"); 
            }else if(route == "/delFile"){
                string fn = strip(postData[0], '\"');
                fs::remove(fn);
                render("files.html");
            }
        }else if(method == "GET"){
            if (route == "/files"){
                render("files.html");
            }else if (route == "/login"){
                Session session;
                string ipAddress = req.getHeader("REMOTE_ADDR");
                string uuid = session.set(ipAddress);
                map<string, string> cookie = {{"sessionId", uuid}};
                render("login.html", cookie);
            }else if (route == ""){
                Session session;
                string sessionId = req.getCookie("sessionId");
                string ipAddress = session.get(sessionId);
                if (req.getHeader("REMOTE_ADDR") == ipAddress){
                    render("users.html");
                }else{
                    cout << "Content-type:text/html\r\n\r\n";
                    cout << "<center><p>login required</p></center>" << "<br>";
                }
            }
        }
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
                            cout << "<td style='display:none;'><input type='text' name='filename' value='"<< row[0] << "'></td>" << endl;
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
    void render(const string& filename, map<string, string> cookie = {}){
        ifstream input(filename);
        string line;
        bool first = true;
        cout << "Content-type:text/html" << endl;
        for (const auto& el:cookie){
            cout << "Set-Cookie:" << el.first << '=' << el.second << endl;
        }
        cout << "\r\n\r\n";
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