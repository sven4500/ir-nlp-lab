#include <cmath>
#include <fstream>
#include "mleproc.h"
using namespace tinyxml2;

template class MLEProc<float>;

template<typename T>
MLEProc<T>::MLEProc()
{}

template<typename T>
MLEProc<T>::~MLEProc()
{}

template<typename T>
T MLEProc<T>::logL(T const k, T const n, T const x)
{
    return std::pow(x, k) * std::pow((T)1 - x, n - k);
}

template<typename T>
void MLEProc<T>::update(Collector<unsigned int> const& monograms, BigramCollector<unsigned int> const& bigrams)
{
    for(auto iter = bigrams.expose().cbegin(), end = bigrams.expose().cend(); iter != end; ++iter)
    {
        std::size_t const pos = iter->first.find(' ');
        if(pos == std::string::npos)
            continue;

        std::string const first = iter->first.substr(0, pos),
            second = iter->first.substr(pos + 1, std::string::npos);
        if(first.empty() || second.empty())
            continue;

        T const p = (T)monograms[second] / (T)bigrams.totalCount(),
            p1 = (T)iter->second / (T)monograms[first],
            p2 = (T)(monograms[second] - iter->second) / (T)(bigrams.totalCount() - monograms[first]);

        T const v = logL(iter->second, monograms[first], p) +
            logL(monograms[second] - iter->second, bigrams.totalCount() - monograms[first], p) -
            logL(iter->second, monograms[first], p1) -
            logL(monograms[second] - iter->second, bigrams.totalCount() - monograms[first], p2);

        if(v < 100.0)
            _collection[iter->first] = v;
    }
}

template<typename T>
bool MLEProc<T>::dump(char const* const filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::trunc);
    if(!fout)
        return false;
    std::vector<std::pair<std::string, T>> const vect = mostFrequent(100);
    for(std::size_t i = 0; i < vect.size(); ++i)
        fout << vect[i].second << ": " << vect[i].first << "\n";
    fout.close();
    return true;
}
