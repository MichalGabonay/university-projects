## Solving rubic cube with Prolog
## Riešenie rubíkovej kocky pomocou Prologu

University project\
FLP 2017/2018\
xgabon00 - Michal Gabonay 1MIS \
Faculty of Information Technology - Brno University of Technology \
points: 8/8

---
Preklad: 	make \
Spustenie:	./flp18-log < [vstup] \
[vstup] je povinný parameter, ktorý reprezentuje názov súboru, ktorý obsahuje vstupú, textovú reprezentáciu rubíkovej kocky.

---
Program rieši rubíkovú kocku o veľkosi 3x3, taktiež vypisuje postup riešenia krok po kroku. \
Program očakáva validný vstup rubíkovej kocky, reprezentujúci v textovej forme. \
Na spracovanie vstupu bol využitý ukážkový súbor input2.pl zo štúdijných materiálov do predmetu FLP.  \
Princíp spočíva v otáčaní jednej zo stránk kocky o 90° v smere hodinových ručiriek,  \
a pomocou backtrackingu otáčať až kým kocka nieje vyriešená.
Program niekedy nevyrieši kocku optymálnym spôsobom (kroky naviac), kvôli tomu, že otáča vždy len v smere hodinových ručičiek.