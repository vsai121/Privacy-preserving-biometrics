#include <helib/FHE.h>

int hamming(long a, long b) {
	int count = 0;
	for(int i = 0; i < 32; i++){
		count += (((a>>i)&1)^((b>>i)&1));
	}
	return count;
}

int main(int argc, char **argv)
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
	
	std::cout << "Initializing context... " << std::flush;
	FHEcontext context(m, p, r); 	                        // Initialize context
	buildModChain(context, L, c);                           // Modify the context, adding primes to the modulus chain
	std::cout << "OK!" << std::endl;

	std::cout << "Creating polynomial... " << std::flush;
	NTL::ZZX G =  context.alMod.getFactorsOverZZ()[0];                // Creates the polynomial used to encrypt the data
	std::cout << "OK!" << std::endl;

	std::cout << "Generating keys... " << std::flush;
	FHESecKey secretKey(context);                           // Construct a secret key structure
	const FHEPubKey& publicKey = secretKey;                 // An "upcast": FHESecKey is a subclass of FHEPubKey
	secretKey.GenSecKey(w);                                 // Actually generate a secret key with Hamming weight w
	std::cout << "OK!" << std::endl;
	/*** END INITIALIZATION ***/
	
	//Ctxt ctx1(publicKey);                // Initialize the first ciphertext (ctx1) using publicKey
	//Ctxt ctx2(publicKey);                // Initialize the first ciphertext (ctx2) using publicKey

    //long num1 = 0b11111111;
    //long num2 = 0b00000000;
	int numbits = 32;
	long num1 = NTL::RandomBits_long(numbits);
    long num2 = NTL::RandomBits_long(numbits);
    //long C = 0b11111111;
	long C = 0;
	for(int i = 0; i < numbits;i++)
		C += 1 << i;
    NTL::ZZX n1, n2, con;
    n1.SetLength(numbits);
    n2.SetLength(numbits);
    con.SetLength(numbits);
    for(int i = 0; i < numbits; i++) {
        n1[i] = (num1>>i)&1;
        n2[i] = ((num2>>(numbits-i-1))&1);
        con[i] = ((C>>i)&1);
    }

    Ctxt enc1(publicKey);
    Ctxt enc2(publicKey);
    publicKey.Encrypt(enc1, n1);
    publicKey.Encrypt(enc2, n2);

	Ctxt enc11 = enc1;
	Ctxt enc22 = enc2;
	enc11.multByConstant(con);
	enc22.multByConstant(con);
	Ctxt hammingenc = enc1;
	hammingenc *= enc2;
	hammingenc.multByConstant(NTL::ZZX(2));
	hammingenc -= enc11;
	hammingenc -= enc22;
	hammingenc.multByConstant(NTL::ZZX(-1));

	NTL::ZZX result;
	secretKey.Decrypt(result, hammingenc);
	
	std::cout << result[numbits-1] << std::endl;
	std::cout << hamming(num1,num2) << std::endl;

	
	return 0;
}
