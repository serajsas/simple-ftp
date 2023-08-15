# Use a base image (e.g., a Linux distribution) from Docker Hub
FROM ubuntu:latest

# Set the working directory inside the container
WORKDIR /app

# Copy your C program and any source files to the container
COPY . .

# Install necessary dependencies (e.g., a C compiler)
RUN apt-get update && \
    apt-get install -y gcc make

# Compile the C program
RUN make

EXPOSE 3000

# Specify the command to run when the container starts
CMD ["./main", "3000"]
