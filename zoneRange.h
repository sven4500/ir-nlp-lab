#pragma once
#ifndef ZONERANGE_H
#define ZONERANGE_H

#include <fstream>
#include <vector>

// ������� ������� ������������. �� ���� ��������� ������ ���������������
// ���������� ��������������� ���������� ������� � ������ ���������������
// ������� �� ������� ������� ��������� ������.
std::vector<std::pair<unsigned int, double>> zoneRange(std::vector<unsigned int> const& docID, std::vector<unsigned int> const& termID, std::ifstream& fin);

#endif
