#ifndef readwrite_h
#define readwrite_h

#include "closest.h"
#include "frequency.h"
#include "display.h"
#include "tool.h"
#include "split_strings.h"
#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <iomanip>



//Write a <string,double> map in a file
void write_map(const std::unordered_map< std::string , double > &map , const std::string &file_name){

	std::ofstream myfile;
  myfile.open (file_name.c_str());

	auto iterator = map.begin();

	while( iterator != map.end() ){

		myfile << iterator->first + " " + std::to_string(iterator->second) + "\n";

		iterator++;

	}

  	myfile.close();

}


//Take a string as input and return a pair of string and double using the delimiter
std::pair<std::string,double> read_pair(const std::string &str, const char delimiter) {

  std::pair<std::string,double> res;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;
  tok[0] = 0;
  getline(ss, tok, delimiter);
  res.first = tok;
  getline(ss, tok, delimiter);
  res.second = atof(tok.c_str());

  return res;
}


//Read a file containing the map
std::unordered_map< std::string , double > read_cos_map_file(const std::string &file_name , const double &threshold){

	FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	size_t nread;

	std::unordered_map< std::string , double > res;

	std::pair<std::string,double> temp;

	while (int(nread = getline(&line, &len, f)) != -1) {

		temp = read_pair( std::string(line) , ' ');
		if(temp.second > threshold){res[temp.first] = temp.second;}

	}

	free(line);

	return res;

}




//Write in a file the set of the closest words of the vocabulary
void write_map_map(const std::unordered_map< std::string , std::unordered_map<std::string,double> > &set_closest_words , const std::string &file_name){

	std::ofstream myfile;
  myfile.open (file_name.c_str());

	auto iterator = set_closest_words.begin();
	std::unordered_map<std::string,double>::const_iterator iterator2;

	while( iterator != set_closest_words.end() ){

		myfile << iterator->first + "\n";

		iterator2 = iterator->second.begin();

		while(iterator2 != iterator->second.end()){

			myfile << iterator2->first + " " + std::to_string(iterator2->second) + " ";
			iterator2++;

		}

		myfile << "\n";

		iterator++;

	}

  	myfile.close();

}




//Write in a file the set of the closest words of the vocabulary
void write_map_map_end_file(const std::unordered_map< std::string , std::unordered_map<std::string,double> > &set_closest_words , const std::string &file_name){

	std::ofstream myfile;
	myfile.open (file_name.c_str() , std::ofstream::app);

	auto iterator = set_closest_words.begin();
	std::unordered_map<std::string,double>::const_iterator iterator2;

	while( iterator != set_closest_words.end() ){

		myfile << iterator->first + "\n";

		iterator2 = iterator->second.begin();

		while(iterator2 != iterator->second.end()){

			myfile << iterator2->first + " " + std::to_string(iterator2->second) + " ";
			iterator2++;

		}

		myfile << "\n";

		iterator++;

	}

  	myfile.close();

}





//Write in a file the set of the closest words of the vocabulary
void write_map_map(const std::unordered_map< int , std::unordered_map<std::string,double> > &set_closest_words , const std::string &file_name){

	std::ofstream myfile;
  myfile.open (file_name.c_str());

	auto iterator = set_closest_words.begin();
	std::unordered_map<std::string,double>::const_iterator iterator2;

	while( iterator != set_closest_words.end() ){

		myfile << std::to_string(iterator->first) + "\n";

		iterator2 = iterator->second.begin();

		while(iterator2 != iterator->second.end()){

			myfile << iterator2->first + " " + std::to_string(iterator2->second) + " ";
			iterator2++;

		}

		myfile << "\n";

		iterator++;

	}

  	myfile.close();

}

//Takes a string that corresponds to one line of the mapmap file as an input and returns an unordered_map
std::unordered_map<std::string,double> read_map(const std::string &line , const double &threshold){

	std::unordered_map<std::string,double> res;

	std::stringstream ss(line);
  std::string tok;
	std::string tok2;
	while(getline(ss, tok, ' ')){

		getline(ss, tok2, ' ');

		if(tok!="\n" && atof(tok2.c_str()) > threshold ){

			res[tok] = atof(tok2.c_str());
		}

	}

	return res;

}






//Read a file containing the mapmap
std::unordered_map< std::string , std::unordered_map<std::string,double> > read_cos_map_map_file(const std::string &file_name , const double &threshold){

	FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);
	char* line2 = (char*)malloc(len);

	size_t nread;

	std::unordered_map< std::string , std::unordered_map<std::string,double> > res;

	while (int(nread = getline(&line, &len, f)) != -1) {

		getline(&line2, &len, f);

		res[std::string(line).erase(std::string(line).size()-1)] = read_map(line2 , threshold);

	}

	free(line);
	free(line2);

	return res;

}

