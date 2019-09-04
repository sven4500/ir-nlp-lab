#include "zoneRange.h"

std::vector<std::pair<unsigned int, double>> zoneRange(std::vector<unsigned int> const& docID, std::vector<unsigned int> const& termID, std::ifstream& fin)
{
    std::vector<std::pair<unsigned int, double>> range;
    range.resize(docID.size(), std::pair<unsigned int, double>(0, 1.0));
    for(std::size_t i = 0; i < range.size(); ++i)
        range[i].first = docID[i];
    return range;
}
