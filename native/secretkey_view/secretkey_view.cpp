#include "secretkey_view.h"
#define DEBUG
#undef DEBUG

int main(int argc, char* args[]){
    EncryptionParameters parms(scheme_type::bfv);
    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(1 << 7);
    SEALContext context(parms);

#ifdef DEBUG
    KeyGenerator keygen(context);               //generate a new secret key
    SecretKey isk = keygen.inverse_secret_key();//compute the multiplicative inverse of secret key
    auto &coeff_modulus = parms.coeff_modulus();
    size_t coeff_size = coeff_modulus.size();

    vector<uint64_t> modulus_RNS;
    mpz_t modulus;
    vector<MPZWrapper> isk_coeffs;

    get_mpz_modulus(coeff_modulus, modulus_RNS, modulus);
    get_mpz_coeffs(poly_modulus_degree, isk.data().data(), modulus_RNS, isk_coeffs);

    double max_ratio = 0;
    double current_ratio = 0;
    for(auto i: isk_coeffs){
        double current_ratio = abs(0.5 - mpz_div_as_double(i.get_value(), modulus));
        max_ratio = max(max_ratio, current_ratio);
    }
    cout<<max_ratio<<endl;
    mpz_clear(modulus);
#else
    for(int i = 0; i<10; i++){
        viewer(context, parms);
    }
#endif    
    return 0;
}