#include <cstring>
#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <fstream>
#include <NTL/ZZX.h>
#include <iostream>

using namespace std;

int hamming(int a, int b) {
        int count = 0;
        for(int i = 0; i < 32; i++){
                count += (((a>>i)&1)^((b>>i)&1));
        }
        return count;
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

	uint numd = 0;		
	uint nslots = 32;
	cout << "Number of slots is " << nslots << endl;

	std::fstream messageFile("feature.txt", fstream::in);
	assert(messageFile.is_open());

    	seekPastChar(messageFile, '[');

	messageFile >> numd;

	seekPastChar(messageFile, ']');

	//read into vectors
	std::vector<std::vector <int>> ptxt(numd, vector<int>(nslots, 0));
	for(int di = 0; di < numd; di++)
	{
		seekPastChar(messageFile, '[');

		for (int count = 0; count < nslots; count++) {
			int v;
			messageFile >> v;
        		ptxt[di][count] = v;
		}
		
    		seekPastChar(messageFile, ']');
	}

	messageFile.close();
	std::cout << ptxt << std::endl;
	 
	//do packing
	int num;
    	NTL::ZZX n[numd];
	
	for(int di = 0; di < numd; di++)
	{
		n[di].SetLength(nslots*8);
    		for(int i = 0; i < nslots; i++) {
				num = ptxt[di][i];
				for(int j = 0; j < 8; j++) {
					n[di][8*(i+1)-j-1] = (num>>j)&1;
				}
    		}
		std::cout << n[di] << std::endl;
	}
	//packed ntl vectors are stored in n, now output to file
	std::fstream ciphertextFile("ciphertext.txt", fstream::out|fstream::trunc);
	assert(ciphertextFile.is_open());
	ciphertextFile << "[" << numd << "]" << endl;
	
	NTL::ZZX con;
	con.SetLength(nslots*8);
	for(int i=0;i<nslots*8;i++)
		con[i] = 1;
	
	for(int di = 0; di < numd; di++)
	{
		Ctxt ct(publicKey);
		publicKey.Encrypt(ct, n[di]);
		Ctxt AC = ct;
		AC.multByConstant(con);
		ciphertextFile << ct;
		ciphertextFile << AC;
	}
    		
	// Output ciphertext to file
	ciphertextFile.close();
    	//ciphertexts written to file
    	std::cout << "Ciphertexts written to file!" << std::endl;
	return 0;
}
