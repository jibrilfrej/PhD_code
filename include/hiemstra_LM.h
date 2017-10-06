#ifndef hiemstra_LM_h
#define hiemstra_LM_h


#include "frequency.h"
#include "tool.h"
#include <cstring>
#include <vector>
#include <unordered_map>
//#include <algorithm>



//Same as before but with a smoothing that takes into account the collection frequency
double Hiemstra_language_model(const std::vector<std::string> &query , const std::vector<std::string> &document , const std::unordered_map <std::string,int>  &cf , const int collection_size , const double lambda){

	size_t doc_length = document.size();

	if(doc_length == 0){return 0;}

	double doc_proba;

	double coll_proba;

	double proba = 0;

	for(unsigned int i  = 0 ; i < document.size() ; i++){

		if(document[i]=="\0"){std::cout<<"|"<<document[i]<<"|"<<" and size of the doc : "<<document.size()<<std::endl;}

	}

	for(unsigned int i = 0 ; i < query.size() ; i++){

			coll_proba =  (1 - lambda)*( (double)coll_freq(cf , query[i])/collection_size );

			if(coll_proba!=0){

				doc_proba = (lambda)*( (double)term_freq(document , query[i])/doc_length );

				proba += log(1 + doc_proba/coll_proba)/log(2);

			}

	}

	return proba;

}



//Same as before but with a smoothing that takes into account the collection frequency
std::unordered_map <int,double> Hiemstra_language_model(const std::vector<std::string> &query , const std::unordered_map < int , std::vector<std::string> > &collection , const std::unordered_map <std::string,int>  &cf , const int collection_size , const double lambda){

	std::unordered_map <int,double> list_doc;

	double proba;

	auto iterator = collection.begin();

	while(iterator != collection.end()){

		proba = Hiemstra_language_model(query , iterator->second , cf , collection_size , lambda);
		if(proba!=0){list_doc[iterator->first]=proba;}

		iterator++;

	}

	return list_doc;

}



//Same as before but with all the queries and sort documents by their score
std::vector< std::vector< std::pair<int,double> > > Hiemstra_language_model(const std::unordered_map< int , std::vector<std::string> > &queries , const std::unordered_map < int , std::vector<std::string> > &collection , const std::unordered_map <std::string,int>  &cf , const int collection_size , const int k , const double lambda){

	std::vector< std::vector< std::pair<int,double> > > list_docs(queries.size());

	auto iterator = queries.begin();

	while(iterator != queries.end()){

		//std::cout<<"\rProcessing query "<<i+1<<"/"<<queries.size()<<std::flush;

		list_docs[iterator->first] = kfirst_docs( Hiemstra_language_model(iterator->second , collection , cf , collection_size , lambda) , k );

		iterator++;

	}

	std::cout<<std::endl;

	return list_docs;

}



#endif
