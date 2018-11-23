# GA

Gli algoritmi genetici sono algoritmi euristici di ricerca e ottimizzazione che si ispirano alla teoria dell'evoluzione di Darwin.  

Un tipico algoritmo genetico parte da un certo numero di possibili soluzioni generate casualmente (individui) chiamate popolazione e provvede a farle evolvere nel corso dell'esecuzione: a ciascuna iterazione, esso opera una selezione di individui della popolazione corrente, impiegandoli per generare nuovi individui della successiva popolazione. Tale successione di generazioni evolve verso una soluzione ottima del problema assegnato.

Il gene di ogni individuo viene analizzato attraverso una funzione chiamata funzione di *fitness*. Gli individui con valore della funzione di fitness piu` alta hanno hanno quindi maggiori possibilità di sopravvivere e riprodursi. Nel nostro progetto la funzione di fitness e` costituita da una prima evoluzione del gene attraverso il Conway's Game of Life e il calcolo della distanza di Manhattan della griglie ottenute nelle ultime dieci generazioni rispetto all'obbiettivo desiderato.

L'evoluzione delle popolazioni viene ottenuta attraverso una parziale ricombinazione delle soluzioni, ogni individuo trasmette parte del suo patrimonio genetico ai propri discendenti, e l'introduzione di mutazioni casuali nella popolazione di partenza, sporadicamente quindi nascono individui con caratteristiche non comprese tra quelle presenti nel corredo genetico della specie originaria.
Tale ricombinazione viene chiamata *crossover* e puo` essere implementato attraverso differenti tecniche. Nel nostro caso il gene rappresentato da un'array di bit viene scisso in 4 parti di equa lunghezza: il gene del figlio sara` costituito dalla prima parte del padre, seconda della madre, terza del padre e quarta
 della madre.

Alla fine ci si aspetta di trovare una popolazione di soluzioni che riescano a risolvere adeguatamente il problema posto. Non vi è modo di decidere a priori se l'algoritmo sarà effettivamente in grado di trovare una soluzione accettabile. Di norma gli algoritmi genetici vengono utilizzati per problemi di ottimizzazione per i quali non si conoscono algoritmi di complessità lineare o polinomiale.

Pseudocodice:

```
for i <- 1 to POPSIZE
	population[i] <- random(gene)		
endfor
for cnt <- 1 to N_GENERATIONS
	for i <- 1 to POPSIZE
		fitness[i]  <- compute_fitness(population[i])		
	endfor
	for i <- 1 to POPSIZE
		mutation_prob = random()
		if mutation_prob > mutation_threshold
			new_population[i] = random(gene)
		else
			parent1 = take_random(population)
			survive_prob = random()
			if survive_prob > survive_threshold:
				new_population[i] = parent1
			else
				parent2 = take_random(population - parent1)
				new_population = crossover(parent1, parent2)
			endif
		endif
	endfor
	population = new_population
endfor
```

# CGL

Il Conway's Game of Life e` un automa cellulare la cui evoluzione e` determinata dallo stato iniziale.
Lo stato iniziale e` definito da una griglia, chiamata *mondo*, dove ogni cella e` definita da uno stato booleano di vita / morte ed ha 8 celle adiacenti.
Il mondo evolve in tempi discreti dove lo stato di ogni cella e` calcolato a partire dallo stato precedente della cella e dei suoi vicini, secondo queste regole:
- qualsiasi cella viva con meno di due celle vive adiacenti muore, come per effetto d'isolamento;
- qualsiasi cella viva con due o tre celle vive adiacenti sopravvive alla generazione successiva;
- qualsiasi cella viva con più di tre celle vive adiacenti muore, come per effetto di sovrappopolazione;
- qualsiasi cella morta con esattamente tre celle vive adiacenti diventa una cella viva, come per effetto di riproduzione;

Vi sono configurazione del Conway's Game of Life per cui la griglia evolve indefinitivamente senza che la vita nel mondo si estingua. 
