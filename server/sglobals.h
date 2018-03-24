//
//  uinfo.h
//  messenger_server
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
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <strings.h>

// c++ / std lib
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

struct friend_info{
    string username;
    string ip;
    string port;
    
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

struct user_info{
    
    // member-data
    string  username;
    string  password;
    vector<string> friends;
    vector<friend_info> online_friend_info;
    bool    online;
    string  ip;
    string  port;
    int     fd;
    
    // default constructor
    user_info(){
        online = false;
        fd = -1;
    }
    
    user_info(const string& user, const string& pass){
        username = user;
        password = pass;
        online = false;
        fd = -1;
    }
    
    // copy constructor
    user_info(const user_info& rhs){
        username = rhs.username;
        password = rhs.password;
        friends = rhs.friends;
        online = rhs.online;
        online_friend_info = rhs.online_friend_info;
        ip = rhs.ip;
        port = rhs.port;
        fd = rhs.fd;
    }
    
    // copy assignment operator
    user_info& operator=(const user_info& rhs){
        if(this != &rhs){
            username = rhs.username;
            password = rhs.password;
            friends = rhs.friends;
            online = rhs.online;
            online_friend_info = rhs.online_friend_info;
            ip = rhs.ip;
            port = rhs.port;
            fd = rhs.fd;
        }
        return *this;
    }
    
    void set_online(string ip_addr, string port_num, int fd_num){
        online = true;
        ip = ip_addr;
        port = port_num;
        fd = fd_num;
    }
    
    void logout(){
        online = false;
        ip = "";
        port = "";
        fd=-1;
        friends.clear();
        online_friend_info.clear();
    }
    
    void add_friend(string user){
        if(find(friends.begin(), friends.end(), user) == friends.end())
            friends.push_back(user);
    }
    
    void add_online_friends(string user_n, string ip_n, string port_n){
        online_friend_info.push_back(friend_info(user_n, ip_n, port_n));
    }
    
    void add_online_friends(vector<friend_info> f_info){
        online_friend_info = f_info;
    }
    
    void remove_online_friend(string usern){
        for(auto itr = online_friend_info.begin(); itr != online_friend_info.end(); itr++){
            if(itr->username == usern){
                online_friend_info.erase(itr);
                break;
            }
        }
    }
    
    string online_friends_toFileStr(){
        string friend_list = "";
        
        if(friends.size() > 0){
            friend_list = friends[0];
            
            for(int i = 1; i < friends.size(); ++i){
                
                friend_list += ":";
                friend_list += friends[i];
            }
            
            cout << "friend_list: " << friend_list << endl;
        } else {
            friend_list = "NONE";
        }
        
        return friend_list;
    }
    
    string online_friends_toString(){
        string friend_list = "";
        
        auto itr = online_friend_info.begin();
        
        if(online_friend_info.size() > 0){
            friend_list = itr->username + ":" + itr->ip + ":" + itr->port;
            
            ++itr;
            for(; itr != online_friend_info.end(); ++itr){
                
                friend_list += "|";
                friend_list += itr->username + ":" + itr->ip + ":" + itr->port;
            }
            
            cout << "friend_list: " << friend_list << endl;
        }
        
        if (friend_list.size() == 0)
            friend_list = "NONE";
        
        return friend_list;
    }
};


/*
 struct user_socket{
 sock_ptr;
 cli_addr;
 cli_port;
 }
 */

// pre-processor directives
#define MAXCONN 10           // change this value

// const globals
const int       NUMUSERFIELDS   = 3;
const int       NUMKEYWORDS     = 1;
const int       INITVALUE       = 0;
const string    PORT            = "port";
const unsigned  MAXBUFLEN       = 1024;
const string    FONLINE         = "fonline";
const string    LOGIN           = "login";
const string    LOGOUT          = "logout";
const string    REGISTER        = "register";
const string    INVITATION      = "invitation";
const string    A_INVITATION      = "a_invitation";
const string    L_RESPONSE      = "l_response";
const string    R_RESPONSE      = "r_response";

// globals
map<string, user_info> users;
string port;
int online_users = 0;
string user_info_file;
string config_file;

pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;

// void server();
void    server();
string  login(string username, string password, string ip_addr, string port_num, int fd_num);
void    logout(user_info u_info);
string  register_user(string username, string password, string ip_addr, string port_num);
//void    *process_connection(void *arg);
void    process_args(const char * argv[]);
string  get_domain_name();
vector<string>      friend_list(string friends);
vector<friend_info> get_online_friends(user_info u_info);
void sig_chld(int signo);


// new stuff
void *process_connection(void *arg);


#endif /* sglobals_h */

