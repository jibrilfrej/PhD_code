using namespace std;
#include <iostream>
#include <assert.h>
#include "include/HDictionary/HDictionary.h"
using namespace IOTA;

struct Freq {
  unsigned value;
  Freq() : value(1) {}
  // From unsigned to Freq
  Freq(unsigned v) : value(v) {}
  // from Freq to unsigned
  operator unsigned() const {return value;}

};

int main(int argc, char** argv) {

	auto dico_test = HDictionary<Freq>(20);

	assert(dico_test.size()==0);

	assert(dico_test.find("totot")==false);

	bool found;

	unsigned pos = 1;

	const char* w = "totot";

	int totot = dico_test.access(w , pos , found);

	assert(dico_test.find("totot")==true);

	assert(dico_test.size()==1);

	assert(!found);

	assert(pos==0);

	totot = dico_test.access(w , pos , found);

	cout<<"pos : "<< pos <<endl;

	assert(found);

	totot = dico_test.access(w , pos , found);

	cout<<"pos : "<< pos <<endl;

	assert(dico_test[w]==1);

		totot = dico_test.access("lulu" , pos , found);

    // Parcourt tout les elements du dico
    for(unsigned i = 0 ; i < dico_test.size() ; i++){

		cout<<i<<"  "<< dico_test[i].word <<" : "<< dico_test[i].data <<endl;

	}
	return 0;

}
