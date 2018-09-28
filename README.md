# Relazione

## Abstract

* Esploriamo le tecniche possibili per la guida alla risoluzione di un cellular automata
  utilizzando un algoritmo genetico.
* Ci proponiamo di dimostrare quale sia lo stato iniziale di un automata dato un target
  finale di densita\` desiderata.

## Introduction

### Il Conway's Game of Life
Il lavoro svolto riguarda la parallelizzazione del famoso *Conway's Game of Life*, un esempio di automa cellulare che a partire da una configurazione iniziale di organismi disposti su una griglia, applica ad ogni iterazione la *Rule of Life*.

La regola aggiorna la griglia seguendo questi principi:
* Qualsiasi cella viva con meno di due celle vive adiacenti muore
* Qualsiasi cella viva con esattamente due o tre celle vive adiacenti sopravvive
* Qualsiasi cella viva con più di tre celle vive adiacenti muore
* Qualsiasi cella morta con esattamente tre celle adiacenti diventa viva

Le celle adiacenti sono rappresentate dalle 8 celle intorno a quella considerata.
L'applicazione della regola della vita aggiorna la griglia ad ogni iterazione.

### Algoritmi genetici

* Funzionamento algoritmo genetico
* Campo di utilizzo
* Come vengono utilizzati da noi

## Metodologia (che titolo brutto)

* Definizione dello stato iniziale
* Definizione dello stato finale
* Perche\` parallelizzare
* Tecniche di parallelizzazione
* Strumenti utilizzati (hardware / software)

## L'algoritmo sequenziale

* Struttura del programma (linguaggio, struttura, tecniche, discorso introduttivo)
* Descrizione dell'algoritmo (step / pseudocodice)
* Profilazione
* Grafici

## Shared memory (Partitioning)

* Motivazione
* Descrizione dell'algoritmo (step / pseudocodice)
* Profilazione
* Grafici

## Message passing (MPI)

* Motivazione
* Descrizione dell'algoritmo (step / pseudocodice)
* Profilazione
* Grafici

## Performance Analysis (Considerazioni)

* quello che ci capita

## Conclusioni

* bella

## Referenze
