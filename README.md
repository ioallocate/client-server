# Client-Server Telemetry App 
##Overview
This project is a C++ client-server application designed as a learning base for networking, multithreading, and GUI integration. The client collects basic telemetry data from the user’s system and sends it to the server via sockets. The server handles multiple clients efficiently using a thread pool and provides a graphical interface for monitoring.

## Features: 


### Client

Collects system information: OS, installed RAM, installed software.
Constructs telemetry data into JSON format.
Sends data to the server using socket communication.
Implements a connection guard that retries up to 3 times if the connection is lost.



### Server

Handles up to 1000 clients using a thread pool.
Displays connection status and server health via an ImGui-based GUI.
Allows dumping and viewing detailed client information.




## Architecture

Client-Server Model using TCP sockets.
Multithreaded Server with a thread pool for scalability.
JSON Serialization for structured data exchange.
ImGui GUI for real-time monitoring.


## Dependencies

nlohmann/json for JSON handling.
ImGui for GUI rendering.
