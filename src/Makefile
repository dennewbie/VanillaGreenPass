all: clean clientCitizen clientS clientT centroVaccinale serverV serverG
	
clientCitizen: clientCitizen.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc -Wall -Werror -g -std=c99 clientCitizen.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientCitizen
	
clientCitizen.o: clientCitizen.c clientCitizen.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 clientCitizen.c
	
	
clientS: clientS.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc -Wall -Werror -g -std=c99 clientS.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientS
	
clientS.o: clientS.c clientS.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 clientS.c
	
	
clientT: clientT.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc -Wall -Werror -g -std=c99 clientT.o GreenPassUtility.o NetWrapper.o UsageUtility.o -o clientT
	
clientT.o: clientT.c clientT.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 clientT.c
	
	
centroVaccinale: centroVaccinale.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc -Wall -Werror -g -std=c99 centroVaccinale.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o centroVaccinale

centroVaccinale.o: centroVaccinale.c centroVaccinale.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 centroVaccinale.c


serverV: serverV.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc -Wall -Werror -g -std=c99 serverV.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o serverV -pthread

serverV.o: serverV.c serverV.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 serverV.c -pthread
	
	
serverG: serverG.o GreenPassUtility.o NetWrapper.o UsageUtility.o
	gcc -Wall -Werror -g -std=c99 serverG.o GreenPassUtility.o NetWrapper.o UsageUtility.o  -o serverG

serverG.o: serverG.c serverG.h GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 serverG.c
	
	

UsageUtility.o: UsageUtility.c UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 UsageUtility.c -pthread
	
NetWrapper.o: NetWrapper.c UsageUtility.c NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 NetWrapper.c UsageUtility.c -pthread
	
GreenPassUtility.o: GreenPassUtility.c NetWrapper.c UsageUtility.c GreenPassUtility.h NetWrapper.h UsageUtility.h
	gcc -c -Wall -Werror -g -std=c99 GreenPassUtility.c NetWrapper.c UsageUtility.c -pthread
	
clean:
	rm -f *.o all
	rm -f serverV
	rm -f clientT
	rm -f clientS
	rm -f clientCitizen
	rm -f centroVaccinale
	rm -f serverG
