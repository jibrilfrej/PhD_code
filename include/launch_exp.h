#ifndef launch_exp_h
#define launch_exp_h


#include "readwrite.h"
#include "basic_LM.h"
#include "hiemstra_LM.h"
#include "dirichlet_LM.h"
#include "display.h"
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <omp.h>


//Performs a set of experiments with a "regular" model
void launch_Hiemstra_experience(const std::string &collection_file , const std::string &queries_file , const std::string &res_file , double lambda , const double lambda_step , const int k){

	std::vector< std::vector< std::pair<int,double> > > results;
	std::unordered_map< int , std::vector<int> > collection;
	std::unordered_map< int , std::vector<int> > queries;
	std::unordered_map <std::string,int> index;
	std::unordered_map <int,int> cf;
	std::string file_name;

	double lambda_temp;

	read_all_info_and_index(collection_file , queries_file , collection , queries , index , cf);

	size_t nb_words = get_size_collection(cf);

	unsigned int nb_iter = floor(1.0/lambda_step) + 1;

	int nthreads, tid;

	#pragma omp parallel private( file_name , lambda_temp , results , tid) shared(lambda)
	{

		tid = omp_get_thread_num();

        //std::cout<<"Thread No "<<tid<<std::endl;
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            std::cout<<"Number of thread: "<<nthreads<<std::endl;
        }


		#pragma omp for schedule(static)
		for(unsigned int current_iter = 0 ; current_iter < nb_iter ; current_iter++){

			lambda_temp += lambda + lambda_step*current_iter;
			results = Hiemstra_language_model(queries , collection , cf , nb_words , k , lambda_temp);
			file_name = res_file;
			file_name += std::to_string(lambda_temp);
			write_res_file(results , file_name , "CHIC-" , lambda_temp);

		}
	}
}




//Performs a set of experiments with an embedded model
void launch_Dirichlet_experience(const std::string &collection_file , const std::string &queries_file , const std::string &res_file , double &mu , const double &mu_step , const int nb_iter , const int k ){

	std::vector< std::vector< std::pair<int,double> > > results;
	std::unordered_map< int , std::vector<int> > collection;
	std::unordered_map< int , std::vector<int> > queries;
	std::unordered_map <std::string,int> index;
	std::unordered_map <int,int> cf;
	std::string file_name;

	double mu_temp;

	read_all_info_and_index(collection_file , queries_file , collection , queries , index , cf);

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
			file_name = res_file;
			file_name += std::to_string(mu_temp);
			write_res_file(results , file_name , "CHIC-" , mu_temp);
			//mu += mu_step;
			//current_iter++;

		}

	}

}




//Performs a set of experiments with an embedded model
void launch_embedded_experience(const std::string &collection_file , const std::string &queries_file , const std::string &index_file , const std::string &res_file , const std::string &collection_cosine_file , const std::string &queries_cosine_file , const double &mu , const double &mu_step , const int &nb_iter_mu , const int k , const double &threshold, const double &threshold_step , const int &nb_iter_threshold , const double &alpha, const double &alpha_step , const int &nb_iter_alpha ){

	std::unordered_map< int , std::unordered_map<int,double> > all_cos;
	std::unordered_map<int,double> all_sum_cos;
	std::unordered_map< int , std::vector<int> > collection;
	std::unordered_map< int , std::vector<int> > queries;
	std::unordered_map <std::string,int> index;
	std::unordered_map <int,int> cf;

	std::cout<< "Total number of iterations : " << nb_iter_mu*nb_iter_threshold*nb_iter_alpha <<std::endl;

	double threshold_max = threshold+threshold_step*(nb_iter_threshold-1);

	read_all_info_and_index2( collection_file , queries_file , index_file , collection_cosine_file , queries_cosine_file , collection , queries , index, cf , all_cos , all_sum_cos , threshold_max);

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

					double mu_temp = mu + mu_step*current_iter_mu;

					double alpha_temp = alpha + current_iter_alpha*alpha_step;

					delete_low_similarities(collection_cosine_file , queries_cosine_file , all_cos , all_sum_cos , threshold_temp);
					std::vector< std::vector< std::pair<int,double> > > results = Dirichlet_embedding_model(mu_temp , queries , collection , cf , all_sum_cos , all_cos , k , nb_words , alpha_temp);
					std::cout<< "Performed all the queries for mu = "<<mu_temp<< " , for alpha = "<< alpha_temp <<" and the threshold = " << threshold_temp <<std::endl;
					std::string file_name = res_file;
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

#endif
