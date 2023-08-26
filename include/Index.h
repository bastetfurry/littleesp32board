#ifndef TINYESPBOARD_INDEX_H
#define TINYESPBOARD_INDEX_H

#include <string>
#include "Board.h"
#include "BBS_Server.h"

class Index
{
private:
    String mainpage_header;
    String mainpage_footer;

    Board *board;
public:
    Index();

    void ReturnPage(int64_t parent = 0);

    void Post(uint64_t parent, std::string user, std::string topic, std::string body);
};


#endif //TINYESPBOARD_INDEX_H
