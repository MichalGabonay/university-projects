## Program for searching informations about users in UNIX type OS
## Program pre vyhľadávanie informácii o užívateľoch Unixového OS

University project\
IPK 2015\
xgabon00 - Michal Gabonay 2BIT \
Faculty of Information Technology - Brno University of Technology \
points: 15/15

### 1. Zadanie
Úlohou projektu bolo vytvoriť protokol pre komunikáciu medzi klientom a serverom.
### 2. Implementácia
Klient zasiela serveru správu, v ktorej píše či chce vyhľadať informácie od užívateľa podľa loginu (argument -l) alebo podľa užívateľského ID (argument -u). Môže vyhľadávať aj viac užívateľov naraz, ale iba podľa rovnakého kritéria. Ďalej klient posiela údaje o tom, aké informácie chce o užívateľovi získať (-LUGNHS). K vyhľadávaniu bola použitá knižnica pwd.h, a funkciami getpwuid a getpwnam sa jednoducho v /etc/passwd pracuje. Program nepodporuje duplicitný výpis informácii. V prípade, že nie je zadaný žiadny prepínač špecifickej požiadavky (-LUGNHS), berie sa to tak, že klient nič nežiada, nič nedostane.
### 3. Príklad použitia
Spustí sa server s povinným parametrom portu a následne klient s rovnakým číslom portu a ďalšími informáciami.

```bash
./server -p 12345
./client -h eva.fit.vutbr.cz -p 12345 -l xgabon00 xkocou08 xkacma03 xlogin00 -NSHG

Gabonay Michal,FIT BIT 2r /bin/ksh /homes/eva/xg/xgabon00 251
Kocour Martin,FIT BIT 2r /bin/ksh /homes/eva/xk/xkocou08 251
Kacmarcik Martin,FIT BIT 2r /bin/ksh /homes/eva/xk/xkacma03 251
Loginova Natalya,FIT BIT 2r /bin/ksh /homes/eva/xl/xlogin00 251
```