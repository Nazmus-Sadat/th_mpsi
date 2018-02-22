
#include <iostream>
#include <string.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <cstddef>
#include <gmp.h>
#include <gmpxx.h>
#include <math.h>
#include <omp.h>
#include <nfl.hpp>
//#include "fvnamespace.h"
#include "fvnamespace2.h"
#include "lib/params/params.cpp"
#include <omp.h>
#include <random>
#include <dirent.h>



#include "lib/prng/fastrandombytes.cpp"
#include "lib/prng/randombytes.cpp"
//#include "FV.hpp"
#include "FV_2.hpp"


#include "common/INIReader.h"
#include "common/network.cpp"

using namespace std;
using namespace FV;
const int CPU = 4;


void readPublicKey(pk_t &pk, std::string path) {
    string line;
    ifstream myfile(path);
    if (myfile.is_open()) {
        pk.a.deserialize_manually(myfile);
        pk.a_shoup = nfl::compute_shoup(pk.a);
        pk.b.deserialize_manually(myfile);
        pk.b_shoup = nfl::compute_shoup(pk.b);
        
        myfile.close();
    } else cout << "Unable to open file";
}


void readEvalKey(evk_t &evk, std::string path) {
    string line;
    ifstream myevalfile(path);

    /*int ell = floor(mpz_sizeinbase(P::moduli_product(), 2) / word_size) + 1;
    const int N = ell;
    std::array <P,  N> arr;*/
    cout << evk.ell << endl;
    if (myevalfile.is_open()) {
        for (unsigned j = 0; j < evk.ell; ++j) {
            evk.values[j] = new FV::params::poly_p[2];
            evk.values_shoup[j] = new FV::params::poly_p[2];
            evk.values[j][0].deserialize_manually(myevalfile);
            evk.values_shoup[j][0] = nfl::compute_shoup(evk.values[j][0]);
            evk.values[j][1].deserialize_manually(myevalfile);
            evk.values_shoup[j][1] = nfl::compute_shoup(evk.values[j][1]);
        }

        myevalfile.close();
    } else cout << "Unable to eval open file";
}



std::string sections(INIReader &reader)
{
    std::stringstream ss;
    std::set<std::string> sections = reader.Sections();
    for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
        ss << *it << ",";
    return ss.str();
}



