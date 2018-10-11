#include <assert.h>
#include <iostream>
#include <tuple>
//#include <mpi.h>
#include "settings.hpp"
#include "libcgl.hpp"
#include "libga.hpp"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/string.hpp>

//LANCIARE CON 4 PROCESSI 

namespace mpi = boost::mpi;

int main(int argc, char* argv[])
{
	mpi::environment env(argc, argv);
	mpi::communicator world;
	
	auto buf = std::vector<string>(); //Elementi da inviare
	auto sizes = vector<int>(world.size()); //Numero di stringhe inviato ad ogni processo, master compreso (obbligatorio)
	sizes[0] = 0;
	sizes[1] = 3;
	sizes[2] = 1;
	sizes[3] = 4;
	auto recv = std::vector<string>(1); //Size >0, altrimenti il master che non deve ricevere nulla si arrabbia (il vettore viene ridimensionato negli slave)

	if (world.rank() == 0) {
		buf.push_back("primo");
		buf.push_back("secondo");
		buf.push_back("terzo");
		buf.push_back("quarto");
		buf.push_back("quinto");
		buf.push_back("sesto");
		buf.push_back("settimo");
		buf.push_back("ottavo");

		 //void scatterv(const communicator & comm, const std::vector< T > & in_values, const std::vector< int > & sizes, T * out_values, int root))
		// T* out_values DEVE essere solamente allocato, non va bene nullptr oppure lunghezza 0; nel caso di un vettore serve il puntatore all'array allocato dalla classe
		mpi::scatterv(world, buf, sizes, &recv[0], 0);
		cout << "Master: " << recv[0] << endl;
	}
	else {
		recv.resize(sizes[world.rank()]);
		//void scatterv(const communicator & comm, T * out_values, int out_size, int root))
		//Stesse considerazioni sul master, out_size contiene il numero di elementi da ricevere
		mpi::scatterv(world, &recv[0], sizes[world.rank()], 0); 
		cout << "Slave " << world.rank() << ": " << recv[0] << endl;
	}

  return 0;
}
