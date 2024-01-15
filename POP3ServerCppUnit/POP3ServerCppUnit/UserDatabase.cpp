#include "UserDatabase.h"

// Structure for filling with user information from database (operation SELECT*)
struct row {
    int id;
    string gmail;
    string password;
	int logged_in;
    int banned;
};

static int callback(void* data, int argc, char** argv, char** colName) {

    struct row* el = (row*) data;

    for(int i = 0; i < argc; i++) {
        if(strcmp(colName[i],"id") == 0)
            el->id = atoi(argv[i]);
        else if(strcmp(colName[i], "gmail") == 0)
            el->gmail = argv[i];
        else if(strcmp(colName[i],"password") == 0)
            el->password = argv[i];
		else if(strcmp(colName[i], "logged_in") == 0)
            el->logged_in = atoi(argv[i]);
        else if(strcmp(colName[i], "banned") == 0)
            el->banned = atoi(argv[i]);
        
    }

    return 0;
}


bool UserDatabase::open_database(const char* data_base) {
    int exit = sqlite3_open(data_base, &db);
    if (exit != SQLITE_OK) {
        fprintf(stderr, "Can't open database %s: %s\n", data_base, sqlite3_errmsg(db));

        return false;
    }

    return true;
}


bool UserDatabase::find_user(char* login_data, const char* data_base) {
    bool rv = false;

    if(!open_database(data_base))
        return false;

    // select
    std::string sql = "SELECT * FROM USERS WHERE gmail='" + char_to_string(login_data) + "' ;";
    

    // Fill 'element' with user information
    struct row element;
    int exit = sqlite3_exec(db, sql.c_str(), callback, &element, NULL);


    // Assign information to class fields
    if (exit == SQLITE_OK && element.gmail != "") {
        rv = true;

        __id = element.id;
        __gmail = element.gmail;
        __password = element.password;
		__logged_in = element.logged_in;
        __banned = element.banned;

    } else if (exit == SQLITE_OK && element.gmail == ""){ 
        cout << "Can't find user " << login_data <<  " in database " << data_base << endl;
    } else {
        cout << "Can't find user " << login_data <<  " in database " << data_base << endl;
    }

    sqlite3_close(db);
    return rv;
}


int UserDatabase::get_id() {
    return __id;
}
string UserDatabase::get_gmail(){
    return __gmail;
}
string UserDatabase::get_password(){
    return __password;
}
int UserDatabase::get_logged_in(){
    return __logged_in;
}
int UserDatabase::get_banned(){
    return __banned;
}

void UserDatabase::set_logged_in(bool state) {

	const char* db_name = DB_NAME;
	string value = state ? "1" : "0";
	std::string sql = "UPDATE USERS SET logged_in = '" + value + "' WHERE gmail = '" + __gmail + "' ;";

	if(!open_database(db_name))
        return;

	int exit = sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL);

	sqlite3_close(db);

	__logged_in = state;
}