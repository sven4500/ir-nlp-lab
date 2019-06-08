#pragma once
#ifndef TFIDFMAKER_H
#define TFIDFMAKER_H

#include <map>
#include <vector>
#include "indexMaker.h"

class TFIDFMaker: public IndexMaker
{
public:
    TFIDFMaker();
    ~TFIDFMaker();

    virtual void clear();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

    unsigned int getDocCount()const;
    unsigned int getTokenCount()const;

protected:
    struct DocFreq
    {
        unsigned int _docID;
        unsigned short _tokenCount;
        unsigned short _tokenTotalCount;
    };

    // Отображение связывает ИД токена с количеством документов в которых он
    // встречается.
    std::map<unsigned int, unsigned int> _tok2CorpFreq;

    // Отображение ИД токена на список документов в которых этот токен
    // встречается.
    std::map<unsigned int, std::vector<DocFreq>> _tok2DocFreq;

    // Количество документов корпуса.
    unsigned int _docCount;

};

#endif
