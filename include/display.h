#ifndef display_h
#define display_h

#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>

//Displays a document/query
void display(const std::vector<std::string> &sentence){

	for(unsigned int i = 0 ; i < sentence.size() ; i++){
		std::cout<<sentence[i]<<" ";
	}
	std::cout<<std::endl;
}

//Displays a document/query
void display(const std::vector<int> &sentence){

	for(unsigned int i = 0 ; i < sentence.size() ; i++){
		std::cout<<sentence[i]<<" ";
	}
	std::cout<<std::endl;
}

//display the contend of an unordered_map<string,double>
void display_map(const std::unordered_map<std::string , double> &map){

	auto iterator = map.begin();

	while(iterator != map.end()){

		std::cout<< iterator->first << " : " << iterator->second <<std::endl;
		iterator++;

	}

}


//display the contend of an unordered_map<int,double>
void display_map(const std::unordered_map<int , double> &map){

	auto iterator = map.begin();

	while(iterator != map.end()){

		std::cout<< iterator->first << " : " << iterator->second <<std::endl;
		iterator++;

	}

}


//display the contend of an unordered_map<string,unordered_map<string,double>>
void display_map_map(const std::unordered_map< std::string , std::unordered_map<std::string,double> > &super_map){

	auto iterator = super_map.begin();

	while(iterator != super_map.end()){

		std::cout<<"Terme : "<< iterator->first <<std::endl;
		display_map( iterator->second );

		iterator++;

	}

}


//Displays all the words in the collection that have a higher frequency than top
int display_top_frequent_words(const int top , const std::unordered_map <std::string,int>  &cf){

	std::unordered_map<std::string,int>::const_iterator p = cf.begin();

	int nb_words = 0;

	while( p!=  cf.end() ){

		nb_words += p->second;

		if(p->second > top){

			std::cout<<"Word of the vocabulary and it's frequency : ( "<< p->first <<" , "<< p->second <<" )"<<std::endl;

		}
		p++;

	}

	std::cout<<"Sum of all the values : " << nb_words <<std::endl;

	return nb_words;

}




//display stuff
size_t display_stuff(const std::unordered_map< int , std::vector<std::string> > &collection , const std::unordered_map< int , std::vector<std::string> > &queries , const std::unordered_map <std::string,int> &cf , const std::unordered_map <std::string,int> &df){


	size_t queries_size = 0;

	auto iterator = queries.begin();

	while( iterator != queries.end() ){

		queries_size += iterator->second.size();

		iterator++;

	}
	std::cout<< "Size of the collection : "<< collection.size() <<std::endl;
	std::cout<<"Number of queries : "<< queries.size() <<std::endl;
	std::cout<<"Number of words in the queries : "<< queries_size <<std::endl;
	std::cout<<"Size of the vocabulary : "<< cf.size() <<std::endl;
	std::cout<<"Size of the vocabulary : "<< df.size() <<std::endl;

	size_t nb_words = 0;
	std::unordered_map<std::string,int>::const_iterator p = cf.begin();

	while( p!=  cf.end() ){
		nb_words += p->second;
		p++;
	}

	std::cout<<"Number of words in the collection : " << nb_words <<std::endl;
	std::cout<<"**********************************************************"<<std::endl;
	std::cout<<"First document : "<<std::endl;
	display(collection.find(1)->second);
	std::cout<<"**********************************************************"<<std::endl;
	std::cout<<"Last document : "<<std::endl;
	display(collection.find(collection.size()-1)->second);
	std::cout<<"**********************************************************"<<std::endl;

	return nb_words;

}




#endif
