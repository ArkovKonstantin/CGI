#include <string>
#include <cpp_redis/cpp_redis>
#include "../../utils.h"
using namespace std;

class Session{
public:
    string set(const string& value);
    string get(const string& key);
private:
    cpp_redis::client client;
};