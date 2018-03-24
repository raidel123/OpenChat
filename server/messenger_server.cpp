//
//  messenger_server.cpp
//  messenger_server
//
//  Created by Raidel Hernandez on 11/4/17.
//  Copyright © 2017 Raidel Hernandez. All rights reserved.
//


// self-built
#include "sglobals.h"

int main(int argc, const char * argv[]) {
    struct sigaction abc;

    abc.sa_handler = sig_chld;
    sigemptyset(&abc.sa_mask);
    abc.sa_flags = 0;

    sigaction(SIGCHLD, &abc, NULL);

    if (argc != 3) {
        printf("usage: messenger_server user_info_file configration_file\n");
        exit(EXIT_FAILURE);
    }
    
    user_info_file = argv[1];
    config_file = argv[2];
    
    process_args(argv);
    //cout << "Port: " << port << endl;
    
    // start the server
    server();
    
    exit(EXIT_SUCCESS);
}

void server(){
    // code below is very similar to echo_server_threads.cpp
    int serv_sockfd, cli_sockfd, *sock_ptr;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t sock_len;
    pthread_t tid;
    
    // get the socket file descriptor
    //cout << "port = " << port << endl;
    if((serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror(": Can't get socket");
        exit(1);
    }
    
    // reset the bytes in memory for 'struct sockaddr_in' to 0
    // and set the new socket address information to bind
    bzero((void*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons((short)atoi(port.c_str()));
    
    // bind the socket and the along with the socket address information
    if(::bind(serv_sockfd, (struct sockaddr *)&serv_addr, (int)sizeof(serv_addr)) < 0){
        perror(": bind");
        exit(1);
    }
    
    // get the created socket information so we can display it
    int len = sizeof(serv_addr);
    if (getsockname(serv_sockfd, (struct sockaddr *)&serv_addr, (socklen_t *)&len) < 0) {
        perror(": can't get name");
        _exit(1);
    }
    
    // print domain name and port number after bind()
    printf("ip = %s, port = %d\n", get_domain_name().c_str(), ntohs(serv_addr.sin_port));
    //printf("ip = %s, port = %d\n", inet_ntoa(serv_addr.sin_addr), htons(serv_addr.sin_port));
    
    // listen for incoming connections
    if (listen(serv_sockfd, 5) < 0){
        perror(": listen");
        exit(1);
    }
    
    // accept connections and create a thread to handle each one
    for (; ;) {
        sock_len = sizeof(cli_addr);
        cli_sockfd = accept(serv_sockfd, (struct sockaddr *)&cli_addr, &sock_len);
        
        cout << "remote client IP == " << inet_ntoa(cli_addr.sin_addr);
        cout << ", port == " << ntohs(cli_addr.sin_port) << endl;
        
        sock_ptr = (int *)malloc(sizeof(int));
        *sock_ptr = cli_sockfd;
        
        pthread_create(&tid, NULL, &process_connection, (void*)sock_ptr);
    }
}

void *process_connection(void *arg) {
    int sockfd;
    int len;
    ssize_t n;
    char buf[MAXBUFLEN];
    string wb;
    
    sockfd = *((int *)arg);
    free(arg);
    
    pthread_detach(pthread_self());
    while ((n = read(sockfd, buf, MAXBUFLEN-1)) > 0) {
        buf[n] = '\0';
        cout << "buf: " << buf << endl;
        string str_buf = buf;
        string wb;
        cout << str_buf << endl;
        stringstream ss;
        ss.str(str_buf);
        
        string service;
        getline(ss, service,'|');
        
        cout << "service: " << service << endl;
        if(service == LOGIN){
            string username;
            string password;
            getline(ss, username, '|');
            getline(ss, password);
            
            cout << "username: " << username << endl;
            cout << "password: " << password << endl;
            
            struct sockaddr_in cli_addr;
            // get the created socket information so we can display it
            len = sizeof(cli_addr);
            if (getpeername(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&len) < 0) {
                perror(": can't get name");
                _exit(1);
            }
            
            // print domain name and port number after bind()
            //printf("ip = %s, port = %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            //printf("ip = %s, port = %d\n", inet_ntoa(serv_addr.sin_addr), htons(serv_addr.sin_port));
            
            //TODO possible issue with write
            pthread_mutex_lock(&mutex1);
            wb = login(username, password, inet_ntoa(cli_addr.sin_addr), to_string(htons(cli_addr.sin_port)), sockfd);
            
            //write(sockfd, wb.c_str(), wb.size());
            string send = L_RESPONSE + "|" + wb + "|" + inet_ntoa(cli_addr.sin_addr) + "|" + users[username].online_friends_toString();
            
            write(sockfd, send.c_str(), send.size());
            pthread_mutex_unlock(&mutex1);
            /*
            if(wb == "200"){
                vector<friend_info> vf_info = get_online_friends(users[username]);
                //users[username].add_online_friends(vf_info);
                
                string send_info = FONLINE + '|' + users[username].username + ":" + users[username].ip + ":" + users[username].port;
                
                cout << "\nsend info login main: " << send_info << endl << endl;
                for (friend_info f_info : vf_info){
                    cout << "f_info.username: " <<  f_info.username << endl;
                    pthread_mutex_lock(&mutex1);
                    write(users[f_info.username].fd, send_info.c_str(), send_info.size()); // good line
                    pthread_mutex_unlock(&mutex1);
                }
            }
             */
            //write(sockfd, users[username].online_friends_toString().c_str(), users[username].online_friends_toString().size());
            
        } else if(service == REGISTER){
            string username;
            string password;
            getline(ss, username, '|');
            getline(ss, password);
            
            cout << "username: " << username << endl;
            cout << "password: " << password << endl;
            
            struct sockaddr_in cli_addr;
            // get the created socket information so we can display it
            len = sizeof(cli_addr);
            if (getsockname(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&len) < 0) {
                perror(": can't get name");
                _exit(1);
            }
            
            // print domain name and port number after bind()
            printf("ip = %s, port = %d\n", inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port));
            //printf("ip = %s, port = %d\n", inet_ntoa(serv_addr.sin_addr), htons(serv_addr.sin_port));
            
            pthread_mutex_lock(&mutex1);
            wb = register_user(username, password, inet_ntoa(cli_addr.sin_addr), to_string(htons(cli_addr.sin_port)));
            
            if(wb == "200"){
                std::ofstream ofs;
                ofs.open(user_info_file, std::ofstream::out | std::ofstream::trunc);
                
                
                for(auto itr = users.begin(); itr != users.end(); ++itr){
                    if(itr->second.username != ""){
                        if(itr->second.online_friends_toFileStr() != "NONE"){
                            ofs << itr->second.username + "|" + itr->second.password + "|" + itr->second.online_friends_toFileStr() << endl;
                            cout << "friends print register: " << itr->second.username + "|" + itr->second.password + "|" + itr->second.online_friends_toFileStr() << endl;
                        }else{
                            ofs << itr->second.username + "|" + itr->second.password + "|" + "" << endl;
                            cout << "friends print register: " << itr->second.username + "|" + itr->second.password + "|" + "" << endl;
                        }
                        
                        cout << "friends print register: " << itr->second.username + "|" + itr->second.password + "|" + itr->second.online_friends_toFileStr() << endl;
                    }
                }
                
                ofs.close();
            }
            
            string snd = R_RESPONSE + "|" + wb;
            
            write(sockfd, snd.c_str(), snd.size());
            pthread_mutex_unlock(&mutex1);
        } else if(service == LOGOUT){
            string username;
            ss >> username;
            
            pthread_mutex_lock(&mutex1);
            logout(users[username]);
            pthread_mutex_unlock(&mutex1);
        } else if(service == INVITATION){
            string from_username;
            string for_username;
            string msg;
            
            getline(ss, from_username, '|');
            getline(ss, for_username, '|');
            getline(ss, msg);
            
            cout << "Invitation for: " << from_username << endl;
            cout << "Invitation from: " << for_username << endl;
            cout << "\tmessage: " << msg << endl;
            string invite = INVITATION + "|" + from_username + "|" + msg;
            
            pthread_mutex_lock(&mutex1);
            write(users[for_username].fd, invite.c_str(), invite.size());
            pthread_mutex_unlock(&mutex1);
        } else if(service == A_INVITATION){
            string from_username;
            string for_username;
            string msg;
            
            getline(ss, from_username, '|');
            getline(ss, for_username, '|');
            getline(ss, msg);
            
            cout << from_username << " Accepted an invitation from: " << for_username << endl;
            //cout << "Accepted Invitation from: " << for_username << endl;
            cout << "\tmessage: " << msg << endl;
            string invite = A_INVITATION + "|" + from_username + "|" + msg;
            
            pthread_mutex_lock(&mutex1);
            write(users[for_username].fd, invite.c_str(), invite.size());
            
            if (users[for_username].online && users[from_username].online){
                
                users[for_username].add_friend(users[from_username].username);
                users[from_username].add_friend(users[for_username].username);
                
                std::ofstream ofs;
                ofs.open(user_info_file, std::ofstream::out | std::ofstream::trunc);
                
                
                for(auto itr = users.begin(); itr != users.end(); ++itr){
                    if(itr->second.username != ""){
                        if(itr->second.online_friends_toFileStr() != "NONE"){
                            ofs << itr->second.username + "|" + itr->second.password + "|" + itr->second.online_friends_toFileStr() << endl;
                            cout << "friends print register: " << itr->second.username + "|" + itr->second.password + "|" + itr->second.online_friends_toFileStr() << endl;
                        }else{
                            ofs << itr->second.username + "|" + itr->second.password + "|" + "" << endl;
                            cout << "friends print register: " << itr->second.username + "|" + itr->second.password + "|" + "" << endl;
                        }
                        
                        cout << "friends print register: " << itr->second.username + "|" + itr->second.password + "|" + itr->second.online_friends_toFileStr() << endl;
                    }
                }
                
                ofs.close();
                
                
                string for_user_info = FONLINE + '|' + users[from_username].username + ":" + users[from_username].ip + ":" + users[from_username].port;
                string from_user_info = FONLINE + '|' + users[for_username].username + ":" + users[for_username].ip + ":" + users[for_username].port;
                
                cout << "\n\nsend info: " << for_user_info << endl;
                cout << "\n\nsend info: " << from_user_info << endl;
                //write(users[friends[i]].fd, FONLINE.c_str(), FONLINE.size());
                write(users[for_username].fd, for_user_info.c_str(), for_user_info.size());
                write(users[from_username].fd, from_user_info.c_str(), from_user_info.size());
            }
            
            pthread_mutex_unlock(&mutex1);
        }
        
        
    }
    if (n == 0) {
        cout << "client closed" << endl;
    } else {
        cout << "something wrong" << endl;
    }
    
    close(sockfd);
    return(NULL);
}

string login(string username, string password, string ip_addr, string port_num, int fd_num){
    
    string retVal = "500";
    
    auto itr = users.find(username);
    
    if(itr->second.password == password){
        retVal = "200";
        
        cout << "ip_addr: " << ip_addr << endl;
        cout << "port_num: " << port_num << endl;
        users[username].set_online(ip_addr, port_num, fd_num);
        ++online_users;
        cout << "Number of Users Online: " << online_users << endl;
        
        vector<friend_info> vf_info = get_online_friends(users[username]);
        users[username].add_online_friends(vf_info);
        
    } else
        retVal = "500";
    
    if (itr != users.end())
        cout << "online? " << (itr->second.online ? "yes" : "no") << endl;
    
    return retVal;
}

void logout(user_info u_info){
    string username = u_info.username;
    
    auto friend_vect = users[username].friends;
    //vector<int> delete_fd;
    
    string send_info = LOGOUT + '|' + u_info.username;
    for (auto i : friend_vect){
        cout << "\n\nsend info logout: " << send_info << endl << endl;
        write(users[i].fd, send_info.c_str(), send_info.size());
        
        //delete_fd.push_back(users[i].fd);
        users[i].remove_online_friend(username);
    }
    
    //for ( int file_d : delete_fd)
        //write(file_d, send_info.c_str(), send_info.size());
    
    online_users--;
    users[username].logout();
}

string register_user(string username, string password, string ip_addr, string port_num){
    
    string retVal = "500";
    
    auto itr = users.find(username);
    
    if(itr != users.end()){     // check if user exists
        retVal = "500";
    } else {    // add new user to the table
        users[username] = user_info(username, password);
        retVal = "200";
    }
    
    if(itr != users.end())
        cout << "online? " << (users[username].online ? "yes" : "no") << endl;
    
    return retVal;
}

vector<friend_info> get_online_friends(user_info u_info){
    
    vector<string> friends = u_info.friends;
    vector<friend_info> user_list;
    for (int i = 0; i < friends.size(); i++){
        cout << "friend online: " << friends[i] << endl;
        if (users[friends[i]].online){
            friend_info tmp(users[friends[i]].username, users[friends[i]].ip, users[friends[i]].port);
            user_list.push_back(tmp);
            
            string send_info = FONLINE + '|' + u_info.username + ":" + u_info.ip + ":" + u_info.port;
            
            cout << "\n\nsend info get_online_friends: " << send_info << endl << endl;
            write(users[friends[i]].fd, send_info.c_str(), send_info.size()); // good line
        }
    }
    
    return user_list;
}

string get_domain_name(){
    
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];
    string host_val;
    
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    
    // Some of this code was obtained from https://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        
        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        
        if(ifa->ifa_addr->sa_family==AF_INET && string(host) != "127.0.0.1")
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                //exit(EXIT_FAILURE);
            }
            //printf("\tInterface : <%s>\n",ifa->ifa_name );
            //printf("\t  Address : %s\n", string(host).c_str());
            
            host_val = host;
        }
    }

    freeifaddrs(ifaddr);
    
    return host_val;
}

 
// parse files passed in as argument
// process args added
// parse files passed in as argument
void process_args(const char * argv[]) {
    // remove print for submission
    //cout << "process args\n";
    
    ifstream user_info_file;
    ifstream config_file;
    string fline;
    
    user_info_file.open(argv[1]);
    config_file.open(argv[2]);
    
    // parse user_info_file
    while(getline(user_info_file, fline)){
        stringstream ss(fline);
        string friend_field;
        
        user_info info;
        
        getline(ss, info.username, '|');
        getline(ss, info.password, '|');
        getline(ss, friend_field);
        info.friends = friend_list(friend_field);
        
        //users.insert(pair<string,user_info>(info.username, info));
        users[info.username] = info;
    }
    
    // parse configuration_file
    for (int i = INITVALUE; i < NUMKEYWORDS; ++i){
        config_file >> fline;
        fline.pop_back();   // remove ":" at the end of keyword
        
        if(fline == PORT)
            config_file >> port;
    }
    
    user_info_file.close();
    config_file.close();
}

vector<string> friend_list(string friends){
    // remove print for submission
    //cout << "friend list\n";
    
    vector<string> flist;
    stringstream ss(friends);
    string line;
    
    while(getline(ss, line, ';'))
        flist.push_back(line);
    
    return flist;
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated.\n", pid);
    return ;
}

