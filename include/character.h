#ifndef character_h
#define character_h

#include <iostream>
#include <cstring>
#include <vector>
#include <unordered_map>

//Check if the input char is a number or a letter
inline bool isValidChar(const char c){return((c>47&&c<58)||(c>64&&c<91)||(c>96&&c<123));}

//Check if the input char is a number
inline bool isNumber(const char c){return((c>47&&c<58));}

//Check if the input char is a number
inline bool isSmallNumber(const char c){return((c>47&&c<51));}

//Check if the input char is a majuscule letter
inline bool isMajChar(const char c){return(c>64&&c<91);}

inline bool isMinChar(const char c){return(c>96&&c<123);}

//Check if the token is valid : no more that 4 digits and no more than 3 identical successiv terms
bool isValidtoken(const char* token){

	if(token=='\0'){return false;}

	size_t token_size = strlen(token);

	if( token_size > 20){return false;}

	int number_count = 0;

	int number_identical_successiv_char = 1;

	for(unsigned int i = 0 ; i < token_size ; i ++){

		if( isNumber(token[i]) ){number_count++;}

		if(i > 0 && token[i]==token[i-1]){number_identical_successiv_char++;}

		if(number_identical_successiv_char > 3 || number_count > 4){return false;}

		if(i > 0 && token[i]!=token[i-1]){number_identical_successiv_char = 1;}

	}

	return true;

}

//Check if the token is valid : no more that 4 digits and no more than 3 identical successiv terms
bool isValidtokendisplay(const char* token){

	int number_count = 0;
	int number_identical_successiv_char = 1;

	for(unsigned int i = 0 ; i < strlen(token) ; i ++){

		if( isNumber(token[i]) ){number_count++;}

		if(i > 0 && token[i]==token[i-1]){number_identical_successiv_char++;}

		if(number_identical_successiv_char > 3 || number_count > 4){return false;}

		if(i > 0 && token[i]!=token[i-1]){number_identical_successiv_char = 1;}

	}

	std::cout<<"Considered term : "<< token<< " associated number of digits : "<< number_count <<std::endl;

	return true;

}

//display a string and its associated number of digits
void nb_digits(const char* token){

	int number_count = 0;

	for(unsigned int i = 0 ; i < strlen(token) ; i++){

		if( isNumber(token[i]) ){number_count++;}

	}

	std::cout<<"Considered term : "<< token<< " associated number of digits : "<< number_count <<std::endl;

}

//Same as before but for each term of a document
void nb_digits(const std::vector<const char*> &document){

	for(unsigned int i = 0 ; i < document.size() ; i++){

		nb_digits(document[i]);

	}

}

//Same as before but for the entire collection
void nb_digits(const std::unordered_map< int , std::vector<const char*> > &collection){


	auto iterator = collection.begin();

	while(iterator != collection.end()){

		nb_digits(iterator->second);
		iterator++;

	}

}


#endif
