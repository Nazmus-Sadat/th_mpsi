
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include <gmp.h>
#include <gmpxx.h>
#include <nfl.hpp>
#include <time.h>
#include <string>
#include "lib/params/params.cpp"

#include "lib/prng/fastrandombytes.cpp"
#include "lib/prng/randombytes.cpp"

#include "fvnamespace2.h"

#include "FV_2.hpp"

#include <dirent.h>

#include <vector>
#include <random>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n


#include "common/INIReader.h"
#include "common/network.cpp"

using namespace std;
using namespace FV;

typedef std::vector<char> char_array;


void saveSecretKey(const char* filename, sk_t &sk){
    ofstream myfile;
    myfile.open (filename);
    sk.value.serialize_manually(myfile);
    
    myfile.close();

}

void savePublicKey(const char* filename, pk_t &pk){
    ofstream myfile;
    myfile.open (filename);
    pk.a.serialize_manually(myfile);
    pk.b.serialize_manually(myfile);

    myfile.close();

}

void saveEvalKey(const char* filename, evk_t &evk){
    ofstream myfile;
    myfile.open (filename);

    for (unsigned j = 0; j < evk.ell; ++j) {
        evk.values[j][0].serialize_manually(myfile);
        evk.values[j][1].serialize_manually(myfile);
    }
    myfile.close();

}

void generateKey()
{
	sk_t sk;
    string secret_key_file = "csp/secret.key";
    saveSecretKey(secret_key_file.c_str(),sk);
    string eval_key_file = "csp/eval.key";

    int word_size = 1 << 6;
    evk_t evk(sk, word_size);
    saveEvalKey(eval_key_file.c_str(),evk);

    pk_t pk(sk, evk);
    string public_key_file = "csp/public.key";
    savePublicKey(public_key_file.c_str(),pk);
}

char_array charset()
{
    //Change this to suit
    return char_array( 
	{'0','1','2','3','4',
	'5','6','7','8','9',
	'A','B','C','D','E','F',
	'G','H','I','J','K',
	'L','M','N','O','P',
	'Q','R','S','T','U',
	'V','W','X','Y','Z',
	'a','b','c','d','e','f',
	'g','h','i','j','k',
	'l','m','n','o','p',
	'q','r','s','t','u',
	'v','w','x','y','z'
	});
};    

std::string random_string( size_t length, std::function<char(void)> rand_char )
{
    std::string str(length,0);
    std::generate_n( str.begin(), length, rand_char );
    return str;
}


int main() {
/*
    std::string dir_to_open_for_check = "csp/";
    auto dir_check = opendir(dir_to_open_for_check.c_str());
    auto entity_check = readdir(dir_check);

    bool key_generated = false;
	int file_count = 0;
    while(entity_check != NULL)
    {
        if(entity_check->d_type == DT_REG)
        { //regular file
           file_count++;
        }
        entity_check = readdir(dir_check);
    }
	if(file_count >= 3) key_generated = true;
	
	if(!key_generated) generateKey();
*/

    generateKey();	
	uint16 csp_port = 34000;
	
	#ifdef WIN32
       // Windows requires this to start up the networking API
       WORD versionWanted = MAKEWORD(1, 1);
       WSADATA wsaData;
       (void) WSAStartup(versionWanted, &wsaData);
    #else 
       // avoid SIGPIPE signals caused by sending on a closed socket
       signal(SIGPIPE, SIG_IGN);
    #endif

    while(1)
	{
		if(ReceiveFile(csp_port, std::string("csp/config.ini").c_str()))
			break;
	}
	
	
	INIReader config_reader("csp/config.ini");

    if (config_reader.ParseError() < 0) {
        std::cout << "Can't load config file \n";
        return 1;
    }
			  
	std::string cs_ip_str =  config_reader.Get("ip_and_port", "cs_ip", "UNKNOWN");
	uint16 cs_port =  config_reader.GetInteger("ip_and_port", "cs_port", -1);
	
	std::string do_1_ip_str =  config_reader.Get("ip_and_port", "do_1_ip", "UNKNOWN");
	uint16 do_1_port =  config_reader.GetInteger("ip_and_port", "do_1_port", -1);
	
	std::string do_2_ip_str =  config_reader.Get("ip_and_port", "do_2_ip", "UNKNOWN");
	uint16 do_2_port =  config_reader.GetInteger("ip_and_port", "do_2_port", -1);
	
	std::string do_3_ip_str =  config_reader.Get("ip_and_port", "do_3_ip", "UNKNOWN");
	uint16 do_3_port =  config_reader.GetInteger("ip_and_port", "do_3_port", -1);
	
	std::string do_4_ip_str =  config_reader.Get("ip_and_port", "do_4_ip", "UNKNOWN");
	uint16 do_4_port =  config_reader.GetInteger("ip_and_port", "do_4_port", -1);
	
	//Comment: message for acknowledgement management
	std::string message = "";
	
	
	ip_address cs_ip = GetHostByName(cs_ip_str.c_str());
	
	while(1)
	{
		if(SendMessage(cs_ip, cs_port, std::string("config_file_received_by_csp").c_str()))
			break;
	}
	
	
	//Comment: Send keys to central server cs 
	//SendFile(cs_ip, cs_port, "csp/eval.key");
	while(1)
	{
		if(SendFile(cs_ip, cs_port, "csp/eval.key"))
			break;
	}
	//Receive acknowledgement from CS 
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(cs_ip, cs_port, "csp/public.key"))
			break;
	}
	//Receive acknowledgement from CS 
	ReceiveMessage(csp_port);
	
	
	
	
	
	//Comment: Send keys to data owner 1
	ip_address do_1_ip = GetHostByName(do_1_ip_str.c_str());
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, "csp/secret.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, "csp/eval.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, "csp/public.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	
	
	//Comment: Send keys to data owner 2
	ip_address do_2_ip = GetHostByName(do_2_ip_str.c_str());
	//SendFile(do_2_ip, do_2_port, "csp/secret.key");
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, "csp/secret.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, "csp/eval.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, "csp/public.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	
	//Comment: Send keys to data owner 3
	ip_address do_3_ip = GetHostByName(do_3_ip_str.c_str());
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, "csp/secret.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, "csp/eval.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, "csp/public.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	
	//Comment: Send keys to data owner 4
	ip_address do_4_ip = GetHostByName(do_4_ip_str.c_str());
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, "csp/secret.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, "csp/eval.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, "csp/public.key"))
			break;
	}
	ReceiveMessage(csp_port);
	
	
	//Generate salt
	const auto ch_set = charset();
    std::default_random_engine rng(std::random_device{}()); 
    std::uniform_int_distribution<> dist(0, ch_set.size()-1);
    auto randchar = [ ch_set,&dist,&rng ](){return ch_set[ dist(rng) ];};
    auto length = 64;
	ofstream salt_writer; 
	salt_writer.open("csp/salt.txt");
	salt_writer << random_string(length,randchar);
	salt_writer.close();
	
	//Comment: Send salt to all the parties
	while(1)
	{
		if(SendFile(do_1_ip, do_1_port, "csp/salt.txt"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_2_ip, do_2_port, "csp/salt.txt"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_3_ip, do_3_port, "csp/salt.txt"))
			break;
	}
	ReceiveMessage(csp_port);
	
	while(1)
	{
		if(SendFile(do_4_ip, do_4_port, "csp/salt.txt"))
			break;
	}
	ReceiveMessage(csp_port);


    return 0;
}
