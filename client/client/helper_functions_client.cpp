#include "helper_functions_client.h"


void login_menu(char* uinput){
    std::string message_arg;

    std::cout << "Enter your gmail: \t";
    std::cin >> message_arg;

    strcpy(uinput, "USER ");
    strcat(uinput, message_arg.c_str());
}

void pin_menu(char* uinput){
    std::string message_arg;

    std::cout << "Enter your password: \t";
    std::cin >> message_arg;

    strcpy(uinput, "PASS ");
    strcat(uinput, message_arg.c_str());
}

int user_menu(char* uinput) {
    int option;

    std::cout << "\n1. STAT \n";
    std::cout << "2. LIST (with argument)\n";
    std::cout << "3. LIST\n";
    std::cout << "4. RETR\n";
    std::cout << "5. DELE\n";
    std::cout << "6. NOOP\n";
    std::cout << "7. RSET\n";
    std::cout << "8. QUIT\n";

    do {
        std::cout << "Enter option: \t";
        std::cin >> option;
    } while (option >8 || option < 1);

    std::string message_arg;
    switch (option) {
        case 1:
            strcpy(uinput, "STAT");
            break;
        case 2:
            std::cout << "Command: LIST ";
            std::cin >> message_arg;

            strcpy(uinput, "LIST ");
            strcat(uinput, message_arg.c_str());
            break;
        case 3:
            strcpy(uinput, "LIST");
            break;
        case 4:
            std::cout << "Command: RETR ";
            std::cin >> message_arg;

            strcpy(uinput, "RETR ");
            strcat(uinput, message_arg.c_str());
            break;
        case 5:
            std::cout << "Command: DELE ";
            std::cin >> message_arg;

            strcpy(uinput, "DELE ");
            strcat(uinput, message_arg.c_str());;
            break;
        case 6:
            strcpy(uinput, "NOOP");
            break;
        case 7:
            strcpy(uinput, "RSET");
            break;
        case 8:
            strcpy(uinput, "QUIT");
            break;
        default:
            std::cout << "ERROR!" << std::endl;
    }

    return option;
}

void feedback(int option, char* message) {

    switch (option) {
        case 1:
            if(strcmp(message, "ACCEPT") == 0)
                std::cout << "\nPin code changed successfully." << std::endl;
            else 
                std::cout << "\nPin code change failed." << std::endl;
            break;
        case 2:
            std::cout << "\nBalance: " << message << std::endl;
            break;
        case 3:
            if(strcmp(message, "ACCEPT") == 0)
                std::cout << "\nWithdrawal successfully." << std::endl;
            else 
                std::cout << "\nWithdrawal failed. Not enough funds." << std::endl;
            break;
        case 4:
            if(strcmp(message, "ACCEPT") == 0)
                std::cout << "\nYou are logged out!" << std::endl;
            else 
                std::cout << "\nLogout failed" << std::endl;
            break;
        default:
            std::cout << "ERROR!" << std::endl;
    }
}

void send_data(char* message, int sock){
    if( send(sock, message , strlen(message), 0) < 0)
    {
        puts("Send failed");
        exit(1);
    }
}

bool receive_data(int sock, char message[]) {
    int read_size = recv(sock, message, DEFAULT_BUFLEN, 0);
    message[read_size] = 0;

    if(read_size <= 0) { 
        std::cout << "Connection error with server!" << std::endl;
        return false;
    }
    
    return true;
} 

