## Simple SMTP client
## Jednoduchý SMTP klient

University project\
ISA 2016\
xgabon00 - Michal Gabonay 3BIT \
Faculty of Information Technology - Brno University of Technology \

---
### Príklad užitia:
./smtpklient -i inputFile.txt
-> Ak všetko prebehlo uspešne, v mailboxe bude odoslaný email.

./smtpklient -a 127.0.1.1 -p 28 -i inputFile.txt -w 20
-> Ak všetko prebehlo uspešne, v mailboxe bude odoslaný email.

./smtpklient -a :1:1 -p 28 -i inputFile.txt -w 20
->Result: In mailbox there are sent mail if everything went correctly