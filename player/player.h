#pragma once
#include "util/common.h"
#include "demuxer.h"



class Player {
public:
    Player() = default;
    void play(const string& filename);


private:
    unique_ptr<Demuxer> _demuxer;
};