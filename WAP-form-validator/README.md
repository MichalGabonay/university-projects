## Form validator
## Validátor formulářů

---
University project\
WAP 2018\
xgabon00 - Michal Gabonay 1MIS \
Faculty of Information Technology - Brno University of Technology \
points: 30/30

---

Je treba importovať skript wap-validator.js (na koniec dokumentu).\
Pre správne fungovanie odporúčam vložiť do dokomuentu taktiež style.css.

---
Na vstupné polia typu text sa pridávajú obmedzenia vkladaním atributu "data-" podľa toho aké obmedzenia chceme pridať. Takéto vstupné polia musia obsahovať vlastné id alebo name.

**Minimálna dĺžka textu:**\
Pre určenie minimálnej dĺžky textu u vstupného pola pridajte atribut "data-min" \
*Príklad:* \
`<input type="text" name="input-text" data-min="5">`

**Maximálna dĺžka textu:**\
Pre určenie maximálnej dĺžky textu u vstupného pola pridajte atribut "data-max" \
*Príklad:* \
`<input type="text" name="input-text" data-max="20">`

**Požadovaný formát dat zapísaný ako regulárny výraz:**\
Pre určenie minimálnej dĺžky textu u vstupného pola pridajte atribut "data-pattern" \
*Príklad:*\
`<input type="text" name="input-text" data-pattern="^[_a-zA-Z0-9\.\-]+@[_a-zA-Z0-9\.\-]+\.[a-zA-Z]{2,4}$">` (obmedzenie na email)

**Povinné alebo nepovinné pole:**\
Pre určenie minimálnej dĺžky textu u vstupného pola pridajte atribut "data-min" \
*Príklad:*\
`<input type="text" name="input-text" data-required="true">`
`<input type="text" name="input-text" data-required="false">` (nepovinné pole môže byť prázdne, potom nemusí splňovať žiadne dalšie omezenia)

---
Na každé textové vstupné pole je možno klásť ľubovoľnú kombináciu týchto obmedzení. Počas zadávania sa dynamicky zobrazuje stav formulára - správne vyplnená polia sú zelene podfarbená, zle vyplnené polia sú červeno podfarbené, nepovinné nevyplnené polia nie sú podfarbené. Tlačidlo pre odoslanie formulára je zobrazené ako zakázané, kým nie sú všetky polia správne vyplnené.