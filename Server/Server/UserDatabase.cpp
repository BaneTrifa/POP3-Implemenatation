#include "UserDatabase.h"

// Structure for filling with user information from database (operation SELECT*)
struct row {
    int id;
    string gmail;
    string password;
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
        else if(strcmp(colName[i], "banned") == 0)
            el->banned = atoi(argv[i]);
    }

    return 0;
}


UserDatabase::UserDatabase() {
    __id = -1;
    __gmail = "";
    __password = "";
    __banned = -1;
    db = NULL;
}

bool UserDatabase::OpenDatabase(const char* database) {
    int exit = sqlite3_open(database, &db);
    if (exit != SQLITE_OK) {
        fprintf(stderr, "Can't open database %s: %s\n", database, sqlite3_errmsg(db));

        return false;
    }

    return true;
}


void UserDatabase::FindUser(string login_data, const char* database) {

    if(!OpenDatabase(database))
        return;

    // select
    std::string sql = "SELECT * FROM USERS WHERE gmail='" + login_data + "' ;";
    

    // Fill 'element' with user information
    struct row element;
    int exit = sqlite3_exec(db, sql.c_str(), callback, &element, NULL);


    // Assign information to class fields
    if (exit == SQLITE_OK && element.gmail != "") {

        __id = element.id;
        __gmail = element.gmail;
        __password = element.password;
        __banned = element.banned;

    } 

    exit = sqlite3_close(db);

}


string UserDatabase::GetGmail(){
    return __gmail;
}
string UserDatabase::GetPassword(){
    return __password;
}
int UserDatabase::GetBanned(){
    return __banned;
}