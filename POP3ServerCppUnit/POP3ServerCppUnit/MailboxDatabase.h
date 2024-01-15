#ifndef MAILBOX_DATABASE_H
#define MAILBOX_DATABASE_H

#include <iostream>
#include "sqlite3.h"
#include "conversion.h"

using namespace std;

class MailDatabase {
private: 
    int __id;		   // ID field in database
    string __sender;   // Mail sender
    string __receiver; // Mail receiver
	string __subject;  // Subject of the mail
	string __content;  // Content of the mail
    int __ordinal_no;  // Ordinal number of the mail

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
    string get_sender();
    string get_receiver();
	string get_subject();
    string get_content();
	int get_ordinal_no();

    /**
     * Find user by gmail
     * @param login_data User's gmail.
     * @param data_base Database name
     * @return If the user exist return true else return false
    */
    bool find_mail(char* login_data, const char* data_base);
};

#endif