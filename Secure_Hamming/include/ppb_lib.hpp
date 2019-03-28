#ifndef PPB_LIB_HPP
#define PPB_LIB_HPP

Ctxt hamming_dist(Ctxt , Ctxt , int);
FHEcontext gen_key_to_file(long ,long, long, long, long, std::string , std::string );
FHESecKey read_sec_key_from_file(std::string );
FHEPubKey read_pub_key_from_file(std::string );
FHEcontext read_context_from_file(std::string );

#endif