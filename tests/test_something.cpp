#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>

#include "util/singleton.h"
#include "util/logger.h"
#include "util/util.h"

extern "C" {
    #include "libavformat/avformat.h"
}


using namespace std;




class test {
public:
    void print() {
        dlog << "debug log";
        int i = 1;
        double d = 3.445677;
        ilog << "i=" << i;
        wlog << "d=" << d;
    }
};




int main() {

    ilog << (StrPrinter << 5 << "  " << " + " << 7);
    string str = (StrPrinter << 5 << "  " << " + " << 7);
    ilog << str;
   
    AsyncLogWriter::instance().join();
    return 0;
}