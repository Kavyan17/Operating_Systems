computer.exe: computer.o cpu.o memory.o load.o shell.o scheduler.o print.o printer.o
	gcc computer.o cpu.o memory.o load.o shell.o scheduler.o print.o printer.o -o computer.exe

computer.o: computer.c
	gcc -c computer.c

memory.o: memory.c
	gcc -c memory.c

shell.o: shell.c
	gcc -c shell.c

load.o: load.c
	gcc -c load.c

print.o: print.c
	gcc -c print.c

printer.o: printer.c
	gcc -c printer.c

scheduler.o: scheduler.c
	gcc -c scheduler.c

cpu.o: cpu.c
	gcc -c cpu.c
