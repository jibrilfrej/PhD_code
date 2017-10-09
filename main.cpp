#include "include/test.h"
#include "include/readwrite.h"

//#include "include/HDictionary.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>




int main(int argc, char** argv) {

	//GIT!!!!!!!!

	std::cout<<"Program start"<<std::endl;

	if(argc == 1){

		write_Average_Precision("../data/AveragePrecision/AP" , "../../../trec_eval.8.1/results/" , true , false , false);

	}

	std::string collection_file = "../data/collection/stop_string_content";
	std::string queries_file = "../data/queries/stop_queries.txt";

	if(argc > 1 && std::string(argv[1]) == "cosine"){

		compute_and_save_cosine(collection_file , queries_file);

		return 0;

	}

	else if(argc > 1 && std::string(argv[1]) == "hiemstra"){

		std::string res_file = "../data/res/hiemstra/results";
		hiemstra_test(collection_file , queries_file , res_file);

	}

	else if(argc > 1 && std::string(argv[1]) == "dirichlet"){

		std::string res_file = "../data/res/dirichlet/results";
		dirichlet_test(collection_file , queries_file , res_file);

	}


	else if(argc > 1 && std::string(argv[1]) == "embedding"){

		std::string res_file = "../data/res/embedding/results|";
		embedding_test(collection_file , queries_file , res_file);

	}

	return 0;

}
