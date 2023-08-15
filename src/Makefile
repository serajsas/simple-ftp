
#This is a hack to pass arguments to the run command and probably only 
#works with gnu make. 
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif


all: main

#The following lines contain the generic build options
CC=gcc
CPPFLAGS=
CFLAGS=-g -Werror-implicit-function-declaration
CLIBS = -pthread

#List all the .o files here that need to be linked 
OBJS=main.o usage.o dir.o socket.o command_handler.o port.o

command_handler.o: command_handler.c command_handler.h

socket.o: socket.c socket.h

usage.o: usage.c usage.h

dir.o: dir.c dir.h

port.o: port.c port.h

main.o: main.c dir.h usage.h port.h

main: $(OBJS) 
	$(CC) -o main $(OBJS) $(CLIBS) 

clean:
	rm -f *.o
	rm -f main

.PHONY: run
run: main  
	./main $(RUN_ARGS)

.PHONY: create_docker
create_docker: 
	docker build -t my-ftp-server -f ../Dockerfile ..

.PHONY: run_docker
run_docker: 
	docker run -d -p 5555-5580:5555:5580 -p 3000:3000 my-ftp-server
