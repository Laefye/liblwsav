#include "liblwsav.h"
#include <stdio.h>
#include <string.h>

int LWSAV::Save::read() {
#ifdef LOGGING
    printf("[INFO] Reading...\n");
#endif
    // Читаем и сравниваем заголовок
    fread(h, sizeof(header), 1, file);
    if (memcmp(h, LOGICWORLD_HEADER, sizeof(header)) != 0) {
        return ERROR_INVALID_FORMAT;
    }
    // Читаем SaveInfo
    fread(&info.version, sizeof(l_uint8), 1, file); // Читаем версию формата сохранение
    fread(&info.game_verison, sizeof(l_uint32), 4, file); // Читаем версию игры
    fread(&info.save_type, sizeof(l_uint8), 1, file); // Читаем тип сохранения
    fread(&info.numbers_components, sizeof(l_uint32), 1, file); // Читаем количество компонентов
    fread(&info.numbers_wires, sizeof(l_uint32), 1, file); // Читаем количество проводов
#ifdef LOGGING
    printf("[INFO] Save version: %d\n", info.version);
    printf("[INFO] Game version: %d.%d.%d.%d\n", info.game_verison[0], info.game_verison[1], info.game_verison[2], info.game_verison[3]);
    printf("[INFO] Save type: %d\n", info.save_type);
    printf("[INFO] Number of components: %d\n", info.numbers_components);
    printf("[INFO] Number of wires: %d\n", info.numbers_wires);
#endif
    // Читаем словарь компонентов
    int status = read_map_components();
    if (status != 0) { // На текущий момент бесполезен.
        return status;
    }
    // Читаем компоненты
    for (l_uint32 i = 0; i < info.numbers_components; i++) {
        status = read_component();
        if (status != 0) {
            return status;
        }
    }
#ifdef LOGGING
    printf("[INFO] SavaData->Components read\n");
#endif
    // Читаем провода
    for (l_uint32 i = 0; i < info.numbers_wires; i++) {
        status = read_wire();
        if (status != 0) {
            return status;
        }
    }
#ifdef LOGGING
    printf("[INFO] SavaData->Wires read\n");
#endif
    // Читаем состоянии
    status = read_states();
    if (status != 0) {
        return status;
    }
#ifdef LOGGING
    printf("[INFO] SavaData->CircuitStates read\n");
#endif
    // Проверяем footer
    footer f;
    fread(f, sizeof(footer), 1, file);
    if (memcmp(f, LOGICWORLD_FOOTER, sizeof(footer)) != 0) {
        return ERROR_INVALID_FORMAT;
    }
    return 0;
}

int LWSAV::Save::read_map_components() {
    fseek(file, 4, SEEK_CUR);
    l_uint32 numbers_components_in_map = 0; // Колличество компонентов в словарь
    fread(&numbers_components_in_map, sizeof(l_uint32), 1, file);
    for (l_uint32 i = 0; i < numbers_components_in_map; i++) {
        l_uint16 id; // Числового ID компонента
        fread(&id, sizeof(l_uint16), 1, file);
        l_uint32 length; // Длина текстового id компонента
        fread(&length, sizeof(l_uint32), 1, file);
        char* textid = new char[length + 1]; // Текстовый id компонента
        fread(textid, sizeof(char), length, file);
        textid[length] = '\0';
        info.map_components[id] = textid; // Добавляем в словарь
        delete[] textid; // Удаляем текстовый id компонента из этого цикла
    }
    return 0;
}

int LWSAV::Save::read_component() {
    // Читаем компонент
    Component component;
    fread(&component.address, sizeof(c_address), 1, file); // Адрес компонента
    fread(&component.parent, sizeof(c_address), 1, file); // Адрес родительского компонента
    fread(&component.type, sizeof(l_uint16), 1, file); // Тип компонента
    fread(&component.pos, sizeof(position), 1, file); // Позиция компонента
    fread(&component.rot, sizeof(rotation), 1, file); // Поворот компонента
    // Получаем входы компонента
    l_uint32 numbers_inputs = 0; // Колличество входов компонента
    fread(&numbers_inputs, sizeof(l_uint32), 1, file);
    for (l_uint8 i = 0; i < numbers_inputs; i++) {
        ComponentInput input;
        fread(&input, sizeof(ComponentInput), 1, file);
        component.inputs.push_back(input);
    }
    // Получаем выходы компонента
    l_uint32 numbers_outputs = 0; // Колличество выходов компонента
    fread(&numbers_outputs, sizeof(l_uint32), 1, file);
    for (l_uint8 i = 0; i < numbers_outputs; i++) {
        ComponentOutput output;
        fread(&output, sizeof(ComponentOutput), 1, file);
        component.outputs.push_back(output);
    }
    // Получаем дополнительные данные
    l_uint32 length; // Длина
    fread(&length, sizeof(l_uint32), 1, file);
    if (length != -1) {
        char* d = new char[length + 1]; // Данные
        fread(d, sizeof(char), length, file);
        for (l_uint32 i = 0; i < length; i++) { // Закидываем в data
            component.data.push_back(d[i]);
        }
        delete[] d; // Удаляем данные
    } 
    // Добавляем компонент в список
    data.components.push_back(component);
    return 0;
}

int LWSAV::Save::read_wire() {
    // Читаем провода
    Wire wire;
    // Читаем начало
    fread(&wire.begin.peg_type, sizeof(l_uint8), 1, file); // Тип
    fread(&wire.begin.address, sizeof(c_address), 1, file); // Адрес компонента
    fread(&wire.begin.index_peg, sizeof(l_uint32), 1, file); // Индекс
    // Читаем конец
    fread(&wire.end.peg_type, sizeof(l_uint8), 1, file); // Тип
    fread(&wire.end.address, sizeof(c_address), 1, file); // Адрес компонента
    fread(&wire.end.index_peg, sizeof(l_uint32), 1, file); // Индекс
    // Читаем circuit_state_id и поворот
    fread(&wire.circuit_state_id, sizeof(l_uint32), 1, file); // circuit_state_id
    fread(&wire.rotation, sizeof(float), 1, file); // Поворот
    // Добавляем провод в список
    data.wires.push_back(wire);
    return 0;
}

int LWSAV::Save::read_states() {
    // Читаем состояние
    l_uint32 length; // Длина состояний
    fread(&length, sizeof(l_uint32), 1, file);
    l_uint8* st = new l_uint8[length]; // Состояния
    fread(st, sizeof(l_uint8), length, file);
    std::vector<l_uint8> states_vector; // Список состояний
    for (l_uint32 i = 0; i < length; i++) {
        states_vector.push_back(st[i]);
    }
    delete[] st; // Удаляем состояния
    states.load(states_vector);
    return 0;
}

