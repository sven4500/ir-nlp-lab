#pragma once
#ifndef MONOGRAMCOLLECTOR_H
#define MONOGRAMCOLLECTOR_H

#include <vector>
#include <map>
#include <string>
#include "icorpusprocessor.h"

// ����� ��� ����� ��������� ����� �� XML ��������� (���������� �������).
// ������ ���� <������, �������>. ����� ������������ ����� ���� ����������� ���
// �������� ��� ���������, ��� � �������.
class Collector: public ICorpusProcessor
{
public:
    Collector();
    virtual ~Collector();

    virtual void update(tinyxml2::XMLElement const* elem);
    virtual void clear();
    virtual bool dump(char const* filename);

    // ����� ���������� ����������� ������ �� ����� ��������� ��� ����� �������
    // �������� ����� ��������� ���������� ��� ����� ���������������.
    std::map<std::string, unsigned int> const& expose()const;

    // ����� ���������� ���������� ������������ ������ str � �������.
    unsigned int operator[](std::string str)const;

    // ����� ���������� ���������� ������������������ ���������� �����.
    unsigned int count()const;

    // ���������� ���������� ����� ����� (������� �������������).
    unsigned int totalCount()const;

    // ����� ���������� ����� �� ������ count ����� ����������� �����.
    std::vector<std::pair<std::string, unsigned int>> mostFrequent(unsigned int count)const;
    std::vector<std::pair<std::string, unsigned int>> leastFrequent(unsigned int count)const;

protected:
    // �������� ���������� �� �����, �.�. �� ������� �������������.
    template<typename ty1, typename ty2>
    static bool descendComp(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b);

    template<typename ty1, typename ty2>
    static bool ascendComp(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b);

    // ����������� ��� �������� ���� <�����, ����������>.
    std::map<std::string, unsigned int> _collection;
    unsigned int _totalCount;

};

#endif
