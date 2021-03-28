#include "posIndexReader.h"

PosIndexReader::PosIndexReader()
{}

PosIndexReader::~PosIndexReader()
{}

std::vector<std::pair<unsigned int, std::vector<unsigned int>>> PosIndexReader::getDocIDToPos(unsigned int const tokenID, std::ifstream& fin)
{
    fin.seekg(0, std::ios::beg);

    #pragma pack(1)
    struct
    {
        unsigned int _sign;
        unsigned int _tokenCount;
        unsigned int _lookupTableBytes;
        unsigned int _reserved;
    }fileHead = {};

    fin.read((char*)&fileHead, sizeof(fileHead));
    if(fileHead._sign != 0xBCBCBCBC || fileHead._tokenCount * 8 != fileHead._lookupTableBytes)
        return std::vector<std::pair<unsigned int, std::vector<unsigned int>>>();

    // ��������� ������� � ������. ��� ����� ����� �������.
    std::vector<std::pair<unsigned int, unsigned int>> lookupTable(fileHead._tokenCount);
    fin.read((char*)&lookupTable[0], fileHead._lookupTableBytes);

    std::pair<unsigned int, unsigned int> lookupRecord(0, 0);

    // ������� � ������� ������ ����������� � �������� ������.
    for(std::size_t i = 0; i < lookupTable.size(); ++i)
    {
        if(lookupTable[i].first == tokenID)
        {
            lookupRecord = lookupTable[i];
            break;
        }
    }

    // ��������� ��� �� ������ ����� ����� � �������.
    if(lookupRecord.first == 0)
        return std::vector<std::pair<unsigned int, std::vector<unsigned int>>>();

    // ��������� ���� ��� ���������� ���� ������� ���������
    // ���������������� � ������� �������.
    fin.seekg(lookupRecord.second, std::ios::beg);

    std::vector<std::pair<unsigned int, std::vector<unsigned int>>> docIDToPos;

    // ��������� ��� ����� ���������� ����������� � ���������� ������.
    while(true)
    {
        #pragma pack(1)
        struct{
            unsigned int _sign;
            unsigned int _tokenID;
            unsigned int _docID;
            unsigned int _posCount;
        }head = {};

        fin.read((char*)&head, sizeof(head));

        // ��������� ������������ ��������� ����� ������. �� ������ ������
        // ��������� ��������� ���������, ��-������ � ��������� ������ ����
        // ������ ������� �����, � ���� ��� �� ���, �� ������ ��������� ���
        // � ���������� ������.
        if(head._sign != 0x9C9C9C9C || head._tokenID != tokenID)
            break;

        if(head._posCount != 0)
        {
            std::pair<unsigned int, std::vector<unsigned int>> p(head._docID, head._posCount);
            fin.read((char*)&p.second[0], sizeof(int) * head._posCount);
            docIDToPos.push_back(p);
        }
    }

    return docIDToPos;
}
