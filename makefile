# Specify the target executable and the source files needed to build it
symnmf: symnmf.o 
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -o symnmf symnmf.o symnmf.h -lm
# Specify the object files that are generated from the corresponding source files
symnmf.o: symnmf.c
	gcc -c symnmf.c