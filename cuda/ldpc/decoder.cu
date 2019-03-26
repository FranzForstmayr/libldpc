#include "ldpc.cuh"
#include <exception>

using namespace ldpc;
using namespace std;


__host__ __device__ Ldpc_Decoder_cl::Ldpc_Decoder_cl() {}
Ldpc_Decoder_cl::Ldpc_Decoder_cl(Ldpc_Code_cl* code) { setup_decoder(code); }
__host__ __device__ Ldpc_Decoder_cl::~Ldpc_Decoder_cl()
{
	if (init)
		destroy_dec();
}

void Ldpc_Decoder_cl::setup_decoder(Ldpc_Code_cl* code)
{
	init = true;
	ldpc_code = code;

	l_c2v = nullptr;
	l_v2c = nullptr;
	f = nullptr;
	b = nullptr;
	lsum = nullptr;

	c_out = nullptr;
	synd = nullptr;

	#ifdef QC_LYR_DEC
	const uint64_t num_layers = ldpc_code->nl();
	#else
	const uint64_t num_layers = 1;
	#endif

	try
	{
		//num layers times num nnz
		l_c2v = new double[num_layers * ldpc_code->nnz()]();
		l_v2c = new double[num_layers * ldpc_code->nnz()]();
		f = new double[num_layers * ldpc_code->max_dc()]();
		b = new double[num_layers * ldpc_code->max_dc()]();

		lsum = new double[ldpc_code->nnz()]();

		c_out = new bits_t[ldpc_code->nc()]();
		synd = new bits_t[ldpc_code->nc()]();
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		destroy_dec();
		exit(EXIT_FAILURE);
	}
}

__device__ void Ldpc_Decoder_cl::setup_decoder_device(Ldpc_Code_cl* code)
{
	init = true;
	ldpc_code = code;

	l_c2v = nullptr;
	l_v2c = nullptr;
	f = nullptr;
	b = nullptr;
	lsum = nullptr;

	c_out = nullptr;
	synd = nullptr;

	const uint64_t num_layers = ldpc_code->nl();

	//num layers times num nnz
	l_c2v = new double[num_layers * ldpc_code->nnz()]();
	l_v2c = new double[num_layers * ldpc_code->nnz()]();
	f = new double[num_layers * ldpc_code->max_dc()]();
	b = new double[num_layers * ldpc_code->max_dc()]();

	lsum = new double[ldpc_code->nnz()]();

	c_out = new bits_t[ldpc_code->nc()]();
	synd = new bits_t[ldpc_code->nc()]();
}

__host__ __device__ void Ldpc_Decoder_cl::destroy_dec()
{
    if (l_c2v != nullptr)
        delete[] l_c2v;
    if (l_v2c != nullptr)
        delete[] l_v2c;
    if (f != nullptr)
        delete[] f;
    if (b != nullptr)
        delete[] b;
    if (lsum != nullptr)
        delete[] lsum;
    if (c_out != nullptr)
        delete[] c_out;
    if (synd != nullptr)
        delete[] synd;
}


__host__ __device__ bool Ldpc_Decoder_cl::is_codeword()
{
    bool is_codeword = true;

    //calc syndrome
    bits_t s;
    for (size_t i = 0; i < ldpc_code->mc(); i++)
    {
        s = 0;
        for (size_t j = 0; j < ldpc_code->cw()[i]; j++)
            s ^= c_out[ldpc_code->c()[ldpc_code->cn()[i][j]]];

        if (s)
        {
            return false;
        }
    }

    return is_codeword;
}

uint64_t Ldpc_Decoder_cl::decode_legacy(double* llr_in, double* llr_out, const uint64_t& max_iter, const bool& early_termination)
{
    size_t it;

    size_t* vn;
    size_t* cn;

    size_t vw;
    size_t cw;

    /* initialize with llrs */
    for(size_t i = 0; i < ldpc_code->nnz(); i++) {
        l_v2c[i] = llr_in[ldpc_code->c()[i]];
    }

    it = 0;
    while(it < max_iter) {
        for(size_t i = 0; i < ldpc_code->mc(); i++) {
            cw = ldpc_code->cw()[i];
            cn = ldpc_code->cn()[i];
            f[0] = l_v2c[*cn];
            b[cw-1] = l_v2c[*(cn+cw-1)];
            for(size_t j = 1; j < cw; j++) {
                f[j] = jacobian(f[j-1], l_v2c[*(cn+j)]);
                b[cw-1-j] = jacobian(b[cw-j], l_v2c[*(cn + cw-j-1)]);
            }

            l_c2v[*cn] = b[1];
            l_c2v[*(cn+cw-1)] = f[cw-2];
            for(size_t j = 1; j < cw-1; j++) {
                l_c2v[*(cn+j)] = jacobian(f[j-1], b[j+1]);
            }
        }

        /* VN node processing */
        for(size_t i = 0; i < ldpc_code->nc(); i++) {
            double tmp = llr_in[i];
            vw = ldpc_code->vw()[i];
            vn = ldpc_code->vn()[i];
            while(vw--) {
                tmp += l_c2v[*vn++];
            }
            vn = ldpc_code->vn()[i];
            vw = ldpc_code->vw()[i];
            while(vw--) {
                l_v2c[*vn] = tmp - l_c2v[*vn];
                vn++;
            }
        }

        // app calculation
        for(size_t i = 0; i < ldpc_code->nc(); i++) {
            llr_out[i] = llr_in[i];
            vn = ldpc_code->vn()[i];
            vw = ldpc_code->vw()[i];
            while(vw--) {
                llr_out[i] += l_c2v[*vn++];
            }
            c_out[i] = (llr_out[i] <= 0);
        }

        it++;

        if (early_termination) {
            if (is_codeword()) {
                break;
            }
        }
    }

    return it;
}

__global__ void cudakernel::setup_decoder(Ldpc_Code_cl* code_managed, Ldpc_Decoder_cl** dec_ptr)
{
	*dec_ptr = new Ldpc_Decoder_cl();
	(**dec_ptr).setup_decoder_device(code_managed);
	printf("Cuda Device :: Decoder set up!\n");
}

__global__ void cudakernel::destroy_decoder(Ldpc_Decoder_cl** dec_ptr)
{
	delete *dec_ptr;
	printf("Cuda Device :: Decoder destroyed!\n");
}



//tmpl fcts need definition in each file?
template<typename T> void ldpc::printVector(T *x, const size_t &l)
{
    cout << "[";
    for (size_t i = 0; i < l-1; ++i)
        cout << x[i] << " ";
    cout << x[l-1] << "]";
}
