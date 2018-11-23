Griglia: 256, 512
Pop: 500
Iterazioni: 100
side: 16

# Relazione

## Introduzione

Il progetto consiste nell'utilizzare algoritmi genetici applicati al Conway's Game of Life
(CGL). Piu\` nel dettaglio ci proponiamo, dato uno stato finale, di trovare lo stato iniziale
della matrice CGL che permette di trovare l'approssimazione migliore del suddetto stato.

**Disegno 1: stato finale (target), 2/3 gene**

## Terminologia

* Griglia: matrice bidimensionale di bit che rappresenta lo stato del CGL durante la sua
  evoluzione. All'interno dell'algoritmo genetico rappresenta un individuo.
* Gene: Griglia all'iterazione 0, utilizzata come gene dell'algoritmo genetico.
* TODO speedup, stencil, halo swap, AG, stato finale, stato iniziale, generazione

## Definizione del problema

Il problema consiste nell'esplorare varie tecniche di parallelizzazione per
confrontare il tempo di esecuzione della versione sequenziale del nostro programma con quella
parallela. I miglioramenti ottenuti sono espressi in termini di *speedup*.

La definizione del CGL e dell'algoritmo genetico utilizzati sono oltre l'obbiettivo di
questa relazione. In appendice si presenta una breve descrizione.

Le tecniche di parallelizzazione utilizzate per la nostra analisi sono le seguenti:
* Locally synchronous computations in shared memory utilizzando la tecnica *stencil* con
  *halo swap*. Questa tecnica e\` implementata attraverso le API di *OpenMP*.
* Architettura master-slave con message passing utilizzando MPI.

## L'algoritmo

**Disegni flowchart**

* Profilazione
* Hotpath con -O0 / -O2
* Grafici hotpath (scrot grafo)

L'algoritmo sequenziale rispetta il seguente *flowchart*:
**Disegno highlevelalg.xml**

L'algoritmo e\` configurato tramite i parametri:
* POPSIZE: Numero degli individui che compongono la popolazione ad ogni iterazione dell'AG.
* N\_ITERAZIONI: Numero di iterazioni per l'evoluzione del CGL.
* N\_GENERATIONS: Numero di iterazioni per l'evoluzione dell'AG.
* DIM: Dimensione della griglia. Una griglia e\` composta da DIM\*DIM celle.
* SIDE: Dimensione della sottogriglia utilizzata per il calcolo del fitness (vedi...). Una
  sottogriglia e\` composta da SIDE\*SIDE celle.

L'algoritmo genera POPSIZE\*N\_GENERATIONS\*N\_ITERAZIONI griglie durante la sua esecuzione.
Il blocco logico CGL evolve un individuo dal gene fino allo stato finale, calcolandone il
fitness.
Una volta evoluti tutti gli individui, il 25% dei migliori ordinati per fitness decrescente
vengono considerati per il calcolo del crossover: in questo modo vengono generati i POPSIZE
individui facenti parte della successiva generazione.

L'algoritmo termina dopo aver raggiunto la generazione numero N\_GENERATIONS.

### Pseudocodice (sequenziale)

```
function SEQUENTIAL(grid <- firstgeneration, fitness <- [])
	for i <- 1 to N_GENERATIONS do
		for j <- 1 to POPSIZE do
			for k <- 1 to N_ITERAZIONI do
				evolve grid[j]
			end for
			compute fitness[j]
		end for
		sort(fitness)
		newgrid <- crossover(grid, fitness)
		grid <- newgrid
	end for
end function
```

## Grafo di esecuzione

La nostra implementazione genera il seguente grafo di esecuzione:
**callgrind.svg**

Si nota come alcuni path traggano un beneficio maggiore dalla parallelizzazione rispetto ad
altri.

Dal grafo si nota che l'evoluzione del CGL (funzione `Cgl<T>::startCgl()`) occupa circa il 94%
del tempo di esecuzione, mentre il calcolo della funzione di fitness `Cgl<T>::fitnessScore()`
solamente il 3% circa e la funzione `Cgl<T>::crossover` solo lo 0.61%. Per questo motivo ci siamo concentrati sulla parallelizzazione del CGL.

Analizzando l'evoluzione del CGL, si nota che il 74% del tempo totale viene spesa all'interno
della funzione `Cgl<T>::updateCell`, che applica le regole del Game of Life a una data cella.
Da questa osservazione presentiamo nella prossima sezione la prima tecnica di
parallelizzazione.

Nota: i parametri utilizzati per generare il grafo di esecuzione sono i seguenti:
```
DIM: 64
SIZE: 8
N\_ITERAZIONI: 20
N\_GENERATIONS: 10
POPSIZE: 50
```
Variando i parametri il grafo presenta una variazione trascurabile nelle percentuale dei
tempi di esecuzione dei metodi sopra citati.

## Shared memory (Stencil)

* Motivazione
* Descrizione dell'algoritmo (step / pseudocodice)
* Profilazione
* Grafici

## Message passing (MPI)

* Motivazione
* Descrizione dell'algoritmo (step / pseudocodice)
* primitive di openmpi / due varianti algoritmo
* Parallellizzazione interna esterna
* Profilazione
* Grafici

## Performance Analysis (Considerazioni)

* quello che ci capita
* possibili ottimizzazioni ulteriori

## Conclusioni

L'utilizzo di C++ e in particolare delle data structure utilizzate per gestire la
griglia in memoria (`std::bitset`), ha portato i seguenti vantaggi:
* Minore consumo in memoria
* Facilita\` di implementazione
* ?

Questo ha portato pero\` i seguenti svantaggi:
* No native implementation di `std::bitset` in CUDA 
* No copy on write (CoW)

## Referenze

## Il Conway's Game of Life

Il lavoro svolto riguarda la parallelizzazione del famoso *Conway's Game of Life*, un esempio di automa cellulare che a partire da una configurazione iniziale di organismi disposti su una griglia, applica ad ogni iterazione la *Rule of Life*.

La regola aggiorna la griglia seguendo questi principi:
* Qualsiasi cella viva con meno di due celle vive adiacenti muore
* Qualsiasi cella viva con esattamente due o tre celle vive adiacenti sopravvive
* Qualsiasi cella viva con più di tre celle vive adiacenti muore
* Qualsiasi cella morta con esattamente tre celle adiacenti diventa viva

Le celle adiacenti sono rappresentate dalle 8 celle intorno a quella considerata.
L'applicazione della regola della vita aggiorna la griglia ad ogni iterazione.

### Algoritmi genetici

* Funzionamento algoritmo genetico (poco)
* Come vengono utilizzati da noi (poco) (vedi abstract).

  L'algoritmo genetico e\` evolutivo
  nel senso che ogni generazione dipende dai risultati della precedente. Per questo
  abbiamo parallelizzato le singole generazinoni, che vengono gestite da un master che
  procede di generazione in generazione iterativamente.
## Metodologia (che titolo brutto)

* Definizione dello stato iniziale (what?)
* Definizione dello stato finale (what?)
* Perche\` parallelizzare
	* L'algoritmo presenta componenti fortemente ripetitive (embarassingly?)
	* L'utilizzo di griglie permette di lavorare con tecniche locally synchronous
	* L'architettura della macchina di test si presta all'utilizzo del modello di Message
	  Passing, implementato con MPI. (blablabla su cos'e` mpi?)
* Strumenti utilizzati (hardware / software)
	* Software: 
* Considerazioni sulle strutture dati (copyGrid)

BENCH: Openmpi + 1 processo + full stencil VS Openmpi + 2/3 processi + stencil bilanciato

