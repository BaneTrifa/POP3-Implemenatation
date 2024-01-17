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
     * @param gmail User's gmail.
     * @param ordinal_no Mail's ordinal number
	 * @param data_base Database name.
     * @return If the mail exist return true else return false
    */
    bool find_mail(char* gmail, int ordinal_no, const char* data_base);

	
	/**
     * This function handles POP3 STAT request from client.
     * @param data_base Database name.
	 * @param receiver Client's email.
	 * @param message_count Number of messages in client's inbox.
     * @return Length of all messages in receiver's inbox.
    */
	int process_stat_request(const char* data_base, const char* receiver, int* message_count);


	/**
     * This function handles POP3 DELETE request from client.
     * @param data_base Database name.
	 * @param gmail Client's email.
	 * @param ordinal_no Ordinal number of the message that client want to delete.
     * @return If message successfuly marked as deleted return true, else false.
    */
	bool mark_message_as_deleted(const char* data_base, const char* gmail, int ordinal_no);

	/**
     * This function handles POP3 RSET request from client, unmark all deleted messages.
     * @param data_base Database name.
	 * @param gmail Client's email.
     * @return If messages successfuly unmarked return true, else false.
    */
	bool unmark_message_deleted(const char* data_base, const char* gmail);

	/**
     * This function handles POP3 QUIT request from client, delete all marked messages.
     * @param data_base Database name.
	 * @param gmail Client's email.
    */
	void delete_marked_message(const char* data_base, const char* gmail);
};

#endif