//Read a file containing the mapmap
std::unordered_map< int , std::unordered_map<std::string,double> > read_cos_sum_query_map_map_file(const std::string &file_name , const double &threshold){

	FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);
	char* line2 = (char*)malloc(len);

	size_t nread;

	std::unordered_map< int , std::unordered_map<std::string,double> > res;

	while (int(nread = getline(&line, &len, f)) != -1) {

		getline(&line2, &len, f);

		res[atoi( std::string(line).erase(std::string(line).size()-1).c_str() )] = read_map(line2 , threshold);

	}

	free(line);
	free(line2);

	return res;

}



//Write the collection/set of queries in input in a file
void write_xml_collection_file(const std::unordered_map< int , std::vector<std::string> > &collection, const std::string &file_name){


	std::ofstream myfile;
  myfile.open (file_name.c_str());

	myfile << "<matrix>\n";

	auto iterator = collection.begin();

	while( iterator != collection.end() ){

		myfile << "<DOC>\n";
		myfile << "<DOCNO>"<<iterator->first<<"</DOCNO>\n";
		myfile << "<TEXT>";
		if(iterator->second.size() != 0){

			for(unsigned int j = 0 ; j < iterator->second.size()-1 ; j++){

				myfile << iterator->second[j] << " ";

			}

			myfile << iterator->second[ iterator->second.size()-1 ];

		}

		myfile << "</TEXT>\n";
		myfile << "</DOC>\n";

		iterator++;

	}

	myfile << "</matrix>\n";
	myfile.close();

}


//Write the unordered map of <std::string,int> in input in a file
void write_tf_file(const std::unordered_map <std::string,int>  &cf , const std::string &file_name){

	std::ofstream myfile;
  myfile.open (file_name.c_str());

	std::unordered_map<std::string,int>::const_iterator p = cf.begin();

	while( p!=  cf.end() ){

		myfile << p->first << " " << p->second << "\n";
		p++;

	}

    myfile.close();

}




//Reads a qrel file, deletes the lines corresponding to the non relevant file ans save the new qrel file
int correct_qrel_file(const std::string &filenamein , const std::string &filenameout){

	std::string line;

  std::ifstream in(filenamein);
  if( !in.is_open())
  {
		std::cout << "Input file failed to open\n";
    return 1;
	}

  std::ofstream out(filenameout);

  while( getline(in,line) )
  {
  	if(line[line.size()-1] != '0')
    	out << line << "\n";
  }
  in.close();
  out.close();
  return 0;

}


//Reads a collection/set of queries in the input file
std::unordered_map< int , std::vector<std::string> > read_some_file(const std::string &file_name){

  FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	size_t nread;

	int current_iteration = 0;

	std::unordered_map< int , std::vector<std::string> > collection;


	while (int(nread = getline(&line, &len, f)) != -1) {

		if( (current_iteration)==377000){collection[current_iteration] =  split_maj(std::string(line) , ' ');}
		else{ skip(std::string(line) , ' ') ;}
		current_iteration++;

	}

	free(line);

	//check_numbers(collection);
	check_collection(collection);

	auto iterator = collection.begin();

	//for(unsigned int i = 0 ; i < collection.size() ; i++){

	while( iterator != collection.end() ){
		std::cout<<"Document "<<iterator->first<<" :"<<std::endl;
		display(iterator->second);
		std::cout<<std::endl<<std::endl;

	}

	return collection;

}


//Reads a collection/set of queries in the input file
std::unordered_map< int , std::vector<std::string> > read_file(const std::string &file_name){

  FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	size_t nread;

	std::unordered_map< int , std::vector<std::string> > collection;

	int current_iteration = 0;

	while (int(nread = getline(&line, &len, f)) != -1) {

		collection[current_iteration] =  split_maj(std::string(line) , ' ');

		current_iteration++;

	}

	free(line);


	//check_numbers(collection);
	check_collection(collection);

	return collection;

}


//Same as before but using the function easy_split on the lines of the file
std::unordered_map< int , std::vector<std::string> > read_clean_file(const std::string &file_name){

  FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	size_t nread;

	std::unordered_map< int , std::vector<std::string> > collection;

	int current_iteration = 0;

	while (int(nread = getline(&line, &len, f)) != -1) {

		collection[current_iteration] = easy_split(std::string(line) , ' ');

		current_iteration++;

	}

	free(line);

	return collection;

}


