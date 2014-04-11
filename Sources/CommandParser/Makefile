CC=clang
CCFLAGS= -g -Wall
RAGELFLAGS=
IMGTYPE=png
DOTFLAGS=

all: test_parse_assignment test_netprot_process_command parse_assignment.$(IMGTYPE)

test_netprot_process_command: test_netprot_process_command.c netprot_command.o netprot_commands_setget.o netprot_commands_misc.o parse_assignment.o
	$(CC) $(CCFLAGS) test_netprot_process_command.c netprot_command.o netprot_commands_setget.o netprot_commands_misc.o parse_assignment.o -o test_netprot_process_command

netprot_command.o: netprot_command.c netprot_command.h netprot_command_list.h netprot_commands_misc.o netprot_commands_setget.o
	$(CC) $(CCFLAGS) -c netprot_command.c netprot_commands_misc.o netprot_commands_setget.o

netprot_commands_misc.o:netprot_commands_misc.c
	$(CC) $(CCFLAGS) -c netprot_commands_misc.c

netprot_commands_setget.o: netprot_commands_setget.c netprot_setget_params.h netprot_setget_params_list.h
	$(CC) $(CCFLAGS) -c netprot_commands_setget.c 

test_parse_assignment: parse_assignment.o test_parse_assignment.c 
	$(CC) $(CCFLAGS) test_parse_assignment.c parse_assignment.o -o test_parse_assignment

parse_assignment.o: parse_assignment.c parse_assignment.h
	$(CC) $(CCFLAGS) -c parse_assignment.c -o parse_assignment.o

parse_assignment.c: parse_assignment.rl
	ragel $(RAGELFLAGS) -C parse_assignment.rl

parse_assignment.$(IMGTYPE): parse_assignment.rl
	ragel -V -p parse_assignment.rl | dot $(DOTFLAGS) -T$(IMGTYPE) -o parse_assignment.$(IMGTYPE)

clean:
	rm -rf parse_assignment.c parse_assignment.png *.o *.dSYM