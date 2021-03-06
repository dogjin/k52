#include <k52/dsp/wavelet.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

namespace k52
{
namespace dsp
{

Wavelet::Wavelet()
    : count_(256)
    , max_lenght_(1024)
    , min_lenght_(10)
{
}

Wavelet::shared_ptr Wavelet::BuildWavelet(IWavelet::shared_ptr wavelet_function, int min_lenght,
                                          int max_lenght, int count, ScaleType type)
{
    Wavelet::shared_ptr wavelet(new Wavelet);

    if (wavelet)
    {
        wavelet->base_function(wavelet_function);
        wavelet->count(count);
        wavelet->max_lenght(max_lenght);
        wavelet->min_lenght(min_lenght);
        wavelet->createWavelet(type);
    }

    return wavelet;
}


void Wavelet::createWavelet(ScaleType type)
{
    if(!base_function_)
    {
        std::cerr << "The base function has been never set for the wavelet\n";
        throw "The base function has been never set for the wavelet\n";
    }

    // prepare real part vector
    real_part_.clear();
    real_part_.resize(count());
    for(int i = 0; i < count(); ++i)
    {
        real_part_[i].resize(max_lenght());
    }

    // prepare imaginary part vector
    imaj_part_.clear();
    imaj_part_.resize(count());
    for(int i = 0; i < count(); ++i)
    {
        imaj_part_[i].resize(max_lenght());
    }

    // prepare Scale
    createScale(type);

    for (int i = 0; i < count(); ++i)
    {
        for (int j = 0; j < max_lenght(); ++j)
        {
            double x = (j - (max_lenght() / 2.0)) * scale_[i];
            x = (x * 4) / (max_lenght() / 2);

            std::complex<double> base_function_value = base_function_->GetValue(x);
            real_part_[i][j] = base_function_value.real();
            imaj_part_[i][j] = base_function_value.imag();
        }
    }
}

void Wavelet::createScale(ScaleType type)
{
    scale_.resize(count());
    switch (type)
    {
        case Log :
        {
            double lg = log(static_cast<double>(max_lenght()) / min_lenght());
            for (int i = 0; i < count(); ++i)
            {
                scale_.at(i) = std::exp(i / (double)count() * lg);
            }
        } break;

        case Linear :
        {
            double k = max_lenght() / min_lenght();
            for (int i = 1; i <= count(); ++i)
            {
                scale_.at(i-1) = i / (double)count() * k;
            }
        } break;
        default:
        {
            throw "Should never be reached\n";
        }
    }
}

} // namespace dsp
} // namespace k52
