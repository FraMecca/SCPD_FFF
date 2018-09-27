# FFF scpd

1. (tutti) ALL THE new (allocare nell'heap i bitset) --> manual memory management
   (Cgl.release()) rilasciando a mano la memoria dopo ogni generazione (oppure shared
   pointer)

   1.1. partition.hpp still ALL THE NEW

3. Integrare l'implementazione del cgl con partitioning in openmpi
---
4. crossover + processi spawnati da mpi (scambio di messaggi)
5. benchmark sulle cgl / cgl\_mp con alg gen.
---
6. Cominciare relazione su progetto
7. Documentare codice (doxygen?)

## Updates 20/09/2018 (FerraMecca)

1. DOCUMENTA SOLO GRIGLIE PARI
2. Eccezioni quando side e' incompatibile con dimensione
4. File di configurazione per parametri
5. Estrazione target da griglia casuale evoluta con Cgl

Possibilita\`:
1. Ad Aldinucci va bene che non ci sia miglioramento
2. Framework generico dove le regole sono importate da file dinamicamente
3. Cambiamo le regole del Conway per adattarlo ai GA
4. Parlare con Aldinucci di come risolvere

## Performance review
1. Le dimensioni della griglia e delle popolazione influenzano in modo critico la durata dell'esecuzione


## Deadlines

To be continued...
