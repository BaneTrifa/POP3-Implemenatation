# POP3-Implemenatation
## Description 

The task of this project is to implement, using the Microsoft Visual C++ development environment, an example application that incorporates the functionality of a POP3 email server. The application expects connection requests on a predefined TCP port. Additionally, the application should allow blocking access to the server for certain users. Users are identified by their email addresses, which are sent via the POP3 "USER" message. Test the application using an email client.

The implementation of the solution is done as a console application within the Microsoft Visual Studio 2022 development environment. To store user data and emails, I use a database implemented in SQLite.

## Implementation
Initially, the server host starts the POP3 service by listening on
TCP port 110. When a client host wishes to make use of the service,
it establishes a TCP connection with the server host. When the
connection is established, the POP3 server sends a greeting. The
client and POP3 server then exchange commands and responses
(respectively) until the connection is closed or aborted.


A POP3 session progresses through a number of states during its
lifetime. Once the TCP connection has been opened and the POP3
server has sent the greeting, the session enters the AUTHORIZATION
state. In this state, the client must identify itself to the POP3
server. Once the client has successfully done this, the server
acquires resources associated with the client’s maildrop, and the
session enters the TRANSACTION state. In this state, the client
requests actions on the part of the POP3 server. When the client has issued the QUIT command, the session enters the UPDATE state. In
this state, the POP3 server releases any resources acquired during
the TRANSACTION state and says goodbye. The TCP connection is then
closed.

### **The AUTHORIZATION State**
Once the TCP connection has been opened by a POP3 client, the POP3
server issues a one line greeting. This can be any positive
response. An example might be:

    S: +OK POP3 server ready

The POP3 session is currently in the AUTHORIZATION state. In the authorization state, the client needs to identify itself using the USER and PASS commands. An example could be:

    C: USER johndoe@gmail.com
    S: +OK ACCEPT
    C: PASS 1122
    S: +OK ACCEPT

After successful authorization, the client and server transition to the TRANSACTION state.

### **The TRANSACTION State**
Once the client has successfully identified itself to the POP3 server
and the POP3 server has locked and opened the appropriate maildrop,
the POP3 session is now in the TRANSACTION state. The client may now
issue any of the following POP3 commands repeatedly. After each
command, the POP3 server issues a response. Eventually, the client
issues the QUIT command and the POP3 session enters the UPDATE state.

#### **STAT**
    Arguments: none
    
    Restrictions:
      may only be given in the TRANSACTION state
    
    The positive response consists of "+OK" followed by a single
    space, the number of messages in the maildrop, a single
    space, and the size of the maildrop in octets.
    
    Possible Responses:
    +OK nn mm
    
    Examples:
        C: STAT
        S: +OK 2 320

#### **LIST [msg]**

    Arguments:
      a message-number (optional), which, if present, may NOT
      refer to a message marked as deleted
    Restrictions:
      may only be given in the TRANSACTION state
  
    If an argument was given and the POP3 server issues a
    positive response with a line containing information for
    that message. This line is called a "scan listing" for
    that message.
    If no argument was given and the POP3 server issues a
    positive response, then the response given is multi-line.
    After the initial +OK, for each message in the maildrop,
    the POP3 server responds with a line containing
    information for that message. This line is also called a
    "scan listing" for that message.

    Possible Responses:
      +OK scan listing follows
      -ERR no such message
    Examples:
      C: LIST
      S: +OK 2 messages (320 octets)
      S: 1 120
      S: 2 200

      C: LIST 2
      S: +OK 2 200

      C: LIST 3
      S: -ERR no such message, only 2 messages in maildrop

#### **RETR msg**

    Arguments:
      a message-number (required) which may NOT refer to a
      message marked as deleted
    Restrictions:
      may only be given in the TRANSACTION state

      If the POP3 server issues a positive response, then the
    response given is multi-line. After the initial +OK, the
    POP3 server sends the message corresponding to the given
    message-number.

    Possible Responses:
    +OK message follows
    -ERR no such message
    
    Examples:
      C: RETR 1
      S: +OK 120 octets
      S: <the POP3 server sends the entire message here>

#### **DELE msg**

    Arguments:
      a message-number (required) which may NOT refer to a
      message marked as deleted
    Restrictions:
      may only be given in the TRANSACTION state
    
    The POP3 server marks the message as deleted. Any future
    reference to the message-number associated with the message
    in a POP3 command generates an error. The POP3 server does
    not actually delete the message until the POP3 session
    enters the UPDATE state.

    Possible Responses:
      +OK message deleted
      -ERR no such message
      
    Examples:
      C: DELE 1
      S: +OK message 1 deleted
      ...
      C: DELE 2
      S: -ERR message 2 already deleted

#### **NOOP**
    Arguments: none
    Restrictions:
      may only be given in the TRANSACTION state
   
    The POP3 server does nothing, it merely replies with a positive response.
    Possible Responses:
      +OK
      
    Examples:
      C: NOOP
      S: +OK

#### **RSET**

    Arguments: none
    Restrictions:
      may only be given in the TRANSACTION state
    
    If any messages have been marked as deleted by the POP3
    server, they are unmarked. The POP3 server then replies with a positive response.

    Possible Responses:
      +OK
      
    Examples:
      C: RSET
      S: +OK 

### **The UPDATE State**

When the client issues the QUIT command from the TRANSACTION state,
the POP3 session enters the UPDATE state.

#### **QUIT**
    Arguments: none
    Restrictions: none

    The POP3 server removes all messages marked as deleted
    from the maildrop and replies as to the status of this
    operation. If there is an error, such as a resource
    shortage, encountered while removing messages, the
    maildrop may result in having some or none of the messages
    marked as deleted be removed. In no case may the server
    remove any messages not marked as deleted.

    Possible Responses:
      +OK
      -ERR some deleted messages not removed
  
    Examples:
    C: QUIT
    S: +OK dewey POP3 server signing off (maildrop empty)
    ...
    C: QUIT
    S: +OK dewey POP3 server signing off

## Author
- [@BrankoTrifkovic](https://www.linkedin.com/in/branko-trifkovic/)

      
