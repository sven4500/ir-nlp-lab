#pragma once
#ifndef POSINDEXREADER_H
#define POSINDEXREADER_H

#include <fstream>
#include <vector>

class PosIndexReader
{
public:
    // ������� ���������� ����������� ���������� �� ������ ������������ ���
    // ������� termID.
    static std::vector<std::pair<unsigned int, std::vector<unsigned int>>> getDocIDToPos(unsigned int tokenID, std::ifstream& fin);

protected:
    PosIndexReader();
    ~PosIndexReader();

};

#endif
