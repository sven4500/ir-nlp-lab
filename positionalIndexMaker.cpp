#include <unicode\unistr.h>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <crc32.h>
#include "positionalIndexMaker.h"
using namespace tinyxml2;

PositionalIndexMaker::PositionalIndexMaker(): _tokenCount(0), _uniqueTokenCount(0)
{}

PositionalIndexMaker::~PositionalIndexMaker()
{}

void PositionalIndexMaker::clear()
{
    _tokenToPos.clear();
}

unsigned int PositionalIndexMaker::tokenCount()const
{
    return _tokenCount;
}

unsigned int PositionalIndexMaker::uniqueTokenCount()const
{
    return _uniqueTokenCount;
}

bool PositionalIndexMaker::update(XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
    if(docID == 0)
        return false;

    std::string const text = elem->GetText();

    static std::string const delim(" ,.!?@$#&%_/*-+|<>(){}[]:;=`\\\"\'\x0A\x0D");
    /*for(std::size_t i = 0; i < delim.size(); ++i)
        std::replace(text.begin(), text.end(), delim[i], ' ');*/

    std::size_t pos = 0;
    std::size_t beg = 0;
    std::size_t end = 0;

    while((beg = text.find_first_not_of(delim, beg)) != std::string::npos && (end = text.find_first_of(delim, beg)) != std::string::npos)
    {
        std::string token = text.substr(beg, end - beg);

        // Средствами библиотеки ICU преобразуем строку в нижний регистр.
        {
            icu::UnicodeString uniToken(token.c_str(), "UTF8");
            uniToken.toLower();
            token.clear();
            uniToken.toUTF8String(token);
        }

        unsigned int const hash = crc32(0, token.c_str(), token.size());
        _tokenToPos[hash].push_back(((unsigned long long)docID << 32) | pos);
        ++_tokenCount;

        beg = end;
        ++pos;
    }

    _uniqueTokenCount = _tokenToPos.size();
    return true;
}

bool PositionalIndexMaker::write(std::string const& filename)
{
    std::ofstream fout;

    fout.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if(!fout)
    {
        std::cout << "Не удалось создать выходной файл" << std::endl;
        return false;
    }

    // Полцчаем количество уникальных токенов и резервируем память словаря.
    std::size_t const tokenCount = _tokenToPos.size();
    std::size_t const lookupTableSize = sizeof(int) * 2 * tokenCount;

    std::vector<std::pair<unsigned int, unsigned int>> lookupTable(tokenCount);

    // Пишем небольшой заголовок файла.
    {
        unsigned int const fileHeader[4] = {0xBCBCBCBC, tokenCount, lookupTableSize, 0};
        fout.write((char*)&fileHeader, sizeof(fileHeader));
    }

    // Пока пропускаем словарь.
    fout.seekp(lookupTableSize, std::ios::cur);

    {
        std::map<unsigned int, std::vector<unsigned long long>>::const_iterator iter = _tokenToPos.cbegin();
        for(std::size_t i = 0; i < tokenCount; ++i)
        {
            // Сохраняем информацию в словаре: идентификатор токена и
            // смещение первого описывающего его блока.
            lookupTable[i].first = iter->first;
            lookupTable[i].second = (unsigned int)fout.tellp();

            // Записываем блок данных токена.
            {
                std::vector<unsigned long long> const& vect = iter->second;
                std::vector<unsigned int> pos;

                unsigned int lastDocID = 0;

                for(std::size_t j = 0; j < vect.size(); ++j)
                {
                    // Идентификатор документа есть старшие биты 64-битного слова.
                    unsigned int const docID = vect[j] >> 32;

                    if((lastDocID != docID || j == vect.size() - 1) && !pos.empty())
                    {
                        unsigned int header[4] = {0x9C9C9C9C, iter->first, lastDocID, pos.size()};
                        fout.write((char*)header, sizeof(header));
                        fout.write((char*)&pos[0], sizeof(int) * pos.size());
                        pos.clear();
                    }

                    // Позиция токена есть младшие биты 64-битного слова.
                    pos.push_back(vect[j] & 0xFFFFFFFF);
                    lastDocID = docID;
                }
            }

            if(i % 2500 == 0)
                std::cout << '\r' << (i * 100) / tokenCount << '%';

            ++iter;
        }
        std::cout << '\r';
    }

    // Теперь когда у нас есть информация касаемо расположения всех блоков
    // в памяти можем сохранить словарь.
    {
        fout.seekp(sizeof(int) * 4, std::ios::beg);
        fout.write((char*)&lookupTable[0], lookupTableSize);
    }

    fout.close();
    std::cout << "Файл успешно записан" << std::endl;
    return true;
}
