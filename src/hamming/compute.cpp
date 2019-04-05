#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include <NTL/ZZX.h>
#include <iostream>
#include <vector>

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
	int nslots = 32;

	readContextBase(pubKeyFile, m, p, r, gens, ords);
	FHEcontext context(m, p, r, gens, ords);
	pubKeyFile >> context;

	FHEPubKey publicKey(context);
	pubKeyFile >> publicKey;	
	
	pubKeyFile.close();
	std::cout << "Read cipher" << std::endl;
	fstream ctext1file("ciphertext.txt", fstream::in);
	int numd1;
	seekPastChar(ctext1file, '[');
	ctext1file >> numd1;
	seekPastChar(ctext1file, ']');
	vector<Ctxt> client_des;
	for(int i = 0; i < numd1; i++) {
		Ctxt des(publicKey);
		ctext1file >> des;
		client_des.push_back(des);
	}
	ctext1file.close();
	std::cout << "Read cipher" << std::endl;
	fstream ctext2file("ciphertext2.txt", fstream::in);
	seekPastChar(ctext2file, '[');
	int numd2;
	ctext2file >> numd2;
	seekPastChar(ctext2file, ']');
	vector<Ctxt> server_des;
	for(int i = 0; i < numd2; i++) {
		Ctxt des(publicKey);
		ctext2file >> des;
		server_des.push_back(des);
	}
	ctext2file.close();
	std::cout << "Started computing" << std::endl;
	vector<Ctxt> computed;
	for(int i=0; i<numd1; i++) {
		Ctxt c1 = client_des.at(i);
		for(int j=0; j<numd2; j++) {
			computed.push_back(hamming_dist(c1, server_des.at(j), 256));
		}
	}

	fstream computed2file("computed.txt", fstream::out);
	computed2file << "[" << numd1 << "]" << endl;
	computed2file << "[" << numd2 << "]" << endl;
	for (int i = 0; i < numd1*numd2; i++) {
		computed2file << computed.at(i);
	}
	computed2file.close();



	/*	// Read ciphertexts from file
	fstream ciphertextFile("ciphertext.txt", fstream::in);	
	Ctxt ctxt1(publicKey);
    	Ctxt ctxt2(publicKey);

    	ciphertextFile >> ctxt1;
	ciphertextFile >> ctxt2;

    	Ctxt hammingenc = hamming_dist(ctxt1, ctxt2, nslots*8);

    	// Output ciphertext to file
	std::fstream resultFile("result.txt", fstream::out|fstream::trunc);
	assert(resultFile.is_open());
	resultFile << hammingenc;

    	//ciphertexts written to file
    	std::cout << "Ciphertexts written to file!" << std::endl;
*/
}
