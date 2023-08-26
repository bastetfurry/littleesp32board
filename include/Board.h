#ifndef TINYESPBOARD_BOARD_H
#define TINYESPBOARD_BOARD_H

#include <string>
#include <vector>


#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "LittleFS.h"

#include "BBS_Server.h"

struct MessageStruct
{
    int64_t id;
    int64_t parent;
    std::string user;
    std::string topic;
    std::string body;
};

class Board
{
private:
    sqlite3 *db;
public:
    Board();

    MessageStruct GetMessage(int64_t id = 0);

    std::vector<int64_t> GetThread(int64_t parentId = 0);

    bool HasParent(int64_t parentId);

    int64_t GetNextFreeParent();

    int64_t PutMessage(MessageStruct message);

    void Clean(std::string &input);
};

#endif //TINYESPBOARD_BOARD_H
