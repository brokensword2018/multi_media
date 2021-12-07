#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>

#include "util/singleton.h"
#include "util/logger.h"
#include "util/util.h"
#include "util/notice_center.h"


using namespace std;




const string event1 = "event1";
const string event2 = "event2";


int main() {

    NoticeCenter::instance().add_listener(
        event1, [](int &a, const char *&b, double &c, string &d) { ilog << a << " " << b << " " << c << " " << d; 
        a++;
        NoticeCenter::instance().delete_listener(event1);
        NoticeCenter::instance().add_listener(event1, [](int &a, const char *&b, double &c, string &d) { dlog << a << " " << b << " " << c << " " << d;});
    });
    
    int a = 1;
    const char* b = "b";
    double c = 3.14;
    string d = "d";
    while (true)
    {
        NoticeCenter::instance().emit_event(event1, a, b, c, d);
        sleep(1);
    }
    
    AsyncLogWriter::instance().join();
    return 0;
}