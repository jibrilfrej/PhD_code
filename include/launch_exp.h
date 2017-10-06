#ifndef launch_exp_h
#define launch_exp_h


#include "readwrite.h"
#include "basic_LM.h"
#include "hiemstra_LM.h"
#include "dirichlet_LM.h"
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <omp.h>


//Performs a set of experiments with a "regular" model
void launch_Hiemstra_experience(const std::string &collection_file , const std::string &queries_file , double lambda , const double lambda_step , const int k){

	std::vector< std::vector< std::pair<int,double> > > results;
	std::unordered_map< int , std::vector<std::string> > collection;
	std::unordered_map< int , std::vector<std::string> > queries;
	std::unordered_map <std::string,int> cf;
	std::unordered_map <std::string,int> df;
	std::string file_name;

	clock_t begin = clock();

	read_all_info(collection_file , queries_file , collection , queries , cf , df);

	write_xml_collection_file(collection, "../../../Terrier4.2/terrier-core-4.2/share/CHIC2012/synthetic/xml_synthetic_collection");

	display_stuff(collection ,queries , cf , df);

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout<< "Time to do all the stuff : "<< elapsed_secs <<std::endl;

	display_stuff(collection , queries , cf , df);

	size_t nb_words = get_size_collection(cf);

	while(lambda < 1 + lambda_step/1000){

		begin = clock();
		results = Hiemstra_language_model(queries , collection , cf , nb_words , k , lambda);
		end = clock();
		elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		std::cout<< "Time to perform all the queries for lambda = "<<lambda<<" : "<< elapsed_secs <<std::endl;
		file_name = "../data/res/logresults";
		file_name += std::to_string(lambda);
		write_res_file(results , file_name , "CHIC-" , lambda);
		lambda += lambda_step;

	}

}



//Performs a set of experiments with an embedded model
void launch_Dirichlet_experience(const std::string &collection_file , const std::string &queries_file , double &mu , const double &mu_step , const int nb_iter , const int k ){

	std::vector< std::vector< std::pair<int,double> > > results;
	std::unordered_map< int , std::vector<std::string> > collection;
	std::unordered_map< int , std::vector<std::string> > queries;
	std::unordered_map <std::string,int> cf;
	std::unordered_map <std::string,int> df;
	std::string file_name;
	double mu_temp;
	clock_t begin = clock();

	read_all_info( collection_file , queries_file , collection , queries , cf , df);

	write_xml_collection_file(collection, "../../../Terrier4.2/terrier-core-4.2/share/CHIC2012/synthetic/xml_synthetic_collection");

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout<< "Time to do all the stuff : "<< elapsed_secs <<std::endl;

	display_stuff(collection , queries , cf , df);

	size_t nb_words = get_size_collection(cf);

	int nthreads, tid;

	#pragma omp parallel private( file_name , mu_temp , results , tid) shared(mu , mu_step)
	{

		tid = omp_get_thread_num();

        //std::cout<<"Thread No "<<tid<<std::endl;
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            std::cout<<"Number of thread: "<<nthreads<<std::endl;
        }


		#pragma omp for schedule(static)
		for(int current_iter = 0 ; current_iter < nb_iter ; current_iter++){

			mu_temp = mu + mu_step*(current_iter);
			results = Dirichlet_language_model(mu_temp , queries , collection , cf , k , nb_words);
			file_name = "../data/res/results|";
			file_name += std::to_string(mu_temp);
			write_res_file(results , file_name , "CHIC-" , mu_temp);
			//mu += mu_step;
			//current_iter++;

		}

	}

}




