# Concurrent Server and Multiplexed Client Messenger

## Server/CLient Set-up

* Server: Concurrent using pthreads()
* Client: Multiplexed using Select()

## Installation Instructions
To install and run either the server or client, go into the directory (server or client) before issuing the following commands.
### How to compile the server:

#### Once in the server directory, just issue the make command to run
```
	$ make
```
#### To use the server run it with the following usage:
```
	$ ./server.x user_info_file configration_file
```
### How to compile the client:

#### Once in the client directory, just issue the make command to run
```
	$ make
```
#### To use the client, run it with the following usage:
```
	$ ./client.x user_info_file configration_file
```

## Program Usage
### Server
```
usage: messenger_server user_info_file configration_file
```
### Client
```
usage: messenger_client configuration_file
```

## Server Files Explained
The messenger server program takes two parameters user_info_file and configuration_file, which are both file names. The file user_info_file contains the user account information. Each user account occupies one line. Each line contains three pieces of user information: username, password, and contact list. They are separated by a vertical bar "|". Contact list may contain multiple friends (i.e., their usernames), which are separated by a semicolon ";". Passwords are in plain text. The following is an example line for user "user1", which has three friends in the contact list.

user1|password1|user2;user5;user6

The parameter configuration_file is a configuration file for the server program. Each configuration entry occupies one line with the following format: keyword: value. Currently only one keyword port is defined. The value of port is a nonnegative integer, which specifies the port number on which the server program should run. When the specified port number is 0, the server asks the system to assign any unused port number for the server.  

After the server program starts, it will first read in the user account information and the configuration information. Then it will create a socket using the socket() system call. It binds its address to the socket using the bind() system call. The server machine's fully-qualified domain name and the (assigned) port number are displayed on the screen. Then listen() call is used to indicate that the server is now ready to receive connect requests from clients.

The server's job is to wait for either connection or disconnection requests from clients. If it receives a connect request, it accepts the connection.  After the connection has been established, the client will either register with the server, or log into the server by sending the user account information and location information to the server (see below for details) if the user has registered before. When a user tries to register with the server system, the user will send a selected username and password to the server. The server will check if the username is available. If it is available, the user has successfully registered with the server system. Otherwise, the server will inform the user that the selected username has been occupied.

When a registered user tries to log into the server system, the server will first check if the username and password supplied by the client match the ones maintained by the server. If they match, the user has successfully logged into the server, and the server will display a message showing the total number of users online. If the username and password do not match, the server will inform the user that the username and password do not match and ask the user to send the username and password again. After a user (say, user1) successfully logs into the server, user1 will also send the location information to the server. The server will record the location information. Then the server will send the location information of user1's friends to user1. In addition, the server will also send the location information of user1 to user1's friends who have logged in (i.e., who are online).

If the server receives a disconnect request from user1, it closes the connection, deletes the location information, and informs the friends of user1 that user1 is offline. In addition, the total number of users online should also be updated.

## Client Files Explained
The configuration_file has the same format as the configuration file of the server program. The client configuration file defines two keywords: servhost and servport, they specify the messenger server's hostname (or dotted-decimal IP address) and port number, respectively. The client first creates a socket using the socket() system call. It then connects to the server using the connect() system call. The whereabouts of the server are specified in the configuration file as discussed above.

Once the connection is established, the user can perform three operations: 1) register with the server using command "r", 2) log into the server using command "l", or 3) exit the client program using command "exit". When the user type either command r or l, the client program will ask the user to enter the username and password. The username and password will be sent to the server via the connection. The server will return status code "200" if the username is available (for registration) or the username and password match (for login). Otherwise, the server will return status code "500" and the client will inform the user the error  and ask the user to enter the username and password again. This process repeats until it succeeds. After logging to the server successfully,  the client will then create another socket using the socket() system. It will then bind the socket to local port number "5100" and call listen() to indicate that it is now ready to receive connect requests from other clients for chat. The client's job is to wait for user input from the keyboard, chat request from other clients, messages from other clients on established connection, or location information of friends from the server.

## Program Functionality (after successful login)
After a user successfully log into the server system, the user can issue a few commands to perform the desired operations.

chat: a user uses the command "m" to send a message to another friend with the following format: m friend_username whatever_message. If a connection has not been established between the user and friend_username, the client program should first establish a TCP connection to the client program of friend_username (based on the location information sent from the server).

invitation: a user can invite a friend by issuing the command "i" with the following format: i potential_friend_username [whatever_message]. The invitation message is sent to the server, which then forward the invitation to the invited user.

accept invitation: after seeing an invitation message, the user can use the command "ia" to accept the invitation: ia inviter_username [whatever_message]. The invitation acceptance message is sent to the server, which in turn forwards the message to the initial inviter. The server will also update the friend list of both users.

log out: a user can log out from the server by issuing the following command: "logout". After the user logs out from the server, the user will have the same interface as when the client program first starts. That is, the user can issue three commands "r" to register, "l" to login again, or "exit" to quit the client program.

When user1 receives chat request from a friend, it will accept the connection request. When user1 receives a message from friend on an established connection, it will display the message in the following format:  friend_username >> whatever_message

When user1 receives location information from the messenger server, it will record this information locally and display all the online friends (including the new one) on the terminal.

## Author
[Raidel Hernandez](https://github.com/raidel123)
