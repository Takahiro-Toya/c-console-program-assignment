OBJ=main.o helpers.o robot.o rubbish.o general.o

#Fix the directories to march your file organisation.
CC_FLAGS=-std=gnu99 -I../ZDK -Wall -Werror -g
LD_FLAGS=-L../ZDK -lzdk -lncurses -lm

main: $(OBJ)
	gcc $(OBJ) $(LD_FLAGS) -o $@
%.o : %.c
	gcc -c $< $(CC_FLAGS)