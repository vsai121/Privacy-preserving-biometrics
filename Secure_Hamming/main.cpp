#include <helib/FHE.h>
#include "ppb_lib.hpp"

int main()
{
    	/*** BEGIN INITIALIZATION ***/
	long m = 0;                   // Specific modulus
	long p = 1021;                // Plaintext base [default=2], should be a prime number
	long r = 1;                   // Lifting [default=1]
	long L = 16;                  // Number of levels in the modulus chain [default=heuristic]
	long c = 3;                   // Number of columns in key-switching matrix [default=2]
	long w = 64;                  // Hamming weight of secret key
	long d = 0;                   // Degree of the field extension [default=1]
	long k = 128;                 // Security parameter [default=80] 
    long s = 0;                   // Minimum number of slots [default=0]

	std::cout << "Finding m... " << std::flush;
	m = FindM(k, L, c, p, d, s, 0);                            // Find a value for m given the specified values
	std::cout << "m = " << m << std::endl;
	

    FHEcontext test = gen_key_to_file(m,p,r,L,c, "pubkey.txt", "seckey.txt");

    //assert(test == pk);
    FHEcontext tmpcontext = read_context_from_file("pubkey.txt");
    assert(test == tmpcontext);
    std::cout << "TRUE" << std::endl;
    FHEPubKey pk = read_pub_key_from_file("pubkey.txt");
    //FHESecKey sk = read_sec_key_from_file("seckey.txt");

    Ctxt enc1(pk);
    Ctxt enc2(pk);

    int numbits = 32;
    long num1 = NTL::RandomBits_long(numbits);
    long num2 = NTL::RandomBits_long(numbits);

    std::cout << "Numbers are " << num1 << " and " << num2 << std::endl;
    NTL::ZZX n1, n2;
    n1.SetLength(numbits);
    n2.SetLength(numbits);
    for(int i = 0; i < numbits; i++){
        n1[i] = (num1>>i)&1;
        n2[i] = ((num2>>(numbits-i-1))&1);
    }
    std::cout << "Numbers packed" << std::endl;

    pk.Encrypt(enc1, n1);
    pk.Encrypt(enc2, n2);

    return 0;
}