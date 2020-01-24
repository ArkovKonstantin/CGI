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
            key = strip(key, ' ');
            val = strip(val, ' ');
            splitedData[key] = val;
        }
        return splitedData;
}

string generateUUID(){
    string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::srand(time(nullptr));
    std::string uuid = std::string(36, ' ');
    int rnd = 0;
    int r = 0;
    uuid[8] = '-';
    uuid[13] = '-';
    uuid[18] = '-';
    uuid[23] = '-';
    uuid[14] = '4';
    for (int i = 0; i < 36; i++) {
        if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
            if (rnd <= 0x02) {
                rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
            }
            rnd >>= 4;
            uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
        }
    }
    return uuid;
}