#ifndef test_h
#define test_h

#include "closest.h"
#include "launch_exp.h"


void compute_and_save_cosine(const std::string &collection_file , const std::string &queries_file){

	std::string collection_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/stop_cos00";
	std::string queries_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/stop_cos_queries_temp";
	std::string embeddings_file = "../data/embeddings/GoogleNews-vectors-negative300/GoogleNews-vectors-negative300.bin";

	Embedding embedding;

	embedding.load_Word2VecBinFormat(embeddings_file.c_str());

	std::unordered_map< int , std::vector<std::string> > collection;

	std::unordered_map< int , std::vector<std::string> > queries;

	std::unordered_map <std::string,int> cf;

	std::unordered_map <std::string,int> df;

	read_all_info( collection_file , queries_file , collection , queries , cf , df);

	std::cout<<"Query size : "<< queries.size() <<std::endl;

	nb_embedded_words_in_voc(embedding , cf);

	nb_embedded_words_in_queries(embedding , queries , false);

	std::unordered_map< std::string , std::unordered_map<std::string,double> > set_closest_words;

	//Queries

	set_closest_words = closest_terms(queries , cf ,embedding ,  0.4);

	write_map_map(set_closest_words , queries_cosine_file);

	std::cout << "Taille set_closest_words : " << set_closest_words.size() <<std::endl;


	//Vocabulary

	//set_closest_words = closest_terms(cf , embedding , 0.1);

	//write_map_map(set_closest_words , collection_cosine_file);

	//save_closest_terms(collection_cosine_file , cf , embedding ,  0.4);


}

void hiemstra_test(const std::string &collection_file , const std::string &queries_file , const std::string &res_file){

	double lambda = 0.0;
	int k = 1000;
	double lambda_step = 0.1;

	launch_Hiemstra_experience(collection_file , queries_file , res_file , lambda , lambda_step , k);

}


void dirichlet_test(const std::string &collection_file , const std::string &queries_file , const std::string &res_file){

	double mu = 80;
	int k = 1000;
	double mu_step = 10;
	int nb_iter = 10;
	launch_Dirichlet_experience(collection_file , queries_file , res_file , mu , mu_step , nb_iter , k );

}


void embedding_test(const std::string &collection_file , const std::string &queries_file){

	std::string collection_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/stop_cos04";
	std::string queries_cosine_file = "../data/embeddings/GoogleNews-vectors-negative300/stop_cos_queries_04";

	int k = 1000;

	double mu_min = 100;

	double mu_max = 130;

	double mu_step = 20;

	double threshold_min = 0.6;

	double threshold_max = 0.71;

	double threshold_step = 0.05;

	double alpha_min = 0.7;

	double alpha_max = 0.81;

	double alpha_step = 0.05;

	launch_embedded_experience(collection_file , queries_file , collection_cosine_file , queries_cosine_file , mu_min , mu_max , mu_step , k , threshold_min , threshold_max , threshold_step , alpha_min , alpha_max, alpha_step);


}



#endif
