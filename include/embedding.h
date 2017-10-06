#ifndef embedding_h
#define embedding_h

#include "character.h"
#include "word.h"
#include "sentence.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>


const long long N = 1;                   // number of closest words that will be shown

#define err1 0x0000 // la fonction lecture_fichier_bin n a pas trouve le repertoire en entree


// To store a vocabulary and the embedded std::vectors
class Embedding {

public:

	Embedding(){}

	// Load all data from binary word2vect format
	// Return 0 if OK
	int load_Word2VecBinFormat(const char* filename ///< the name of the binary file
	);
    int load_Word2VecBinFormat(const std::string& filename ///< the name of the binary file
	) {return load_Word2VecBinFormat(filename.c_str());}

	//Save the vocabulary in a file
	void save_voc(const std::string &file_name);

	//Read the vocabulary from a file
	void set_hmap(const std::unordered_map<std::string,int> &stemmed , const std::string &file_name);

    // Return a ponter to the embedded std::vector of the word word
	float* get(const char* word);

	//return the number of terms in the query that has an embedding
	int check_embedding(const std::vector<std::string> &query );

	// Return the cosine similarity between two words
	float cosine(const char* , const char*);

	// Return the cosine similarity between a sums of words and a word
	float cosine(const std::vector<std::string>  &set1, const  std::string  &set2);

	// Return the cosine similarity between two sums of words
	float cosine(const std::vector<std::string>  &set1, const  std::vector<std::string>  &set2);

	// Write the expanded queries in the file named name_doc_expanded_queries
	void expand_queries(std::string name_doc_queries, std::string name_doc_expanded_queries);

	//Displays vocab and M sizes and the size of the embedded std::vectors
	void display_attributes();

	//Get size of vocabulary
	size_t size_voc()const{return vocab.size();}

	//Return the list of all the words (to use for debug only)
	const char* operator[](const unsigned int id ) const{return vocab[id].c_str();}


private:

	//hMAP of the vocabulary
	std::unordered_map<std::string,int> hmap;

	// the table of all words
	std::vector<Word> vocab;

	// Matrix of all embeded std::vectors
	std::vector<float> M;

	//Size of each embedded std::vectors
	uint64_t size;

	// Return a ponter to the embedded std::vector that starts at indice i
	float* get(size_t i);

};


void Embedding::save_voc(const std::string &file_name){

	std::ofstream myfile;

  	myfile.open (file_name.c_str());

	auto iterator = hmap.begin();

	while(iterator != hmap.end()){

		myfile << iterator->first + " " + std::to_string(iterator->second) + "\n";

		iterator++;

	}

  	myfile.close();

}



//Take a std::string as input and return a pair of std::string and double using the delimiter
std::pair<std::string,int> read_line(const std::string &str, const char delimiter) {

  std::pair<std::string,int> res;
  std::stringstream ss(str); // Turn the std::string into a stream.
  std::string tok;
  tok[0] = 0;
  getline(ss, tok, delimiter);
  res.first = tok;
  getline(ss, tok, delimiter);
  res.second = atoi(tok.c_str());

  return res;
}



//Read a file containing the map
void Embedding::set_hmap(const std::unordered_map<std::string,int> &stemmed , const std::string &file_name){

	FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	size_t nread;

	std::pair<std::string,int> temp;

	while (int(nread = getline(&line, &len, f)) != -1) {

		temp = read_line( std::string(line) , ' ');

		hmap[temp.first] = temp.second;

	}

	free(line);

}








