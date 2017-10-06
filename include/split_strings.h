#ifndef split_strings_h
#define split_strings_h

#include "character.h"
#include <cmath>
#include <utility>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>



//Take a std::string as input and return a std::vector of std::strings using the delimiter
std::vector<std::string> easy_split(const std::string &str, const char delimiter) {

  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  tok[0] = 0;
  while(getline(ss, tok, delimiter)) {
			internal.push_back(tok);
  }

  return internal;
}

//Take a std::string as input and return a pair<std::string,int>
std::pair<std::string,int> tf_split(const std::string &str, const char delimiter) {

  std::pair<std::string,int> internal;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  tok[0] = 0;
  getline(ss, tok, delimiter);
  internal.first = tok;
  getline(ss, tok, delimiter);
  internal.second = stoi(tok);

  return internal;
}





//Take a std::string as input and return a std::vector of std::strings using the delimiter also get rides of the caracters that are not numbers and letters
std::vector<std::string> split(const std::string &str, const char delimiter) {

  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  tok[0] = 0;
  unsigned int i = 0;
  while(getline(ss, tok, delimiter)) {
		while(tok.size() > 1 && !isValidChar(tok[0]) ){
			tok.erase(0);
		}
		if(tok.size()>1){
			i = 0;
			while(i < tok.size()){
				//if(!isValidChar(tok[i]) && tok[i]!='-'){
				if( !isValidChar(tok[i]) ){
					internal.push_back(tok.substr(0,i));
					tok = tok.substr(i+1);
					while(tok.size() > 1 && !isValidChar(tok[0]) ){
						tok.erase(0);
					}
					i = -1;
				}
				i++;
			}
		}

		while(tok.size() > 1 && !isValidChar(tok[0]) ){
			tok.erase(0);
		}

		while(tok.size() > 1 && !isValidChar(tok[tok.size()-1])){
			tok.erase(tok.size()-1);
		}
		if(tok.size() != 0 && isValidChar(tok[0])){
			internal.push_back(tok);
		}
  }

  return internal;
}








//Take a std::string as input and return a std::vector of std::strings using the delimiter also get rides of the caracters that are not numbers and letters and changes majuscules into minuscules
std::vector<std::string> split_maj(const std::string &str, const char delimiter) {

  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  std::string sub_tok;
  tok[0] = 0;
  unsigned int i = 0;
  while(getline(ss, tok, delimiter)) {
		while(tok.size() > 1 && !isValidChar(tok[0]) ){
			tok.erase(0);
		}
		if(isMajChar(tok[0])){tok[0] = tok[0] + 32;}
		if(tok.size()>1){
			i = 0;
			while(i < tok.size()){
				if(isMajChar(tok[i])){tok[i] = tok[i] + 32;}
				//if(!isValidChar(tok[i]) && tok[i]!='-'){
				if( !isValidChar(tok[i]) ){
					sub_tok = tok.substr(0,i);
					if(isValidtoken(sub_tok.c_str())){internal.push_back(sub_tok);}
					tok = tok.substr(i+1);
					while(tok.size() > 1 && !isValidChar(tok[0]) ){
						tok.erase(0);
					}
					i = -1;
				}
				i++;
			}
		}

		while(tok.size() > 1 && !isValidChar(tok[0]) ){
			tok.erase(0);
		}

		while(tok.size() > 1 && !isValidChar(tok[tok.size()-1])){
			tok.erase(tok.size()-1);
		}
		if(tok.size() != 0 && isValidChar(tok[0]) && isValidtoken(tok.c_str()) ){
			internal.push_back(tok);
		}
  }

  return internal;
}



//Take a std::string as input and return a std::vector of std::strings using the delimiter also get rides of the caracters that are not numbers and letters and changes majuscules into minuscules
std::vector<std::string> skip(const std::string &str, const char delimiter) {

  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  std::string sub_tok;
  tok[0] = 0;
  unsigned int i = 0;
  while(getline(ss, tok, delimiter)) {
		i++;
  }

  return internal;
}


//Take a std::string as input and return a std::vector of std::strings using the delimiter also get rides of the caracters that are not numbers and letters and changes majuscules into minuscules
std::vector<std::string> split_maj_display(const std::string &str, const char delimiter) {

  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  tok[0] = 0;
  unsigned int i = 0;
  while(getline(ss, tok, delimiter)) {
		while(tok.size() > 1 && !isValidChar(tok[0]) ){
			tok.erase(0);
		}
		if(isMajChar(tok[0])){tok[0] = tok[0] + 32;}
		if(tok.size()>1){
			i = 0;
			while(i < tok.size()){
				if(isMajChar(tok[i])){tok[i] = tok[i] + 32;}
				//if(!isValidChar(tok[i]) && tok[i]!='-'){
				if( !isValidChar(tok[i]) && isValidtokendisplay(tok.substr(0,i).c_str()) ){

					internal.push_back(tok.substr(0,i));
					tok = tok.substr(i+1);
					while(tok.size() > 1 && !isValidChar(tok[0]) ){
						tok.erase(0);
					}
					i = -1;
				}
				i++;
			}
		}

		while(tok.size() > 1 && !isValidChar(tok[0]) ){
			tok.erase(0);
		}

		while(tok.size() > 1 && !isValidChar(tok[tok.size()-1])){
			tok.erase(tok.size()-1);
		}
		if(tok.size() != 0 && isValidChar(tok[0]) && isValidtokendisplay(tok.c_str() )){
			internal.push_back(tok);
		}
  }

  return internal;
}




#endif
