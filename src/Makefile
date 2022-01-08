all: clientCitizen centroVaccinale serverV
	
clientCitizen: clientCitizen.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc clientCitizen.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientCitizen
	
clientCitizen.o: clientCitizen.c clientCitizen.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c clientCitizen.c
	
	
centroVaccinale: centroVaccinale.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc centroVaccinale.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o centroVaccinale
	
centroVaccinale.o: centroVaccinale.c centroVaccinale.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c centroVaccinale.c
	
	
serverV: serverV.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc serverV.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o serverV

serverV.o: serverV serverV.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c serverV.c
	
	
	
	

UsageUtility.o: UsageUtility.c UsageUtility.h
	gcc -c UsageUtility.c
	
NetWrapper.o: NetWrapper.c UsageUtility.c NetWrapper.h UsageUtility.h
	gcc -c NetWrapper.c UsageUtility.c
	
GreenPassUtility.o: GreenPassUtility.c NetWrapper.c UsageUtility.c GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c GreenPassUtility.c NetWrapper.c UsageUtility.c
	
clean:
	rm *.o all
