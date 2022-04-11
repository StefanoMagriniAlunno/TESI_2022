# TESI_2022
Questo software permette di analizzare delle immagini .pbm in formato P1. Il codice fa uso di librerie dello standard ISO/IEC 9899:1999 (alias C99).

## Come funziona
Nelle cartelle src sono tenuti i codici sorgente, mentre nelle cartelle hdr gli header, due di questi introducono macro per la gestione degli errori ( err_mng.h ) e i tipi usati da più file sorgente ( types.h ).
La gestione degli errori permette di illustrare in un file chiamato ( err_report.txt ) gli esiti di tutti gli assert e i vari report ( messaggi incondizioanti dal programma ). Tuttavia non tutti gli errori saranno inseriti poiché la mole di controlli renderebbe difficile un eventuale debug, basti sapere che non vi sono controlli laddove vengono allocati iterativamente molti blocchi di memoria.
I report leggibili sono quelli riguardanti ad esempio il nome dell'immagine visionata ed eventuali parametri che possono far orientare meglio il programmatore in fase di debug. In presenza di assert violati ci si può imbattere in 3 situazioni distinte: il programma continua comunque lasciando un avviso ( evento anomalo che l'algoritmo sa gestire ), il programma si ferma come in un break point ( anomalia non gestibile priva di errori ), il programma si chiude ( errore ).

## Come si usa
Tutte le immagini in formato pbm P1 devono essere inserite nella cartella DATA anche organizzate in cartelle. Se queste vengono organizzate in sotto cartelle sarà necessario imitare queste directory anche nelle cartelle: SINTEX, DISTANCE, SORT_SINTEX, SORT_DISTANCE, EVIDENCE. Ad esempio se in DATA c'è la cartella "autoreA/immagine.pbm" allora in SINTEX deve esservi la cartella "autoreA" (senza il file immagine.pbm). Una volta inserite le immagini assicurarsi di aver scritto in register.txt le directory relative a DATA, registrare un'immagine inesistente porterà ad errore il programma, un'immagine non registrata non verrà inclusa nelle analisi del programma.
Ad esempio se in DATA troviamo "autoreA/immagine1.pbm" "autoreB/immagine1.pbm" "autoreB/immagine2.pbm" allora in register si dovrà scrivere:

    3
    autoreA/immagine1.pbm
    autoreB/immagine1.pbm
    autoreB/immagine2.pbm
  
Assicurarsi che il numero di immagini registrate non sia inferiore al numero scritto in linea 1.
L'esecuzione del programma è divisa in 5 fasi: lettura del file register.txt nel quale sono elencate tutte le directory delle immagini relative a DATA, processo di sintesi nel quale vengono elaborate una ad una le immagini producendo un file binario salvato in SINTEX con stessa directory, processo distance nel quale vengono calcolate le distanze tra immagini producendo un file di testo salvato in DISTANCE e il processo di sorting che riordina i dati in SINTEX producendo file txt in SORT_SINTEX, riordina i dati in DISTANCE producendo file txt in SORT_DISTANCE, infine c'è il processo evidence che ricolora le immagini mostrando la posizione dei grams più rari (lo scopo è mostrato nella tesi stessa).

## Come si leggono i risultati
I risultati sono organizzati in directory identiche a quelle di DATA.
In SORT_SINTEX si trovano file txt che riordinano le sintesi delle immagini, in essi vi è un elenco solitamente lungo di chiavi. Ogni chiave è costituita da numeri scritti nella medesima riga, l'ultimo numero viene detto "occorrenza" gli altri costituiscono il gram, la chiave riporterà il gram con rispettiva occorrenza nell'immagine. I numeri che indicano il gram sono numeri unsigned ed ognuno di essi simboleggia una colonna secondo il sistema numerico binario, ad esempio leggendo il numero 6 si deduce che la colonna sarà 1 1 0 (leggendo dall'alto verso il basso). Le chiavi sono ordinate per occorrenza e prima dell'elenco viene riportato il numero di chiavi.
In SORT_DISTANCE si trovano file txt che riordinano le distanze tra immagini, in essi vi è un elenco solitamente corto di chiavi. Ogni chiave è costituita da una stringa e da un double compreso tra 0 e 1 (estremi inclusi). Ogni stringa indica una directory di ogni immagine esaminata. L'immagine relativa al file viene detta "soggetto" mentre le immagini relative alle directory sono dette "oggetto" il numero double sarà la distanza tra soggetto e oggetto e le chiavi sono ordinate per tale distanza.
Ad esempio nel file autoreA/immagine1.pbm in SORT_DISTANCE troviamo scritto:

    3
    autoreA/immagine1.pbm 0.00000
    autoreB/immagine1.pbm 0.23456
    autoreB/immagine2.pbm 0.78900

### Note sulle estensioni
Tutti i file di output mantengono l'estensione del file originario, quindi se il file in input è con estensione pbm allora lo sarà anche in output.
