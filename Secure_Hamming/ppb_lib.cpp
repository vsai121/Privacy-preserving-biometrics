#include "FHE.h"

Ctxt hamming_dist(Ctxt inp1, Ctxt inp2) {
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

void gen_key_to_file(FHEcontext context, string pubkey_file, string privkey_file) {
    fstream sec_key_file(privkey_file, fstream::out|fstream::trunc);
    fstream pub_key_file(pubkey_file, fstream::out|fstream::trunc);
    writeContextBase(sec_key_file, context);
    writeContextBase(pub_key_file, context);
    sec_key_file << context << std::endl;
    pub_key_file << context << std::endl;
    FHESecKey secretKey(context);
    const FHEPubKey& publickey = secretKey;
    secretKey.GenSecKey(64);
    addSome1DMatrices(secretKey);
    sec_key_file << secretKey << std::endl;
    pub_key_file << publicKey << std::endl;
    sec_key_file.close();
    pub_key_file.close();
}

FHESecKey read_sec_key_from_file(string seckey_file) {
    fstream sec_key_file(seckey_file, fstream::in);
    unsigned long m, p, r;
    vector<long> gens, ords;
    sec_key_file >> context;
    FHESecKey secret_key(context);
    sec_key_file >> secret_key;
    sec_key_file.close();
    return secret_key;
}

FHEPubKey read_pub_key_from_file(string pubkey_file) {
    fstream pub_key_file(pubkey_file, fstream::in);
    unsigned long m, p, r;
    vector<long> gens, ords;
    readContextBase(pub_key_file, m, p, r, gens, ords);
    FHEContext context(m, p, r, gens, ords);
    pub_key_file >> context;
    FHEPubKey public_key(context);
    pub_key_file >> public_key;
    pub_key_file.close();
    return public_key;
}