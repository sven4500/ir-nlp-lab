#include <cassert>
#include <map>
#include "zoneRange.h"
#include "posIndexReader.h"

std::vector<std::pair<unsigned int, double>> zoneRange(std::vector<unsigned int> const& docID, std::vector<unsigned int> const& termID, std::ifstream& fin)
{
    // Весовые коэффициенты для каждой зоны (идентификатора зоны). Зоны:
    // заголовок, первая половина документа. вторая половина документа.
    std::map<unsigned int, double> w;
    w[1] = 0.5;
    w[2] = 0.4;
    w[3] = 0.1;

    std::vector<std::pair<unsigned int, double>> range;
    range.resize(docID.size());

    for(std::size_t i = 0; i < range.size(); ++i)
    {
        range[i].first = docID[i];
        range[i].second = 0.0;
    }

    for(std::size_t i = 0; i < termID.size(); ++i)
    {
        // Формат файла зон идентичен формату файла координатного индекса
        // поэтому здесь используем класс PosIndexReader для получения
        // информации о зонах. В результате имеем вектор пар <документ; зоны>
        // для одного термина из поискового запроса.
        std::vector<std::pair<unsigned int, std::vector<unsigned int>>> const zones = PosIndexReader::getDocIDToPos(termID[i], fin);

        for(std::size_t j = 0; j < zones.size(); ++j)
        {
            for(std::size_t k = 0; k < range.size(); ++k)
            {
                if(zones[j].first == range[k].first)
                {
                    if(zones[j].second.empty() == false && w.find(zones[j].second.front()) != w.end())
                        range[k].second += w[zones[j].second.front()];
                    break;
                }
            }
        }
    }

    for(std::size_t i = 0; i < range.size(); ++i)
        range[i].second /= termID.size();

    return range;
}
