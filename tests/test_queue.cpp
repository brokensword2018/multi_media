#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <thread>

#include "util/common.h"
#include "util/queue.h"


using namespace std;


Queue<int> q;

void sleep_random() {
    usleep(rand() % 100000);
}

void write_to_queue() {
    for (int i = 0; i < 600; ++i) {
        q.push(i);
        sleep_random();
        ilog << "push " << i;
    }
}

void read_from_queue() {
    while (true) {
        sleep_random();
        int msg;
        if (q.pop(msg)) {
            ilog << "pop " << msg << ", qsize " << q.size();
        }
    }
}





int main() {

    thread t1(write_to_queue);
    thread t2(read_from_queue);
    
    t1.join();
    t2.join();
    cerr << "all print thread quit"; 
    AsyncLogWriter::instance().join();
    cout << "main return" << endl;
    return 0;
}