#ifndef dirichlet_LM_h
#define dirichlet_LM_h


#include "frequency.h"
#include "tool.h"
#include <cstring>
#include <vector>
#include <unordered_map>
//#include <algorithm>


//Computes the log of the probability that query was generated by the document w.r.t dirichlet language model pdir(query | document)
inline
double Dirichlet_language_model(const double &mu , const std::vector<int> &query , const std::vector<int> &document , const std::unordered_map <int,int>  &cf , const int collection_size){

	if(document.size()==0){return 0;}

	double res = 0;

	//double collection_proba;

	for(unsigned int i = 0 ; i < query.size() ; i++){

		//collection_proba = (double)coll_freq(cf , query[i])/collection_size;

		//if(coll_freq(cf , query[i]) != 0){

			//res += log(mu/(mu+document.size()));

			//if(term_freq(document , query[i]) != 0){

				//res += log( 1 + term_freq(document , query[i])/(mu*(double)coll_freq(cf , query[i])/collection_size)) + log(mu/(mu+document.size()));

			//}

		//}



		if(coll_freq(cf , query[i]) != 0){

			res += log( ( term_freq(document , query[i]) + mu*((double)coll_freq(cf , query[i])/collection_size))/(document.size() + mu) );

		}

	}


	return res;

}



//Same as before but over the entire collection
std::unordered_map <int,double> Dirichlet_language_model(const double &mu , const std::vector<int> &query , const std::unordered_map< int , std::vector<int> > &collection , const std::unordered_map <int,int>  &cf , const int collection_size ){

	std::unordered_map <int,double> list_doc;

	double proba;

	auto iterator = collection.begin();

	while(iterator != collection.end()){

		proba = Dirichlet_language_model(mu , query , iterator->second , cf , collection_size);
		if(proba!=0){list_doc[iterator->first]=proba;}

		iterator++;

	}

	return list_doc;

}


//Same as before but with all the queries and sort documents by their score
std::vector< std::vector< std::pair<int,double> > > Dirichlet_language_model(const double &mu , const std::unordered_map< int , std::vector<int> > &queries , const std::unordered_map < int , std::vector<int> > &collection , const std::unordered_map <int,int>  &cf , const int k , const int collection_size){

	std::vector< std::vector< std::pair<int,double> > > list_docs(queries.size());

	auto iterator = queries.begin();

	while(iterator != queries.end()){

		//std::cout<<"\rProcessing query "<<i+1<<"/"<<queries.size()<<std::flush;

		list_docs[iterator->first] =  kfirst_docs( Dirichlet_language_model(mu , iterator->second , collection , cf , collection_size) , k );

		iterator++;

	}

	return list_docs;

}



//Computes the log of the probability that query was generated by the document w.r.t dirichlet language model  pdir(query | document)
double Dirichlet_embedding_model(const double &mu , const std::vector<int> &query , const std::vector<int> &document , const std::unordered_map <int,int>  &cf , const std::unordered_map<int , double> &sum_cosine_map , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map , const int collection_size , const double &alpha ){

	double res = 0;

	double proba;

	int collection_freq;

	for(unsigned int i = 0 ; i < query.size() ; i++){

		proba = proba_doc_generate_term(query[i] , document , sum_cosine_map , cosine_map , alpha);

		collection_freq = coll_freq(cf , query[i]);

		if(collection_freq != 0 && proba != 0){

			res += log( ( (proba*document.size()) + mu*((double)collection_freq/collection_size))/(document.size() + mu) );

		}

		else if(collection_freq != 0 && proba == 0){

			res += log((double)collection_freq/collection_size );

		}

		else if(collection_freq == 0 && proba != 0){

			res += log( proba );

		}

	}

	return res;

}



//Same as before but over the entire collection
std::unordered_map <int,double> Dirichlet_embedding_model(const double &mu , const std::vector<int> &query , const std::unordered_map< int , std::vector<int> > &collection , const std::unordered_map <int,int>  &cf , const std::unordered_map<int , double> &sum_cosine_map , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map , const int collection_size , const double &alpha){

	std::unordered_map <int,double> list_doc;

	double proba;

	auto iterator = collection.begin();

	while( iterator != collection.end() ){

		proba = Dirichlet_embedding_model(mu , query , iterator->second , cf , sum_cosine_map , cosine_map , collection_size , alpha );
		if(proba!=0){list_doc[iterator->first]=proba;}

		iterator++;

	}

	return list_doc;

}


//Same as before but with all the queries and sort documents by their score
std::vector< std::vector< std::pair<int,double> > > Dirichlet_embedding_model(const double &mu , const std::unordered_map< int , std::vector<int> > &queries , const std::unordered_map< int , std::vector<int> > &collection , const std::unordered_map <int,int>  &cf , const std::unordered_map<int , double> &sum_cosine_map , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map , const int k , const int collection_size , const double &alpha ){

	std::vector< std::vector< std::pair<int,double> > > list_docs(queries.size());

	auto iterator = queries.begin();

	while(iterator != queries.end() ){

		list_docs[iterator->first] = kfirst_docs( Dirichlet_embedding_model(mu , iterator->second , collection , cf , sum_cosine_map , cosine_map , collection_size , alpha ) , k );

		iterator++;

	}

	std::cout<<std::endl;

	return list_docs;

}


#endif
