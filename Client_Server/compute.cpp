#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include <NTL/ZZX.h>
#include <iostream>

using namespace std;

//script to read ciphertexts and compute

Ctxt hamming_dist(Ctxt enc1, Ctxt enc2, int numbits) {
    Ctxt enc11 = enc1;
	Ctxt enc22 = enc2;
	NTL::ZZX con;
	con.SetLength(numbits);
	for(int i=0;i<numbits;i++)
		con[i] = 1;
	enc11.multByConstant(con);
	enc22.multByConstant(con);
	Ctxt hammingenc = enc1;
	hammingenc *= enc2;
	hammingenc.multByConstant(NTL::ZZX(2));
	hammingenc -= enc11;
	hammingenc -= enc22;
	hammingenc.multByConstant(NTL::ZZX(-1));
	return hammingenc;
}

int main(int argc, char **argv)
{
    // Read the public key from disk
	fstream pubKeyFile("pubkey.txt", fstream::in);
	assert(pubKeyFile.is_open());
	unsigned long m, p, r;
	vector<long> gens, ords;

	readContextBase(pubKeyFile, m, p, r, gens, ords);
	FHEcontext context(m, p, r, gens, ords);
	pubKeyFile >> context;

	FHEPubKey publicKey(context);
	pubKeyFile >> publicKey;	
	
	pubKeyFile.close();

    // Read ciphertexts from file
	fstream ciphertextFile("ciphertext.txt", fstream::in);	
	Ctxt ctxt1(publicKey);
    Ctxt ctxt2(publicKey);

    ciphertextFile >> ctxt1;
    ciphertextFile >> ctxt2;

    Ctxt hammingenc = hamming_dist(ctxt1, ctxt2, 32);

    // Output ciphertext to file
	std::fstream resultFile("result.txt", fstream::out|fstream::trunc);
	assert(resultFile.is_open());
	resultFile << hammingenc;
    //ciphertexts written to file
    std::cout << "Ciphertexts written to file!" << std::endl;
}