all: clean clientCitizen clientS clientT centroVaccinale serverV serverG
	
clientCitizen: clientCitizen.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc clientCitizen.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientCitizen
	
clientCitizen.o: clientCitizen.c clientCitizen.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c clientCitizen.c
	
	
clientS: clientS.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc clientS.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientS
	
clientS.o: clientS.c clientS.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c clientS.c
	
	
clientT: clientT.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc clientT.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientT
	
clientT.o: clientT.c clientT.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c clientT.c
	
	
centroVaccinale: centroVaccinale.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc centroVaccinale.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o centroVaccinale

centroVaccinale.o: centroVaccinale.c centroVaccinale.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c centroVaccinale.c


serverV: serverV.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc serverV.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o serverV -pthread

serverV.o: serverV.c serverV.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c serverV.c -pthread
	
	
serverG: serverG.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc serverG.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o serverG

serverG.o: serverG.c serverG.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c serverG.c
	
	

UsageUtility.o: UsageUtility.c UsageUtility.h
	gcc -c UsageUtility.c -pthread
	
NetWrapper.o: NetWrapper.c UsageUtility.c NetWrapper.h UsageUtility.h
	gcc -c NetWrapper.c UsageUtility.c -pthread
	
GreenPassUtility.o: GreenPassUtility.c NetWrapper.c UsageUtility.c GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c GreenPassUtility.c NetWrapper.c UsageUtility.c -pthread
	
clean:
	rm -f *.o all
	rm -f serverV
	rm -f clientT
	rm -f clientS
	rm -f clientCitizen
	rm -f centroVaccinale
	rm -f serverG
