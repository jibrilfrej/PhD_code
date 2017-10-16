#ifndef tool_h
#define tool_h

#include "character.h"
#include "display.h"
#include "embedding.h"
#include <cmath>
#include <utility>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <dirent.h>

#define err1 0x0000 // la fonction lecture_fichier_bin n a pas trouve le repertoire en entree


//Return the smallest closest integer as a double (except if it is 0)
inline double myround(double x)
{
  return x < 1 ? ceil(x) : ceil(x)+1;
}

//Compare the two pairs in input by comparing their second element
bool compare_pairs(const std::pair<int, double> &p1 , const std::pair<int, double> &p2) { return (p1.second < p2.second); }



//Aggregate map1 and map2 into map1
void aggregate_map( std::unordered_map<std::string,double> &map1 , const std::unordered_map<std::string,double> &map2){

	auto iterator = map2.begin();

	while(iterator != map2.end()){


		if(map1.find(iterator->first)==map1.end()){map1[iterator->first]=iterator->second;}
		iterator++;

	}

}



//Aggregate two maps maps together
void aggregate_map( std::unordered_map< std::string , std::unordered_map<std::string,double> > &map1 , const std::unordered_map< std::string , std::unordered_map<std::string,double> > &map2){

	auto iterator = map2.begin();

	while(iterator != map2.end()){

		if(map1.find(iterator->first)==map1.end()){map1[iterator->first]=iterator->second;}
		iterator++;

	}

}


//Aggregate map1 and map2 into map1
void aggregate_map( std::unordered_map<int,double> &map1 , const std::unordered_map<int,double> &map2){

	auto iterator = map2.begin();

	while(iterator != map2.end()){


		if(map1.find(iterator->first)==map1.end()){map1[iterator->first]=iterator->second;}
		iterator++;

	}

}



//Aggregate two maps maps together
void aggregate_map( std::unordered_map< int , std::unordered_map<int,double> > &map1 , const std::unordered_map< int , std::unordered_map<int,double> > &map2){

	auto iterator = map2.begin();

	while(iterator != map2.end()){

		if(map1.find(iterator->first)==map1.end()){map1[iterator->first]=iterator->second;}
		iterator++;

	}

}


std::unordered_map<int,std::string> switch_index(const std::unordered_map<std::string,int> &index){

  std::unordered_map<int,std::string> switched_index;
  auto iterator = index.begin();

  while (iterator!=index.end()) {

    switched_index[iterator->second] = iterator->first;

    iterator++;
  }

  return switched_index;

}


//Deletes any big number in the collection
void check_numbers(std::unordered_map< int , std::vector<std::string>  > &collection){

	auto iterator = collection.begin();

	while(iterator != collection.end()){

		if(iterator->second.size() > 0){

			for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

				if( (iterator->second[j].size() > 4 && isNumber(iterator->second[j][0]) && isNumber(iterator->second[j][4]) )  ){iterator->second.erase(iterator->second.begin() + j);}

			}

		}

		iterator++;

	}

}


//Deletes any empty term of a non empty document in the collection
void check_collection(std::unordered_map< int , std::vector<std::string>  > &collection){

	auto iterator = collection.begin();

	while(iterator != collection.end()){

		if(iterator->second.size() > 0){

			for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

				if(iterator->second[j] == "\0"  ){iterator->second.erase(iterator->second.begin() + j);}

			}

		}

		iterator++;

	}

}


//Return the neighbouring terms of the input term
std::vector<std::string> neighbors(const std::vector<std::string> &query , const unsigned int term_position ,const int size_neighborhood){

	unsigned int pos = 0;

	if(term_position - size_neighborhood > 0){pos = term_position - size_neighborhood;}

	std::vector<std::string> neighborhood;

	while(pos < term_position + size_neighborhood + 1 && pos < query.size()){

		neighborhood.push_back(query[pos]);
		pos++;

	}

	return neighborhood;

}





