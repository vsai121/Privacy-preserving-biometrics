#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>

void add_bit(LweSample* sum, LweSample* carry, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk) {
    bootsXOR(sum, a, b, bk);
    bootsAND(carry, a, b, bk);
}

// this function increments a multi-bit word by inc_value (either 0 or 1)
void increment(LweSample* result, const LweSample* a, const LweSample* inc_value, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* carry = new_gate_bootstrapping_ciphertext(bk->params);
    
    bootsCOPY(carry,inc_value,bk);

    for (int i=0; i<nb_bits; i++) {
        add_bit(&result[i], carry, &a[i], carry, bk);
    }

    delete_gate_bootstrapping_ciphertext(carry);    
}

// elementary full comparator gate that is used to compare the i-th bit:
//   input: ai and bi the i-th bit of a and b
//          lsb_carry: the result of the comparison on the lowest bits
//   algo: if (a==b) return const_zero else return const_one

void compare_bit(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* const_zero, const LweSample* const_one, LweSample* tmp, const TFheGateBootstrappingCloudKeySet* bk) {
    bootsXNOR(tmp, a, b, bk);
    bootsMUX(result, tmp, const_zero, const_one, bk);
}

void hammingdistance(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* inc_value = new_gate_bootstrapping_ciphertext(bk->params);
    LweSample* const_arr = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    LweSample* tmp = new_gate_bootstrapping_ciphertext(bk->params);

    bootsCONSTANT(&const_arr[0], 0, bk);
    bootsCONSTANT(&const_arr[1], 1, bk);

    for(int i = 0; i<nb_bits;i++) {
        bootsCONSTANT(&result[i], 0, bk);
    }

    for(int i = 0;i<nb_bits;i++) {
        compare_bit(inc_value, &a[i], &b[i], &const_arr[0], &const_arr[1], tmp, bk);
        increment(result,result,inc_value,nb_bits,bk);
    }

    delete_gate_bootstrapping_ciphertext_array(2, const_arr);
    delete_gate_bootstrapping_ciphertext(inc_value);
    delete_gate_bootstrapping_ciphertext(tmp);
}

int main() {
    
    //reads the cloud key from file
    FILE* cloud_key = fopen("cloud.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = bk->params;

    //read the 2x16 ciphertexts
    LweSample* ciphertext1 = new_gate_bootstrapping_ciphertext_array(16, params);
    LweSample* ciphertext2 = new_gate_bootstrapping_ciphertext_array(16, params);

    //reads the 2x16 ciphertexts from the cloud file
    FILE* cloud_data = fopen("cloud.data","rb");
    for (int i=0; i<16; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i], params);
    for (int i=0; i<16; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i], params);
    fclose(cloud_data);

    //do some operations on the ciphertexts: here, we will compute the
    //minimum of the two
    LweSample* result = new_gate_bootstrapping_ciphertext_array(16, params);
    
    hammingdistance(result,ciphertext1,ciphertext2,16,bk);
    //export the 32 ciphertexts to a file (for the cloud)
    FILE* answer_data = fopen("answer.data","wb");
    for (int i=0; i<16; i++) export_gate_bootstrapping_ciphertext_toFile(answer_data, &result[i], params);
    fclose(answer_data);

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(16, result);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertext2);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertext1);
    delete_gate_bootstrapping_cloud_keyset(bk);

}
