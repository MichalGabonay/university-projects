## Simulácia križovatky
## Simulation of crossroad

University project\
SIN 2018/2019\
xgabon00 - Michal Gabonay 1MIS \
Faculty of Information Technology - Brno University of Technology \
points: 13/15

---
### Príklad užitia:
Preklad pomocou príkazu make.\
Spustenie pomocou ./bkg-2-cnf [volby] [vstup]\
Vstup je nepovinný argument obsahujúci názov súboru s BKG. Ak neni zadaný, vstup sa berie so štandartného vstupu.\
Volby sú parametre, které ovlivňujú chovanie programu a môžu byť nasledujúce:\
&nbsp;&nbsp;&nbsp;-i: Načíta a skontroluje gramatiku a následne je vypíše.\
&nbsp;&nbsp;&nbsp;-1: Gramatiku na vstupu prevedie na gramatiku bez jednoduchých pravidel a vypíše ju.\
&nbsp;&nbsp;&nbsp;-2: Vstupnú gramatiku prevedie na gramatiku v Chomského normálnej forme a vypíše ju.

---
## Zadanie:
Dopravní telematika. Vytvořte model křižovatky s okolím a realizujte adaptivní řízení křižovatky nebo více křižovatek v romci dopravní sítě. Místo řízení křižovatky lze zvolit i jiný podsystém dopravní telematiky. Použijte RENEW (http://www.renew.de), JADE, DEVS nebo jiný simulační framework pro realizaci inteligentního řízení. Použijte SUMO (http://sumo.dlr.de/wiki/) nebo podobný nástroj pro simulaci dopravy a pro vizualizaci. Realizujte i jednoduchý SCADA systém pro monitorování a řízení.