## Transform context free grammare to CNF form in Haskell
## Prevod bezkontextovej gramatiky do CNF formy v Haskell

University project\
FLP 2017/2018\
xgabon00 - Michal Gabonay 1MIS \
Faculty of Information Technology - Brno University of Technology \
points: 12/12 (+2 bonus)

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
Implementáciu som rozdelil do nasledujúcich 3 suborov:\
Bkg2cnfMain.hs :\
Nachádza sa tu hlavná funkcia main, spracovanie argumentov, podľa ktorých sa následne vyhodnotí, čo sa má vypisovať na výstup.\

Bkg2cnfData.hs :\
Obsahuje datové štruktúry pre bezkontextovú gramatiku (BKG) a pre pravidlá z tejto gramatiky (Rule). Taktiež obsahuje, ako sa tieto štruktúry majú zobrazovať.\

Bkg2cnfFuncs.hs :\
Tu sa nachádzajú všetky podstatné funkcie pre správne fungovanie. Sú rozdelené do 3 častí. \
Na začiatku sa spracuje vstup, načíta sa do vnútornej reprezentácie a skontroluje, či má správny formát.\
Ďalšia časť slúži na odstránenie jednoduchých pravidiel z gramatiky. Bol použitý algoritmus 4.5 z opory do predmetu TIN. V skratke sa jedná o vytvorenie pomocnej množiny pre každý neterminál, na základe ktorej potom buď schvalujem alebo opravujem/vytváram pravidlá tak, aby gramatika neobsahovala žiadne jednoduché pravidlá.\
Posledná časť sa zaoberá transformáciou BKG do Chomského normálnej formy. Bol použitý algoritmus 4.7 z opory do predmetu TIN.