# Concurrent Server and Multiplexed Client Messenger

## Server/CLient Set-up

* Server: Concurrent using pthreads()
* Client: Multiplexed using Select()

## Installation Instructions

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

## Author
[Raidel Hernandez](https://github.com/raidel123)
