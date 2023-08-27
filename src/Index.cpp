#include "FS.h"
#include <LittleFS.h>
#include "Index.h"

Index::Index()
{
    File file = LittleFS.open("/header.html");
    while (file.available())
    {
        mainpage_header += file.readString();
    }
    file.close();

    file = LittleFS.open("/footer.html");
    while (file.available())
    {
        mainpage_footer += file.readString();
    }
    file.close();

    board = new Board();
}

void Index::ReturnPage(int64_t parent)
{
    std::string page = "";
    std::string newtopic = "A Topic";
    int64_t i;

    printf("Returning page for parent %" PRId64 "\n", parent);

    std::vector<int64_t> thread = board->GetThread(parent);


    if (parent > 0)
    {
        page += "<a href=\"/\">Back to main</a><br>\n";
        thread.insert(thread.begin(), parent);
    }

    printf("Found %d posts.\n", thread.size());

    for (i = 0; i < thread.size(); i++)
    {
        MessageStruct message = board->GetMessage(thread[i]);

        board->Clean(message.user);
        board->Clean(message.topic);
        board->Clean(message.body);

        page += message.user + " - " + message.topic;
        if (parent == 0)
        {
            page += " - <a href=\"./?thread=" + std::to_string(message.id) + "\">Goto Thread</a>";
        }

        page += "<br>\n<table border=\"1\"><tr><td>" + message.body + "</td></tr><br></table><br>\n";
    }

    if (parent > 0)
    {
        // We are in a thread? Grab the last topic.
        MessageStruct message = board->GetMessage(thread[i - 1]);
        board->Clean(message.topic);
        newtopic = message.topic;
    }

    std::string form = "<form action=\"/new\" method=\"post\">\n"
                       "<input type=\"hidden\" name=\"parent\" value=\"" +
                       std::to_string(parent) +
                       "\">\n"
                       "<table>\n"
                       "<tr><td>User (16 max):</td><td><input type=\"text\" name=\"user\" value=\"Bernd\" maxlength=\"16\"></td></tr>\n"
                       "<tr><td>Topic (32 max):</td><td><input type=\"text\" name=\"topic\" value=\"" +
                       newtopic +
                       "\" maxlength=\"16\"></td></tr>\n"
                       "<tr><td>Body (4096 max):</td><td><textarea name=\"body\" rows=\"5\" cols=\"40\" maxlength=\"4096\"></textarea></td></tr>\n"
                       "<tr><td colspan=\"2\"><input type=\"submit\" value=\"Post\"></td></tr>\n"
                       "</table>\n"
                       "</form>\n"
                       "<hr>\n";

    server.send(200, "text/html", mainpage_header + form.c_str() + page.c_str() + mainpage_footer);
}

void Index::Post(uint64_t parent, std::string user, std::string topic, std::string body)
{
    MessageStruct msg;

    printf("Trying to post a message.\n");

    if (parent == 0)
    {
        printf("Create new thread.\n");
        msg.parent = 0;
    }
    else if (board->HasParent(parent))
    {
        printf("Using parent: %d", parent);
        msg.parent = parent;
    }
    else
    {
        server.send(400, "text/html", "Parent not found!");
        return;
    }

    if (user.empty())
    {
        server.send(400, "text/html", "User is empty!");
        return;
    }
    if (user.length() > 16)
    {
        server.send(400, "text/html", "User too long!");
        return;
    }
    msg.user = user;

    if (topic.empty())
    {
        server.send(400, "text/html", "Topic is empty!");
        return;
    }
    if (topic.length() > 32)
    {
        server.send(400, "text/html", "Topic too long!");
        return;
    }
    msg.topic = topic;

    if (body.empty())
    {
        server.send(400, "text/html", "Body is empty!");
        return;
    }
    if (body.length() > 4096)
    {
        server.send(400, "text/html", "Body too long!");
        return;
    }
    msg.body = body;

    int newid = board->PutMessage(msg);

    if (parent == 0)
    {
        printf("Open new thread.\n");
        ReturnPage(newid);
        return;
    }

    ReturnPage(msg.parent);
}

