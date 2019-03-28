#include <fstream>
#include <helib/FHE.h>
#include "ppb_lib.hpp"

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

FHEcontext gen_key_to_file(long m, long p, long r, long L, long c, std::string pubkey_file, std::string privkey_file) {
	FHEcontext context(m, p, r); 	                        // Initialize context
	buildModChain(context, L, c);                           // Modify the context, adding primes to the modulus chain

	NTL::ZZX G =  context.alMod.getFactorsOverZZ()[0];                // Creates the polynomial used to encrypt the data

    std::fstream sec_key_file(privkey_file, std::fstream::out|std::fstream::trunc);
    std::fstream pub_key_file(pubkey_file, std::fstream::out|std::fstream::trunc);
    writeContextBase(sec_key_file, context);
    writeContextBase(pub_key_file, context);
    sec_key_file << context << std::endl;
    pub_key_file << context << std::endl;

    FHESecKey secretKey(context);
    pub_key_file << secretKey << std::endl;
    //const FHEPubKey& publicKey = secretKey;
    secretKey.GenSecKey(64);
    //addSome1DMatrices(secretKey);
    
    sec_key_file << secretKey << std::endl;
    sec_key_file.close();
    pub_key_file.close();
    return context;
}

FHESecKey read_sec_key_from_file(std::string skfname) {
    std::fstream skfptr(skfname, std::fstream::in);
    unsigned long m, p, r;
    std::vector<long> gens, ords;
    readContextBase(skfptr, m, p, r, gens, ords);
    FHEcontext context(m, p, r);

    skfptr >> context;
    FHESecKey secret_key(context);
    skfptr >> secret_key;
    skfptr.close();
    return secret_key;
}

FHEPubKey read_pub_key_from_file(std::string pkfname) {
    std::fstream pkfptr(pkfname, std::fstream::in);
    unsigned long m, p, r;
    std::vector<long> gens, ords;
    readContextBase(pkfptr, m, p, r, gens, ords);
    FHEcontext context(m, p, r, gens, ords);
    pkfptr >> context;
    FHESecKey secret_key(context);
    const FHEPubKey& public_key = secret_key;
    pkfptr >> secret_key;
    pkfptr.close();
    return public_key;
}

FHEcontext read_context_from_file(std::string fname) {
    std::fstream fptr(fname, std::fstream::in);
    unsigned long m, p, r;
    std::vector<long> gens, ords;
    readContextBase(fptr, m, p, r, gens, ords);
    FHEcontext context(m, p, r, gens, ords);
    fptr >> context;
    return context;
}