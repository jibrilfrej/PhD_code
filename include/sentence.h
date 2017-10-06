#ifndef sentence_h
#define sentence_h

#include <vector>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <iostream>

const long long max_size = 2000;         // max length of strings

inline
bool is_letter(char c){return (c >= 'a' && c <= 'z')||( c >= 'A' && c <= 'Z')||(c == '_')||(c >= '0' && c <= '9');}

class Sentence {

	private:
		char *data;
		std::vector<char*> words;
		size_t max_size;

	public:
		Sentence(size_t s=100*max_w):data((char*)malloc(s)),max_size(s){}
		~Sentence(){free(data);}
		size_t size(){return words.size();}
		char* operator[](size_t i);
		void read_file(FILE* f);
		void read_string(std::string s);
		void split();

};

char* Sentence::operator[](size_t i){

  assert(i < words.size());

  return words[i];

}


void Sentence::read_file(FILE* f = stdin){

	unsigned int a = 0;

	while (1) {

		data[a] = fgetc(f);

		if ((data[a] == '\n') || (a >= max_size - 1)) {

        	data[a] = 0;
        	break;

      	}

      	a++;

    }

}




void Sentence::read_string(std::string s){

	unsigned int a = 0;

	while (1) {

		data[a] = s[a];

		if ((data[a] == '\n') || (a >= max_size - 1)) {

			data[a] = 0;
			break;

		}

		a++;

	}

}



void Sentence::split(){

	char* t = data;

	while(*t != 0 && !is_letter(*t)){t++;}

	while(*t != 0){

		words.push_back(t);
		std::cout<<"boucle2 split : "<<t<<std::endl;

		while(*t != 0 && is_letter(*t)){t++;}

		if(*t != 0){

			*t = 0;
			t++;

		}

	while(*t != 0 && !is_letter(*t)){t++;}

	}

}

#endif