int main() {
	
	INIReader config_reader("central_server/config.ini");

    if (config_reader.ParseError() < 0) {
        std::cout << "Can't load config file \n";
        return 1;
    }
   
	std::string exp_code = config_reader.Get("settings", "exp_code", "UNKNOWN");
			  
	std::string cs_ip_str =  config_reader.Get("ip_and_port", "cs_ip", "UNKNOWN");
	uint16 cs_port =  config_reader.GetInteger("ip_and_port", "cs_port", -1);
	
	std::string csp_ip_str =  config_reader.Get("ip_and_port", "csp_ip", "UNKNOWN");
	uint16 csp_port =  config_reader.GetInteger("ip_and_port", "csp_port", -1);
	
	std::string do_1_ip_str =  config_reader.Get("ip_and_port", "do_1_ip", "UNKNOWN");
	uint16 do_1_port =  config_reader.GetInteger("ip_and_port", "do_1_port", -1);
	uint16 cs_port_1 =  config_reader.GetInteger("ip_and_port", "cs_port_1", -1);
	
	std::string do_2_ip_str =  config_reader.Get("ip_and_port", "do_2_ip", "UNKNOWN");
	uint16 do_2_port =  config_reader.GetInteger("ip_and_port", "do_2_port", -1);
	uint16 cs_port_2 =  config_reader.GetInteger("ip_and_port", "cs_port_2", -1);

	std::string do_3_ip_str =  config_reader.Get("ip_and_port", "do_3_ip", "UNKNOWN");
	uint16 do_3_port =  config_reader.GetInteger("ip_and_port", "do_3_port", -1);
	uint16 cs_port_3 =  config_reader.GetInteger("ip_and_port", "cs_port_3", -1);
	
	std::string do_4_ip_str =  config_reader.Get("ip_and_port", "do_4_ip", "UNKNOWN");
	uint16 do_4_port =  config_reader.GetInteger("ip_and_port", "do_4_port", -1);
	uint16 cs_port_4 =  config_reader.GetInteger("ip_and_port", "cs_port_4", -1);
	
	std::string do_5_ip_str =  config_reader.Get("ip_and_port", "do_5_ip", "UNKNOWN");
	uint16 do_5_port =  config_reader.GetInteger("ip_and_port", "do_5_port", -1);
	uint16 cs_port_5 =  config_reader.GetInteger("ip_and_port", "cs_port_5", -1);
	
    #ifdef WIN32
       // Windows requires this to start up the networking API
       WORD versionWanted = MAKEWORD(1, 1);
       WSADATA wsaData;
       (void) WSAStartup(versionWanted, &wsaData);
    #else 
       // avoid SIGPIPE signals caused by sending on a closed socket
       signal(SIGPIPE, SIG_IGN);
    #endif
	
	//Comment: message for acknowledgement management
	std::string message="";
	
	//Comment: Send configuration file to all parties.
	
	ip_address do_1_ip = GetHostByName(do_1_ip_str.c_str());
	//SendFile(do_1_ip, do_1_port, "central_server/config.ini");
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, "central_server/config.ini"))
			break;
	}
	
	
	ReceiveMessage(cs_port_1);
	
	
	ip_address do_2_ip = GetHostByName(do_2_ip_str.c_str());
	//SendFile(do_2_ip, do_2_port, "central_server/config.ini");
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, "central_server/config.ini"))
			break;
	}
	ReceiveMessage(cs_port_2);
	
	
	ip_address do_3_ip = GetHostByName(do_3_ip_str.c_str());
	//SendFile(do_3_ip, do_3_port, "central_server/config.ini");
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, "central_server/config.ini"))
			break;
	}
	ReceiveMessage(cs_port_3);
	
	ip_address do_4_ip = GetHostByName(do_4_ip_str.c_str());
	//SendFile(do_3_ip, do_3_port, "central_server/config.ini");
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, "central_server/config.ini"))
			break;
	}
	ReceiveMessage(cs_port_4);
	
	ip_address do_5_ip = GetHostByName(do_5_ip_str.c_str());
	//SendFile(do_3_ip, do_3_port, "central_server/config.ini");
	while(1)
	{
		if(SendFile(do_5_ip, do_5_port, "central_server/config.ini"))
			break;
	}
	ReceiveMessage(cs_port_5);

	
	ip_address csp_ip = GetHostByName(csp_ip_str.c_str());
	//SendFile(csp_ip, csp_port, "central_server/config.ini");
	while(1)
	{
		if(SendFile(csp_ip, csp_port, "central_server/config.ini"))
			break;
	}
	ReceiveMessage(cs_port);
	
	
	//Comment: Receive keys from the csp
	//ReceiveFile(cs_port, std::string("central_server/eval.key").c_str()); 
	while(1)
	{
		if(ReceiveFile(cs_port, std::string("central_server/eval.key").c_str()))
			break;
	}
	//Comment: Send acknowledgement for eval key
	while(1)
	{
		if(SendMessage(csp_ip, csp_port, std::string("eval_key_received_by_cs").c_str()))
			break;
	}
	
	
	//ReceiveFile(cs_port, std::string("central_server/public.key").c_str()); 
	while(1)
	{
		if(ReceiveFile(cs_port, std::string("central_server/public.key").c_str()))
			break;
	}
	//Comment: Send acknowledgement for public key
	while(1)
	{
		if(SendMessage(csp_ip, csp_port, std::string("public_key_received_by_cs").c_str()))
			break;
	}
	
	
	
	#pragma omp parallel sections num_threads(6)
    {
        #pragma omp section
        {
			//Comment: Receive hashes from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_1, std::string("central_server/" + exp_code + "/1_hashes.txt").c_str()))
			    break;
	        }
	        //Comment: Send acknowledge for hahses
	        while(1)
	        {
		        if(SendMessage(do_1_ip, do_1_port, std::string("hashes_received_by_cs").c_str()))
			    break;
	        }
        }
        #pragma omp section
        {
			//Comment: Receive hashes from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_2, std::string("central_server/" + exp_code + "/2_hashes.txt").c_str()))
			    break;
	        }
	        //Comment: Send acknowledge for hahses
	        while(1)
	        {
		        if(SendMessage(do_2_ip, do_2_port, std::string("hashes_received_by_cs").c_str()))
			    break;
	        }
        }
        #pragma omp section
        {
			//Comment: Receive hashes from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_3, std::string("central_server/" + exp_code + "/3_hashes.txt").c_str()))
			     break;
	        }
	        //Comment: Send acknowledge for hahses
	        while(1)
	        {
		        if(SendMessage(do_3_ip, do_3_port, std::string("hashes_received_by_cs").c_str()))
			    break;
	        }
        }
		#pragma omp section
        {
			//Comment: Receive hashes from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_4, std::string("central_server/" + exp_code + "/4_hashes.txt").c_str()))
			     break;
	        }
	        //Comment: Send acknowledge for hahses
	        while(1)
	        {
		        if(SendMessage(do_4_ip, do_4_port, std::string("hashes_received_by_cs").c_str()))
			    break;
	        }
        }
		#pragma omp section
        {
			//Comment: Receive hashes from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_5, std::string("central_server/" + exp_code + "/5_hashes.txt").c_str()))
			     break;
	        }
	        //Comment: Send acknowledge for hahses
	        while(1)
	        {
		        if(SendMessage(do_5_ip, do_5_port, std::string("hashes_received_by_cs").c_str()))
			    break;
	        }
        }
		
		
    }
	
	
	//Comment: Now compute the intersection of the hashes
	vector<std::string> do_1_hashes, do_2_hashes, do_3_hashes, do_4_hashes, do_5_hashes;
	
	#pragma omp parallel sections num_threads(6)
    {
        #pragma omp section
        {
			//vector<std::string> do_1_hashes;
            ifstream hashes_of_data_1_stream("central_server/" + exp_code + "/1_hashes.txt");
	        std::string line1;
	        if (hashes_of_data_1_stream.is_open()) {

                while (!hashes_of_data_1_stream.eof() && getline(hashes_of_data_1_stream, line1)){
                    line1.erase(line1.find_last_not_of(" \n\r\t")+1);
			        do_1_hashes.push_back(line1);	 
                }

            } else{
                cout << "File opening error";
            }
        }
        #pragma omp section
        {
			//vector<std::string> do_2_hashes;
            ifstream hashes_of_data_2_stream("central_server/" + exp_code + "/2_hashes.txt");
	        std::string line2;
	        if (hashes_of_data_2_stream.is_open()) {

                while (!hashes_of_data_2_stream.eof() && getline(hashes_of_data_2_stream, line2)){
                    line2.erase(line2.find_last_not_of(" \n\r\t")+1);
			        do_2_hashes.push_back(line2);	 
                }

            } else{
                cout << "File opening error";
            }
        }
        #pragma omp section
        {
			//vector<std::string> do_3_hashes;
            ifstream hashes_of_data_3_stream("central_server/" + exp_code + "/3_hashes.txt");
	        std::string line3;
	        if (hashes_of_data_3_stream.is_open()) {

                while (!hashes_of_data_3_stream.eof() && getline(hashes_of_data_3_stream, line3)){
                    line3.erase(line3.find_last_not_of(" \n\r\t")+1);
			        do_3_hashes.push_back(line3);	 
                }

            } else{
                cout << "File opening error";
            }
        }
		#pragma omp section
        {
			//vector<std::string> do_4_hashes;
            ifstream hashes_of_data_4_stream("central_server/" + exp_code + "/4_hashes.txt");
	        std::string line4;
	        if (hashes_of_data_4_stream.is_open()) {

                while (!hashes_of_data_4_stream.eof() && getline(hashes_of_data_4_stream, line4)){
                    line4.erase(line4.find_last_not_of(" \n\r\t")+1);
			        do_4_hashes.push_back(line4);	 
                }

            } else{
                cout << "File opening error";
            }
        }
		#pragma omp section
        {
			//vector<std::string> do_5_hashes;
            ifstream hashes_of_data_5_stream("central_server/" + exp_code + "/5_hashes.txt");
	        std::string line5;
	        if (hashes_of_data_5_stream.is_open()) {

                while (!hashes_of_data_5_stream.eof() && getline(hashes_of_data_5_stream, line5)){
                    line5.erase(line5.find_last_not_of(" \n\r\t")+1);
			        do_5_hashes.push_back(line5);	 
                }

            } else{
                cout << "File opening error";
            }
        }
		
    }
	
	//cout << "size of vectors  " << do_1_hashes.size() << "  " <<  do_2_hashes.size() << "  " <<  do_3_hashes.size() << " " << do_4_hashes.size() << " " << do_5_hashes.size() << endl;
	
	
	#pragma omp parallel sections num_threads(6)
    {
        #pragma omp section
        {
			std::sort(do_1_hashes.begin(), do_1_hashes.end());
        }
        #pragma omp section
        {
			std::sort(do_2_hashes.begin(), do_2_hashes.end());
        }
        #pragma omp section
        {
			std::sort(do_3_hashes.begin(), do_3_hashes.end());
        }
		#pragma omp section
        {
			std::sort(do_4_hashes.begin(), do_4_hashes.end());
        }
		#pragma omp section
        {
			std::sort(do_5_hashes.begin(), do_5_hashes.end());
        }
		
    }
	
	
	
	vector<std::string> intersect12;
    set_intersection(do_1_hashes.begin(),do_1_hashes.end(),do_2_hashes.begin(),do_2_hashes.end(),
                  std::back_inserter(intersect12));
				  
	vector<std::string> intersect123;
    set_intersection(intersect12.begin(),intersect12.end(),do_3_hashes.begin(),do_3_hashes.end(),
                  std::back_inserter(intersect123));
				  
	vector<std::string> intersect1234;
    set_intersection(intersect123.begin(),intersect123.end(),do_4_hashes.begin(),do_4_hashes.end(),
                  std::back_inserter(intersect1234));
				  
	vector<std::string> intersect12345;
    set_intersection(intersect1234.begin(),intersect1234.end(),do_5_hashes.begin(),do_5_hashes.end(),
                  std::back_inserter(intersect12345));
	
	
	
	//Comment: Write the intersection to disk
	ofstream intersection_hash_stream("central_server/" + exp_code + "/intersection_hashes.txt");
	
	std::vector<std::string>::iterator its;
	int cardinality = 0;
    for (its = intersect12345.begin(); its != intersect12345.end(); ++its)
    {
		intersection_hash_stream << *its << endl;
		cardinality++;
    }
	

	
	
	//Comment: Send the intersection to all the data owners
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, std::string("central_server/" + exp_code + "/intersection_hashes.txt").c_str()))
			break;
	}
	ReceiveMessage(cs_port_1);
			
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, std::string("central_server/" + exp_code + "/intersection_hashes.txt").c_str()))
		    break;
	}
	ReceiveMessage(cs_port_2);
	
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, std::string("central_server/" + exp_code + "/intersection_hashes.txt").c_str()))
			break;
	}
	ReceiveMessage(cs_port_3);
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, std::string("central_server/" + exp_code + "/intersection_hashes.txt").c_str()))
			break;
	}
	ReceiveMessage(cs_port_4);
	while(1)
	{
		if(SendFile(do_5_ip, do_5_port, std::string("central_server/" + exp_code + "/intersection_hashes.txt").c_str()))
			break;
	}
	ReceiveMessage(cs_port_5);
	
	
	
	
	
	//Comment: Receive packed ciphertexts from the data owners
	#pragma omp parallel sections num_threads(6)
    {
        #pragma omp section
        {
			//Comment: Receive encrypted counts/ciphertexts from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_1, std::string("central_server/" + exp_code + "/ciphertext/1_cipher_file.encrypted").c_str()))
			    break;
	        }
	        //Comment: Send acknowledge for ciphertexts
	        while(1)
	        {
		        if(SendMessage(do_1_ip, do_1_port, std::string("ciphertext_received_by_cs").c_str()))
			    break;
	        }
			
        }
        #pragma omp section
        {
			//Comment: Receive encrypted counts/ciphertexts from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_2, std::string("central_server/" + exp_code + "/ciphertext/2_cipher_file.encrypted").c_str()))
			    break;
	        }
	        //Comment: Send acknowledge for ciphertexts
	        while(1)
	        {
		        if(SendMessage(do_2_ip, do_2_port, std::string("ciphertext_received_by_cs").c_str()))
			    break;
	        }
			
        }
        #pragma omp section
        {
			//Comment: Receive encrypted counts/ciphertexts from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_3, std::string("central_server/" + exp_code + "/ciphertext/3_cipher_file.encrypted").c_str()))
			     break;
	        }
	        //Comment: Send acknowledge for ciphertexts
	        while(1)
	        {
		        if(SendMessage(do_3_ip, do_3_port, std::string("ciphertext_received_by_cs").c_str()))
			    break;
	        }
			
        }
		#pragma omp section
        {
			//Comment: Receive encrypted counts/ciphertexts from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_4, std::string("central_server/" + exp_code + "/ciphertext/4_cipher_file.encrypted").c_str()))
			     break;
	        }
	        //Comment: Send acknowledge for ciphertexts
	        while(1)
	        {
		        if(SendMessage(do_4_ip, do_4_port, std::string("ciphertext_received_by_cs").c_str()))
			    break;
	        }
			
        }
		#pragma omp section
        {
			//Comment: Receive encrypted counts/ciphertexts from data owners
	        while(1)
	        {
		        if(ReceiveFile(cs_port_5, std::string("central_server/" + exp_code + "/ciphertext/5_cipher_file.encrypted").c_str()))
			     break;
	        }
	        //Comment: Send acknowledge for ciphertexts
	        while(1)
	        {
		        if(SendMessage(do_5_ip, do_5_port, std::string("ciphertext_received_by_cs").c_str()))
			    break;
	        }
			
        }
		
    }
	
	
	//Comment: Performing homomorphic computations
	
	//Read keys from disk
	evk_t evk(1 << 6);
    readEvalKey(evk, "central_server/eval.key");

    pk_t pk1;
    readPublicKey(pk1, "central_server/public.key");
    //pk.evk=&evk;
    pk_t pk = pk_t(pk1.a, pk1.a_shoup, pk1.b, pk1.b_shoup, evk);
	
	
    int number_of_ciphertexts;
	
	//if size is not multiple of 4096, add dummy elements
	int padding_count = DEGREE - cardinality%DEGREE;
	number_of_ciphertexts = (cardinality + padding_count -1)/DEGREE;
	
	//cout << "in CS number of ciphertexts " << number_of_ciphertexts << endl;
		
		
	//istreams to read data, ostream to write output encrypted results
	ifstream read_counts_one("central_server/" + exp_code + "/ciphertext/1_cipher_file.encrypted");
	if(read_counts_one.is_open())
		cout<< "encrypted count from do 1 opened" << endl;
	 else
		cout << "file opening error" << endl;
	
	ifstream read_counts_two("central_server/" + exp_code + "/ciphertext/2_cipher_file.encrypted");
	if(read_counts_two.is_open())
		cout<< "encrypted count from do 2 opened" << endl;
	else
		cout << "file opening error" << endl;
	
	ifstream read_counts_three("central_server/" + exp_code + "/ciphertext/3_cipher_file.encrypted");
	if(read_counts_three.is_open())
		cout<< "encrypted count from do 3 opened" << endl;
	else
		cout << "file opening error" << endl;
	
	ifstream read_counts_four("central_server/" + exp_code + "/ciphertext/4_cipher_file.encrypted");
	if(read_counts_four.is_open())
		cout<< "encrypted count from do 4 opened" << endl;
	else
		cout << "file opening error" << endl;
	
	ifstream read_counts_five("central_server/" + exp_code + "/ciphertext/5_cipher_file.encrypted");
	if(read_counts_five.is_open())
		cout<< "encrypted count from do 5 opened" << endl;
	else
		cout << "file opening error" << endl;
	   
	 //To write encrypted and randomized intersection counts to file
	ofstream intersection_counts_encrypted; //printf("214 \n");
	intersection_counts_encrypted.open("central_server/" + exp_code + "/ciphertext/result.encrypted"); //printf("215 \n");
		
	//threshold and random number
	//int threshold = 20;
	std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(3, 30); // guaranteed unbiased
    int random_integer = uni(rng);
	
	
	//Comment: construct the thtreshold polynomial
	FV::params::poly_pl threshold_poly;
	std::array<mpz_t, DEGREE> ntt_threshold_poly;
	for(int i = 0; i < DEGREE; i++)
	{
		mpz_init_set_si(ntt_threshold_poly[i], 100);
	}
	threshold_poly.mpz2poly(ntt_threshold_poly);
	
	ciphertext_t c_th;
	FV::encrypt_poly(c_th, pk, threshold_poly);
	
	for (int i = 0; i < DEGREE; i++) 
	{
        mpz_clears(ntt_threshold_poly[i], nullptr);
    }
	
	
    //measure time for hme operations
	const clock_t begin_hme_time = clock();
		
    unsigned long seek_position = 0;
		
	//hme operations
	for(int i = 0; i < number_of_ciphertexts; i++)
	{
		ciphertext_t do_one_count_cipher;
		seek_position = i*2*4*16384L;
		read_counts_one.seekg((unsigned long)seek_position, read_counts_one.beg);
		do_one_count_cipher.c0.deserialize_manually(read_counts_one);
		do_one_count_cipher.c1.deserialize_manually(read_counts_one);
		do_one_count_cipher.isnull= false;
		do_one_count_cipher.pk = &pk;
		
		
		ciphertext_t do_two_count_cipher;
		//seek_position = (*its).second.at(1)*2*4*4096L;
		read_counts_two.seekg((unsigned long)seek_position, read_counts_two.beg);
		do_two_count_cipher.c0.deserialize_manually(read_counts_two);
		do_two_count_cipher.c1.deserialize_manually(read_counts_two);
		do_two_count_cipher.isnull= false;
		do_two_count_cipher.pk = &pk;
		
		
		ciphertext_t do_three_count_cipher;
		//seek_position = (*its).second.at(2)*2*4*4096L;
		read_counts_three.seekg((unsigned long)seek_position, read_counts_three.beg);
		do_three_count_cipher.c0.deserialize_manually(read_counts_three);
		do_three_count_cipher.c1.deserialize_manually(read_counts_three);
		do_three_count_cipher.isnull= false;
		do_three_count_cipher.pk = &pk;
		
		ciphertext_t do_four_count_cipher;
		//seek_position = (*its).second.at(2)*2*4*4096L;
		read_counts_four.seekg((unsigned long)seek_position, read_counts_four.beg);
		do_four_count_cipher.c0.deserialize_manually(read_counts_four);
		do_four_count_cipher.c1.deserialize_manually(read_counts_four);
		do_four_count_cipher.isnull= false;
		do_four_count_cipher.pk = &pk;
		
		ciphertext_t do_five_count_cipher;
		//seek_position = (*its).second.at(2)*2*4*4096L;
		read_counts_five.seekg((unsigned long)seek_position, read_counts_five.beg);
		do_five_count_cipher.c0.deserialize_manually(read_counts_five);
		do_five_count_cipher.c1.deserialize_manually(read_counts_five);
		do_five_count_cipher.isnull= false;
		do_five_count_cipher.pk = &pk;
		
		
		//Comment: construct the random polynomial
	    FV::params::poly_pl random_poly;
	    std::array<mpz_t, DEGREE> ntt_random_poly;
	    for(int k = 0; k < DEGREE; k++)
	    {
		    mpz_init_set_si(ntt_random_poly[k], uni(rng));
	    }
	    random_poly.mpz2poly(ntt_random_poly);
	
	    ciphertext_t c_rand;
	    FV::encrypt_poly(c_rand, pk, random_poly);
	
	    for (int k = 0; k < DEGREE; k++) 
	    {
            mpz_clears(ntt_random_poly[k], nullptr);
        }
		
		    
	    ciphertext_t count_sum_cipher;
		count_sum_cipher = (do_one_count_cipher + do_two_count_cipher + do_three_count_cipher + do_four_count_cipher + do_five_count_cipher - c_th) * c_rand;
		
		count_sum_cipher.c0.serialize_manually(intersection_counts_encrypted);
		count_sum_cipher.c1.serialize_manually(intersection_counts_encrypted);
			
    }
		
	const clock_t end_hme_time = clock();
	float hme_time = float(end_hme_time - begin_hme_time)/CLOCKS_PER_SEC;
	//cout << "HME time is " << hme_time << " seconds" << endl;
	
    //cout << "HME done" << endl;
		
		
	intersection_counts_encrypted.close();
	read_counts_five.close();
	read_counts_four.close();
	read_counts_three.close();
	read_counts_two.close();
	read_counts_one.close();
	
	
	
	
	//Comment: Send the encrypted result to the data owners
	//measure time to send the ciphers
	//const clock_t begin_cipher_sending_time = clock();
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, std::string("central_server/" + exp_code + "/ciphertext/result.encrypted").c_str()))
			break;
	}
	//measure time to send the ciphers
	//const clock_t end_cipher_sending_time = clock();
	//float cipher_sending_time = float(end_cipher_sending_time - begin_cipher_sending_time)/CLOCKS_PER_SEC;
	//cout << "Cipher sending time is " << cipher_sending_time << " seconds" << endl;
	ReceiveMessage(cs_port_1);
			
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, std::string("central_server/" + exp_code + "/ciphertext/result.encrypted").c_str()))
		    break;
	}
	ReceiveMessage(cs_port_2);
	
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, std::string("central_server/" + exp_code + "/ciphertext/result.encrypted").c_str()))
			break;
	}
	ReceiveMessage(cs_port_3);
	
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, std::string("central_server/" + exp_code + "/ciphertext/result.encrypted").c_str()))
			break;
	}
	ReceiveMessage(cs_port_4);
	
	while(1)
	{
		if(SendFile(do_5_ip, do_5_port, std::string("central_server/" + exp_code + "/ciphertext/result.encrypted").c_str()))
			break;
	}
	ReceiveMessage(cs_port_5);
	
	cout << "System protocol executed successfully" << endl;
	
	//sleep(1);
	
    return 0;
}