#include "MailboxDatabase.h"

// Structure for filling with mail information from database (operation SELECT*)
struct row {
    int id;
    string sender;
    string receiver;
    string subject;
    string content;
	int ordinal_no; 
};

static int callback(void* data, int argc, char** argv, char** colName) {

    struct row* el = (row*) data;

    for(int i = 0; i < argc; i++) {
        if(strcmp(colName[i],"id") == 0)
            el->id = atoi(argv[i]);
        else if(strcmp(colName[i], "sender") == 0)
            el->sender = argv[i];
        else if(strcmp(colName[i],"receiver") == 0)
            el->receiver = argv[i];
		else if(strcmp(colName[i], "subject") == 0)
            el->subject = argv[i];
        else if(strcmp(colName[i], "content") == 0)
            el->content = argv[i];
        else if(strcmp(colName[i], "ordinal_no") == 0)
            el->ordinal_no = atoi(argv[i]);
    }

    return 0;
}

bool MailDatabase::open_database(const char* data_base) {
    int exit = sqlite3_open(data_base, &db);
    if (exit != SQLITE_OK) {
        fprintf(stderr, "Can't open database %s: %s\n", data_base, sqlite3_errmsg(db));

        return false;
    }

    return true;
}

int MailDatabase::process_stat_request(const char* data_base, const char* receiver, int* message_count) {
	if(!open_database(data_base))
        return false;

	std::string sql = "SELECT LENGTH(content) AS content_length FROM MAILBOX WHERE receiver='" + char_to_string(receiver) + "';";

	sqlite3_stmt* stmt;
    int exit = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

	if (exit != SQLITE_OK) {
        std::cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		sqlite3_close(db);
        return -1;
    }

	
	int total_length = 0;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		total_length += sqlite3_column_int(stmt, 0);
		(*message_count)++;
	}

	sqlite3_finalize(stmt);
    sqlite3_close(db);

	return total_length;

}