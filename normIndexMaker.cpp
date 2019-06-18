#include <algorithm>
#include <fstream>
#include <iostream>
#include "normIndexMaker.h"
#include "crc32.h"
using namespace std;

size_t NormIndexMaker::equalSize(string const& s1, string const& s2)
{
    size_t const size = (min(s1.size(), s2.size()) / 2) * 2;
    if(size < 2)
        return 0;

    size_t i = 0;
    for(i = 0; i < size; i += 2)
        if(*(short*)&s1[i] != *(short*)&s2[i])
            break;

    return i;
}

NormIndexMaker::NormIndexMaker()
{

}

NormIndexMaker::~NormIndexMaker()
{

}

void NormIndexMaker::addAsTerm(string const& token, unsigned int const docID)
{
    // «десь можем сохранить значение end потому что цикл об€зан завершитьс€
    // после модификации карты.
    map<string, vector<unsigned int>>::iterator begin = _termToDocID.begin(), end = _termToDocID.end(), iter = begin;

    while(true)
    {
        if(iter == end)
        {
            // ≈сли достигли конца, то добавл€ем термин и ссылку на документ.
            _termToDocID[token].push_back(docID);
            break;
        }
        else if(iter->first == token)
        {
            // ≈сли уже имеем такой термин то добавл€ем ссылку на документ.
            iter->second.push_back(docID);
            break;
        }
        else if(token < iter->first)
        {
            // ƒошли до того момента когда текущий элемент больше токена. Ёто
            // может означать что либо (1) они однокоренные либо
            // (2) однокоренные с предыдущим элементом.

            // «адаЄм количество байт которые могут несоответствовать.
            static size_t const threshold = 6;
            size_t eqSize = 0;

            // ѕолучаем размер совпадающей части и вычисл€ем длину хвоста.
            // ≈сли в пределах погрешности, то считаем однокоренными словами.
            if((eqSize = equalSize(token, iter->first)) && (iter->first.size() - eqSize < threshold))
            {
                std::string const term = token.substr(0, eqSize);
                iter->second.push_back(docID);
                _termToDocID[term] = iter->second;
                _termToDocID.erase(iter);
            }
            else if((iter != begin) && (&(--iter)) && (eqSize = equalSize(token, iter->first)) && (iter->first.size() - eqSize < threshold))
            {
                // ќпасный момент! ƒеинкрементировали итератор так что очень
                // важно завершить цикл иначе зависнем.
                //iter->second.push_back(docID);
                std::string const term = token.substr(0, eqSize);
                iter->second.push_back(docID);
                _termToDocID[term] = iter->second;
                _termToDocID.erase(iter);
            }
            else
            {
                _termToDocID[token].push_back(docID);
            }

            break;
        }

        ++iter;
    }
}

bool NormIndexMaker::update(tinyxml2::XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
	if(docID == 0)
		return false;

	std::size_t pos = 0;
	std::size_t end = 0;

	std::string const text = elem->GetText();

	// tinyxml2 автоматически конвертирует пару CR+LF в LF, поэтому как
    // разделитель ищем только LF.
	while((end = text.find_first_of('\n', pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
        if(size > 0)
        {
            std::string const token = text.substr(pos, size);
            addAsTerm(token, docID);
        }
		pos = end + 1;
	}

	return !_termToDocID.empty();
}

bool NormIndexMaker::write(std::string const& filename)
{
    std::cout << std::endl << "¬сего терминов проиндексировано "
        << _termToDocID.size() << std::endl;

    {
        std::ofstream fout;
        fout.open("normtokens.txt", std::ios::trunc | std::ios::out);
        for(auto iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
            fout << iter->first << std::endl;
        fout.close();
    }

    return true;

    for(map<string, vector<unsigned int>>::iterator iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
    {
        unsigned int const termID = crc32(0, iter->first.c_str(), iter->first.size());
        InvertedIndexMaker::_termToDocID[termID] = iter->second;
    }

    // ¬ызываем родительскую реализацию метода write.
    return InvertedIndexMaker::write(filename);
}
