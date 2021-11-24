#include "player.h"


void Player::play(const string& filename) {
    _demuxer.reset(new Demuxer(filename));

    for (int i = 0; i < 100; ++i) {
        AVPacket pkt;
        _demuxer->get_avpacket(pkt);
        ilog << pkt.stream_index;
    }
}