// Read in memory a binary file of vod2vec output
int Embedding::load_Word2VecBinFormat (
const char* file_name ///< the name of the binary file
	)
{
	std::cout<<"Before opening binary file"<<std::endl;
	// Open the binary file
	FILE *f = fopen(file_name, "rb");
	if(f == NULL) return int(err1);

	std::cout<<"After opening binary file"<<std::endl;

	uint64_t words;

	//Reading the header of the file
	//Warning(1) : the integers must have a lower value than 2^64
	//Warning(2) : the scanf must get than l means 64 bits
	fscanf(f, "%lu", &words);
	fscanf(f, "%lu", &size);

	std::cout<< "Size of the vocab : " <<words<<std::endl;
	std::cout<< "Size of the std::vectors : " <<size<<std::endl;
	// Reserve memory for the vocabulary
	vocab.resize(words);
	std::cout<<"After allocation of the vocab"<<std::endl;
	// Reserve memory for the matrix of embedded std::vectors
	M.resize(words*size);
	std::cout<<"After allocation of the vectors"<<std::endl;
	//separator char between the word and the embedded std::vector
	char ch;
	std::cout<<"Starting reading binary file : "<<std::endl;
	for (size_t b = 0; b < words; b++)
	{

		std::cout<<"\rProcessing ...................."<< 100*(b+1)/words <<"%"<<std::flush;

		//Reads the bth word
		fscanf(f, "%s%c", &vocab[b][0], &ch);
		hmap[vocab[b].to_string()] = b;
		//Read the bth std::vector
		fread(&M[b * size], sizeof(float), size , f);
		//Normalization of the std::vectors
		float len = 0;
		for (size_t a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
		len = sqrt(len);
		for (size_t a = 0; a < size; a++) M[a + b * size] /= len;

  	}
	std::cout<<std::endl;
	fclose(f);

	//save_voc("non_stemmed_voc");

	return 0;
}







float* Embedding::get(const char* word){

	std::string term = std::string(word);

	auto it = hmap.find( term );

	if(it != hmap.end()){

		return get(it->second);

	}
	/*
	for( unsigned int i = 0 ; i < term.size() ; i++){

		if( term[i] > 47 && term[i] < 58 ){

			term[i]='#';

		}
	}

	it = hmap.find( term );

	if(it != hmap.end()){

		return get(it->second);

	}



	if( ( (term[0]) > 96 && (term[0]) < 123 ) ){

		term[0] = term[0] - 32;

		it = hmap.find( term );

		if(it != hmap.end()){

			return get(it->second);

		}

	}
	*/


	return nullptr;

}

inline
float* Embedding::get(size_t i){
	assert( i < vocab.size() );
	return &M[i*size];

}


int Embedding::check_embedding(const std::vector<std::string> &query){

	int res = 0;

	for(unsigned int i = 0 ; i < query.size() ; i++){

		if(get(query[i].c_str()) != nullptr){res++;}

	}

	return res;

}



float Embedding::cosine(const char* word1, const char* word2){

	float *vect1 = get(word1);
	float *vect2 = get(word2);
	if(vect1 == nullptr || vect2 == nullptr){return 0;}
	float dist = 0;
	for(unsigned int i = 0; i < size ; i++){

		dist += (*vect1)*(*vect2);
		vect1++;
		vect2++;

	}

	return dist;
}


float Embedding::cosine(const std::vector<std::string> &set1 , const std::string &set2 ){

	if(set1.size() == 0){return 0;}

	float *term2 = get(set2.c_str());

	if(term2==nullptr){return 0;}

	float *sumvect1 = nullptr;

	unsigned int iterator1 = 0;

	while(sumvect1 == nullptr && iterator1 < set1.size()){


		sumvect1 = get(set1[iterator1].c_str());
		iterator1++;

	}

	if(sumvect1 == nullptr){return 0;}

	float *vect;

	float len = 0;

	for(unsigned int i = iterator1+1 ; i < set1.size() ; i++){

		vect = get(set1[i].c_str());

		if(vect != nullptr){


				for(unsigned int j = 0; j < size ; j++){

					sumvect1[j] += (*vect);
					vect++;

				}

		}

	}
	len = 0;
	for (unsigned int i = 0; i < size ; i++) len += sumvect1[i]*sumvect1[i];
	len = sqrt(len);
	for (unsigned int i = 0; i < size ; i++) sumvect1[i]/= len;


	float dist = 0;

	for(unsigned int i = 0; i < size ; i++){

		dist += (*sumvect1)*(*term2);
		sumvect1++;
		term2++;

	}

	return dist;

}






float Embedding::cosine(const std::vector<std::string> &set1 , const std::vector<std::string> &set2 ){

	if(set1.size() == 0 || set2.size() == 0 ){return 0;}

	float *sumvect1 = nullptr;
	float *sumvect2 = nullptr;

	unsigned int iterator1 = 0;

	while(sumvect1 == nullptr && iterator1 < set1.size()){


		sumvect1 = get(set1[iterator1].c_str());
		iterator1++;

	}

	if(sumvect1 == nullptr){return 0;}

	unsigned int iterator2 = 0;

	while(sumvect2 == nullptr && iterator2 < set2.size()){


		sumvect2 = get(set2[iterator2].c_str());
		iterator2++;

	}

	if(sumvect2 == nullptr){return 0;}


	float *vect;

	float len = 0;

	for(unsigned int i = iterator1+1 ; i < set1.size() ; i++){

		vect = get(set1[i].c_str());

		if(vect != nullptr){


				for(unsigned int j = 0; j < size ; j++){

					sumvect1[j] += (*vect);
					vect++;

				}

		}

	}
	len = 0;
	for (unsigned int i = 0; i < size ; i++) len += sumvect1[i]*sumvect1[i];
	len = sqrt(len);
	for (unsigned int i = 0; i < size ; i++) sumvect1[i]/= len;

	for(unsigned int i = 1 ; i < set2.size() ; i++){

		vect = get(set2[i].c_str());

		if(vect != nullptr){

			for(unsigned int j = 0; j < size ; j++){

				sumvect2[j] += (*vect);
				vect++;

			}

		}

	}


	len = 0;
	for (unsigned int i = 0; i < size ; i++) len += sumvect2[i]*sumvect2[i];
	len = sqrt(len);
	for (unsigned int i = 0; i < size ; i++) sumvect2[i]/= len;


	float dist = 0;

	for(unsigned int i = 0; i < size ; i++){

		dist += (*sumvect1)*(*sumvect2);
		sumvect1++;
		sumvect2++;

	}

	return dist;

}



//Displays vocab and M sizes and the size of the embedded std::vectors
void Embedding::display_attributes(){

	std::cout<<std::endl;
	std::cout<<"Size of vocab : "<<vocab.size()<<std::endl;
	std::cout<<"Size of M : "<<M.size()<<std::endl;
	std::cout<<"Size of vectors : "<<size<<std::endl;
	std::cout<<std::endl;

}




void Embedding::expand_queries(std::string name_doc_queries, std::string name_doc_expanded_queries){


	std::vector<char> st1(max_size);
	std::vector< std::vector<char> > bestw;
	bestw.resize( max_size , std::vector<char>( N , 0 ) );
	std::vector< std::vector<char> > st;
	st.resize( max_size , std::vector<char>( 100 , 0 ) );
	float dist, len;
	std::vector<float> bestd(N);
	std::vector<float> vec(max_size);
	long long a , b, c, d, cn;
	long long words = vocab.size();
	std::cout<<"Size of vocab : " << words <<std::endl;
	std::vector<long long> bi(100);
	std::string line;
	int compteur = 0;


	std::ofstream doc_expanded_queries(name_doc_expanded_queries);
	std::ifstream doc_queries(name_doc_queries);

	while ( getline (doc_queries,line) ){
		compteur++;
    		for (long a = 0; a < N; a++) bestd[a] = 0;
    		for (long a = 0; a < N; a++) bestw[a][0] = 0;
		for(a = 0; a < int(line.size()) ; a++) st1[a] = line[a];
		st1[line.size()] = 0;
    		if (!strcmp(&st1[0], "EXIT")) break;
    		cn = 0;
    		b = 0;
    		c = 0;

	//Store each word in st1 in st
    		while (1) {
      			st[cn][b] = st1[c];
      			b++;
      			c++;
      			st[cn][b] = 0;
      			if (st1[c] == 0) break;
      			if (st1[c] == ' ') {

        			cn++;
       				b = 0;
        			c++;

      			}
    		}

    		cn++;

	//For each word in st check if it is in the vocabulary if a word is not in the vocabulary, the query will not be expanded
    		for (int a = 0; a < cn; a++) {

      			for (b = 0; b < words; b++) if (vocab[b]==&st[a][0]) break;
      			if (b == words) b = -1;
      			bi[a] = b;
      			printf("\nWord: %s Position in vocabulary: %lld\n", &st[a][0], bi[a]);
      			if (b == -1) {

        			printf("Out of dictionary word!\n");
				doc_expanded_queries << line + "\n";
        			break;

      			}
    		}

    		if (b == -1) continue;
    		printf("\n                                              Word       Cosine distance\n----------------------------------------------------------------\n");

	//If st is composed of 1 word, fill vec with the std::vectorial representation of st, otherwise, the sentence will be represented by the sum of the std::vector composing it
    		for (uint64_t a = 0; a < size; a++) vec[a] = 0;
    		for (b = 0; b < cn; b++) {

      			if (bi[b] == -1) continue;
      			for (uint64_t a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];

    		}

	//Normalization
    		len = 0;
    		for (uint64_t a = 0; a < size; a++) len += vec[a] * vec[a];
    		len = sqrt(len);
    		for (uint64_t a = 0; a < size; a++) vec[a] /= len;

    		for (a = 0; a < N; a++) bestd[a] = 0;
    		for (a = 0; a < N; a++) bestw[a][0] = 0;

	//For each word in the vocabulary
    		for (c = 0; c < words; c++) {

		//Skip the identical terms
      			a = 0;
	      		for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
	      		if (a == 1) continue;
	      		dist = 0;

		//Compute the distance with st and check if and where it is in the top N
	      		for (uint64_t a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
	      		for (a = 0; a < N; a++) {

				if (dist > bestd[a]) {

		  			for (d = N - 1; d > a; d--) {

		    				bestd[d] = bestd[d - 1];
		    				strcpy(&bestw[d][0], &bestw[d - 1][0]);

		  			}

		  			bestd[a] = dist;
		  			strcpy(&bestw[a][0], &(vocab[c][0]));
		  			break;
				}
	      		}
    		}

	//Write in the file expanded_queries the new queries
    		for (a = 0; a < N; a++){

			printf("%50s\t\t%f\n", &bestw[a][0], bestd[a]);
			doc_expanded_queries << line + " " + std::string(&bestw[a][0]) + "\n";

  		}
  	}


	doc_queries.close();
	doc_expanded_queries.close();


}




void get_size(std::string file_name,size_t& words, size_t& size)
{

	FILE *f = fopen(file_name.c_str(), "rb");

	if(f == NULL) throw int(err1);


	fscanf(f, "%ld", &words);
  	fscanf(f, "%ld", &size);

	fclose(f);

}



template<typename T>
void ecriture_fichier_bin(T *var, std::string nom_fichier, long long size)
{


	FILE*f=fopen(nom_fichier.c_str(),"wb");

	fwrite(var ,sizeof(T),size,f);

	fclose(f);

}

template<typename T>
void lecture_fichier_bin(T *var, std::string nom_fichier, long long size)
{


	FILE *f = fopen(nom_fichier.c_str(), "rb");

	fread(var, sizeof(T), size, f);

	fclose(f);

}


void test_sentence(){

	FILE*f=fopen("true_queries.txt","rb");
	Sentence sentence;
	sentence.read_file(f);
	sentence.split();
	for(unsigned int i = 0; i < sentence.size() ; i++){

		std::cout<< i << " "<< sentence[i] <<std::endl;

	}

}

/*

int main(int argc, char **argv) {


	//test_sentence();
	//return 0;


//Warning message if there is no file

  	if (argc < 2) {
    		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
    		return 0;
  	}

//Declaration
	// Name of the binary file containing the embeded std::vectors
	std::string file_name;
	file_name = std::string(argv[1]);

	Embedding embedding;
	cout<<"loading "<< file_name <<endl;

	clock_t begin = clock();

	embedding.load_Word2VecBinFormat(file_name);

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout<< "Time to do all the stuff : "<< elapsed_secs <<endl;

	embedding.display_attributes();

	cout<<"Cosine similarity between test and tests : "<<embedding.cosine("test","tests") <<endl;

	return 0;

	cout<<"Query : "<< argv[2] <<endl;
	cout<<"Exp query "<< argv[3] <<endl;
	embedding.expand_queries(argv[2], argv[3]);





//Reads the file in input and get the number of words and their associated std::vectors's length in the variables words and size

	// Vocabulary size
	size_t words = 0;
	// Embeded std::vectors size
	size_t size = 0;
	// Read in tghe header the two sizes
  	get_size( file_name, words , size);


//Allocation of memory for the variables vocab that will contain the vocabulary and M the matrix that will contain their associated std::vector

    // A basic word
    typedef char Word[max_w];
	// the table of all words
	std::vector<Word> vocab(words);

	// Matrix of all embeded std::vectors
	std::vector<float> M(words * size);

//Check if the memory of the matrix has been allocated

  	if (M.size() == 0) {
    		printf("Cannot allocate memory: %lu MB    %lu  %lu\n", words * size * sizeof(float) / 1048576, words, size);
    		return -1;
  	}

//Read the vocabulary and the std::vectors

	lecture_fichier_bin(&M[0], "std::vectors.bin" ,  size*words);

	lecture_fichier_bin(&vocab[0], "vocabulaire.bin" ,  max_w*words);

//Take a file of queries and write it's "expanded" version

	//expand_queries("true_queries.txt", "expanded_queries.txt", &vocab,  &M, words, size);

  	return 0;
}

*/

#endif
