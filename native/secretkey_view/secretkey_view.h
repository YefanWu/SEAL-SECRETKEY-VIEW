#pragma once

#include "seal/seal.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <gmp.h>
#include <memory>

using namespace std;
using namespace seal;

class MPZWrapper {
public:
    mpz_t value;

    MPZWrapper() {
        mpz_init(value);
    }

    ~MPZWrapper() {
        mpz_clear(value);
    }

    MPZWrapper(const MPZWrapper& other) {
        mpz_init(value);
        mpz_set(value, other.value);
    }

    MPZWrapper& operator=(const MPZWrapper& other) {
        if (this != &other) {
            mpz_set(value, other.value);
        }
        return *this;
    }

    mpz_t& get_value() {
        return value;
    }
};

void get_multiplication_inverse(mpz_t inv, const mpz_t a, const mpz_t m) {
    mpz_t gcd, x, y;
    mpz_inits(gcd, x, y, NULL);
    mpz_gcdext(gcd, x, y, a, m);

    if (mpz_cmp_ui(gcd, 1) != 0) {
        std::cerr << "模逆不存在" << std::endl;
        mpz_set_ui(inv, 0);
    } else {
        // 保证是正数
        mpz_mod(inv, x, m);
    }

    mpz_clears(gcd, x, y, NULL);
}

void chinese_remainder(const std::vector<uint64_t>& mods, const std::vector<uint64_t>& remainders, mpz_t& result) {
    size_t n = mods.size();

    mpz_t M, total, mi, inv, term, ri, mi_mpz, mod_mpz;
    mpz_inits(M, total, mi, inv, term, ri, mi_mpz, mod_mpz, NULL);

    // M = m1 * m2 * ... * mn
    mpz_set_ui(M, 1);
    for (uint64_t mod : mods) {
        mpz_mul_ui(M, M, mod);
    }

    mpz_set_ui(total, 0);

    for (size_t i = 0; i < n; ++i) {
        mpz_set_ui(mod_mpz, mods[i]);
        mpz_set_ui(ri, remainders[i]);

        // mi = M / mods[i]
        mpz_divexact(mi, M, mod_mpz);

        // inv = mi^{-1} mod mods[i]
        get_multiplication_inverse(inv, mi, mod_mpz);

        // term = ri * mi * inv
        mpz_mul(term, ri, mi);
        mpz_mul(term, term, inv);
        mpz_add(total, total, term);
    }

    mpz_mod(result, total, M);  // result = total mod M

    mpz_clears(M, total, mi, inv, term, ri, mi_mpz, mod_mpz, NULL);
}

void get_mpz_modulus(const vector<Modulus>& coeff_modulus, vector<uint64_t>& modulus_RNS, mpz_t& modulus){   
    size_t coeff_size = coeff_modulus.size();    
        
    mpz_init(modulus);    
    mpz_set_ui(modulus, 1);
    for(int i = 0; i < coeff_size - 1; i++){
        modulus_RNS.push_back(coeff_modulus[i].value());        
        mpz_mul_ui(modulus, modulus, coeff_modulus[i].value());
    }
}

void get_mpz_coeffs(size_t poly_modulus_degree, uint64_t* pointer, const vector<uint64_t>& modulus_RNS, vector<MPZWrapper>& isk_coeffs){
    size_t coeff_size = modulus_RNS.size();
    
    mpz_t result;
    mpz_init(result);
    for(int i = 0; i < poly_modulus_degree; i++){
        vector<uint64_t> residue;
        for(int j = 0; j < coeff_size; j++){
            residue.emplace_back(*(pointer + i + j * poly_modulus_degree));
        }
        chinese_remainder(modulus_RNS, residue, result);
        isk_coeffs.emplace_back();
        mpz_set(isk_coeffs[i].value, result);
    }
    mpz_clear(result);
}

double mpz_div_as_double(const mpz_t n, const mpz_t q) {
    mpf_t fn, fq, result;
    double d;

    mpf_init(fn);
    mpf_init(fq);
    mpf_init(result);

    mpf_set_z(fn, n);
    mpf_set_z(fq, q);
    mpf_div(result, fn, fq);
    d = mpf_get_d(result);  // 精度受限于 double 类型

    mpf_clear(fn);
    mpf_clear(fq);
    mpf_clear(result);

    return d;
}

void viewer(SEALContext& context, EncryptionParameters& parms){
    KeyGenerator keygen(context);               //generate a new secret key
    SecretKey isk = keygen.inverse_secret_key();//compute the multiplicative inverse of secret key
    auto &coeff_modulus = parms.coeff_modulus();
    size_t coeff_size = coeff_modulus.size();

    vector<uint64_t> modulus_RNS;
    mpz_t modulus;
    vector<MPZWrapper> isk_coeffs;

    get_mpz_modulus(coeff_modulus, modulus_RNS, modulus);
    get_mpz_coeffs(parms.poly_modulus_degree(), isk.data().data(), modulus_RNS, isk_coeffs);

    double min_ratio = 1;
    double max_ratio = 0;
    double avg_ratio = 0;
    for(auto i: isk_coeffs){
        double current_ratio = abs(0.5 - mpz_div_as_double(i.get_value(), modulus));
        min_ratio = min(current_ratio, min_ratio);
        max_ratio = max(current_ratio, max_ratio);
        avg_ratio+= current_ratio/parms.poly_modulus_degree();
    }
    cout<<max_ratio<<" "<<min_ratio<<" "<<avg_ratio<<endl;
    mpz_clear(modulus);
}