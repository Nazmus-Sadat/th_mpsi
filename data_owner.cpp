/*
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/gpl-3.0.txt
 */
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string.h>
#include <sstream>
#include <cstddef>
#include <gmp.h>
#include <gmpxx.h>
#include <nfl.hpp>
#include <time.h>
#include <sstream>
#include <map>
#include <openssl/sha.h>
#include "lib/params/params.cpp"

#include "lib/prng/fastrandombytes.cpp"
#include "lib/prng/randombytes.cpp"

//#include "fvnamespace.h"
#include "fvnamespace2.h"
//#include "nfl/prng/crypto_stream_salsa20.h"

//#include "FV.hpp"
#include "FV_2.hpp"



#include "common/INIReader.h"
#include "common/network.cpp"


using namespace std;
#include <openssl/sha.h>
using namespace FV;


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

void readSecretKey(sk_t &sk, std::string path) {
    string line;
    ifstream myfile(path);
    if (myfile.is_open()) {
        sk.value.deserialize_manually(myfile);
        sk.value_shoup = nfl::compute_shoup(sk.value);
        myfile.close();
    } else cout << "Unable to open file";
}

void readEvalKey(evk_t &evk, std::string path) {
    string line;
    ifstream myevalfile(path);

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


void printEncValPrint(ciphertext_t &ct, sk_t &sk, pk_t &pk) {
    mess_t m_dec;

    decrypt(m_dec, sk, pk, ct);
    //std::cout <<m_dec;
    mpz_t tmp;
    FV::params::poly_t pt = (FV::params::poly_t) m_dec.getValue();
    std::array<mpz_t, DEGREE> arr = pt.poly2mpz();
    mpz_t &s = arr[0];
    mpz_init(tmp);
    mpz_cdiv_q_ui(tmp, FV::params::plaintextModulus<mpz_class>::value().get_mpz_t(), 2);
    //std::cout<<tmp<<std::endl;std::cout<<s<<std::endl;
    if (mpz_cmp(s, tmp) > 0) {
        mpz_sub(tmp, FV::params::plaintextModulus<mpz_class>::value().get_mpz_t(), s);
        //mpz_tdiv_q(tmp,tmp,quantization);
        //std::cout << "beta " << i << ": -" <<tmp << std::endl;
        std::cout << "-" << tmp;
    } else {
        //mpz_tdiv_q(tmp,tmp,quantization);
        //std::cout << "beta " << i << ": " << m_dec << std::endl;
        std::cout << m_dec;
    }
    mpz_clear(tmp);

    //return;
}

std::string sections(INIReader &reader)
{
    std::stringstream ss;
    std::set<std::string> sections = reader.Sections();
    for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
        ss << *it << ",";
    return ss.str();
}

string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}



