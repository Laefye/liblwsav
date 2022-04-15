#pragma once
// Errors
#define ERROR_NOT_FOUND -1
#define ERROR_INVALID_FORMAT -2
#define ERROR_INVALID_STATE_ID -3
// Constants
#define LOGICWORLD_HEADER "Logic World save"
#define LOGICWORLD_FOOTER "redstone sux lol"
#define LOGGING
// Includes
#include <string>
#include <stdio.h>
#include <map>
#include <vector>

namespace LWSAV {
    typedef unsigned char l_uint8;
    typedef unsigned int l_uint32;
    typedef unsigned short l_uint16;
    typedef l_uint32 c_address;
    typedef float position[3];
    typedef float rotation[4];

    typedef l_uint8 header[16];
    typedef l_uint8 footer[16];
    
    struct SaveInfo {
        char version; // Save Format Version
        l_uint32 game_verison[4]; // Game Version. [0] - Major, [1] - Minor, [2] - Patch, [3] - Build
        char save_type; // Save Type
        l_uint32 numbers_components; // Number of Components
        l_uint32 numbers_wires; // Number of Components

        std::map<l_uint16, std::string> map_components;
    };

    struct ComponentInput {
        l_uint32 circuit_state_id;
    };

    struct ComponentOutput {
        l_uint32 circuit_state_id;
    };

    struct Component {
        c_address address;
        c_address parent;
        l_uint16 type;
        position pos;
        rotation rot;
        std::vector<ComponentInput> inputs;
        std::vector<ComponentOutput> outputs;
        std::vector<l_uint8> data;
    };

    struct Peg {
        l_uint8 peg_type; // false - input, true - output
        c_address address;
        l_uint32 index_peg;
    };
    

    struct Wire {
        Peg begin;
        Peg end;
        l_uint16 circuit_state_id;
        float rotation;
    };

    struct SaveData {
        std::vector<Component> components;
        std::vector<Wire> wires;
    };

    class CircuitStates {
        std::vector<l_uint8> states;

    public:
        CircuitStates();
        void load(std::vector<l_uint8> states);
        l_uint8 get(l_uint16 id);
    };

    class Save {
        FILE* file;
        std::string p;
        int read();
        int read_map_components();
        int read_component();
        int read_wire();
        int read_states();

        // File
        header h;
        SaveInfo info;
        SaveData data;
        CircuitStates states;
    public:
        // path - Path to .logicworld file
        Save(std::string path);
        ~Save();
        // Open and read file
        int open();
        // Get SaveInfo
        SaveInfo* get_info();
        // Get SaveData
        SaveData* get_data();
        // Get CircuitStates
        CircuitStates* get_states();
    };
}