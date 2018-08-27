## DHCP starvation attack and DHCP rogue server 

University project\
PDS 2018\
xgabon00 - Michal Gabonay 2BIT \
Faculty of Information Technology - Brno University of Technology \
points: 17/25

Projekt do predmetu PDS - Přenos dat, počítačové sítě a protokoly.\
Cielom projektu bolo naštudovať a implementovať DHCP útoky, a to starvation a rogue server.

Build pomocou príkazu make.

Spustenie:
DHCP Starvation útok sa spúšťa pomocou príkazu: \
	./pds-dhcpstarve –i interface

	–i interface znamená meno rozhrania podľa OS, na ktoré útočník vygeneruje príslušnú prevádzku s kompromitujúcimi účinkami na DHCP server.

DHCP Rogue server sa spúšťa pomocou príkazu: \
	./pds-dhcprogue -i interface -p pool -g gateway -n dns-server -d domain -l lease-time 

	-i interface znamená meno rozhrania podľa OS, na ktoré útočník vygeneruje príslušnú prevádzku s kompromitujúcimi účinkami na DHCP server;
	-p pool znamená pool adries reprezentovaný vo formáte <prvá_IPv4_adresa>-<posledná_IPv4_adresa> (napr. 192.168.1.100-192.168.1.199);
	-g gateway znamená IPv4 adresa sieťovej brány pre segment, v ktorom sa nachádza obeť;
	-n dns-server znamená IPv4 adresa DNS serveru;
	-d domain znamená meno domény, v ktorej sa zariadenie nachádza;
	-l lease-time znamená počet sekúnd reprezentujúci dobu DHCP pôžičky;

Obmedzenia:\
DHCP starvation útok je plne funkčný.\
DHCP rogue server nie je úplne dokončený, z dôvodu nedostatku času (viem ako dané nedostatky vyriešiť), napriek tomu je schopný demonštrovať princíp útoku.\
* ráta s validným vstupom
* odosiela OFFER ako odpoveď na DISCOVER a odosiela ACK ako odpoveď na REQUEST, ale nekontroluje validitu správy REQUEST (vždy pošle ACK)
* reaugeje len na správy DISCOVER a REQUEST
* nepodporuje renew adries
* na strojoch pracujúcich s little endian sa neposiela správne domain name
* nepriraduje adresu so svojho DHCP poolu, vždy pevne priraduje 1. adresu z rozsahu
* server identifikátor je natvrdo daný (nie pravý zistený)
* kód je škaredo napísaný a chýbajú komenty

viac informácií v dokumente dokumentace.pdf