#include "ldpcsim.h"

#include <omp.h>

namespace ldpc
{
//start simulation on cpu
void ldpc_sim::start(bool *stopFlag)
{
    double sigma2;
    u64 frames;
    u64 bec = 0;
    u64 fec = 0;
    u64 iters;
    u64 bec_tmp;

    std::vector<std::string> printResStr(mSnrs.size() + 1, std::string());
    std::ofstream fp;
    char resStr[128];

    #ifndef LIB_SHARED
    #ifdef LOG_FRAME_TIME
    printResStr[0].assign("snr fer ber frames avg_iter frame_time");
    #else
    printResStr[0].assign("snr fer ber frames avg_iter");
    #endif
    #endif

    std::cout << "========================================================================================" << std::endl;
    std::cout << "  FEC   |      FRAME     |   SNR   |    BER     |    FER     | AVGITERS  |  TIME/FRAME   \n";
    std::cout << "========+================+=========+============+============+===========+==============" << std::endl;

    for (u64 i = 0; i < mSnrs.size(); ++i)
    {
        bec = 0;
        fec = 0;
        frames = 0;
        iters = 0;
        sigma2 = pow(10, -mSnrs[i] / 10);

        auto timeStart = std::chrono::high_resolution_clock::now();

        #pragma omp parallel default(none) num_threads(mThreads) private(bec_tmp) firstprivate(sigma2, mLdpcCode, stdout) shared(stopFlag, timeStart, mX, mY, mC, mLdpcDecoder, fec, bec, frames, printResStr, fp, resStr, i, mMinFec, mMaxFrames) reduction(+:iters)
        {
            unsigned tid = omp_get_thread_num();

            do
            {
                simulate_awgn(sigma2, tid);

                mLdpcDecoder[tid].calc_llrs(mY[tid], sigma2);

                //decode
                iters += mLdpcDecoder[tid].decode();

                if (fec < mMinFec)
                {
                    #pragma omp atomic update
                    ++frames;

                    bec_tmp = 0;
                    for (u64 j = 0; j < mLdpcCode->nc(); ++j)
                    {
                        bec_tmp += (mLdpcDecoder[tid].llr_out()[j] <= 0);
                    }

                    if (bec_tmp > 0)
                    {
                        auto timeNow = std::chrono::high_resolution_clock::now();
                        auto timeFrame = timeNow - timeStart; //eliminate const time for printing etc
                        u64 tFrame = static_cast<u64>(std::chrono::duration_cast<std::chrono::microseconds>(timeFrame).count());
                        tFrame = tFrame / frames;
                        #pragma omp critical
                        {
                            bec += bec_tmp;
                            ++fec;

                            #ifndef LIB_SHARED
                            printf("\r %2lu/%2lu  |  %12lu  |  %.3f  |  %.2e  |  %.2e  |  %.1e  |  %.3fms",
                                   fec, mMinFec, frames, mSnrs[i],
                                   static_cast<double>(bec) / (frames * mLdpcCode->nc()), //ber
                                   static_cast<double>(fec) / frames,                     //fer
                                   static_cast<double>(iters) / frames,                   //avg iters
                                   static_cast<double>(tFrame) * 1e-3);                        //frame time tFrame
                            fflush(stdout);

                            #ifdef LOG_FRAME_TIME
                            sprintf(resStr, "%lf %.3e %.3e %lu %.3e %.6f",
                                    mSnrs[i], static_cast<double>(fec) / frames, static_cast<double>(bec) / (frames * mLdpcCode->nc()),
                                    frames, static_cast<double>(iters) / frames, static_cast<double>(tFrame) * 1e-6);
                            #else
                            sprintf(resStr, "%lf %.3e %.3e %lu %.3e",
                                    mSnrs[i], static_cast<double>(fec) / frames, static_cast<double>(bec) / (frames * mLdpcCode->nc()),
                                    frames, static_cast<double>(iters) / frames);
                            #endif
                            printResStr[i + 1].assign(resStr);

                            try
                            {
                                fp.open(mLogfile);
                                for (const auto &x : printResStr)
                                {
                                    fp << x << "\n";
                                }
                                fp.close();
                            }
                            catch (...)
                            {
                                printf("Warning: can not open logfile %s for writing", mLogfile.c_str());
                            }

                            #ifdef LOG_CW
                            log_error(frames, mSnrs[i]);
                            #endif
                            #endif

                            //save to result struct
                            if (mResults != nullptr)
                            {
                                mResults->fer[i] = static_cast<double>(fec) / frames;
                                mResults->ber[i] = static_cast<double>(bec) / (frames * mLdpcCode->nc());
                                mResults->avg_iter[i] = static_cast<double>(iters) / frames;
                                mResults->time[i] = static_cast<double>(tFrame) * 1e-6;
                                mResults->fec[i] = fec;
                                mResults->frames[i] = frames;
                            }

                            timeStart += std::chrono::high_resolution_clock::now() - timeNow; //dont measure time for printing files
                        }
                    }
                }
            } while (fec < mMinFec && frames < mMaxFrames && !*stopFlag); //end while
        }
        #ifndef LIB_SHARED
        printf("\n");
        #endif
    } //end for

    *resStr = 0;
}
} // namespace ldpc
