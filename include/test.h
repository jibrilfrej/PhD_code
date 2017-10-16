#ifndef test_h
#define test_h


#include "readwrite.h"
#include "closest.h"
#include "launch_exp.h"


void compute_and_save_index_and_cosine(const std::string &collection_file , const std::string &queries_file , const	std::string &index_file , const	std::string &collection_cosine_file , const	std::string &queries_cosine_file , const	std::string &embeddings_file ){


	Embedding embedding;

	embedding.load_Word2VecBinFormat(embeddings_file.c_str());

	std::unordered_map< int , std::vector<int> > collection;
	std::unordered_map< int , std::vector<int> > queries;
	std::unordered_map <std::string,int> index;
	std::unordered_map <int,int> cf;

	read_all_info_and_index(collection_file , queries_file , collection , queries , index , cf);

	std::cout<<"Query size : "<< queries.size() <<std::endl;

	nb_embedded_words_in_voc(embedding , index , cf);

	nb_embedded_words_in_queries(embedding , queries , index , true);

	//save index
	write_map(index , index_file);

	std::unordered_map< int , std::unordered_map<int,double> > set_closest_words;

	//Queries

	set_closest_words = closest_terms(queries , index ,embedding ,  0.4);

	write_map_map(set_closest_words , queries_cosine_file);

	std::cout << "Taille set_closest_words : " << set_closest_words.size() <<std::endl;

	//Vocabulary

	set_closest_words = indexed_closest_terms(index , embedding , 0.4);

	write_map_map(set_closest_words , collection_cosine_file);

	//save_closest_terms(collection_cosine_file , index , embedding ,  0.4);

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
	system("../scripts/super_trec.sh dirichlet");
	write_Average_Precision("../data/AveragePrecision/AP" , "../../../trec_eval.8.1/results/" , true , false , false);

}

void embedding_test(const std::string &collection_file , const std::string &queries_file , const std::string &index_file , const std::string &res_file , const std::string &collection_cosine_file , const std::string &queries_cosine_file){

	int k = 1000;

	double mu = 10;

	double mu_step = 10;

	int nb_iter_mu = 50;

	double threshold = 1.0;

	double threshold_step = 0.05;

	int nb_iter_threshold = 1;

	double alpha = 0.0;

	double alpha_step = 0.05;

	int nb_iter_alpha = 1;

	launch_embedded_experience(collection_file , queries_file , index_file , res_file , collection_cosine_file , queries_cosine_file , mu, mu_step , nb_iter_mu, k , threshold, threshold_step ,nb_iter_threshold, alpha, alpha_step , nb_iter_alpha);

	system("../scripts/super_trec.sh embedding");

	write_Average_Precision("../data/AveragePrecision/AP" , "../../../trec_eval.8.1/results/" , true , false , false);

}

#endif
