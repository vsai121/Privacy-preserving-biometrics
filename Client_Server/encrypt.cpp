#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include <NTL/ZZX.h>
#include <iostream>

using namespace std;

//script to read key from file then encrypt

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
		
	uint nslots = 32;
	cout << "Number of slots is " << nslots << endl;

	std::fstream messageFile("message.txt", fstream::in);
	assert(messageFile.is_open());

	std::vector<long int> plaintext1(nslots, 0);    
    std::vector<long int> plaintext2(nslots, 0);    

    //vector <NTL::ZZX> plaintext1;
    //plaintext1.resize(nslots);
    seekPastChar(messageFile, '[');

	for (uint count = 0; count < nslots; count++) {
		int v;
        messageFile >> v;
        plaintext1[count] = v;
	}
 
    seekPastChar(messageFile, ']');

    seekPastChar(messageFile, '[');

	for (uint count =0; count < nslots; count++) {
		int v;
        messageFile >> v;
        plaintext2[count] = v;
	}

	messageFile.close();
	
	std::cout << "Message 1 to be encrypted: " << std::endl << plaintext1 << std::endl;
    std::cout << "Message 2 to be encrypted: " << std::endl << plaintext2 << std::endl;

    //do packing
    NTL::ZZX n1, n2;

    n1.SetLength(nslots);
    n2.SetLength(nslots);

    for(int i = 0; i < nslots; i++){
        n1[i] = plaintext1[i];
        n2[i] = plaintext2[nslots-i-1];
    }

    std::cout << "Packed messages are: " << std::endl << n1 << std::endl << n2 << std::endl;
	
    Ctxt enc1(publicKey);
    Ctxt enc2(publicKey);
    publicKey.Encrypt(enc1, n1);
    publicKey.Encrypt(enc2, n2);

	// Output ciphertext to file
	std::fstream ciphertextFile("ciphertext.txt", fstream::out|fstream::trunc);
	assert(ciphertextFile.is_open());
	ciphertextFile << enc1;
    ciphertextFile << enc2;
	ciphertextFile.close();

    //ciphertexts written to file
    std::cout << "Ciphertexts written to file!" << std::endl;


	return 0;
}
