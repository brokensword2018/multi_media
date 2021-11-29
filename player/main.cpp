#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>

#include "util/singleton.h"
#include "util/logger.h"
#include "util/util.h"

#include "player.h"

extern "C" {
    #include "libavformat/avformat.h"
    #include "SDL2/SDL.h"
}


using namespace std;




int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "need one arg, like ./player <filename>" << endl;
        exit(1);
    }

    try {
        Player player(argv[1]);
        player.play();
    }
    catch(const std::exception& e) {
        ilog << e.what() << '\n';
    }
    catch(...) {
        ilog << "unknown exception" << '\n';
    }
    


   
    AsyncLogWriter::instance().join();
    return 0;
}