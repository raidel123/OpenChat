//
//  messenger_client.cpp
//  messenger_client
//
//  Created by Raidel Hernandez on 11/4/17.
//  Copyright © 2017 Raidel Hernandez. All rights reserved.
//

// project header, contains all globals
#include "cglobals.h"

void    connect();
void    process_args(const char * argv[]);

int main(int argc, const char * argv[]) {
    
    if (argc != 2) {
        printf("usage: messenger_client configration_file\n");
        exit(EXIT_FAILURE);
    }
    
    process_args(argv);
    
    // cout << SERVPORT << " : " << servport << endl;
    // cout << SERVHOST << " : " << servhost << endl;
    
    connect();
    
    exit(EXIT_SUCCESS);
}

// code below is very similar to lec14/example3.cpp
void connect(){
    int sockfd;
    bool login_success = false;
    struct sockaddr_in addr;
    char buf[1024];
    //fd_set  rset, orig_set;
    
    int rv, flag;
    struct addrinfo hints, *res, *ressave;
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(servhost.c_str(), servport.c_str(), &hints, &res)) != 0) {
        cout << "getaddrinfo wrong: " << gai_strerror(rv) << endl;
        exit(1);
    }
    
    ressave = res;
    flag = 0;
    
    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
            flag = 1;
            break;
        }
        close(sockfd);
    } while ((res = res->ai_next) != NULL);
    freeaddrinfo(ressave);
    
    if (flag == 0) {
        fprintf(stderr, "cannot connect\n");
        exit(1);
    }
    
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    
    string username;
    string password;
    
    int serv_sockfd, rec_sock, len, i;
    struct sockaddr_in serv_addr, recaddr;
    
    if ((serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(": Can't get socket");
        exit(1);
    }
    
    // uncomment this block for lohin to work properly with messenger option
    // comment starts here *********
    enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    
    //((struct sockaddr_in *)res2_2->ai_addr)->sin_addr.s_addr;
    
    //cout << "local IP: " << local_ip << endl;
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((short)atoi(client_serv_port.c_str()));
    
    if (::bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror(": bind");
        exit(1);
    }
    
    
    len = sizeof(serv_addr);
    if (getsockname(serv_sockfd, (struct sockaddr *)&serv_addr, (socklen_t *)&len) < 0) {
        perror(": can't get name");
        _exit(1);
    }
    
    //printf("ip = %s, port = %d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    
    if (listen(serv_sockfd, 5) < 0) {
        perror(": bind");
        exit(1);
    }
    /*Comment end here**************/
    
    while(1){
        //string option = menu();
        string option = server_menu();
        
        if(option == "exit"){
            close(serv_sockfd);
            close(sockfd);
            exit(1);
        }else{
            //string option = server_menu();
            
            if(option == "exit")
                return;
            else if (option == "l"){
                cout << "Username: ";
                cin >> username;
                
                cout << "Password: ";
                cin >> password;
                
                string server_cmd = LOGIN + STR_S + username + STR_S + password;
                
                write(sockfd, server_cmd.c_str(), server_cmd.size()+1);
                
            } else if (option == "r"){
                string username;
                string password;
                
                cout << "Username: ";
                cin >> username;
                
                cout << "Password: ";
                cin >> password;
                
                string server_cmd = REGISTER + STR_S + username + STR_S + password;
                
                write(sockfd, server_cmd.c_str(), server_cmd.size()+1);
            }
        }
        
        // start here next time, check select fd and make sure i am doing it correctly
        //if(login_success){
        //print_messenger_menu();
        
        
        int client[MAXCONN];
        char serv_buf[MAXBUFLEN];
        fd_set allset, rset;
        int maxf;
        
        FD_ZERO(&allset);
        FD_SET(sockfd, &allset);
        FD_SET(STDIN_FILENO, &allset);
        FD_SET(serv_sockfd, &allset);
        maxf = sockfd;
        
        if (STDIN_FILENO > maxf)
            maxf = STDIN_FILENO;
        
        if (serv_sockfd > maxf)
            maxf = serv_sockfd;
        
        
        for (i=0; i<MAXCONN; i++)
            client[i] = -1;
        while (1) {
            rset = allset;
            select(maxf + 1, &rset, NULL, NULL, NULL);
            ssize_t n;
            
            if (FD_ISSET(sockfd, &rset)) {
                string str_buf;
                
                if ((n = read(sockfd, buf, MAXBUFLEN-1)) == 0) {
                    printf("server crashed.\n");
                    exit(0);
                }
                buf[n] = '\0';
                str_buf = buf;
                //cout << "\n\nstr_buf serve: " << str_buf << endl << endl;
                
                stringstream ss;
                ss << str_buf;
                
                string str_val;
                getline(ss, str_val, '|');
                
                if(str_val == L_RESPONSE){
                    string str_buff;
                    getline(ss, str_buff, '|');
                    if (str_buff == "200"){
                        cout << "\n\nLogin successful!!!\n" << endl;
                        login_success = true;
                        current_user.username = username;
                        current_user.password = password;
                        
                        getline(ss, local_ip, '|');
                        getline(ss, str_buff);
                        
                        //cout << "local ip: " << local_ip << endl;

                        if(str_buff != "NONE")
                            build_online_friends(str_buff);
                    }else{
                        cout << "\nIncorrect Username or Password, please try again\n\n";
                        break;
                    }
                } else if(str_val == R_RESPONSE){
                    string str_buff;
                    getline(ss, str_buff, '|');
                    
                    if (str_buff == "200")
                        cout << "\nRegistered successfully!!!\n\n";
                    else{
                        cout << "\nUsername already exists, could not register username.\n\n";
                    }
                    break;
                } else if(str_val == FONLINE){
                    string f_val;
                    getline(ss, f_val);
                    //cout << "\n\nFONLINE str_buf: " << f_val << endl << endl;
                    
                    add_online_friend(f_val);
                } else if (str_val == LOGOUT){
                    string user;
                    ss >> user;
                    remove_online_friend(user);
                } else if (str_val == "invitation"){
                    string msg;
                    string from_username;
                    getline(ss, from_username, '|');
                    getline(ss, msg);
                    
                    cout << "invite from: " << from_username << endl;
                    cout << "invite message: " << msg << endl;
                    cout << endl;
                } else if (str_val == "a_invitation"){
                    string msg;
                    string from_username;
                    getline(ss, from_username, '|');
                    getline(ss, msg);
                    
                    cout << "accept invite accepted from: " << from_username << endl;
                    cout << "accept invite message: " << msg << endl;
                    cout << endl;
                }
                
                //cout << "\nFriends Online: " << online_friends_toString() << endl;
                
                if(login_success)
                    print_messenger_menu();

            }
            
            if (FD_ISSET(serv_sockfd, &rset)) {
                /* somebody tries to connect */
                
                //cout << "\n\nsomeone is trying to connect: \n" << endl;
                if ((rec_sock = accept(serv_sockfd, (struct sockaddr *)(&recaddr), (socklen_t *)&len)) < 0) {
                    if (errno == EINTR)
                        continue;
                    else {
                        perror(":accept error");
                        exit(1);
                    }
                }
                
                if (rec_sock < 0) {
                    perror(": accept");
                    exit(1);
                }
                
                /* print the remote socket information */
                
                //printf("remote machine = %s, port = %d.\n", inet_ntoa(recaddr.sin_addr), ntohs(recaddr.sin_port));
                
                for (i=0; i<MAXCONN; i++) {
                    if (client[i] < 0) {
                        client[i] = rec_sock;
                        FD_SET(client[i], &allset);
                        break;
                    }
                }
                
                if (i== MAXCONN) {
                    printf("too many connections.\n");
                    close(rec_sock);
                }
                
                if (rec_sock > maxf) maxf = rec_sock;
                
                print_messenger_menu();
            }
            
            if (FD_ISSET(STDIN_FILENO, &rset)) {
                string line;
                string selection;
                stringstream ss_stdin;
                if (!(getline(cin, line))) {
                    printf("SIGINT handled.\n");
                    
                    cout << "\n\nLogout Successful***\n" << endl;
                    string logout_cmd = LOGOUT + "|" + current_user.username;
                    write(sockfd, logout_cmd.c_str(), logout_cmd.size());
                    login_success = false;
                    close(serv_sockfd);

                    for (int i = 0; i < MAXCONN; ++i){
                        if (client[i] < 0) continue;
                        close(client[i]);
                        FD_CLR(client[i], &allset);
                        client[i] = -1;
                    }
                    
                    close(sockfd);
                    exit(0);
                }
                //string selection = messenger_menu();
                
                ss_stdin << line;
                ss_stdin >> selection;
                
                //string selection = buf;
                if(selection == LOGOUT){
                    // tell server I logged out
                    cout << "\n\nLogout Successful***\n" << endl;
                    string logout_cmd = LOGOUT + "|" + current_user.username;
                    write(sockfd, logout_cmd.c_str(), logout_cmd.size());
                    login_success = false;
                    close(serv_sockfd);
                    
                    for (int i = 0; i < MAXCONN; ++i){
                        if (client[i] < 0) continue;
                        close(client[i]);
                        FD_CLR(client[i], &allset);
                        client[i] = -1;
                    }
                    
                    //close(sockfd);
                    break;
                }else if (selection == "m"){
                    //cout << "\n\n***M***\n" << endl;
                    
                    int m_sockfd;
                    struct sockaddr_in m_addr;
                    char m_buf[1024];
                    
                    string user_name;
                    string msg;
                    
                    ss_stdin >> user_name;
                    
                    getline(ss_stdin, msg);
                    //cout << "\n\n sending info to: " << user_name << endl;
                    
                    //cout << "\nusername: " << user_name ;
                    
                    if(online_friends.find(user_name) != online_friends.end()) {
                        
                        if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                            perror(": Can't get socket");
                            exit(1);
                        }
                        
                        memset(&m_addr, 0, sizeof(struct sockaddr_in));
                        m_addr.sin_family = AF_INET;
                        m_addr.sin_addr.s_addr = inet_addr(online_friends[user_name].ip.c_str());
                        m_addr.sin_port = htons(atoi(client_serv_port.c_str()));
                        
                        //cout << "\n\nuser2 IP: " << online_friends[user_name].ip.c_str() << endl << endl;
                        //cout << "\n\nuser2 port htons: " << client_serv_port.c_str() << endl;
                        
                        if (connect(m_sockfd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0){
                            perror(": connect");
                            exit(1);
                        }
                        
                        
                        string send = IMESSAGE + STR_S + current_user.username + STR_S + msg;
                        //cout << "\nwriting msg: " << msg << endl;
                        write(m_sockfd, send.c_str(), send.size());
                        close(m_sockfd);
                        
                    }
                    
                    
                }else if (selection == "i"){
                    //cout << "\n\n***I***\\n" << endl;
                    
                    string user_name;
                    string msg;
                    
                    ss_stdin >> user_name;
                    if(!getline(ss_stdin, msg)){
                        msg = " ";
                    }
                    
                    string invitation = "invitation" + STR_S + current_user.username + STR_S + user_name + STR_S + msg;
                    
                    write(sockfd, invitation.c_str(), invitation.size());
                    
                } else if (selection == "ia"){
                    //cout << "\n\n***IA***\n" << endl;
                    
                    string user_name;
                    string msg;
                    
                    ss_stdin >> user_name;
                    if(!(getline(ss_stdin, msg))){
                        msg = " ";
                    }
                    
                    string a_invitation = "a_invitation" + STR_S + current_user.username + STR_S + user_name + STR_S + msg;
                    
                    write(sockfd, a_invitation.c_str(), a_invitation.size());
                }
                
                //write(sockfd, selection.c_str(), selection.size());
                print_messenger_menu();
            }
            
            for (i=0; i<MAXCONN; i++) {
                if (client[i] < 0) continue;
                if (FD_ISSET(client[i], &rset)) {
                    //cout << "client set: " << i << endl;
                    ssize_t num;
                    num = read(client[i], serv_buf, MAXBUFLEN-1);
                    if (num == 0) {
                        /* client exists */
                        close(client[i]);
                        FD_CLR(client[i], &allset);
                        client[i] = -1;
                    } else {
                        serv_buf[num] = '\0';
                        string str_buf = serv_buf;
                        
                        string client_line;
                        stringstream ss_client(str_buf);
                        
                        getline(ss_client, client_line, '|');
                        
                        //cout << "client message: " << str_buf << endl;
                        if(client_line == IMESSAGE){
                            string sender;
                            string msg;
                            
                            getline(ss_client, sender, '|');
                            getline(ss_client, msg);
                            cout << sender << " >> " << msg << endl;
                        }
                        
                        close(client[i]);
                        FD_CLR(client[i], &allset);
                        client[i] = -1;

                    }
                    
                    print_messenger_menu();
                }
            }
        }
    }
}

