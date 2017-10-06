#ifndef frequency_h
#define frequency_h

#include "character.h"
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>

//Builds the vocabulary given the collection in input and stores it in an std::unordered_map of <std::string,int> the std::string being the words and the int their associated collection frequency
std::unordered_map <std::string,int> build_cf(const std::unordered_map< int , std::vector<std::string> > &collection){

	std::unordered_map <std::string,int>  cf;

	auto iterator = collection.begin();

	while( iterator != collection.end() ){

		for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

			if(iterator->second[j].size() > 1 || isValidChar(iterator->second[j][0])){
				cf[ iterator->second[j] ]++;
			}
		}

		iterator++;

	}

	return cf;

}

//Given the collection and the vocabulary in input computes and store the document frequency of the terms of the vocabulary in an std::unordered_map
std::unordered_map <std::string,int> build_df(const std::unordered_map< int , std::vector<std::string> > &collection, const std::unordered_map <std::string,int> &cf){

	std::unordered_map <std::string,int>  df;

	std::unordered_map<std::string,int>::const_iterator p = cf.begin();

	std::pair<std::string,int> temp("",0);

	while( p!=  cf.end() ){

		temp.first = p->first;
		df.insert(temp);
		p++;

	}

	std::vector<std::string> doc_memory;

	auto iterator = collection.begin();

	while( iterator != collection.end() ){

		for(unsigned int j = 0 ; j < iterator->second.size() ; j++){

			if( count( doc_memory.begin() , doc_memory.end() , iterator->second[j] ) == 0 && (df.find(iterator->second[j]) != df.end())){

				df[ iterator->second[j] ]++;
				doc_memory.push_back(iterator->second[j]);

			}

		}

		doc_memory.clear();

		iterator++;

	}

	return df;

}







//Return the tf of a term in a document
int term_freq(const std::vector<std::string> &doc , const std::string &term){

	return count(doc.begin() , doc.end() , term);

}

//Return the collection frequency corresponding to the term in input
int coll_freq(const std::unordered_map <std::string,int>  &cf , const std::string &term){

	std::unordered_map<std::string,int>::const_iterator q = cf.find(term);

	if(q == cf.end() || q->second == -1){return 0;}
	else{return q->second;}

}
#endif
