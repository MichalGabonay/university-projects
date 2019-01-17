## ASK modulátor/demodulátor
## ASK modulator/demodulator

University project\
BMS 2018/2019\
xgabon00 - Michal Gabonay 2MIS \
Faculty of Information Technology - Brno University of Technology \
points: 4/7

---

Dva konzolové programy (bms1A - modulátor, bms1B - demodulátor) v jazyku C++, ktoré modulujú vstupné resp. demodemodulujú výstupnú postupnost bitov za pomoci štvorstavového ASK.

Pri implementácii bola použitá knižnica libsndfile na jednoduchú prácu s WAV soubormi.

Parametry modulácie:
* Nosná frekvencia: 1000 Hz.
* Prenosová rychlost minimálne: 1000 bit/s.
* Na začiatek prenášaných dát je synchronizačná sekvencia pre demodulátor: 00110011.

Parametry demodulácie:

* ASK s nosnou frekvenciou 1000 Hz.
* Baudovou/prenosová rychlost na základe synchronizačnej sekvencie vstupného signálu.
* Výstup demodulátoru tvorí opät textový soubor obsahující znaky 0 a 1 bez synchronizačnej sekvencie.

