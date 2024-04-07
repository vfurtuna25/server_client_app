# server_client_app
This project implements a basic web proxy server written in C. 

Features:
- Listens for client connections on a specified port;
- Forks a process to handle each client request;
- Accepts commands from clients;
- Fetches webpages based on specific commands;
- Relays the fetched webpage content back to the client;
- Saves the fetched webpage content to a file.

Getting Started:
1. Clone this repository.
2. Compile the code using a C compiler (e.g., gcc).
3. Run the server executable, specifying the port number (default: 22111).
