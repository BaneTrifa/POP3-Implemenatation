#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <iostream>
#include <string>
#include "sqlite3.h"
#include "conversion.h"
#include "const.h"

using namespace std;

class UserDatabase {
private: 
    int __id;		// ID field in database
    string __gmail; // Gmail field in database
    string __password; // Password field in database
    int __banned; // Banned field in database

    sqlite3* db; // sqlite3 object for communicating between database and c++

    /**
     * Function for opening database.
     * @param database Database name.
     * @return True if database is successfully open else false.
    */
    bool OpenDatabase(const char* database);

public:

    UserDatabase();

    // getters for fields in database
    string GetGmail();
    string GetPassword();
    int GetBanned();

    /**
     * Find user by gmail
     * @param login_data User's gmail.
     * @param database Database name
     * @return If the user exist return true else return false
    */
    void FindUser(string login_data, const char* database);
};



#endif