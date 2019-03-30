#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include "myUtils.h"
#include <NTL/ZZX.h>
#include <iostream>

using namespace std;

//script to generate keys and write to a file

int main(int argc, char **argv)
{
	long p = 257;                                // Plaintext base [default=2], should be a prime number
	long r = 1;                                  // Lifting [default=1]
	long L = 15;                                 // Number of levels in the modulus chain [default=heuristic]
	long c = 2;                                  // Number of columns in key-switching matrix [default=2]
	long w = 64;                                 // Hamming weight of secret key
	long d = 1;                                  // Degree of the field extension [default=1]
	long k = 80;                                 // Security parameter [default=80]
	long s = 0;                                  // Minimum number of slots [default=0]
	bool verbose = false;                        // Output message about m and related parameters
	bool chosen_m = false;                       // Specify the existance of a particular m
	long m = FindM(k, L, c, p, d, s, chosen_m, verbose);
	FHEcontext context(m, p, r); 	             
	buildModChain(context, L, c);
	
	fstream secKeyFile("seckey.txt", fstream::out|fstream::trunc);
	fstream pubKeyFile("pubkey.txt", fstream::out|fstream::trunc);	

	assert(secKeyFile.is_open());
	assert(pubKeyFile.is_open());	

	writeContextBase(secKeyFile, context);
	writeContextBase(pubKeyFile, context);	

	secKeyFile << context << std::endl;
	pubKeyFile << context << std::endl;	

	FHESecKey secretKey(context);                
	const FHEPubKey& publicKey = secretKey;      
	secretKey.GenSecKey(w);
	addSome1DMatrices(secretKey);

	// Output keys to files
	secKeyFile << secretKey << std::endl;
	pubKeyFile << publicKey << std::endl;

	secKeyFile.close();
	pubKeyFile.close();

	return 0;
}
