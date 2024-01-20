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
    int __deleted;     // Is mail marked as deleted

    sqlite3* db; // sqlite3 object for communicating between database and c++

    /**
     * Function for opening database.
     * @param database Database name.
     * @return True if database is successfully open else false.
    */
    bool OpenDatabase(const char* database);

public:

    // getters for fields in database
    string get_sender();
    string GetReceiver();
    string GetSubject();
    string GetContent();

    /**
     * Find user by gmail
     * @param gmail User's gmail.
     * @param ordinal_no Mail's ordinal number
     * @param database Database name.
     * @return If the mail exist return true else return false
    */
    bool FindMail(const char* database, const char* gmail, int ordinal_no);


    /**
     * This function handles POP3 STAT request from client.
     * @param database Database name.
     * @param receiver Client's email.
     * @param message_count Number of messages in client's inbox.
     * @return Length of all messages in receiver's inbox.
    */
    int ProcessStatRequest(const char* database, const char* receiver, int* message_count);


    /**
     * This function handles POP3 DELETE request from client.
     * @param database Database name.
     * @param gmail Client's email.
     * @param ordinal_no Ordinal number of the message that client want to delete.
     * @return If message successfuly marked as deleted return true, else false.
    */
    bool MarkMessageAsDeleted(const char* database, const char* gmail, int ordinal_no);

    /**
     * This function handles POP3 RSET request from client, unmark all deleted messages.
     * @param database Database name.
     * @param gmail Client's email.
     * @return If messages successfuly unmarked return true, else false.
    */
    bool UnmarkDeletedMessages(const char* database, const char* gmail);

    /**
     * This function handles POP3 QUIT request from client, delete all marked messages.
     * @param database Database name.
     * @param gmail Client's email.
    */
    void DeleteMarkedMessages(const char* database, const char* gmail);

    /**
     * This function handles POP3 LIST request (without argument) from client (send informations about every message in clients inbox).
     * @param sock Client's socket where we send informations.
     * @param ThreadId Client's thread id in ThreadPool.
     * @param database Database name.
     * @param gmail Client's email.
     * @return All informations that will be sent to client.
    */
    string ListAllMessages(const char* database, const char* gmail);
};

#endif