//Performs a set of experiments with an embedded model
void launch_embedded_experience(const std::string &collection_file , const std::string &queries_file , const std::string &collection_cosine_file , const std::string &queries_cosine_file , double &mu_min , double &mu_max , const double &mu_step , const int k , const double &threshold_min , const double &threshold_max , const double &threshold_step , const double &alpha_min , const double &alpha_max , const double &alpha_step){

	std::unordered_map< std::string , std::unordered_map<std::string,double> > all_cos;
	std::unordered_map<std::string,double> all_sum_cos;
	std::unordered_map< int , std::vector<std::string> > collection;
	std::unordered_map< int , std::vector<std::string> > queries;
	std::unordered_map <std::string,int> cf;
	std::unordered_map <std::string,int> df;
	int nb_iter_mu  = (int)myround((mu_max - mu_min)/abs(mu_step));
	int nb_iter_threshold = (int)myround((threshold_max - threshold_min)/abs(threshold_step));
	int nb_iter_alpha  = (int)myround((alpha_max - alpha_min)/abs(alpha_step));

	std::cout<< "Number of iterations over the variable mu = " << nb_iter_mu <<std::endl;

	std::cout<< "Number of iterations over the threshold = " << nb_iter_threshold <<std::endl;

	std::cout<< "Number of iterations over the variable alpha = " << nb_iter_alpha <<std::endl;

	std::cout<< "Total number of iterations : " << nb_iter_mu*nb_iter_threshold*nb_iter_alpha <<std::endl;

	clock_t begin = clock();

	read_all_info2( collection_file , queries_file , collection_cosine_file , queries_cosine_file , collection , queries , cf , df , all_cos , all_sum_cos , threshold_max);

	write_xml_collection_file(collection, "../../../Terrier4.2/terrier-core-4.2/share/CHIC2012/synthetic/xml_synthetic_collection");

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout<< "Time to do all the stuff : "<< elapsed_secs <<std::endl;

	display_stuff(collection , queries , cf , df);

	size_t nb_words = get_size_collection(cf);

	int nthreads;

	#pragma omp parallel private( all_cos , all_sum_cos )
	{

		int tid = omp_get_thread_num();

		//std::cout<<"Thread No "<<tid<<std::endl;
		if (tid == 0)
		{
			nthreads = omp_get_num_threads();
			std::cout<<"Number of thread: "<<nthreads<<std::endl;
		}

		#pragma omp for schedule(static) collapse(3)
		for(int current_iter_threshold = 0 ; current_iter_threshold < nb_iter_threshold ; current_iter_threshold++ ){

			for(int current_iter_alpha = 0  ; current_iter_alpha < nb_iter_alpha ; current_iter_alpha++ ){

				for(int current_iter_mu = 0 ; current_iter_mu < nb_iter_mu ; current_iter_mu++){

					double threshold_temp = threshold_max - current_iter_threshold*threshold_step;

					double mu_temp = mu_min + mu_step*current_iter_mu;

					double alpha_temp = alpha_min + current_iter_alpha*alpha_step;

					delete_low_similarities(collection_cosine_file , queries_cosine_file , all_cos , all_sum_cos , threshold_temp);
					std::vector< std::vector< std::pair<int,double> > > results = Dirichlet_embedding_model(mu_temp , queries , collection , cf , all_sum_cos , all_cos , k , nb_words , alpha_temp);
					std::cout<< "Performed all the queries for mu = "<<mu_temp<< " , for alpha = "<< alpha_temp <<" and the threshold = " << threshold_temp <<std::endl;
					std::string file_name = "../data/res/results|";
					file_name += std::to_string(mu_temp);
					file_name.erase (file_name.size()-4,4);
					file_name += "|";
					file_name += std::to_string(threshold_temp);
					file_name.erase (file_name.size()-4,4);
					file_name += "|";
					file_name += std::to_string(alpha_temp);
					file_name.erase (file_name.size()-4,4);
					write_res_file(results , file_name , "CHIC-" , mu_temp);

				}

			}

		}

	}

}


//Performs a set of experiments with an embedded model
void launch_expanded_experience(const std::string &collection_file , const std::string &queries_file , const std::string &collection_cosine_file , const std::string &queries_cosine_file , const std::string &sum_queries_cosine_file , double &mu ,  const double &mu_step , const int nb_iter , const int k , double &threshold , const double &threshold_min , const double &threshold_max , const double &threshold_step ){

	std::unordered_map< std::string , std::unordered_map<std::string,double> > all_cos;
	std::unordered_map< int , std::unordered_map<std::string,double> > all_cos_sum_queries;
	std::unordered_map<std::string,double> all_sum_cos;
	std::vector< std::vector< std::pair<int,double> > > results;
	std::unordered_map< int , std::vector<std::string> > collection;
	std::unordered_map< int , std::vector<std::string> > queries;
	std::unordered_map <std::string,int> cf;
	std::unordered_map <std::string,int> df;
	std::string file_name;
	double mu_temp;


	clock_t begin = clock();

	read_all_info3( collection_file , queries_file , collection_cosine_file , queries_cosine_file , sum_queries_cosine_file , collection , queries , cf , df , all_cos , all_sum_cos , all_cos_sum_queries , threshold);


	write_xml_collection_file(collection, "../../../Terrier4.2/terrier-core-4.2/share/CHIC2012/synthetic/xml_synthetic_collection");

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout<< "Time to do all the stuff : "<< elapsed_secs <<std::endl;

	display_stuff(collection , queries , cf , df);

	size_t nb_words = get_size_collection(cf);

	//int current_iter;

	int nthreads, tid;

	while(threshold < threshold_max + abs(threshold_step/1000) && threshold > threshold_min - abs(threshold_step/1000)){

		//current_iter = 0;



	#pragma omp parallel private(file_name , mu_temp , results , tid) shared(mu , mu_step )
	{


		tid = omp_get_thread_num();

        //std::cout<<"Thread No "<<tid<<std::endl;
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            std::cout<<"Number of thread: "<<nthreads<<std::endl;
        }

		mu_temp = mu;

		#pragma omp for schedule(static)
		for(int current_iter = 0 ; current_iter < nb_iter ; current_iter++){
		//while(current_iter < nb_iter){
			mu_temp = mu + mu_step*(current_iter);
			results = Dirichlet_embedding_model_Qexp(mu_temp , queries , collection , cf , all_sum_cos , all_cos , all_cos_sum_queries , k , nb_words);
			std::cout<< "Performed all the queries for mu = "<<mu_temp<< " and the threshold = " <<threshold<<std::endl;
			file_name = "../data/res/results";
			file_name += std::to_string(mu_temp);
			file_name += "thresh";
			file_name += std::to_string(threshold);
			write_res_file(results , file_name , "CHIC-" , mu_temp);
			//mu_temp += mu_step;
			//current_iter++;

		}

	}

	threshold += threshold_step;

	delete_low_similarities(collection_cosine_file , queries_cosine_file , all_cos , all_sum_cos , threshold);

	}


}


#endif