//Same as before but using the function tf_split on the lines of the file
std::unordered_map <std::string,int> read_tf_file(const std::string &file_name){


	 FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	size_t nread;

	std::unordered_map <std::string,int> cf;


	while (int(nread = getline(&line, &len, f)) != -1) {


		cf.insert( tf_split(std::string(line) , ' ') );


	}

	free(line);

    return cf;

}


//Reads a qrel file, separates the results between a training and a test set
int separate_qrel_file(const std::string &filename , const std::string &filename_train , const std::string &filename_test , const std::unordered_map<int , std::vector<std::string> > &collection){

  std::string line;

    std::ifstream in(filename);
    if( !in.is_open() )
    {
          std::cout << "Input file failed to open\n";
          return 1;
    }

    std::ofstream train(filename_train);

		std::ofstream test(filename_test);

    while( getline(in,line) )
    {

        if( collection.find(stoi( easy_split(line, ' ')[2] )) != collection.end()  ){

			train << line << "\n";

		}

		else{test << line << "\n";}

    }
    in.close();
    train.close();
	  test.close();
    return 0;

}




//Write the results in a file
void write_res_file(const std::vector< std::vector< std::pair<int,double> > > &results , const std::string &file_name , const std::string &query_Id , const double &lambda){

	std::ofstream myfile;
  	myfile.open (file_name.c_str() , std::ofstream::trunc);

	int rank;

	for(unsigned int i = 0 ; i < results.size() ; i++){

		rank = 0;
		if(i < 25){

			for(unsigned int j = 0 ; j < results[i].size() ; j++){

				myfile.precision(17);
				myfile << query_Id << std::setfill('0') << std::setw(3) << i+1 << " Q0 "<< results[i][j].first <<" "<< rank <<" "<< results[i][j].second <<" Hiemstra_LM"<<lambda<<"\n";
				rank++;

			}


		}

		else{

			for(unsigned int j = 0 ; j < results[i].size() ; j++){

				myfile.precision(17);
				myfile << query_Id << std::setfill('0') << std::setw(3) << i+26 << " Q0 "<< results[i][j].first <<" "<< rank <<" "<< results[i][j].second <<" Hiemstra_LM"<<lambda<<"\n";
				rank++;

			}


		}
	}

    myfile.close();

}

//Read a result file produced by trec_eval and return the map
std::vector<std::string> read_res_file(const std::string &file_name){

	FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	int current_line = 1;

	size_t nread;

	std::vector<std::string> res;
	std::string fra;
	std::cout<<"COUCOU"<<std::endl;
	int query_id = 1;
	while(int(nread = getline(&line, &len, f)) != -1 ){

		if(current_line%27==4){

			fra = std::string(line);
			fra = fra.substr(fra.size()-7,6);
			res.push_back( fra );
			query_id++;
		}
		current_line++;

	}

	free(line);

	return res;

}





//Read a result file produced by trec_eval and return the map
std::string read_res_file(const std::string &file_name , bool mu , bool threshold , bool alpha ){

	FILE* f = fopen(file_name.c_str(),"r");

	size_t len = 10000;

	char* line = (char*)malloc(len);

	int current_line = 1;

	size_t nread;

	std::string res;

	std::string var;

	int query_id = 1;

	while(int(nread = getline(&line, &len, f)) != -1 ){

		if(current_line == 1){

			if(mu){

				var = std::string(line);
				var = var.substr(var.size()-17,6);

			}


			else if(threshold){

				var = std::string(line);
				var = var.substr(var.size()-10,4);

			}

			else if(alpha){

				var = std::string(line);
				var = var.substr(var.size()-5,4);

			}


		}

		if(current_line==6){

			res = std::string(line);
			res = res.substr(res.size()-7,6);
			res =  "(" + var + "," + res + ")";
			query_id++;

		}

		current_line++;

	}

	free(line);

	return res;

}



//Write a set of values (parameter , MAP) in a file
void write_Average_Precision(const std::string &file_name , const std::string &directory_name , bool mu , bool threshold , bool alpha){

	std::vector<std::string> set_file_res = ensemble_fichier(directory_name.c_str());

	std::vector<std::string> map;

	for(unsigned int i = 0 ; i < set_file_res.size() ; i++){

		map.push_back(read_res_file(set_file_res[i] , mu , threshold , alpha ));

	}

	std::ofstream myfile;

  	myfile.open(file_name.c_str() , std::ofstream::trunc);

	for(unsigned int i = 0 ; i < map.size() ; i++){

		myfile << map[i];

	}

  	myfile.close();

}


