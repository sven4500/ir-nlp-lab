#include <cmath> // log
#include <algorithm> // sort
#include "TFIDFRange.h"

#pragma pack(push, 1)

// Структура описывает документ в котором встречается запрашиваемый токен. На
// список таких структур указывает запись в словаре.
struct DocFreq
{
    unsigned int _docID;                // идентификатор документа
    unsigned short _tokenCount;         // количество вхождений токена
    unsigned short _tokenTotalCount;    // всего токенов в документе
};

// Структура описывает обду запись в словаре.
struct DictItem
{
    unsigned int _tokenID;
    unsigned int _docCount;
    unsigned int _offBytes;
    unsigned int _unused;
};

#pragma pack(pop)

// Структура описывает пару <токен, документ>.
struct TFIDFPairMetric
{
    unsigned int _tokenID;
    unsigned int _docCount;
    unsigned int _docTotalCount;
    unsigned int _docID;
    unsigned int _tokenCount;
    unsigned int _tokenTotalCount;
};

struct TFIDFMetric
{
    TFIDFMetric(): _tokenID(0), _docCount(0), _docTotalCount(0)
    {}

    // Метод конвертирует полную метрику в метрику для пары токен - документ.
    TFIDFPairMetric toPairMetric(unsigned int docID)const
    {
        TFIDFPairMetric metric = {};
        for(std::size_t i = 0; i < _docFreq.size(); ++i)
        {
            if(_docFreq[i]._docID == docID)
            {
                metric._tokenID = _tokenID;
                metric._docCount = _docCount;
                metric._docTotalCount = _docTotalCount;

                metric._docID = docID;
                metric._tokenCount = _docFreq[i]._tokenCount;
                metric._tokenTotalCount = _docFreq[i]._tokenTotalCount;
                
                return metric;
            }
        }
        return metric;
    }

    unsigned int _tokenID;
    unsigned int _docCount;
    unsigned int _docTotalCount;
    std::vector<DocFreq> _docFreq;
};

TFIDFMetric getTFIDF(std::ifstream& fin, unsigned int tokenID)
{
    TFIDFMetric metric /*= {}*/;
    // Идентификатор токена не может быть нулевым иначе вызов функции
    // бессмысленный. Однако идентификатор документа вполне может быть и
    // нулнвым если нас не интересует информация относительно конкретного
    // документа.
    if(!fin || tokenID == 0)
        return metric;

    fin.seekg(0, std::ios::beg);

    struct
    {
        unsigned int _sign;
        unsigned int _tokenCount;
        unsigned int _docCount;
        unsigned int _unused;
    }fileHd;
    fin.read((char*)&fileHd, sizeof(fileHd));

    if(fileHd._sign != 0x8C8C8C8C || fileHd._tokenCount == 0 || fileHd._docCount == 0)
        return metric;

    DictItem dictItem;
    for(unsigned int i = 0; i < fileHd._tokenCount; ++i)
        if(fin.read((char*)&dictItem, sizeof(dictItem)) && dictItem._tokenID == tokenID)
            break;

    // Если искомый токен не был найден, то возвращаем пустую метрику.
    if(dictItem._tokenID != tokenID || dictItem._docCount == 0)
        return metric;

    metric._tokenID = tokenID;
    metric._docCount = dictItem._docCount;
    metric._docTotalCount = fileHd._docCount;
    metric._docFreq.resize(metric._docCount);

    fin.seekg(dictItem._offBytes, std::ios::beg);
    fin.read((char*)&metric._docFreq[0], sizeof(DocFreq) * metric._docCount);

    return metric;
}

// Предикат для сортировки вектора пар <документ, ранг>.
/*bool pred(std::pair<unsigned int, double> const& p1, std::pair<unsigned int, double> const& p2)
{
    // Возвращает true если элемент p1 должен находится в списке до p2.
    return p1.second > p2.second;
}*/

// Функция ранжирования TF-IDF.
std::vector<std::pair<unsigned int, double>> TFIDFRange(std::vector<unsigned int>& docID, std::vector<unsigned int> const& tokenID, std::ifstream& fin)
{
    if(docID.empty() || tokenID.empty())
        return std::vector<std::pair<unsigned int, double>>();

    std::vector<std::vector<TFIDFPairMetric>> metrics(docID.size(), std::vector<TFIDFPairMetric>(tokenID.size()));

    // Получаем метрику токена (учитывает все возможные документы).
    {
        std::vector<TFIDFMetric> temp(tokenID.size());

        // Получаем TF-IDF метрики на каждый токен. Метрика учитывает все
        // возможные документы в состав которых входит токен.
        for(std::size_t i = 0; i < temp.size(); ++i)
            temp[i] = getTFIDF(fin, tokenID[i]);

        // Преобразовываем метрику в пару <токен, документ>.
        for(std::size_t i = 0; i < docID.size(); ++i)
            for(std::size_t j = 0; j < tokenID.size(); ++j)
                metrics[i][j] = temp[j].toPairMetric(docID[i]);
    }

    // Вектор пар <документ, ранг>. После того как он будет заполнен
    // каждому документу будет соответствовать определённый ранг.
    // Отсортировав этот список полцчим ранжированный список документов.
    std::vector<std::pair<unsigned int, double>> rangePair(docID.size(), std::pair<unsigned int, double>(0, 0.0));

    for(std::size_t i = 0; i < docID.size(); ++i)
    {
        rangePair[i].first = docID[i];

        for(std::size_t j = 0; j < tokenID.size(); ++j)
        {
            double const TF = (metrics[i][j]._tokenTotalCount > 0) ? (double)metrics[i][j]._tokenCount / metrics[i][j]._tokenTotalCount : 0.0;
            double const IDF = (metrics[i][j]._docTotalCount > 0 && metrics[i][j]._docCount > 0) ? log((double)metrics[i][j]._docTotalCount / metrics[i][j]._docCount) : 0.0;
            rangePair[i].second += TF * IDF;
        }
    }

    /*std::sort(rangePair.begin(), rangePair.end(), pred);

    for(std::size_t i = 0; i < docID.size(); ++i)
        docID[i] = rangePair[i].first;*/

    return rangePair;
}
