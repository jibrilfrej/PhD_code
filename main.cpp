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

	std::string collection_file = "../data/collection/porter_stop_string_content";
	std::string queries_file = "../data/queries/porter_stop_queries.txt";
	std::string index_file = "../data/index/porter_index";

	if(argc > 2 && std::string(argv[2]) == "nostem"){

			collection_file = "../data/collection/porter_stop_string_content";
			queries_file = "../data/queries/porter_stop_queries.txt";
			index_file = "../data/index/porter_index";

	}

	if(argc > 1 && std::string(argv[1]) == "cosine"){

		//std::string collection_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/stop_cos00";
		//std::string queries_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/stop_cos_queries_temp";
		std::string collection_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/indexed_porter_stop_cos";
		std::string queries_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/indexed_porter_stop_cos_queries";
		std::string embeddings_file = "../data/embeddings/GoogleNews-vectors-negative300/GoogleNews-vectors-negative300.bin";
		compute_and_save_index_and_cosine(collection_file , queries_file , index_file , collection_cosine_file , queries_cosine_file , embeddings_file );

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
		std::string collection_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/indexed_porter_stop_cos";
		std::string queries_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/indexed_porter_stop_cos_queries";
		if(argc > 2 && std::string(argv[2]) == "nostem"){

			collection_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/indexed_stop_cos";
			queries_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/indexed_stop_cos_queries";

		}
		embedding_test(collection_file , queries_file , index_file , res_file , collection_cosine_file , queries_cosine_file);

	}

	return 0;

}
