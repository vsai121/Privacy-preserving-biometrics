#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include <NTL/ZZX.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	// Read secret key from file
	
	fstream secKeyFile("seckey.txt", fstream::in);
	unsigned long m, p, r;
	vector<long> gens, ords;
	readContextBase(secKeyFile, m, p, r, gens, ords);
	FHEcontext context(m, p, r, gens, ords);
	secKeyFile >> context;

	FHESecKey secretKey(context);
	const FHEPubKey& publicKey = secretKey;

	secKeyFile >> secretKey;


	// Read ciphertext from file
	fstream ciphertextFile("result.txt", fstream::in);	
	Ctxt ctxt(publicKey);
	ciphertextFile >> ctxt;
	
    	int nslots = 32;
	NTL::ZZX result;
	secretKey.Decrypt(result, ctxt);
	
	std::cout << "Hamming Distance: " << std::endl <<  result[nslots*8-1] << std::endl;

	return 0;
}
