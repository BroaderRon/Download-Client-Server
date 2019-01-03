--------------------------------------------------------
Authors: Aaron Raff and Ronald Markovich
Major: Computer Science
Creation Date: 11/17/2018
Due Date: 12/08/2018
Course: CSC 328 020
Professor Name: Dr. Frye
Assignment: Download Server
Files: DLClient.c server.c README.txt
------------------------------------------------------


---Credits---
Referenced code from Dr. Frye’s printDir.c.
Used to learn about stat structure and how to navigate directories.
Also referenced the code to check if a stat structure is a file or directory (bitwise AND).


---Description---
Download client and server where the client can navigate through the file system and download files of their choosing. We utilized TCP sockets to connect the two hosts and send messages back and forth between the two. The server is implemented using threads.


---Compiling---
To compile the server run:
        gcc server.c -o server -lpthread


To compile the client run:
        gcc DLClient.c -o client


---How to Run the Programs---
To run the server:
        ./server <port number>
* The port number specifies which port the server should run on


To run the client:
        ./client hostname <port number>
* The hostname specifies the host that the client should connect to
* The port number specifies which port the client should connect to


Note: The default port for the client and server is 62123.


---Information About the Protocol---
* Connecting/Disconnecting
   * Client attempts to connect to server and waits for HELLO message. Server sends a hello message to acknowledge that the client has connected successfully.
   * Client sends a BYE message to the server to inform the server that it is disconnecting.


* Command Usage
   * PWD- enter pwd to get the current directories absolute path
   * DIR- enter dir to list the current directory
   * CD- enter cd and on the next line enter the directory
   * DOWNLOAD- enter download and on the next line enter the file
   * BYE- enter bye to disconnect from the server


* Message passing protocol
   * The first three characters of the message indicate the size of the rest of the incoming message.
   * If the first three characters are “-01” that indicates an error, therefore the client will then need to receive another message that contains the error message (prepended with the three characters representing the message size).
   * For the download command, we send the file in chunks of 1024 bytes. Before we send the actual file, the server sends the client the size of the file to be downloaded (prepended with the three characters representing the message size).


---Known Issues---
Due to the use of threads on the server, two clients will end up changing each others directories since the server does not keep track of each client’s current state.


---Time Estimates---
Server
Estimated Time
Program Design - 1 hour
Program Implementation - 3 hours
Debugging - 1 hour
Test Program - 1 hour


Actual Time
Program Design - 1 hour
Program Implementation - 5 hours
Debugging - 4 hours
Test Program - 2.5 hours




Client
Estimated Time        
Design – 2hr 
Implementation- 2hr
Debugging- 30min
Test program – 30min




Actual Time
Program Design - 1 hour
Program Implementation - 5 hours
Debugging - 4 hours
Test Program - 2.5 hours