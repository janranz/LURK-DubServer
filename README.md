# LURK-DubServer
Private Repo for DubServer

---
## Plan of action
\[Main\]
* Compile and build objects containing text file strings
    * error messages
    * game messages
    * random messages
    * baddie messages
* Establish initial connection / port
* Loop to accept new clients, assign a socket file descriptor
* Assign threads

\[Globally Accessible Data\]
* We must allow clients on their respective threads to communicate with one another.
    * Private Messages
    * Group Fights (JOIN_BATTLE)
    * Looting
    * Visually display who is in a room
* Lock only when writing to it.
\[Thread\]
* Determine if multithreading *receive* and *send* is neccessary (probably not)
* Intention is to thread each client with their own *receive* and *send* functionality. Consider if this is a performance flaw. Consider if mutex'ing commonly accessible functions is more appropriate.

\[Receive\]
* Standard LURK protocol. Read one byte, and sort.
* Consider slow-write clients, but if they are on their own thread, it shouldn't affect others. Maybe we can time them out, but we must be careful not to get off on data received.
* Lock only when writing to it.

\[send\]

* If the client-per-thread concept checks out, we will *usually* only send in *response* to some *receive* message. A blocking *receive* action should not hinder the graceful connection to the client as in most cases we are *reacting* rather than *acting*.
* Do plan for random events/messages. Assume the client will acknowledge such messages.

\[Messages\]
* Parse .dat files

\[Rooms\]
* Portal Room connects to all rooms. This is room 0
* Randomly generated names and descriptions.
* Need to find some way to connect said rooms.
* Mystery rooms with some way to return to previous room.

\[Fight Logic\]

\[Handle cases\]

---

Scratch pad...

Have a while loop that accepts connections.

Create one class object inside of main, and pass it by reference to each thread (for global access). Mutex lock the *writes*, don't worry about the reads.

Properly handle client disconnects and gracefully close associated threads when done.

main function

{

    Server initialization
    initialize data structures.
    Create a class to handle the interconnection between clients.
    Pass this by reference.
    <-start loop on some condition related to lifetime of serv->
    listen for connections
    add client file descriptor to a global list most likely a pointer.
    instantiate threads.
    Join threads.
    <-end loop->
    clean up/free any allocated mems.


}

---

##TO-FIX:

If CLIENT Ctrl + C's from inital accept, it will create an infinite loop (severe)
