#include "ldpc/ldpc.cuh"
#include "simulation.cuh"

using namespace ldpc;
using namespace std;


//nvcc -std=c++11 sim_cuda.cu simulation.cu ldpc/ldpc.cu ldpc/decoder.cu -o sim_cuda -arch sm_35 -rdc=true -O3
int main()
{
	Ldpc_Code_cl* code_managed;
	cudaMallocManaged(&code_managed, sizeof(Ldpc_Code_cl));
	*code_managed = Ldpc_Code_cl();
	code_managed->setup_code_managed("../src/code/test_code/code10K.txt", "../src/code/test_code/layer10K.txt");

	Sim_AWGN_cl sim = Sim_AWGN_cl(code_managed, "../src/sim.txt", "../src/code/test_code/map10K.txt");

	Ldpc_Decoder_cl** dec_ptr;
	cudaMallocManaged(&dec_ptr, sizeof(Ldpc_Decoder_cl*));

	double *llrin, *llrout;
	cudaMallocManaged(&llrin, code_managed->nc()*sizeof(double));
	cudaMallocManaged(&llrout, code_managed->nc()*sizeof(double));
	for (size_t i=0; i<code_managed->nc(); ++i)
	{
		llrin[i] = Sim_AWGN_cl::randn();
		llrout[i] = 0.0;
	}


	cudakernel::setup_decoder<<<1, 1>>>(code_managed, dec_ptr);


	TIME_PROF("GPU", sim.decode_lyr(dec_ptr, llrin, llrout, 50, false), "ms");

	Ldpc_Decoder_cl dec = Ldpc_Decoder_cl(code_managed);
	TIME_PROF("CPU", dec.decode_layered_legacy(llrin, llrout, 50, false), "ms");

	cudakernel::destroy_decoder<<<1, 1>>>(dec_ptr);

	cudaFree(llrin);
	cudaFree(llrout);
	code_managed->destroy_ldpc_code_managed();
	cudaFree(code_managed);
	cudaFree(dec_ptr);

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

/*
struct timespec tstart={0,0}, tend={0,0};
clock_gettime(CLOCK_MONOTONIC, &tstart);

clock_gettime(CLOCK_MONOTONIC, &tend);
printf("Time GPU: %.5f ms\n", (((double)tend.tv_nsec) - ((double)tstart.tv_nsec))*1e-6);
*/
