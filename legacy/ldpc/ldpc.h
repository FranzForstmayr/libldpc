#pragma once

#include <iostream>
#include <cinttypes>
#include <vector>
#include <memory>
#include <cmath>
#include <cstring>
#include <chrono>
#include <fstream>
#include <random>
#include <omp.h>

namespace ldpc
{
using bits_t = std::uint8_t;
using labels_t = std::uint16_t;
using symbols_t = std::uint16_t;

using vec_bits_t = std::vector<bits_t>;
using vec_labels_t = std::vector<labels_t>;
using vec_symbols_t = std::vector<symbols_t>;
using vec_size_t = std::vector<std::size_t>;
using vec_double_t = std::vector<double>;

using mat_bits_t = std::vector<std::vector<bits_t>>;
using mat_size_t = std::vector<std::vector<std::size_t>>;
using mat_double_t = std::vector<std::vector<double>>;

/**
 * @brief LDPC code class
 * 
 */
class ldpc_code
{
public:
    ldpc_code(const char *pFileName);
    void print();

    //getter functions
    std::size_t nc() const { return mN; };
    std::size_t kc() const { return mK; };
    std::size_t mc() const { return mM; };
    std::size_t nnz() const { return mNNZ; };
    const mat_size_t &cn() const { return mCN; };
    const mat_size_t &vn() const { return mVN; };
    const vec_size_t &r() const { return mR; };
    const vec_size_t &c() const { return mC; };
    std::size_t nct() const { return mNCT; };
    std::size_t kct() const { return mKCT; };
    std::size_t mct() const { return mMCT; };
    const vec_size_t &puncture() const { return mPuncture; };
    const vec_size_t &shorten() const { return mShorten; };
    std::size_t max_dc() const { return mMaxDC; };

private:
    std::size_t mN;
    std::size_t mK;
    std::size_t mM;
    std::size_t mNNZ;
    mat_size_t mCN;       /* denotes the check neighbors, i.e. connected VN, for each check node as index in c/r; dimensions cn[mc][cw[i]] */
    mat_size_t mVN;       /* denotes the var neighbors, i.e., connected CN, for each variable node as index in c/r; dimensions vn[nc][vw[i]] */
    vec_size_t mR;        /* non zero row indices; length nnz */
    vec_size_t mC;        /* non zero check indices; length nnz */
    vec_size_t mPuncture; /* array pf punctured bit indices */
    vec_size_t mShorten;  /* array of shortened bit indices */
    std::size_t mNCT;     /* number of transmitted code bits */
    std::size_t mKCT;     /* number of transmitted information bits */
    std::size_t mMCT;     /* number of transmitted parity check bits */
    std::size_t mMaxDC;
};

class ldpc_sim;

/**
 * @brief LDPC Decoder class
 * 
 */
class ldpc_decoder
{
public:
    ldpc_decoder(ldpc_code *pCode, ldpc_sim *pSim, std::int16_t pI, bool pEarlyTerm);

    void calc_llrs(const vec_double_t &y, double sigma2);

    std::int16_t decode();
    bool is_codeword_legacy();

    //getter functions
    std::size_t max_iter() const { return mMaxIter; }
    bool early_termination() const { return mEarlyTerm; }

    ldpc_code *ldpc() const { return mLdpcCode; }
    const vec_double_t &lv2c() const { return mLv2c; }
    const vec_double_t &lc2v() const { return mLc2v; }
    const vec_double_t &llr_in() const { return mLLRIn; }
    const vec_double_t &llr_out() const { return mLLROut; }

    const vec_bits_t &syndrome() const { return mSynd; }
    const vec_bits_t &estm_cw() const { return mCO; }

private:
    ldpc_code *mLdpcCode;
    ldpc_sim *mSim;

    vec_double_t mLv2c;
    vec_double_t mLc2v;
    vec_double_t mExMsgCN;

    vec_double_t mLLRIn;
    vec_double_t mLLROut;

    vec_bits_t mSynd;
    vec_bits_t mCO;

    std::int16_t mMaxIter;
    bool mEarlyTerm;
};

void dec2bin(std::size_t val, uint8_t m);
int sign(double a);

} // namespace ldpc