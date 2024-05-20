# Specify the target executable and the source files needed to build it
symmnf: symmnf.o 
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -o symmnf symmnf.o symmnf.h -lm
# Specify the object files that are generated from the corresponding source files
symmnf.o: symmnf.c
	gcc -c symmnf.c