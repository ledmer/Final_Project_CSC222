CC=g++
# Hwy!, I am comment no.2. I want to say that CFLAGS will be the
#options I'll pass to the compiler

vamp:hello

hello:main.cpp
	$(CC) main.cpp -o vamp -lsfml-graphics -lsfml-window -lsfml-system