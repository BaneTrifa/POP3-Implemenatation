#include "MailboxDatabase.h"

// Structure for filling with mail information from database (operation SELECT*)
struct row {
	int id;
	string sender;
	string receiver;
	string subject;
	string content;
	int ordinal_no;
	int deleted;
};

static int callback(void* data, int argc, char** argv, char** colName) {

	struct row* el = (row*)data;

	for (int i = 0; i < argc; i++) {
		if (strcmp(colName[i], "id") == 0)
			el->id = atoi(argv[i]);
		else if (strcmp(colName[i], "sender") == 0)
			el->sender = argv[i];
		else if (strcmp(colName[i], "receiver") == 0)
			el->receiver = argv[i];
		else if (strcmp(colName[i], "subject") == 0)
			el->subject = argv[i];
		else if (strcmp(colName[i], "content") == 0)
			el->content = argv[i];
		else if (strcmp(colName[i], "ordinal_no") == 0)
			el->ordinal_no = atoi(argv[i]);
		else if (strcmp(colName[i], "deleted") == 0)
			el->deleted = atoi(argv[i]);
	}

	return 0;
}

bool MailDatabase::OpenDatabase(const char* database) {
	int exit = sqlite3_open(database, &db);
	if (exit != SQLITE_OK) {
		fprintf(stderr, "Can't open database %s: %s\n", database, sqlite3_errmsg(db));

		return false;
	}

	return true;
}

int MailDatabase::ProcessStatRequest(const char* database, const char* receiver, int* message_count) {
	if (!OpenDatabase(database))
		return false;

	std::string sql = "SELECT LENGTH(content) AS content_length FROM MAILBOX WHERE receiver='" + CharToString(receiver) + "' AND deleted=0;";

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

bool MailDatabase::FindMail(const char* database, const char* gmail, int ordinal_no) {
	bool rv = false;

	if (!OpenDatabase(database))
		return false;

	std::string sql = "SELECT * FROM MAILBOX WHERE receiver='" + CharToString(gmail) + "' AND ordinal_no=" + to_string(ordinal_no) + " AND deleted=0;";

	struct row element = { -1, "", "", "", "", -1 };
	int exit = sqlite3_exec(db, sql.c_str(), callback, &element, NULL);

	if (exit == SQLITE_OK && element.receiver != "") {
		rv = true;

		__id = element.id;
		__receiver = element.receiver;
		__sender = element.sender;
		__subject = element.subject;
		__content = element.content;
		__ordinal_no = element.ordinal_no;
		__deleted = element.deleted;

	}

	sqlite3_close(db);
	return rv;
}

bool MailDatabase::MarkMessageAsDeleted(const char* database,  const char* gmail, int ordinal_no) {

	if (FindMail(database, gmail, ordinal_no) == false) return false;

	bool rv = false;

	if (!OpenDatabase(database))
		return false;

	std::string sql = "UPDATE MAILBOX SET deleted=1 WHERE receiver='" + CharToString(gmail) + "' AND ordinal_no=" + to_string(ordinal_no) + " ;";

	int exit = sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);

	if (exit == SQLITE_OK) {
		rv = true;
	}

	return rv;
}

bool MailDatabase::UnmarkDeletedMessages(const char* database, const char* gmail) {
	bool rv = false;

	if (!OpenDatabase(database))
		return false;

	std::string sql = "UPDATE MAILBOX SET deleted=0 WHERE receiver='" + CharToString(gmail) + "' ;";

	int exit = sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);

	if (exit == SQLITE_OK) {
		rv = true;
	}

	return rv;
}

void MailDatabase::DeleteMarkedMessages(const char* database, const char* gmail) {

	if (!OpenDatabase(database))
		return;

	// delete mails marked as deleted
	std::string sql = "DELETE FROM MAILBOX WHERE deleted=1 AND receiver='" + CharToString(gmail) + "' ;";

	sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);

	// Reassagin ordinal_no values to rest non deleted mails
	sql = "UPDATE MAILBOX SET ordinal_no = (SELECT COUNT(*) FROM MAILBOX t WHERE t.ordinal_no < MAILBOX.ordinal_no AND t.receiver='" + CharToString(gmail) + "') + 1";
	sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);

}

string MailDatabase::ListAllMessages(const char* database, const char* gmail) {

	string message = "";

	if (!OpenDatabase(database))
		return "-ERR";

	string sql = "SELECT * FROM MAILBOX WHERE receiver = '" + CharToString(gmail) + "' AND deleted = 0;";

	sqlite3_stmt* stmt;
	int exit = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (exit != SQLITE_OK) {
		std::cerr << "Error preparing SQL query: " << sqlite3_errmsg(db) << std::endl;
		return "-ERR";
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) 
	{
		int ordinal_no = sqlite3_column_int(stmt, 5); // Extract fifth column (ordinal_no)
		const char* content = (char*) sqlite3_column_text(stmt, 4);

		message += "+OK ";
		message += to_string(ordinal_no);
		message += " ";
		message += to_string(strlen(content));
		message += "\n";

	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return message;
}

string MailDatabase::get_sender() {
	return __sender;
}
string MailDatabase::GetReceiver() {
	return __receiver;
}
string MailDatabase::GetSubject() {
	return __subject;
}
string MailDatabase::GetContent() {
	return __content;
}