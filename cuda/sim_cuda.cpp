#include "ldpcsim.h"

using namespace ldpc;
using namespace std;


// /usr/local/cuda-9.2/bin/nvcc -x cu -std=c++11 sim_cuda.cpp ldpcsim.cpp ldpc/ldpc.cpp ldpc/decoder.cpp device/cudamgd.cpp device/kernel.cpp -o sim_cuda -arch sm_35 -rdc=true -O3 -w
int main()
{
    //set up code class on unified memory
    ldpc_code* code_dev = new ldpc_code("../src/code/test_code/code_rand_proto_3x6_400_4.txt", "../src/code/test_code/layer_rand_proto_3x6_400_4.txt", true);
/*
    //set up simulation
    ldpc_sim sim(code_dev, "../src/sim.txt", "../src/code/test_code/mapping_rand_proto_3x6_400_4.txt");
    sim.print();
    sim.start();
*/

    //set up decoder on unified memory
    ldpc_decoder* dec_dev = new ldpc_decoder(code_dev, 50, false);

    for (size_t i = 0; i < code_dev->nc(); ++i) {
        dec_dev->mLLRIn[i] = ldpc_sim::randn();
    }

    dec_dev->decode_layered();
    TIME_PROF("GPU Layered", dec_dev->decode_layered(), "ms");
    TIME_PROF("CPU Layered", dec_dev->decode_layered_legacy(), "ms");
    TIME_PROF("CPU Legacy", dec_dev->decode_legacy(), "ms");

    //destroy decoder
    delete dec_dev;

    //destroy code
    delete code_dev;

    return 0;
}


//tmpl fcts need definition in each file?
template<typename T> void ldpc::printVector(T *x, const size_t &l)
{
    cout << "[";
    for (size_t i = 0; i < l-1; ++i)
        cout << x[i] << " ";
    cout << x[l-1] << "]";
}
