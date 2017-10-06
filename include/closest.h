#ifndef closest_h
#define closest_h


#include "embedding.h"
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <omp.h>



//Returns the terms the have a higher similarity than a given threshold of a given term in the vocabulary
std::unordered_map<std::string,double> closest_terms(const std::string &term , const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::unordered_map<std::string,double> most_sim;

	if(embedding.get(term.c_str())==nullptr){return most_sim;}

	float cos;

	auto iterator = cf.begin();
	while(iterator != cf.end()){

		cos =embedding.cosine(term.c_str() , iterator->first.c_str());

		if( cos > threshold && term != iterator->first){

			most_sim[iterator->first.c_str()] = cos;

		}

		iterator++;

	}

	return most_sim;

}


//Same as before but over the entire vocabulary
std::unordered_map< std::string , std::unordered_map<std::string,double> > closest_terms(const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::unordered_map< std::string , std::unordered_map<std::string,double> > set_most_sim;

	std::vector<std::string> temp;

	auto iterator = cf.begin();

	while(iterator != cf.end()){

		temp.push_back(iterator->first);
		iterator++;

	}

	int nthreads, tid;

	#pragma omp parallel private(tid) shared(temp , cf , embedding , threshold)
    {
        tid = omp_get_thread_num();

        std::unordered_map< std::string , std::unordered_map<std::string,double> > mapLocal;

        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            std::cout<<"Number of thread: "<<nthreads<<std::endl;
        }

		#pragma omp for schedule(static)
		for(unsigned int i = 0 ; i < temp.size() ; i++){

		//while(iterator != cf.end()){

			//set_most_sim[iterator->first] = closest_terms(iterator->first , cf , embedding , threshold);
			mapLocal[temp[i]] = closest_terms(temp[i] , cf , embedding , threshold);

			//iterator++;
		}



		#pragma omp critical
        {
            std::cout<<"Fusion "<<tid<<" map size="<<mapLocal.size()<<std::endl;
            set_most_sim.insert(mapLocal.begin(),mapLocal.end());

        }


	}

	return set_most_sim;

}






//Same as before but over the queries
std::unordered_map< std::string , std::unordered_map<std::string,double> > closest_terms(const std::unordered_map< std::string , std::vector<std::string> > &queries , const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::unordered_map< std::string , std::unordered_map<std::string,double> > set_most_sim;

	int compteur = 0;

	auto iterator = queries.begin();

	while(iterator != queries.end()){

		for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

			set_most_sim[iterator->second[j]] = closest_terms(iterator->second[j] , cf , embedding , threshold);

		}

		compteur++;

		std::cout<<"\rProgress : ("<< compteur <<"/" << queries.size() << ")"<<std::flush;

		iterator++;

	}

	std::cout << std::endl;

	return set_most_sim;

}




//Same as before but over the queries
std::unordered_map< std::string , std::unordered_map<std::string,double> > closest_terms(const std::unordered_map< int , std::vector<std::string> > &queries , const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::unordered_map< std::string , std::unordered_map<std::string,double> > set_most_sim;

	int compteur = 0;

	auto iterator = queries.begin();

	while(iterator != queries.end()){

		for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

			set_most_sim[iterator->second[j]] = closest_terms(iterator->second[j] , cf , embedding , threshold);

		}

		compteur++;

		std::cout<<"\rProgress : ("<< compteur <<"/" << queries.size() << ")"<<std::flush;

		iterator++;

	}

	return set_most_sim;

}


//Returns the terms the have a higher similarity than a given threshold of a given term in the vocabulary
std::unordered_map<std::string,double> closest_terms_sum_query(const std::vector<std::string> &query , const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::unordered_map<std::string,double> most_sim;

	if(embedding.check_embedding(query) < 2){return most_sim;}

	float cos;

	auto iterator = cf.begin();
	while(iterator != cf.end()){

		cos =embedding.cosine(query , iterator->first.c_str());

		if( cos > threshold ){

			most_sim[iterator->first.c_str()] = cos;

		}

		iterator++;

	}

	return most_sim;

}



//Same as before but over the queries
std::unordered_map< int , std::unordered_map<std::string,double> > closest_terms_sum_query(const std::unordered_map< int , std::vector<std::string> > &queries , const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::unordered_map< int , std::unordered_map<std::string,double> > set_most_sim;

	int compteur = 0;

	auto iterator = queries.begin();

	while(iterator != queries.end()){


		set_most_sim[iterator->first] = closest_terms_sum_query(iterator->second , cf , embedding , threshold);

		compteur++;

		std::cout<<"\rProgress : ("<< compteur <<"/" << queries.size() << ")"<<std::flush;

		iterator++;

	}

	return set_most_sim;

}

#endif
