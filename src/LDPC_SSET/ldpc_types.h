#pragma once

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

// structure for messages
/*
struct
{
    size_t size;
    uint64_t data[];
} typedef ex_msg_t;
*/

struct {
    uint64_t nc; /* code length */
    uint64_t kc;
    uint64_t mc; /* # of parity checks */
    uint64_t nct;
    uint64_t kct;
    uint64_t mct;
    uint64_t nnz; /* # of non-zero entries in H, i.e., # of edges in the Tanner graph */
    uint64_t girth;
    size_t* puncture;
    size_t num_puncture;
    size_t num_puncture_sys;
    size_t num_puncture_par;
    size_t* shorten;
    size_t num_shorten;
    uint64_t max_dc;
    uint8_t** genmat;
    size_t M;
    size_t N;
    size_t L;
    size_t mu;
    size_t window;
    size_t* cw; /* denotes the check weight of each check node, i.e., # of connected VN; dimensions cw[mc] */
    size_t* vw; /* denotes the variable weight, i.e., # of connected CN; dimensions vw[nc] */
    size_t** cn; /* denotes the check neighbors, i.e. connected VN, for each check node as index in c/r; dimensions cn[mc][cw[i]] */
    size_t** vn; /* denotes the var neighbors, i.e., connected CN, for each variable node as index in c/r; dimensions vn[nc][vw[i]] */
    size_t* r; /* non zero row indices; length nnz */
    size_t* c; /* non zero column indices; length nnz */
    // trapping sets;
    size_t st_max_size;
    size_t* stw;
    char** st;
    //size_t* dw;
    //size_t** ds;
} typedef ldpc_code_t;

struct {
    uint64_t n;
    uint16_t M;
    uint16_t bits;
    uint64_t max_frames;
    uint64_t min_fec;
    uint64_t bp_iter;
    double* snrs;
    uint16_t* labels;
    uint16_t* labels_rev;
    double SE;
    size_t num_snrs;
    char logfile[256];
    size_t** bit_mapper;
    size_t* bits_pos;
} typedef ldpc_sim_t;
