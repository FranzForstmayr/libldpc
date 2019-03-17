#pragma once

#include "ldpc/ldpc.h"


#define MAX_FILENAME_LEN 256
#define MAX_LLR 9999.9
#define MIN_LLR -9999.9

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

struct {
    double *pX;
    double *X;
    double *A;
    uint16_t M;
    uint16_t log2M;
} typedef cstll_t;


class Sim_AWGN_cl
{
public:
    Sim_AWGN_cl(ldpc::Ldpc_Code_cl* code, const char* simFileName, const char* mapFileName);
    ~Sim_AWGN_cl();

    void read_bit_mapping_file(const char* filename);
    void print_sim();
    void destroy_sim();

    void calc_llrs(const double& y, const double& sigma2, double* llrs_out);
    double simulate_awgn(uint64_t* x, double* y, const double& sigma2);
    double randn();

private:
    ldpc::Ldpc_Code_cl* ldpc_code;

    cstll_t* cstll;

    uint64_t n;
    uint16_t M;
    uint16_t bits;
    uint64_t max_frames;
    uint64_t min_fec;
    uint64_t bp_iter;
    double* snrs;
    uint16_t* labels;
    uint16_t* labels_rev;
    uint8_t decoder_terminate_early;
    double SE;
    size_t num_snrs;
    char logfile[MAX_FILENAME_LEN];
    size_t** bit_mapper;
    size_t* bits_pos;
};
