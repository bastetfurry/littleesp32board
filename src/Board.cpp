#include "Board.h"

// From https://stackoverflow.com/a/24315631 by Gauthier Boaglio
static inline void ReplaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

Board::Board()
{
    int retval;
    sqlite3_initialize();
    retval = sqlite3_open("/littlefs/board.db", &db);
    if (retval)
    {
        printf("Could not open database! SQLite Error: %s\n", sqlite3_errmsg(db));
        while (true);
    }
}

MessageStruct Board::GetMessage(int64_t id)
{
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "select id,parent,user,topic,body from msg where id=?", -1, &stmt, NULL))
    {
        printf("Could not prepare SQL! SQLite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        while (true);
    }

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        return MessageStruct
                {
                        id: -1,
                        parent: -1,
                        user: "",
                        topic: "",
                        body: ""
                };
    }

    return MessageStruct
            {
                    id: sqlite3_column_int64(stmt, 0),
                    parent: sqlite3_column_int64(stmt, 1),
                    user: reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)),
                    topic: reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                    body: reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))
            };
}

std::vector<int64_t> Board::GetThread(int64_t parentId)
{
    std::vector<int64_t> retvector;

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "select id from msg where parent=?", -1, &stmt, NULL))
    {
        printf("Could not prepare SQL! SQLite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        while (true);
    }

    sqlite3_bind_int64(stmt, 1, parentId);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        retvector.push_back(sqlite3_column_int64(stmt, 0));
    }

    return retvector;
}

bool Board::HasParent(int64_t parentId)
{
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "select count(*) from msg where parent=? or id=?", -1, &stmt, NULL))
    {
        printf("Could not prepare SQL! SQLite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        while (true);
    }

    sqlite3_bind_int64(stmt, 1, parentId);
    sqlite3_bind_int64(stmt, 2, parentId);
    sqlite3_step(stmt);
    return sqlite3_column_int64(stmt, 0) > 0;
}

int64_t Board::GetNextFreeParent()
{
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "select parent from msg order by parent limit 1", -1, &stmt, NULL))
    {
        printf("Could not prepare SQL! SQLite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        while (true);
    }

    int step = sqlite3_step(stmt);

    if (step != SQLITE_ROW)
    { // No threads yet
        return 1;
    }
    return sqlite3_column_int64(stmt, 0) + 1;
}

int64_t Board::PutMessage(MessageStruct message)
{
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "insert into msg (parent, user, topic, body) values(?,?,?,?)", -1, &stmt, NULL))
    {
        printf("Could not prepare SQL! SQLite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        while (true);
    }

    sqlite3_bind_int64(stmt, 1, message.parent);
    sqlite3_bind_text(stmt, 2, message.user.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, message.topic.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, message.body.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        printf("Could not write to DB! SQLite Error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        while (true);
    }

    printf("Message posted with user: %s\n", message.user.c_str());
    printf("Topic: %s\n", message.topic.c_str());
    printf("Body: %s\n", message.body.c_str());
    printf("Parent: %" PRId64 "\n", message.parent);

    return sqlite3_last_insert_rowid(db);
}

void Board::Clean(std::string &input)
{
    ReplaceAll(input, "<", "&lt;");
    ReplaceAll(input, ">", "&gt;");
    ReplaceAll(input, "\"", "&quot;");
    ReplaceAll(input, "'", "&#039;");
    ReplaceAll(input, "\n", "<br>");
}