int main(int argc, char ** argv) {
    if (argc < 2) {printf("Please enter the id of data owner. \n"); return 0;}
	const char* do_id = argv[1];
	string do_id_str = string(do_id);
	
	uint16 do_port;
	
    

	
    #ifdef WIN32
       // Windows requires this to start up the networking API
       WORD versionWanted = MAKEWORD(1, 1);
       WSADATA wsaData;
       (void) WSAStartup(versionWanted, &wsaData);
    #else 
       // avoid SIGPIPE signals caused by sending on a closed socket
       signal(SIGPIPE, SIG_IGN);
    #endif
	
	//ip_address do_1_ip = GetHostByName(do_1_ip_str.c_str());
	switch(do_id_str[0]) {
      case '1' :
         do_port = 34001; 
         break;
      case '2' :
	     do_port = 34002; 
         break;
      case '3' :
         do_port = 34003; 
         break;
      case '4' :
         do_port = 34004; 
         break;
      case '5' :
         do_port = 34005; 
         break;
	  case '6' :
         do_port = 34006; 
         break;
      default :
         cout << "Invalid data owner id or port" << endl;
   }
	
	
	//ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/config.ini").c_str());
    //Comment: Receive config file from central server
	while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/config.ini").c_str()))
			break;
	}
	
	INIReader config_reader(std::string("data_owner"+ do_id_str +"/config.ini"));

    if (config_reader.ParseError() < 0) {
        std::cout << "Can't load config file \n";
        return 1;
    }
	
	std::string exp_code = config_reader.Get("settings", "exp_code", "UNKNOWN");
	
	std::string cs_ip_str =  config_reader.Get("ip_and_port", "cs_ip", "UNKNOWN");
	uint16 cs_port =  config_reader.GetInteger("ip_and_port", std::string("cs_port_" + do_id_str), -1);
	
	std::string csp_ip_str =  config_reader.Get("ip_and_port", "csp_ip", "UNKNOWN");
	uint16 csp_port =  config_reader.GetInteger("ip_and_port", "csp_port", -1);
	
	//Comment: Message for acknowledgement
	std::string message="";
	
	ip_address cs_ip = GetHostByName(cs_ip_str.c_str());
	//Comment: Send acknowledgement message for receiving config file successfully
	while(1)
	{
		if(SendMessage(cs_ip, cs_port, std::string("config_file_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	
	
	//Comment: Receive keys from csp
	ip_address csp_ip = GetHostByName(csp_ip_str.c_str());
	
	//ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/secret.key").c_str());
	while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/secret.key").c_str()))
			break;
	}
	//Comment: Send acknowledgement for secret key
	while(1)
	{
		if(SendMessage(csp_ip, csp_port, std::string("secret_key_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	//ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/eval.key").c_str());
    while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/eval.key").c_str()))
			break;
	}
	while(1)
	{
		if(SendMessage(csp_ip, csp_port, std::string("eval_key_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	//ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/public.key").c_str()); 
	while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/public.key").c_str()))
			break;
	}
	while(1)
	{
		if(SendMessage(csp_ip, csp_port, std::string("public_key_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	
	//Comment: Receive salt from csp
	while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str +"/salt.txt").c_str()))
			break;
	}
	//Send acknowledgement for salt
	while(1)
	{
		if(SendMessage(csp_ip, csp_port, std::string("salt_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	//Comment: read the salt
	ifstream salt_stream("data_owner"+ do_id_str + "/salt.txt");
	std::string salt;
	std::string line;
	if(salt_stream.is_open()  && getline(salt_stream, line))
	{
		line.erase(line.find_last_not_of(" \n\r\t")+1);
		salt = line;
	}
	
	//Comment: Read data files and hash the bigrams
	//cout << sha256("1234567890_1") << endl;
	
	multimap<std::string, int> data_owner_map; 
    multimap<std::string, std::string> hash_pt_map; 	
	
    ifstream data_stream("data_owner" + do_id_str + "/" + exp_code + "/" + exp_code + "_" + do_id_str +"_pt_count_map.txt");
	line = "";
	
	ofstream hash_writer; 
	hash_writer.open("data_owner" + do_id_str + "/" + exp_code + "/" + exp_code + "_" + do_id_str +"_hashes.txt"); 	
	int test_count = 0;
	if (data_stream.is_open()) {

        while (!data_stream.eof() && getline(data_stream, line)) {
			 line.erase(line.find_last_not_of(" \n\r\t")+1);
			 
             std::string hash_part, count_part;
			 hash_part = sha256(line.substr(0, line.find("=")) + salt);
             count_part = line.substr(line.find("=")+1, line.length());
			 
			 data_owner_map.insert(make_pair(hash_part, atoi(count_part.c_str())));
			 
			 hash_pt_map.insert(make_pair(hash_part, line.substr(0, line.find("="))));
			
			 hash_writer << hash_part << "\n";
			 
        }
		
		hash_writer.close();

    } else {
        cout << "File opening error";
    }
	
	//Comment: Send hash fils to the central server
	//measure time to send the hashes
	//const clock_t begin_hash_sending_time = clock();
	
	while(1)
	{
		if(SendFile(cs_ip, cs_port, std::string("data_owner" + do_id_str + "/" + exp_code + "/" + exp_code + "_" + do_id_str +"_hashes.txt").c_str()));
			break;
	}
	//measure time to send the hashes
	//const clock_t end_hash_sending_time = clock();
	//float hash_sending_time = float(end_hash_sending_time - begin_hash_sending_time)/CLOCKS_PER_SEC;
	//cout << "Hash sending time is " << hash_sending_time << " seconds" << endl;
	//Comment: Receive acknowledge 
	ReceiveMessage(do_port);
	
	
	
	//Comment: Receive the intersection of hashes
	while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str + "/" + exp_code +"/intersection_hashes.txt").c_str()))
			break;
	}
	//Comment: Send acknowledgement for the intersection
	while(1)
	{
		if(SendMessage(cs_ip, cs_port, std::string("hash_intersection_file_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	
	
	//Comment: Process and encrypt counts of the intersected elements' counts
	
	//Read Keys
	sk_t sk;
    readSecretKey(sk, "data_owner"+ do_id_str + "/secret.key");

    evk_t evk(1 << 6);
    readEvalKey(evk, "data_owner" + do_id_str + "/eval.key");

    pk_t pk1;
    readPublicKey(pk1, "data_owner" + do_id_str + "/public.key");
    //pk.evk=&evk;
    pk_t pk = pk_t(pk1.a, pk1.a_shoup, pk1.b, pk1.b_shoup, evk);
	
	ifstream intersection_stream("data_owner"+ do_id_str + "/" + exp_code +"/intersection_hashes.txt");
	line = "";
	vector<int> counts_vector;
	 
	
	if (intersection_stream.is_open()) {

        while (!intersection_stream.eof() && getline(intersection_stream, line)) {
			//trim the line to prepare for matching //What's the best way to trim std::string?
			line.erase(line.find_last_not_of(" \n\r\t")+1);
			if(data_owner_map.find(line) != data_owner_map.end())
			{
				counts_vector.push_back(data_owner_map.find(line)->second);
				//hash_pt_map.insert(make_pair(line, data_owner_map.find(line)->first));
			}
        }

    } else {
        cout << "File opening error";
    }
	
	intersection_stream.close();
	
	line = "";
	
	//if size is not multiple of 4096, add dummy elements
	if(counts_vector.size()%DEGREE != 0)
	{
		int padding_count = DEGREE - counts_vector.size()%DEGREE;
		for(int i =0; i < padding_count; i++)
		{
			counts_vector.push_back(0);
		}
	}
	
	//cout << "size of the vector " << counts_vector.size() << endl;
	
	//Encryption
	//measure encryption time
	//const clock_t begin_encryption_time = clock();
	
	int index  = 0;
	vector<int>::iterator it_vector;
	int number_of_ciphertexts = 0;
	
	FV::params::poly_pl data_poly;
	std::array<mpz_t, DEGREE> mpz_arr_data_poly;
		
	ofstream packed_cipher_stream; //printf("214 \n");
	packed_cipher_stream.open("data_owner" + do_id_str + "/" + exp_code + "/cipher_file.encrypted"); 	
	
	for(it_vector = counts_vector.begin(); it_vector != counts_vector.end(); ++it_vector) 
	{
		mpz_init_set_si(mpz_arr_data_poly[index], *it_vector);
		
		if(index == DEGREE - 1)
		{
			data_poly.mpz2poly(mpz_arr_data_poly);
			ciphertext_t c;
		    FV::encrypt_poly(c, pk, data_poly);
			c.c0.serialize_manually(packed_cipher_stream);
		    c.c1.serialize_manually(packed_cipher_stream);
			number_of_ciphertexts++; 
			
			for (int k = 0; k < DEGREE; k++) 
	        {
                mpz_clears(mpz_arr_data_poly[k], nullptr);
            }
			
			index  = 0;
			continue;
		}
		index++;
    
    }
	
	packed_cipher_stream.close();
	
	//measure encryption time
	//const clock_t end_encryption_time = clock();
	//float encryption_time = float(end_encryption_time - begin_encryption_time)/CLOCKS_PER_SEC;
	//cout << "Encryption time is " << encryption_time << " seconds" << endl;
	
	
	ofstream number_of_cipher_writer; 
	number_of_cipher_writer.open("data_owner" + do_id_str + "/" + exp_code + "/" + "ciphertext_count.txt"); 	
	number_of_cipher_writer << number_of_ciphertexts;
	number_of_cipher_writer.close();
	
	
	//Comment: Send packed ciphertexts to the central server
	//measure time to send the ciphers
	//const clock_t begin_cipher_sending_time = clock();
	while(1)
	{
		if(SendFile(cs_ip, cs_port, std::string("data_owner" + do_id_str + "/" + exp_code + "/cipher_file.encrypted").c_str()));
			break;
	}
	//measure time to send the ciphers
	//const clock_t end_cipher_sending_time = clock();
	//float cipher_sending_time = float(end_cipher_sending_time - begin_cipher_sending_time)/CLOCKS_PER_SEC;
	//cout << "Cipher sending time is " << cipher_sending_time << " seconds" << endl;
	//Comment: Receive acknowledge 
	ReceiveMessage(do_port);
	
	
	//Comment: Receive the encrypted result from the central server
	while(1)
	{
		if(ReceiveFile(do_port, std::string("data_owner"+ do_id_str + "/" + exp_code +"/result.encrypted").c_str()))
			break;
	}
	//Comment: Send acknowledgement for the receiving result
	while(1)
	{
		if(SendMessage(cs_ip, cs_port, std::string("encrypted_result_received_by_data_owner_" + do_id_str).c_str()))
			break;
	}
	
	
	//Comment: Read the received result 
	ifstream result_stream("data_owner"+ do_id_str + "/" + exp_code +"/result.encrypted");
	if(result_stream.is_open())
		cout<< "Processing encrypted result received from the central server ..." << endl;
	else
		cout << "file opening error" << endl;
	
	ofstream decrypted_result_stream;
	decrypted_result_stream.open("data_owner"+ do_id_str + "/" + exp_code +"/plaintext_output.txt");
	
	//Comment: Decrypt the received result
	unsigned long seek_position = 0;
	
	int position = 0;
	vector<int> position_vector;
	
	//measure time for decryption
	//const clock_t begin_decrypt_time = clock();

    for(int i = 0; i < number_of_ciphertexts; i++)
	{
		ciphertext_t result_cipher;
		//if(i != 0)
		seek_position = i*2*4*16384L;
		result_stream.seekg((unsigned long)seek_position, result_stream.beg);
		result_cipher.c0.deserialize_manually(result_stream);
		result_cipher.c1.deserialize_manually(result_stream);
		result_cipher.isnull= false;
		result_cipher.pk = &pk;
		
		// Initialize polym
        std::array<mpz_t, DEGREE> poly_for_decrypt;
        for (size_t k = 0; k < DEGREE; k++) {
            mpz_inits(poly_for_decrypt[k], nullptr);
        }
		
		// decrypt to the polym
        FV::decrypt_poly(poly_for_decrypt, sk, pk, result_cipher, true);
		
		for (size_t m = 0; m < DEGREE; m++) {
            //std::cout << mpz_class(poly_for_decrypt[i]).get_str() << "*X^" << i
            //<< (i == coeff_count - 1 ? ";\n" : "+");
			mpz_t tmp;	
            //mpz_t &s = poly_for_decrypt[m];
            mpz_init(tmp);
            mpz_cdiv_q_ui(tmp, FV::params::plaintextModulus<mpz_class>::value().get_mpz_t(), 2);
    
            if (mpz_cmp(poly_for_decrypt[m], tmp) > 0) {
                mpz_sub(tmp, FV::params::plaintextModulus<mpz_class>::value().get_mpz_t(), poly_for_decrypt[m]);
                //std::cout << "-" << tmp << endl;
				decrypted_result_stream << "-" << tmp << endl;
				//position_vector.push_back(position);
            } else {
       
                //std::cout << mpz_class(poly_for_decrypt[m]).get_str() << endl;
				decrypted_result_stream << mpz_class(poly_for_decrypt[m]).get_str() << endl;
				position_vector.push_back(position);
            }
            mpz_clear(tmp);	
			
			position++;
				  
			//cout << mpz_class(poly_for_decrypt[m]).get_str() << endl;
			//decrypted_result_stream << mpz_class(poly_for_decrypt[m]).get_str() << endl;

        }
		
			
	}
	
	//measure decryption time
	//const clock_t end_decrpt_time = clock();
	//float decrypt_time = float(end_decrpt_time - begin_decrypt_time)/CLOCKS_PER_SEC;
	//cout << "Decryption time is " << decrypt_time << " seconds" << endl;
	
	//cout << "Number of elements meeting the threshold criteria: " << position_vector.size() << endl;
	
	decrypted_result_stream.close();
	
	cout << "System protocol executed successfully, now performing local computation ....." << endl;
	
	line = "";
	int position_check = 0;
	
	ifstream intersection_stream1("data_owner"+ do_id_str + "/" + exp_code +"/intersection_hashes.txt");
	
	ofstream text_result_stream;
	text_result_stream.open("data_owner"+ do_id_str + "/" + exp_code +"/plaintext_output_bigrams.txt");
	
	if (intersection_stream1.is_open()) {

        while (!intersection_stream1.eof() && getline(intersection_stream1, line)) {
			//trim the line to prepare for matching //What's the best way to trim std::string?
			line.erase(line.find_last_not_of(" \n\r\t")+1);
			
			if(std::find(position_vector.begin(), position_vector.end(), position_check) !=  position_vector.end())
			{
				if(hash_pt_map.find(line) != hash_pt_map.end())
			    {
				    //cout <<  hash_pt_map.find(line)->second << endl;
					text_result_stream << hash_pt_map.find(line)->second << endl;
			    }
				
			}
			
			
			position_check++;
        }

    } else {
        cout << "File opening error";
    }
	
	intersection_stream1.close();
	text_result_stream.close();
	


    return 0;
}

