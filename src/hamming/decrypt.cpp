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

	int numd1, numd2;
	// Read number of ciphers
	fstream resultFile("result.txt", fstream::in);	
	seekPastChar(resultFile, '[');
	resultFile >> numd1;
	seekPastChar(resultFie, ']');
	seekPastChar(resultFile, '[');
	resultFile >> numd2;
	seekPastChar(resultFile, ']');

	std::unique_ptr<Ctxt> ctxts[numd1][numd2];

	for(int i = 0; i < numd1; i++)
	{
		for(int j = 0; j < numd2; j++)
		{
			ctxts[i][j].reset(new Ctxt(publicKey));
			resultFile >> *ctxts[i][j];
		}
	}	

	Ctxt ctxt(publicKey);
	ciphertextFile >> ctxt;
	
    	int nslots = 32;
	NTL::ZZX result[numd1][numd2];
	
	for(int i = 0; i < numd1; i++)
	{
		for(int j = 0; j < numd2; j++)
		{
			secretKey.Decrypt(result[i][j], *ctxts[i][j]);
		}
	}	

	return 0;
}
