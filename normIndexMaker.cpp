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

void NormIndexMaker::addTokenAsTerm(string const& token, unsigned int const docID)
{
    // «десь можем сохранить значение end потому что цикл об€зан завершитьс€
    // после модификации карты.
    map<string, vector<unsigned int>>::iterator begin = _termToDocID.begin(), end = _termToDocID.end(), iter = begin;

    while(true)
    {
        if(iter == end)
        {
            // ≈сли достигли конца, то добавл€ем термин и ссылку на документ.
            pair<string, vector<unsigned int>> p;
            p.first = token;
            p.second.push_back(docID);
            _termToDocID.insert(iter, p);
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
            // (2) однокоренные с предыдущим элементом, (3) либо нет.

            // «адаЄм количество байт которые могут несоответствовать
            // (3 русских символа UTF-8).
            static size_t const threshold = 6;
            size_t eqSize = 0;

            // ѕолучаем размер совпадающей части и вычисл€ем длину хвоста.
            // ≈сли в пределах погрешности, то считаем однокоренными словами.
            if((eqSize = equalSize(token, iter->first)) && (iter->first.size() - eqSize < threshold))
            {
                pair<string, vector<unsigned int>> p;
                p.first = token.substr(0, eqSize);
                p.second.swap(iter->second);
                p.second.push_back(docID);
                _termToDocID.insert(iter, p);
                _termToDocID.erase(iter);
            }
            // «десь несколько важных моментов: (1) используем выражение
            // (&(--iter)) чтобы декрементировать итератор в выражении под if;
            // (2) разницу считаем от меньшего по длине токена иначе потер€ем
            // токены.
            else if((iter != begin) && (&(--iter)) && (eqSize = equalSize(token, iter->first)) && (iter->first.size() - eqSize < threshold))
            {
                pair<string, vector<unsigned int>> p;
                p.first = token.substr(0, eqSize);
                p.second.swap(iter->second);
                p.second.push_back(docID);
                _termToDocID.insert(iter, p);
                _termToDocID.erase(iter);
            }
            else
            {
                // «десь важно что мы не полагаемс€ на текущий итератор.
                pair<string, vector<unsigned int>> p;
                p.first = token;
                p.second.push_back(docID);
                _termToDocID.insert(iter, p);
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
            addTokenAsTerm(token, docID);
        }
		pos = end + 1;
	}

	return !_termToDocID.empty();
}

bool NormIndexMaker::write(std::string const& filename)
{
    std::cout << std::endl << "¬сего терминов проиндексировано "
        << _termToDocID.size() << std::endl;

    #if defined(_DEBUG)
    {
        std::ofstream fout;
        fout.open("normtokens.txt", std::ios::trunc | std::ios::out);
        for(auto iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
            fout << iter->first << std::endl;
        fout.close();
    }
    #endif

    for(map<string, vector<unsigned int>>::iterator iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
    {
        unsigned int const termID = crc32(0, iter->first.c_str(), iter->first.size());
        InvertedIndexMaker::_termToDocID[termID] = iter->second;
    }

    // ¬ызываем родительскую реализацию метода write.
    return InvertedIndexMaker::write(filename);
}
