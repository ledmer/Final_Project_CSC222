CC=g++

hello:main.cpp
	$(CC) main.cpp -o shotgunguy -lsfml-graphics -lsfml-window -lsfml-system