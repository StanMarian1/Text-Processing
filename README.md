# Proiect Algoritmi Paraleli și Distribuiți
 
## Descrierea Problemei:
Proiectul vizează paralelizarea algoritmului de numărare a frecvenței cuvintelor dintr-un fișier text de mari dimensiuni. Această operație este esențială în procesarea limbajului natural și text mining, dar devine foarte costisitoare pe un singur fir de execuție atunci când fișierele au dimensiuni foarte mari.

## Limbaj și Tehnologii:

Limbaj de programare: C++ (Standard ISO C++20)

Tehnologie de paralelizare: Biblioteca MPI (Message Passing Interface), varianta Microsoft MPI (MS-MPI) rulată în Visual Studio.

## Algoritmul Implementat:

Varianta Secvențială:
Parcurge fișierul cuvânt cu cuvânt, curăță textul de semne de punctuație și cifre (folosind isalpha), convertește literele în litere mici (tolower) pentru a nu diferenția între litere mari/mici și salvează rezultatele într-o structură de tip std::map<std::string, int>.

Varianta Paralelă (MPI): * Împărțirea echitabilă: Procesul Master (Rank 0) află dimensiunea totală a fișierului în bytes și calculează pentru fiecare proces un interval egal de bytes pe care să lucreze (low_bound și high_bound). Logica de împărțire pe intervale este similară cu cea de la Sita lui Eratostene segmentată.

Corectarea limitelor: Deoarece o împărțire matematică brută poate tăia un cuvânt în două (ex: o jumătate la procesul 0, o jumătate la procesul 1), fiecare proces (cu excepția lui Rank 0) sare peste primul cuvânt întâlnit (folosind isspace), lăsându-l în grija procesului anterior.

Procesarea locală: Fiecare proces își creează un dicționar local (std::map).

Agregarea (Reducerea): Procesele Worker își transformă dicționarele într-un text lung (serializare) și îl trimit prin MPI_Send. Procesul Master primește textul cu MPI_Recv, îl decodează (std::istringstream) și adună toate frecvențele într-un map global final.

## Rezultatele Experimentale (Timpii de rulare):

Pentru teste a fost utilizat un fișier de text mare (input.txt), încărcat cu text repetat pentru a genera un volum de lucru real pentru procese și a justifica comunicarea prin MPI.

Secvențial (Baseline) - Timp Execuție (Secunde): 4.62

MPI (8 procese) - Timp Execuție (Secunde): 3.09
