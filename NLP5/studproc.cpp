#include <fstream>
#include <cassert>
#include "studproc.h"

template class StudProc<double>;
template class StudProc<float>;

template<typename T>
StudProc<T>::StudProc()
{}

template<typename T>
StudProc<T>::~StudProc()
{}

template<typename T>
void StudProc<T>::update(Collector<unsigned int> const& monograms, BigramCollector<unsigned int> const& bigrams)
{
    unsigned int const totalCount = bigrams.totalCount();

    // TODO: провести анализ и брать среднее по значениям monogram.
    /*T const freqMin = 250,
        freqBigramMin = std::sqrt(freqMin);*/

    for(auto iter = bigrams.expose().cbegin(), end = bigrams.expose().cend(); iter != end; ++iter)
    {
        std::size_t const pos = iter->first.find(' ');
        if(pos == std::string::npos)
            continue;

        std::string const first = iter->first.substr(0, pos);
        std::string const second = iter->first.substr(pos + 1, std::string::npos);

        if(first.empty() || second.empty())
            continue;

        T const freqBigram = iter->second,
            freqFirst = monograms[first],
            freqSecond = monograms[second];

        /*if(freqFirst < freqMin && freqSecond < freqMin && freqBigram < freqBigramMin)
            continue;*/

        T const mu = (freqFirst * freqSecond) / (totalCount * totalCount),
            x = freqBigram / totalCount,
            t = (x - mu) / std::sqrt(x / totalCount);

        if(t > 0 && t < 2.576)
            _collection[iter->first] = t;
    }
}

template<typename T>
bool StudProc<T>::dump(char const* const filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::trunc);
    if(!fout)
        return false;
    std::vector<std::pair<std::string, T>> const vect = leastFrequent(100);
    for(std::size_t i = 0; i < vect.size(); ++i)
        fout << vect[i].second << ": " << vect[i].first << "\n";
    fout.close();
    return true;
}
