#pragma once
#include <stdint.h>

namespace LcdTetris{
    void setup();
    bool update(uint16_t stress_level);
    void thanks_winner();
    void thanks_looser();
    int get_score();
}