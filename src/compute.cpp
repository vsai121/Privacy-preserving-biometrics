#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include <NTL/ZZX.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <sys/time.h>

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

Ctxt hamming_dist_opt(Ctxt enc1, Ctxt enc2, Ctxt enc11, Ctxt enc22, int numbits) {
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
	std::cout << "Read cipher 1" << std::endl;
	fstream ctext1file("ciphertext.txt", fstream::in);
	int numd1;
	seekPastChar(ctext1file, '[');
	ctext1file >> numd1;
	seekPastChar(ctext1file, ']');

	std::unique_ptr<Ctxt> cdesc[numd1];
	std::unique_ptr<Ctxt> precdesc[numd1];
	for(int i = 0; i < numd1; i++) {
		cdesc[i].reset(new Ctxt(publicKey));
		precdesc[i].reset(new Ctxt(publicKey));
		ctext1file >> *cdesc[i];
		ctext1file >> *precdesc[i];
	}
	ctext1file.close();

	std::cout << "Read cipher 1" << std::endl;
	fstream ctext2file("ciphertext2.txt", fstream::in);
	int numd2;
	seekPastChar(ctext2file, '[');
	ctext2file >> numd2;
	seekPastChar(ctext2file, ']');
	
	std::unique_ptr<Ctxt> sdesc[numd2];
	std::unique_ptr<Ctxt> presdesc[numd2];
	for(int i = 0; i < numd2; i++) {
		sdesc[i].reset(new Ctxt(publicKey));
		presdesc[i].reset(new Ctxt(publicKey));
		ctext2file >> *sdesc[i];
		ctext2file >> *presdesc[i];
	}
	ctext2file.close();

	std::unique_ptr<Ctxt> hvec[numd1][numd2];
	fstream resfp("computed.txt", fstream::out);
	resfp << "[ " << numd1 << " ]" << endl;
	resfp << "[ " << numd2 << " ]" << endl;
	std::cout << "Started computing" << std::endl;

	struct timeval start, end;
	gettimeofday(&start, NULL);
	for(int i = 0; i < numd1; i++) 
	{
		#pragma omp parallel for shared(cdesc, sdesc, precdesc, presdesc, hvec, publicKey, i)
		for(int j = 0; j < numd2; j++)
		{
			hvec[i][j].reset(new Ctxt(publicKey));
			*hvec[i][j] = hamming_dist_opt(*cdesc[i],*sdesc[j],*precdesc[i],*presdesc[j],nslots*8);
		}
	}
	gettimeofday(&end, NULL);
	double hamm_time = (end.tv_sec  - start.tv_sec) * 1000L + 
         (end.tv_usec - start.tv_usec) / 1000L;
	std::cout << "Total hamming distance calculation time is " << hamm_time << std::endl;
	std::cout << "Finished computing" << std::endl;
	std::cout << "Started writing" << std::endl;
	for(int i = 0; i < numd1; i++)
	{
		for(int j =0; j < numd2; j++)
		{
			resfp << *hvec[i][j];
		}
	}
	std::cout << "Finished writing" << std::endl;

	resfp.close();
}
