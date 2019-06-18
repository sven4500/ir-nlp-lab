#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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
    static size_t const threshold = 6;
    for(size_t i = 0, size = token.size(); i <= threshold && i < size; i += 2)
    {
        string const term = token.substr(0, size-i);
        vector<unsigned int>& vect = _termToDocID[term];
        if(std::find(vect.begin(), vect.end(), docID) == vect.end())
            vect.push_back(docID);
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
    std::cout << std::endl << "Всего терминов проиндексировано "
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

    std::cout << "Подготавливаю данные для записи..." << std::endl;
    for(map<string, vector<unsigned int>>::iterator iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
    {
        unsigned int const termID = crc32(0, iter->first.c_str(), iter->first.size());
        InvertedIndexMaker::_termToDocID[termID] = iter->second;
    }

    // Вызываем родительскую реализацию метода write.
    return InvertedIndexMaker::write(filename);
}
