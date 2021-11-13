#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>

#include "util/singleton.h"
#include "util/logger.h"


using namespace std;



void print () {
    for (int i = 0; i < 100000; ++i) {
        ostringstream oss;
        ilog << "i=" << i << " " << gettid() << " say hello wrold";
    }
}





int main() {

    thread t1(print);
    thread t2(print);
    thread t3(print);
    thread t4(print);
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    cerr << "all print thread quit"; 
    AsyncLogWriter::instance().join();
    cout << "main return" << endl;
    return 0;
}