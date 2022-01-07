all: main

main: main.o NetWrapper.o UsageUtility.o
	gcc main.o NetWrapper.o UsageUtility.o -o main
	
main.o: main.c NetWrapper.h UsageUtility.h
	gcc -c main.c
	
NetWrapper.o: NetWrapper.c UsageUtility.c NetWrapper.h UsageUtility.h
	gcc -c NetWrapper.c UsageUtility.c
	
UsageUtility.o: UsageUtility.c UsageUtility.h
	gcc -c UsageUtility.c
	
clean:
	rm *.o all
