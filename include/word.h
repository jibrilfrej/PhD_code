#ifndef word_h
#define word_h

#include <string>
#include <cstring>
#include <cstdio>

const long long max_w = 128;              // max length of vocabulary entries

// A simple word
class Word {

	private:
	  char data[max_w];

	public:
	  Word(){}
	  char& operator[](size_t i){return data[i];}
	  bool operator!=(const char* w){return strcmp(data,w)!=0;}
	  bool operator==(const char* w){return strcmp(data,w)==0;}
	  const char* c_str()const{return data;}
	  const std::string to_string()const{return std::string(data);}

};

#endif
