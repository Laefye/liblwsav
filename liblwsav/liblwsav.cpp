#include "liblwsav.h"
#include <stdio.h>
#include <string.h>


LWSAV::CircuitStates::CircuitStates() {

}

void LWSAV::CircuitStates::load(std::vector<l_uint8> states) {
    this->states = states;
}

int LWSAV::Save::open() {
#ifdef _WIN32 // Добавление кросс платформенности (Чтоб компилятор от MSVC не ругался на fopen)
    errno_t err = fopen_s(&file, p.c_str(), "rb");
    if (err != 0) {
        return ERROR_NOT_FOUND;
    }
#else
    file = fopen(p.c_str(), "rb");
    if (file == NULL) {
        return ERROR_NOT_FOUND;
    }
#endif
#ifdef LOGGING
    printf("[INFO] Opened file\n");
#endif
    return 0;
}

LWSAV::l_uint8 LWSAV::CircuitStates::get(l_uint32 id) {
    if (id / 8 >= this->states.size()) {
        return ERROR_INVALID_STATE_ID;
    }
    l_uint8 state = this->states[id / 8];
    state >>= id % 8;
    return (state & 1);
}

void LWSAV::CircuitStates::set(l_uint32 id, l_uint8 st) {
    if (id / 8 >= this->states.size()) {
        return;
    }
    l_uint8 state = this->states[id / 8];
    if (st == 0) {
        state &= ~(1 << (id % 8));
    } else {
        state |= (1 << (id % 8));
    }
    this->states[id / 8] = state;
}


LWSAV::Save::Save(std::string path) {
    p = path;
}

void LWSAV::Save::close() {
    fclose(file);
}

LWSAV::SaveInfo* LWSAV::Save::get_info() {
    return &info;
}

LWSAV::SaveData* LWSAV::Save::get_data() {
    return &data;
}

LWSAV::CircuitStates* LWSAV::Save::get_states() {
    return &states;
}

std::vector<LWSAV::l_uint8>* LWSAV::CircuitStates::vector() {
    return &states;
}