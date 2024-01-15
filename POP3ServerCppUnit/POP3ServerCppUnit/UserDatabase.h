#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <iostream>
#include "sqlite3.h"
#include "conversion.h"
#include "const.h"

using namespace std;

class UserDatabase {
private: 
    int __id;		// ID field in database
    string __gmail; // Gmail field in database
    string __password; // Password field in database
	int __logged_in; //  Logged_in field in database
    int __banned; // Banned field in database

    sqlite3* db; // sqlite3 object for communicating between database and c++

    /**
     * Function for opening database.
     * @param data_base Database name.
     * @return True if database is successfully open else false.
    */
    bool open_database(const char* data_base);

public:

    // getters for fields in database
    int get_id();
    string get_gmail();
    string get_password();
	int get_logged_in();
    int get_banned();

	// setter for looged_in field
	void set_logged_in(bool state);

    /**
     * Find user by gmail
     * @param login_data User's gmail.
     * @param data_base Database name
     * @return If the user exist return true else return false
    */
    bool find_user(char* login_data, const char* data_base);
};



#endif