void build_online_friends(string friend_list){
    string friend_n, user_n, ip_n, port_n;
    
    stringstream ss_friend, ss_friend_list;
    ss_friend << friend_list;
    
    while (getline(ss_friend, friend_n, '|')){
        ss_friend_list << friend_n;
        getline(ss_friend_list, user_n, ':');
        getline(ss_friend_list, ip_n, ':');
        getline(ss_friend_list, port_n);
        
        friend_info inf(user_n, ip_n, port_n);
        online_friends[user_n] = inf;
        
        //cout << "build online friends(str)" <<endl;
        cout << "\nFriends online: " << online_friends_toString() << endl << endl;
    }
}

void add_online_friend(string friend_line){
    string user_n, ip_n, port_n;
    stringstream ss_friend_list;
    ss_friend_list << friend_line;
    getline(ss_friend_list, user_n, ':');
    getline(ss_friend_list, ip_n, ':');
    getline(ss_friend_list, port_n);
    
    friend_info inf(user_n, ip_n, port_n);
    online_friends[user_n] = inf;
    
    //cout << "add online friends(str)" <<endl;
    cout << "\nFriends online: " << online_friends_toString() << endl << endl;
}

void remove_online_friend(string user){
    if(online_friends.find(user) != online_friends.end())
        online_friends.erase(user);
    
    cout << "\n" << user << " has logged out!" << endl;
    cout << "Friends online: " << online_friends_toString() << endl << endl;
}

