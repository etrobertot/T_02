queue: sistema_de_colas.o lcgrand.o 
	g++ sistema_de_colas.o lcgrand.o -o queue

lcgrand.o: lcgrand.cpp
	g++ -c lcgrand.cpp

sistema_de_colas.o: sistema_de_colas.cpp
	g++ -c sistema_de_colas.cpp

clean:
	rm *.o queue