#pragma once
#ifndef TFIDFRANGE_H
#define TFIDFRANGE_H

#include <fstream>
#include <vector>

// ������� ���������� ������ ����� ������ ���������� �� ������� docID. ������
// tokenID ����� �������������� ������� ������� ���� ������������ ���
// ����������� �������. ������� ���������� ������ ��� �� ����� ��� � ������
// docID. ���������� ������� �������� ���� �������� <������������� ���������;
// ���� TF-IDF>.
std::vector<std::pair<unsigned int, double>> TFIDFRange(std::vector<unsigned int> const& docID, std::vector<unsigned int> const& tokenID, std::ifstream& fin);

#endif
