#include <fstream>
#include <cassert>
#include "studproc.h"

StudProc::StudProc()
{}

StudProc::~StudProc()
{}

void StudProc::update(Collector const& monograms, BigramCollector const& bigrams)
{
    double const totalCount = bigrams.totalCount();

    for(auto iter = bigrams.expose().cbegin(), end = bigrams.expose().cend(); iter != end; ++iter)
    {
        std::size_t const pos = iter->first.find(' ');
        if(pos == std::string::npos)
            continue;

        std::string const first = iter->first.substr(0, pos);
        std::string const second = iter->first.substr(pos + 1, std::string::npos);

        if(first.empty() || second.empty())
            continue;

        double const freqBigram = iter->second;

        double const freqFirst = monograms[first];
        double const freqSecond = monograms[second];

        double const mu = (freqFirst * freqSecond) / (totalCount * totalCount);
        double const x = freqBigram / totalCount;
        double const t = (x - mu) / std::sqrt(x / totalCount);

        if(t < 2.576)
            _collection[iter->first] = (int)(t * 1000000);
    }
}

bool StudProc::dump(char const* const filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::trunc);
    if(!fout)
        return false;
    std::vector<std::pair<std::string, unsigned int>> const vect = leastFrequent(100);
    for(std::size_t i = 0; i < vect.size(); ++i)
        fout << (int)vect[i].second << ": " << vect[i].first << "\n";
    fout.close();
    return true;
}
