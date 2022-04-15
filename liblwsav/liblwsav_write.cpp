#include "liblwsav.h"
#include <stdio.h>
#include <string.h>

void LWSAV::Save::write(std::string path) {
    close();
#ifdef _WIN32 // Добавление кросс платформенности (Чтоб компилятор от MSVC не ругался на fopen)
    errno_t err = fopen_s(&file, path.c_str(), "wb");
    if (err != 0) {
        return;
    }
#else
    file = fopen(path.c_str(), "wb");
    if (file == NULL) {
        return;
    }
#endif
#ifdef LOGGING
    printf("[INFO] Created file\n");
#endif
    // Запись заголовка
    fwrite(LOGICWORLD_HEADER, sizeof(header), 1, file);
#ifdef LOGGING
    printf("[INFO] Header written\n");
#endif
    // Запись информации
    fwrite(&info.version, sizeof(l_uint8), 1, file); // Версия файла
    fwrite(&info.game_verison, sizeof(l_uint32), 4, file); // Версия игры
    fwrite(&info.save_type, sizeof(l_uint8), 1, file); // Тип сохранения
    info.numbers_components = data.components.size();
    info.numbers_wires = data.wires.size();
    fwrite(&info.numbers_components, sizeof(l_uint32), 1, file); // Количество компонентов
    fwrite(&info.numbers_wires, sizeof(l_uint32), 1, file); // Количество компонентов
    write_map_components(); // Запись словарь компонентов
#ifdef LOGGING
    printf("[INFO] SaveInfo written\n");
#endif
    for (l_uint32 i = 0; i < info.numbers_components; i++) {
        write_component(i); // Запись компонента
    }
#ifdef LOGGING
    printf("[INFO] GameInfo->Components written\n");
#endif
    for (l_uint32 i = 0; i < info.numbers_wires; i++) {
        write_wire(i); // Запись компонента
    }
#ifdef LOGGING
    printf("[INFO] GameInfo->Wires written\n");
#endif
    write_states();
#ifdef LOGGING
    printf("[INFO] GameInfo->Circuit States written\n");
#endif
    fwrite(LOGICWORLD_FOOTER, sizeof(footer), 1, file);
#ifdef LOGGING
    printf("[INFO] Footer written\n");
#endif
    fflush(file);
    fclose(file);
#ifdef LOGGING
    printf("[INFO] Saved\n");
#endif
}

void LWSAV::Save::write_map_components() {
    // Запись словарь компонентов
    fwrite("\x0\x0\x0\x0", sizeof(char), 4, file); // Пропускаем 4 байта
    l_uint32 length = info.map_components.size();
    fwrite(&length, sizeof(l_uint32), 1, file); // Количество компонентов в словаре
    for (auto i : info.map_components)
    {
        fwrite(&i.first, sizeof(l_uint16), 1, file); // Запись числового ID
        l_uint32 length = i.second.size(); // Длина текстового ID
        fwrite(&length, sizeof(l_uint32), 1, file);
        fwrite(i.second.c_str(), sizeof(l_uint8), length, file); // Запись текстового ID
    }
}

void LWSAV::Save::write_component(l_uint32 index) {
    fwrite(&data.components[index].address, sizeof(c_address), 1, file); // Адрес компонента
    fwrite(&data.components[index].parent, sizeof(c_address), 1, file); // Адрес родителя
    fwrite(&data.components[index].type, sizeof(l_uint16), 1, file); // Тип компонента
    fwrite(&data.components[index].pos, sizeof(position), 1, file); // Позиция компонента
    fwrite(&data.components[index].rot, sizeof(rotation), 1, file); // Поворот компонента

    l_uint32 numbers_inputs = data.components[index].inputs.size(); // Количество входов
    fwrite(&numbers_inputs, sizeof(l_uint32), 1, file); // Запись количества входов
    for (auto i : data.components[index].inputs) {
        fwrite(&i, sizeof(ComponentInput), 1, file); // Запись входа
    }

    l_uint32 numbers_outputs = data.components[index].outputs.size(); // Количество выходов
    fwrite(&numbers_outputs, sizeof(l_uint32), 1, file); // Запись количества выходов
    for (auto i : data.components[index].outputs) {
        fwrite(&i, sizeof(ComponentOutput), 1, file); // Запись выхода
    }

    l_uint32 length = data.components[index].data.size(); // Длина допольнительных данных
    fwrite(&length, sizeof(l_uint32), 1, file);
    fwrite(data.components[index].data.data(), sizeof(l_uint8), length, file); // Запись дополнительных данных
}

void LWSAV::Save::write_wire(l_uint32 index) {
    // Запись провода
    Wire wire = data.wires[index];
    // Записываем начало
    fwrite(&wire.begin.peg_type, sizeof(l_uint8), 1, file);
    fwrite(&wire.begin.address, sizeof(c_address), 1, file);
    fwrite(&wire.begin.index_peg, sizeof(l_uint32), 1, file);
    // Записываем конец
    fwrite(&wire.end.peg_type, sizeof(l_uint8), 1, file);
    fwrite(&wire.end.address, sizeof(c_address), 1, file);
    fwrite(&wire.end.index_peg, sizeof(l_uint32), 1, file);
    // Записываем circuit state id и поворот
    fwrite(&wire.circuit_state_id, sizeof(l_uint32), 1, file);
    fwrite(&wire.rotation, sizeof(float), 1, file);
}

void LWSAV::Save::write_states() {
    l_uint32 length = states.vector()->size();
    fwrite(&length, sizeof(l_uint32), 1, file);
    fwrite(states.vector()->data(), sizeof(l_uint8), length, file);
}