string server_menu(){
    
    bool ask = true;
    
    string selection;
    while(ask) {
        print_server_menu();
        cin >> selection;
        
        if(selection == "r" || selection == "l" || selection == "exit"){
            ask = false;
        }else
            cout << "\nIncorrect selection, try again\n\n";
        
    }
    
    return selection;
}

void print_messenger_menu(){
    
    cout << current_user.username << " > ";
    cout.flush();
}

void print_server_menu(){
    cout << "Select one of the following options:\n";
    cout << "(r) Register\n";
    cout << "(l) Login\n";
    cout << "(exit) Exit the program\n\n";
    cout << "Make Selection: ";
    cout.flush();
}

// parse files passed in as argument
void process_args(const char * argv[]) {
    // remove print for submission
    //cout << "process args\n";
    
    //ifstream user_info_file;
    ifstream config_file;
    string keyword;
    
    //user_info_file.open(argv[1]);
    config_file.open(argv[1]);
    
    // parse configuration_file
    for (int i = INITVALUE; i < NUMKEYWORDS; ++i){
        config_file >> keyword;
        keyword.pop_back();   // remove ":" at the end of keyword
        
        if (keyword == SERVHOST)
            config_file >> servhost;
        else if (keyword == SERVPORT)
            config_file >> servport;
    }
    
    config_file.close();
}

string online_friends_toString(){
    string friend_list = "";
    
    auto itr = online_friends.begin();
    
    if(online_friends.size() > 0){
        friend_list = itr->second.username;
        
        ++itr;
        for(; itr != online_friends.end(); ++itr){
            
            if (itr->second.username == "")
                continue;
            friend_list += "|";
            friend_list += itr->second.username;
        }
    }
    
    return friend_list;
}
