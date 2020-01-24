#include "utils.h"


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

map<string,string> split(string s, char d1, char d2){
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