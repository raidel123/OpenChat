//
//  cglobals.h
//  messenger_client
//
//  Created by Raidel Hernandez on 11/5/17.
//  Copyright © 2017 Raidel Hernandez. All rights reserved.
//

#ifndef SGLOBALS_H
#define SGLOBALS_H

// c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <netdb.h>

// c++
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>

using namespace std;

#define MAXCONN 5

// const globals
const int       NUMKEYWORDS     = 2;
const int       INITVALUE       = 0;
const string    SERVHOST        = "servhost";
const string    SERVPORT        = "servport";
const string    STR_S           = "|";
const string    LOGIN           = "login";
const string    LOGOUT           = "logout";
const string    REGISTER        = "register";
const string    FONLINE         = "fonline";
const string    RECEIVED        = "received";
const unsigned  MAXBUFLEN       = 1024;
const string    L_RESPONSE      = "l_response";
const string    R_RESPONSE      = "r_response";
const string    IMESSAGE        = "imessage";

struct friend_info{
    string username;
    string ip;
    string port;
    
    friend_info(){
        
    }
    
    friend_info(string user_p, string ip_p, string port_p){
        username = user_p;
        ip = ip_p;
        port = port_p;
    }
    
    friend_info(const friend_info& rhs){
        username = rhs.username;
        ip = rhs.ip;
        port = rhs.port;
    }
    
    friend_info& operator=(const friend_info& rhs){
        if(this != &rhs){
            username = rhs.username;
            ip = rhs.ip;
            port = rhs.port;
        }
        
        return *this;
    }
};

struct login_info{
    string username;
    string password;
};

// globals
string servhost;
string servport;
string client_serv_port = "5100";
string local_ip;
map<string,friend_info> online_friends;
login_info current_user;

//pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;

//void    *process_input(void* arg);
//void    *process_connection(void *arg);
void    *process_connection_thread(void *arg);
void    *listen_server(void *arg);
void    build_online_friends(string friend_list);
void    connect();
string  server_menu();
string messenger_menu();
void    process_args(const char * argv[]);
//string get_domain_name();

void print_messenger_menu();
void print_server_menu();
void add_online_friend(string friend_line);
void remove_online_friend(string user);

string online_friends_toString();

#endif /* sglobals_h */

