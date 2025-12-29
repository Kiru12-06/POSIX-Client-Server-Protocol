# POSIX-Client-Server-Protocol

## Overview
This repository implements a custom POSIX-based client–server communication system consisting of two protocol components: a transmitting (client) side and a 
receiving (server) side. The client and server are designed to run on separate Raspberry Pi devices, enabling message transmission from the client to the 
server over a network connection.

## Architecture

Each protocol is organized into layers and is designed to run across two
Raspberry Pi devices in a client–server configuration.

### Client Raspberry Pi

- **Pipeline**
  - `pipeline.py`  
    Orchestrates execution of each layer sequentially and performs local network
    discovery, allowing the user to select the server’s IP address before
    launching the client components.

- **Layer 1 – Data Generation**
  - `chargen.h`
  - `chargen_source.cpp`
  - `chargen_main.cpp`  
    Reads user input word by word and prepares message packets for transmission.

- **Layer 2 – Encryption**
  - `encrypt.h`
  - `encrypt_source.cpp`
  - `encrypt_main.cpp`  
    Applies encryption logic to each character of the message packet.

- **Layer 3 – Network Transmission**
  - `connect.h`
  - `connect_source.cpp`
  - `connect_main.cpp`  
    Implements POSIX-based socket communication and transmits data to the server.



### Server Raspberry Pi

- **Layer 1 – Transmission Reception**
  - `rec_connect.h`
  - `rec_connect.cpp`
  - `rec_connect_main.cpp`  
    Listens for incoming connections, receives transmitted message packets,
    applies receiver-side protocol logic (including decryption), and displays
    the received message.


## How It Works

1. The client-side data generation layer runs continuously and listens for user input. When a new message is entered, the pipeline is triggered.
2. The pipeline scans the local network, presents a list of available IP addresses, and prompts the user to select a target server. If the server is not detected, an IP address can be entered manually.
3. Once a target is selected, the pipeline automatically executes the remaining client-side layers, applying encryption and initiating network transmission without further user intervention.
4. On the server side, the receiver listens for incoming connections and maintains the established connection with the client.
5. Upon receiving the initial message specifying the length of the incoming packet, the server begins receiving the transmitted data.
6. After the full message packet has been received and decrypted, the server outputs the decrypted message to the terminal.
