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
template<typename T>
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
    std::map<std::string, T> const& expose()const;

    // ������� count ������������ ��������� �� ������.
    void erase(unsigned int count);

    // ����� ���������� ���������� ������������ ������ str � �������.
    T operator[](std::string str)const;

    // ����� ���������� ���������� ������������������ ���������� �����.
    unsigned int count()const;

    // ���������� ���������� ����� ����� (������� �������������).
    unsigned int totalCount()const;

    // ����� ���������� ����� �� ������ count ����� ����������� �����.
    std::vector<std::pair<std::string, T>> mostFrequent(unsigned int count)const;
    std::vector<std::pair<std::string, T>> leastFrequent(unsigned int count)const;

protected:
    // �������� ���������� �� �����, �.�. �� ������� �������������.
    static bool descendComp(std::pair<std::string, T> const& a, std::pair<std::string, T> const& b);
    static bool ascendComp(std::pair<std::string, T> const& a, std::pair<std::string, T> const& b);

    // ����������� ��� �������� ���� <�����, ����������>.
    std::map<std::string, T> _collection;
    unsigned int _totalCount;

};

#endif
