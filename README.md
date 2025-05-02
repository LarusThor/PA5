# PA5

Basic Networked Chat Server

**Short Description of the Project:**

For this project, we will create a basic networked chat server in C++. The program will allow multiple clients to connect to a server using TCP sockets. Once connected, each client can send plain text messages, which the server will relay to all other connected clients.

The focus will be on building a working, simple system using real-world networking tools and learning how to use threads in C++ for handling multiple clients. we will use the asio C++ library (a header-only networking library) to handle socket communication.

This project is intended to help me learn how networking and concurrency work in C++ in a hands-on way, while keeping the codebase simple and understandable as a beginner.

**Items we Plan to Fulfill (and Estimated Points):**
Use networking with proper concurrency management — 40 points
How it's fulfilled: we will use TCP sockets to allow multiple clients to connect to a server and send messages.

Tool: asio library (standalone, no Boost required).

Estimated worth: 40 points

Use threads with basic concurrency handling — 40 points
How it's fulfilled: The server will use threads to handle each client connection. A shared list of clients will be protected with a simple mutex.

Estimated worth: 40 points

Make program take simple decisions (if/else) — 20 points
How it's fulfilled: The server will handle basic commands such as /exit for a client to disconnect.

Estimated worth: 20 points

**Expected Challenges:**
Learning how to use sockets and threads in C++ for the first time.

Making sure shared data (like a list of connected clients) is accessed safely.

Handling client disconnects without crashing the server.

**Interesting Things we Expect to Learn:**
Basics of TCP networking in C++

- How to use threads and mutexes in C++

- How to use a third-party library (asio)

- How to manage input/output between multiple users in a networked program

**Functionality we Want to Avoid Implementing Myself:**
we will use the asio library for:

Creating and managing TCP sockets

Sending and receiving data between the server and clients

This helps me avoid manually writing low-level socket code (like using socket(), bind(), etc.), which can be more error-prone and complex.

**Estimated Points (Self-Evaluation):**
Minimum Viable Product (MVP):

One server

Multiple clients can connect

Messages sent by one client are broadcast to all others

Clients can type /exit to leave
➤ Estimated: 60 points

**Feature-Complete Version:**

Add /list command to show connected users

Handle graceful disconnections
➤ Estimated: 75 points

**Dream Version:**

Private messages (/whisper)

Nicknames for users

Basic logging of chat messages to a file
➤ Estimated: 90–100 points