//Read the collection and the set of queries and write the collection frequency and document frequency of the terms in files
void write_all_files(const std::string &collection_file , const std::string &queries_file , const std::string &cf_file , const std::string &df_file){

	std::unordered_map< int ,  std::vector<std::string> > collection = read_file(collection_file);
	std::unordered_map< int ,  std::vector<std::string> > queries = read_file(queries_file);
	std::unordered_map <std::string,int>  cf = build_cf(collection);
	write_tf_file(cf , cf_file);
	std::unordered_map <std::string,int> df = build_df(collection, cf);
	write_tf_file(df , df_file);

}

//Builds the collection, queries cf and df
void read_all_info(const std::string &collection_file , const std::string &queries_file , std::unordered_map< int , std::vector<std::string> > &collection , std::unordered_map< int , std::vector<std::string> > &queries , std::unordered_map <std::string,int> &cf , std::unordered_map <std::string,int> &df){

	collection = read_file(collection_file);
	//collection = read_some_file(collection_file);
	queries = read_file(queries_file);
	cf = build_cf(collection);
	df = build_df(collection, cf);

}





//Read the similarities file without considering the similarities lower than a threshold
void delete_low_similarities(const std::string &collection_cosine_file , const std::string &queries_cosine_file , std::unordered_map< std::string , std::unordered_map<std::string,double> > &all_cos , std::unordered_map<std::string,double> &all_sum_cos , const double &threshold){

	std::unordered_map< std::string , std::unordered_map<std::string,double> > queries_cosine =  read_cos_map_map_file(queries_cosine_file , threshold);

	/*auto iterator = queries_cosine.begin();

	int stop = 0;

	while(iterator != queries_cosine.end()){

		stop += iterator->second.size();

		if(iterator->second.size() != 0){

			std::cout<< "Considered word : " << iterator->first <<std::endl;

			display_map(iterator->second);

		}

		iterator++;

	}

	std::cout<<"Sum size elements queries_cosine : "<< stop <<std::endl;

	cin>>stop;*/

	//std::unordered_map<std::string , double> all_sum_cos_queries  = read_cos_map_file(sum_queries_cosine_file , threshold);

	all_cos =  read_cos_map_map_file(collection_cosine_file , threshold);

	/*std::cout<<"Size all_cos : "<< all_cos.size() <<std::endl;

	auto iterator = all_cos.begin();

	int stop = 0;

	while(iterator != all_cos.end()){

		stop += iterator->second.size();

		if(iterator->second.size() != 0){

			std::cout<< "Considered word : " << iterator->first <<std::endl;

			display_map(iterator->second);

		}

		iterator++;

	}

	std::cout<<"Sum size elements all_cos : "<< stop <<std::endl;

	cin>>stop;*/

	//all_sum_cos  = read_cos_map_file(sum_collection_cosine_file , threshold);

	aggregate_map( all_cos , queries_cosine );

	//aggregate_map( all_sum_cos , all_sum_cos_queries );

	all_sum_cos = all_fast_sum_cos_sim(all_cos);

}






void read_all_info2( const std::string &collection_file , const std::string &queries_file , const std::string &collection_cosine_file , const std::string &queries_cosine_file , std::unordered_map< int , std::vector<std::string> > &collection , std::unordered_map< int ,  std::vector<std::string> > &queries , std::unordered_map <std::string,int> &cf , std::unordered_map <std::string,int> &df , std::unordered_map< std::string , std::unordered_map<std::string,double> > &all_cos , std::unordered_map<std::string,double> &all_sum_cos , const double &threshold){

	collection = read_file(collection_file);
	queries = read_file(queries_file);
	cf = build_cf(collection);
	df = build_df(collection, cf);

	std::cout<<"Location of the cosine files :"<< queries_cosine_file <<" and "<< collection_cosine_file <<std::endl;

	clock_t begin = clock();

	std::unordered_map< std::string , std::unordered_map<std::string,double> > queries_cosine =  read_cos_map_map_file(queries_cosine_file , threshold);


	/*auto iterator = queries_cosine.begin();

	int stop = 0;

	while(iterator != queries_cosine.end()){

		stop += iterator->second.size();

		if(iterator->second.size() != 0){

			std::cout<< "Considered word : " << iterator->first <<std::endl;

			display_map(iterator->second);

		}

		iterator++;

	}

	std::cout<<"Sum size elements queries_cosine : "<< stop <<std::endl;

	cin>>stop;*/



	//std::unordered_map<std::string , double> all_sum_cos_queries  = read_cos_map_file(sum_queries_cosine_file , threshold);

	all_cos =  read_cos_map_map_file(collection_cosine_file , threshold);

	/*std::cout<<"Size all_cos : "<< all_cos.size() <<std::endl;

	auto iterator = all_cos.begin();

	int stop = 0;

	while(iterator != all_cos.end()){

		stop += iterator->second.size();

		if(iterator->second.size() != 0){

			std::cout<< "Considered word : " << iterator->first <<std::endl;

			display_map(iterator->second);

		}

		iterator++;

	}

	std::cout<<"Sum size elements all_cos : "<< stop <<std::endl;

	cin>>stop;*/

	//all_sum_cos  = read_cos_map_file(sum_collection_cosine_file , threshold);

	aggregate_map( all_cos , queries_cosine );

	//aggregate_map( all_sum_cos , all_sum_cos_queries );

	all_sum_cos = all_fast_sum_cos_sim(all_cos);

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout<< "Time to read the cosine files : "<< elapsed_secs <<std::endl;



}