//Takes as an input an unordered map of pairs of <int,double> that correspond to the id of the documents and their associated score and returns a std::vector of pairs containing the k documents that have the highest score
std::vector< std::pair<int,double> > kfirst_docs(const std::unordered_map<int,double> &unsorted, const int k){

	std::vector< std::pair<int,double> > res;

	if(unsorted.size() == 0){return res;}

	std::pair<int,double> temp;

	std::unordered_map<int,double>::const_iterator p = unsorted.begin();

	while(p != unsorted.end()){

		temp.first = p->first;
		temp.second = p->second;
		res.push_back(temp);
		p++;

	}
	std::sort(res.begin(), res.end(),compare_pairs);

	if(k != -1 && k < (int)res.size() ){
		res.assign(res.end()-k,res.end());

	}

	std::reverse(res.begin() , res.end());

	return res;

}




//Return the cosine similarity between two terms ; return 0 if one of the term is not in the input map
double fast_cos(const int term1 , const int term2 , const std::unordered_map< int , std::unordered_map<int,double> > &set_cos){

	if(term1==term2){return 1;}

	auto iterator = set_cos.find( term1 );

	if(iterator != set_cos.end()){

		auto iterator2 = iterator->second.find(term2);

		if(iterator2 == iterator->second.end()){return 0;}

		else{return iterator2->second;}

	}

	else return 0;

}




//Read the sum of the similarities between a term and all the terms of a given set of terms by using a map
double fast_sum_proba_pairs(const int term , const std::vector<int> &set , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map){

	double sum = 0;

	for(unsigned int i = 0 ; i < set.size() ; i++){

		sum += fast_cos(term , set[i] , cosine_map);

	}

	return sum;

}



//Read a map and computes the sum of all the cosine similarities of a given term
double fast_sum_cos_sim(const std::string &term , const std::unordered_map< std::string , std::unordered_map<std::string,double> > &cosine_map){

	auto iterator = cosine_map.find( term );

	if(iterator != cosine_map.end()){

		auto iterator2 = iterator->second.begin();

		double sum = 0;

		while( iterator2 != iterator->second.end() ){

			sum+=iterator2->second;
			iterator2++;

		}

		return sum;

	}

	else{return 0;}

}


//Read a map and computes the sum of all the cosine similarities of a given term
double fast_sum_cos_sim(const int term , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map){

	auto iterator = cosine_map.find( term );

	if(iterator != cosine_map.end()){

		auto iterator2 = iterator->second.begin();

		double sum = 0;

		while( iterator2 != iterator->second.end() ){

			sum+=iterator2->second;
			iterator2++;

		}

		return sum;

	}

	else{return 0;}

}



//Read a map and computes and store into a map the sum of all the cosine similarities of all the terms in the input map
std::unordered_map<std::string , double> all_fast_sum_cos_sim(const std::unordered_map< std::string , std::unordered_map<std::string,double> > &cosine_map){

	std::unordered_map<std::string , double> res;

	auto iterator = cosine_map.begin();

	while(iterator != cosine_map.end()){


		res[iterator->first] = fast_sum_cos_sim( iterator->first , cosine_map );
		iterator++;

	}

	return res;

}


//Read a map and computes and store into a map the sum of all the cosine similarities of all the terms in the input map
std::unordered_map<int , double> all_fast_sum_cos_sim(const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map){

	std::unordered_map<int , double> res;

	auto iterator = cosine_map.begin();

	while(iterator != cosine_map.end()){


		res[iterator->first] = fast_sum_cos_sim( iterator->first , cosine_map );
		iterator++;

	}

	return res;

}



double fast_sum_translation_proba(const std::string &term1 , const std::string &term2 , const std::unordered_map< std::string , std::unordered_map<std::string,double> > &sum_proba ){

	auto iterator = sum_proba.find(term1);

	if(iterator == sum_proba.end()){return 0;}

	auto iterator2 = iterator->second.find(term1);

	if(iterator2 == iterator->second.end() ){

		return 0;

	}

	else{return iterator2->second;}

}


//Computes the translation probability of a term2 into term1 : p( term1 | term2 )
double translation_proba(const int term1 , const int term2 , const std::unordered_map<int , double> &sum_cosine_map , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map ){

	if(term1==term2){return 1;}

	//else{return 0;}

	auto iterator = sum_cosine_map.find(term2);

	if(iterator == sum_cosine_map.end() || iterator->second == 0){

		return fast_cos(term1 , term2 , cosine_map);

	}

	else{

		return  fast_cos(term1 , term2 , cosine_map)/iterator->second;

	}

}




