all: clientCitizen
	
clientCitizen: clientCitizen.o NetWrapper.o UsageUtility.o GreenPassUtility.o
	gcc clientCitizen.o NetWrapper.o UsageUtility.o GreenPassUtility.o -o clientCitizen
	
clientCitizen.o: clientCitizen.c NetWrapper.h UsageUtility.h GreenPassUtility.h clientCitizen.h
	gcc -c clientCitizen.c





UsageUtility.o: UsageUtility.c UsageUtility.h
	gcc -c UsageUtility.c
	
NetWrapper.o: NetWrapper.c UsageUtility.c NetWrapper.h UsageUtility.h
	gcc -c NetWrapper.c UsageUtility.c
	
GreenPassUtility.o: GreenPassUtility.c NetWrapper.c UsageUtility.c GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c GreenPassUtility.c NetWrapper.c UsageUtility.c
	
clean:
	rm *.o all
