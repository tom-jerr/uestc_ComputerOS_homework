SHELL = cmd.exe
OBJS = test.o proc.o

test: $(OBJS)
	g++ -o test $(OBJS)
test.o: test.c list.h proc.h 
	g++ -c test.c
proc.o: proc.c proc.h list.h
	g++ -c proc.c
clean:
	del *.o *.exe