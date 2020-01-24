#include "Headers/Session.h"

string Session::set(const string& value){
    string uuid = generateUUID();
    client.connect("127.0.0.1", 6379);
    client.set(uuid, value);
    client.sync_commit();

    return uuid;
}
string Session::get(const string& key){
    string val;
    client.connect("127.0.0.1", 6379);
    client.get(key, [&val](cpp_redis::reply& reply) {
        if(reply.is_string()){
            val = reply.as_string();
        }else{
            val = "";
        }
    });
    client.sync_commit();
    return val;
}