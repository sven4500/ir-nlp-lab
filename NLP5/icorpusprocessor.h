#pragma once
#ifndef ICORPUSPROCESSOR_H
#define ICORPUSPROCESSOR_H

#include <tinyxml2.h>

// ������� ����� ��� ������ ����������� ������� ����������. ������ ������
// ����� ������ ������ ��������� ���� ���� ������� �������� ��� ��������. ��
// ����� ������������ ��� ���������� ��� ��������� ������� ������������. ������
// ������ ���������� ������ ����� ���� �� ��� ��������������� ������ update �
// clear - ���� ��� ���������� ����������, ������ ��� ������ ���������. �����
// dump �������������� �� �����������.
class ICorpusProcessor
{
public:
    virtual void update(tinyxml2::XMLElement const* elem)=0;
    virtual void clear()=0;

    // ���������� ������ ������ � ����. ������� ����� ���� �������������, ����
    // �������� ���������� ������ �������.
    virtual bool dump(char const* filename)
    {
        return true;
    }

protected:
    ICorpusProcessor()
    {

    }

    virtual ~ICorpusProcessor()
    {
    
    }

};

#endif