//Computes the probability that term was generated by the document p(term | document)
double proba_doc_generate_term(const int term , const std::vector<int> &document , const std::unordered_map<int , double> &sum_cosine_map , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map ){

	double proba = 0;

	for(unsigned int i = 0 ; i < document.size() ; i++){

		proba+=translation_proba(term , document[i] , sum_cosine_map , cosine_map )/document.size();

	}

	return proba;

}



//Computes the probability that term was generated by the document p(term | document) by taking into account the self translation probability
double proba_doc_generate_term(const int term , const std::vector<int> &document , const std::unordered_map<int , double> &sum_cosine_map , const std::unordered_map< int , std::unordered_map<int,double> > &cosine_map , const double &alpha ){

	double proba = 0;

	for(unsigned int i = 0 ; i < document.size() ; i++){

		if(term == document[i]){

			proba+= (alpha + (1 - alpha)*translation_proba(term , document[i] , sum_cosine_map , cosine_map ))/document.size();

		}

		else{

			proba+=(1 - alpha)*translation_proba(term , document[i] , sum_cosine_map , cosine_map )/document.size();

		}

	}

	return proba;

}




//Read a map and computes the sum of all the cosine similarities of a given term
double fast_cos_sum_queries(const int query_id , const std::string &term , const std::unordered_map< int , std::unordered_map<std::string,double> > &all_cos_sum_queries){

	auto iterator = all_cos_sum_queries.find( query_id );

	if(iterator != all_cos_sum_queries.end()){

		auto iterator2 = iterator->second.find(term);

		if(iterator2 != iterator->second.end() ){

			return iterator2->second;

		}

	}

	return 0;

}



//Computes the translation probability of a term2 into phrase : p( phrase | term2 )
double translation_proba(const int query_id , const std::string &term2 , const std::unordered_map<std::string , double> &sum_cosine_map , std::unordered_map< int , std::unordered_map<std::string,double> > &all_cos_sum_queries){

	//if(term1==term2){return 1;}

	//else{return 0;}

	auto iterator = sum_cosine_map.find(term2);

	if(iterator == sum_cosine_map.end() || iterator->second == 0){

		return fast_cos_sum_queries(query_id , term2 , all_cos_sum_queries);

	}

	else{

		return  fast_cos_sum_queries(query_id , term2 , all_cos_sum_queries)/iterator->second;

	}

}




//Computes the probability that the phrase was generated by the document p(term | document)
double proba_doc_generate_term(const int query_id , const std::vector<std::string> &document , const std::unordered_map<std::string , double> &sum_cosine_map , std::unordered_map< int , std::unordered_map<std::string,double> > &all_cos_sum_queries){

	double proba = 0;

	for(unsigned int i = 0 ; i < document.size() ; i++){

		proba+=translation_proba( query_id , document[i] , sum_cosine_map , all_cos_sum_queries )/document.size();

	}

	return proba;

}


//Return the path and name of all the files contained in the directory in input. Does not work if there is a directory in the directory in input.
std::vector<std::string> ensemble_fichier(const char* chemin)
{

	DIR* rep = NULL;
	struct dirent* fichierLu = NULL;
	std::string chemin_rep(chemin);
	std::string temp;
	std::vector<std::string> ensemble_nom_fichiers;

	rep = opendir(chemin);

	if(rep == NULL)
	{

		throw int(err1);

	}

	fichierLu = readdir(rep);
	fichierLu = readdir(rep);

	while ((fichierLu = readdir(rep)) != NULL)
	{

		temp = chemin;
		temp += "/";
		temp += fichierLu->d_name;
		ensemble_nom_fichiers.push_back(temp);

	}

	if(closedir(rep) == -1)
	{

		exit(-1);

	}

	free(fichierLu);

	return ensemble_nom_fichiers;
}


//return the size of the collection
size_t get_size_collection( const std::unordered_map <std::string,int> &cf ){

	size_t nb_words = 0;
	std::unordered_map<std::string,int>::const_iterator p = cf.begin();

	while( p!=  cf.end() ){
		nb_words += p->second;
		p++;
	}
	return nb_words;

}

//return the size of the collection
size_t get_size_collection( const std::unordered_map <int,int> &cf ){

	size_t nb_words = 0;
	std::unordered_map<int,int>::const_iterator p = cf.begin();

	while( p!=  cf.end() ){
		nb_words += p->second;
		p++;
	}
	return nb_words;

}

