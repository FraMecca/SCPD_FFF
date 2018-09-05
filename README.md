# Relazione
## Il Conway's Game of Life
Il lavoro svolto riguarda la parallelizzazione del famoso *Conway's Game of Life*, un esempio di automa cellulare che a partire da una configurazione iniziale di organismi disposti su una griglia, applica ad ogni iterazione la *Rule of Life*.

La regola aggiorna la griglia seguendo questi principi:
* Qualsiasi cella viva con meno di due celle vive adiacenti muore
* Qualsiasi cella viva con esattamente due o tre celle vive adiacenti sopravvive
* Qualsiasi cella viva con più di tre celle vive adiacenti muore
* Qualsiasi cella morta con esattamente tre celle adiacenti diventa viva

Le celle adiacenti sono rappresentate dalle 8 celle intorno a quella considerata.
L'applicazione della regola della vita aggiorna la griglia ad ogni iterazione.

## 