void read_all_info3( const std::string &collection_file , const std::string &queries_file , const std::string &collection_cosine_file , const std::string &queries_cosine_file , const std::string &sum_queries_cosine_file , std::unordered_map< int , std::vector<std::string> > &collection , std::unordered_map< int , std::vector<std::string> > &queries , std::unordered_map <std::string,int> &cf , std::unordered_map <std::string,int> &df , std::unordered_map< std::string , std::unordered_map<std::string,double> > &all_cos , std::unordered_map<std::string,double> &all_sum_cos , std::unordered_map< int , std::unordered_map<std::string,double> > &all_cos_sum_queries , const double &threshold){

	collection = read_file(collection_file);
	queries = read_file(queries_file);
	cf = build_cf(collection);
	df = build_df(collection, cf);

	std::cout<<"Location of the cosine files :"<< queries_cosine_file <<" and "<< collection_cosine_file <<std::endl;

	clock_t begin = clock();

	std::unordered_map< std::string , std::unordered_map<std::string,double> > queries_cosine =  read_cos_map_map_file(queries_cosine_file , threshold);

	//std::unordered_map<std::string , double> all_sum_cos_queries  = read_cos_map_file(sum_queries_cosine_file , threshold);

	all_cos =  read_cos_map_map_file(collection_cosine_file , threshold);

	//all_sum_cos  = read_cos_map_file(sum_collection_cosine_file , threshold);

	aggregate_map( all_cos , queries_cosine );

	//aggregate_map( all_sum_cos , all_sum_cos_queries );

	all_sum_cos = all_fast_sum_cos_sim(all_cos);

	all_cos_sum_queries =  read_cos_sum_query_map_map_file(sum_queries_cosine_file , threshold);

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout<< "Time to read the cosine files : "<< elapsed_secs <<std::endl;

}




//Reads everything
void fast_setup(const std::string &collection_file , const std::string &queries_file , const std::string &cf_file , const std::string &df_file , std::unordered_map< int , std::vector<std::string> > &collection , std::unordered_map< int , std::vector<std::string> > &queries , std::unordered_map <std::string,int> &cf , std::unordered_map <std::string,int> &df){

	collection = read_file(collection_file);
	queries = read_file(queries_file);
	cf = read_tf_file(cf_file);
	df = read_tf_file(df_file);

}



//Same as before but over the entire vocabulary
void save_closest_terms(const std::string &file_name , const std::unordered_map <std::string,int> &cf , Embedding &embedding ,  const double &threshold){

	std::vector<std::string> temp;

	auto iterator = cf.begin();

	while(iterator != cf.end()){

		temp.push_back(iterator->first);
		iterator++;

	}

	int nthreads, tid;

	bool first = true;

	#pragma omp parallel private(tid) shared(temp , cf , embedding , threshold , first)
    {
        tid = omp_get_thread_num();

        std::unordered_map< std::string , std::unordered_map<std::string,double> > mapLocal;

        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            std::cout<<"Number of thread: "<<nthreads<<std::endl;
        }

		#pragma omp for schedule(static)
		for(unsigned int i = 0 ; i < temp.size() ; i++){

		//while(iterator != cf.end()){

			//set_most_sim[iterator->first] = closest_terms(iterator->first , cf , embedding , threshold);
			mapLocal[temp[i]] = closest_terms(temp[i] , cf , embedding , threshold);

			//iterator++;
		}



		#pragma omp critical
        {

			std::cout<<"Ecriture "<<tid<<" map size="<<mapLocal.size()<<std::endl;
			if(first){

				write_map_map(mapLocal ,file_name);
				first = false;

			}

			else{

				write_map_map_end_file(mapLocal ,file_name);

			}

        }


	}

}










#endif