//count and displays the number of words in the vocabulary that have an embedding
int nb_embedded_words_in_voc(Embedding &embedding , const std::unordered_map <std::string,int> &index, const std::unordered_map <int,int> &cf){

	int count = 0;
	int nb_words_tot = 0;
	int	nb_words_emb = 0;
	int nb_iter = 0;

	std::unordered_map<std::string,int>::const_iterator p = index.begin();

	while( p!=  index.end() ){

		if( embedding.get(p->first.c_str()) != nullptr ){

			count++;
			nb_words_emb += cf.at(p->second);

		}

		nb_words_tot += cf.at(p->second);
		nb_iter++;
		p++;

	}

	std::cout<<"Percentage of words of the vocabulary that have an embedding : "<< double(count)*100/cf.size()<<"%"<<std::endl;
	std::cout<<"Percentage of words of the collection that have an embedding : "<< double(nb_words_emb)*100/nb_words_tot<<"%"<<std::endl;

	return count;

}

//count the number of words in the queries that have an embedding
int nb_embedded_words_in_queries(Embedding &embedding , const std::unordered_map< int , std::vector<std::string> > &queries , const bool displ){

	unsigned int count = 0;
	unsigned int nb_words_tot = 0;
	unsigned int temp = 0;
	unsigned int nb_empty_queries = 0;
	unsigned int nb_full_queries = 0;

	auto iterator = queries.begin();

	while(iterator != queries.end()){

		temp = iterator->second.size();

		for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

			if( embedding.get(iterator->second[j].c_str()) != nullptr ){
				count++;
				temp--;

			}

			else if(displ==true){

					//std::cout<<queries[i][j]<<std::endl;

				}

			nb_words_tot++;

		}

		if(temp == iterator->second.size()){

			nb_empty_queries++;
			if(displ == true){
				std::cout<<"Empty Query "<<iterator->first + 1<<" ";
				display(iterator->second);
			}

		}


		if(temp == 0){

			nb_full_queries++;
			if(displ == true){
				std::cout<<"Full Query "<<iterator->first + 1<<" ";
				display(iterator->second);
			}

		}

		iterator++;

	}

	std::cout<<"Percentage of words of the queries that have an embedding : "<< double(count)*100/nb_words_tot<<"%"<<std::endl;
	std::cout<<"Percentage of queries that does not have any term that have an embedding : "<< double(nb_empty_queries)*100/queries.size()<<"%"<<std::endl;
	std::cout<<"Percentage of queries that have all their terms that have an embedding : "<< double(nb_full_queries)*100/queries.size()<<"%"<<std::endl;

	return count;

}


//count the number of words in the queries that have an embedding
int nb_embedded_words_in_queries(Embedding &embedding , const std::unordered_map< int , std::vector<int> > &queries , const std::unordered_map<std::string,int> &index , const bool displ){

	unsigned int count = 0;
	unsigned int nb_words_tot = 0;
	unsigned int temp = 0;
	unsigned int nb_empty_queries = 0;
	unsigned int nb_full_queries = 0;

  std::unordered_map<int,std::string> index_temp = switch_index(index);

  std::string str;

	auto iterator = queries.begin();

	while(iterator != queries.end()){

		temp = iterator->second.size();

		for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

			if( embedding.get(index_temp[ iterator->second[j] ].c_str()) != nullptr ){
				count++;
				temp--;

			}

			else if(displ==true){

					//std::cout<<queries[i][j]<<std::endl;

				}

			nb_words_tot++;

		}

		if(temp == iterator->second.size()){

			nb_empty_queries++;
			if(displ == true){
				std::cout<<"Empty Query "<<iterator->first + 1<<" ";
				display(iterator->second);
			}

		}


		if(temp == 0){

			nb_full_queries++;
			if(displ == true){
				std::cout<<"Full Query "<<iterator->first + 1<<" ";
				display(iterator->second);
			}

		}

		iterator++;

	}

	std::cout<<"Percentage of words of the queries that have an embedding : "<< double(count)*100/nb_words_tot<<"%"<<std::endl;
	std::cout<<"Percentage of queries that does not have any term that have an embedding : "<< double(nb_empty_queries)*100/queries.size()<<"%"<<std::endl;
	std::cout<<"Percentage of queries that have all their terms that have an embedding : "<< double(nb_full_queries)*100/queries.size()<<"%"<<std::endl;

	return count;

}


#endif
