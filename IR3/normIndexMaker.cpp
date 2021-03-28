#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include "normIndexMaker.h"
#include "crc32.h"
using namespace std;

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
        unsigned int const hash = crc32(0, &term[0], term.size());
        vector<unsigned int>& vect = _termToDocID[hash];
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
