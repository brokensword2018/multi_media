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
    #include "SDL2/SDL.h"
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

void test_func() {

}



int main() {

    SDL_CreateMutex();
    AVFormatContext* format_context = NULL;
    int ret = avformat_open_input(&format_context, "666", NULL, NULL);
    ilog << ret;
    test t;
    t.print();
   
    AsyncLogWriter::instance().join();
    return 0;
}