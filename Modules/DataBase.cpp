#include "Headers/DataBase.h"

DataBase::DataBase(const string& fp){
    filePath = fp;
}
void DataBase::add(vector<string> data){
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
void DataBase::del(string id){
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
vector<vector<string>> DataBase::readAll(){
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