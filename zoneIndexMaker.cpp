#include <cassert> // assert
#include <iostream> // cout
#include "ZoneIndexMaker.h"
using namespace tinyxml2;

ZoneIndexMaker::ZoneIndexMaker()
{}

ZoneIndexMaker::~ZoneIndexMaker()
{}

bool ZoneIndexMaker::update(XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
    if(docID == 0)
        return false;

    bool const success = PositionalIndexMaker::update(elem);
    if(!success)
        return false;

    // �������� �������� ��������� ���������. ���� ���� �� �������� �� �����
    // ���������� true ������ ��� update �������� ����� ���� ����������.
    char const* const title = elem->Attribute("title");
    if(!title)
        return true;

    std::string const text = title;

    unsigned int hash = 0;
    std::size_t beg = 0;
    std::size_t end = 0;

    while((hash = getTokenID(text, beg, end)) != 0)
    {
        // -1 � ������� ��������� �������� ��� ��� ���� ��������� ���������. -1
        // ����� ������� "ul" ������ ��� ��� ����� ������� 32 ����. ����� ���
        // �������� ��������������� ��� ���� ���������� ���������.
        std::vector<unsigned long long>& vect = _tokenToPos[hash];
        vect.push_back(((unsigned long long)docID << 32) | -1ul);
        ++_totalTokenCount;
        beg = end;
    }

    return false;
}

bool ZoneIndexMaker::write(std::string const& filename)
{
    std::cout << "�������� �������� �� ����..." << std::endl;

    // �������������� ���������� � ����.
    for(std::map<unsigned int, std::vector<unsigned long long>>::iterator iter = _tokenToPos.begin(), end = _tokenToPos.end(); iter != end; ++iter)
    {
        std::vector<unsigned long long>& vect = iter->second;
        std::vector<unsigned long long> zones;

        assert(!vect.empty());

        for(std::size_t i = 0; i < vect.size(); ++i)
        {
            unsigned long long index = vect[i];

            unsigned int const docID = index >> 32;
            int const pos = index & -1l;

            // ������������� ��������� ������ ���������.
            assert(docID != 0);

            unsigned int currentZone = 0;

            // ���� ������ ����� ��������� ���� ����� ������ �����������
            // �������� ��� �����-������ ��� ���.
            if(pos == -1)
            {
                currentZone = 1;
            }
            else if(pos >= 0)
            {
                // ������������ �� ����� ���� ������ ���������� ���� �
                // ���������. +2 ���������� � ���� ������ ��� 0 ��������������,
                // � 1 ����� ����������.
                assert((unsigned int)pos <= _docIDToTokenCount[docID]);
                currentZone = pos / (_docIDToTokenCount[docID] / _numberOfZones) + 2;
            }

            // � ������� ������ ������ ���� �� ����� ���� ����� ����.
            assert(currentZone > 0);

            index = ((unsigned long long)docID << 32) | currentZone;

            // ������ �������������. ���������� ��������� ��������� ������.
            if(zones.empty() || zones.back() != index)
                zones.push_back(index);
        }

        assert(!zones.empty());
        assert(zones.size() <= vect.size());

        // ������ ���������� � ������������� �� ���������� � �����.
        iter->second = zones;
    }

    return PositionalIndexMaker::write(filename);
}
