# Simple FTP Server using Docker

This repository contains a simple FTP server setup that can be built and run using Docker. The provided Makefile helps streamline the Docker image building and container running process.

## Getting Started

### Prerequisites

Make sure you have Docker installed on your system. If not, you can download and install Docker from [here](https://www.docker.com/products/docker-desktop).

### Building the Docker Image

1. Open a terminal window.
2. Run chmod +x script.sh
3. Run './script.sh'

Now you will have the ftp server build and run inside the docker container. 

### Accessing the FTP Server
You can now connect to the FTP server using an FTP client of your choice. Use the IP address or hostname of the machine where the Docker container is running, along with the port number 3000, to connect.

Supported FTP Commands: "USER", "PASV", "NLST", "RETR", "MODE", "STRU", "TYPE", "CDUP", "CWD", "QUIT".