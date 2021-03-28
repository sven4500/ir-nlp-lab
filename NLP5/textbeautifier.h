#pragma once
#ifndef TEXTBEAUTIFIER_H
#define TEXTBEAUTIFIER_H

#include <string>

// ������� ������ �� ������� ������� UTF-8 ���� EN_DASH, MINUS_SIGN � ��.
void wipe_all(std::string& str);

// ������� ��������� ����� ���� ����� ����������� ����� ����� ��������������
// ���������� � ������ delim. ��������� �������� pos �� ����� ������.
std::size_t extract_token(std::string const& str, std::string& token, std::size_t pos);

void to_lower_case(std::string& token);

// isalnum ������� �� ��������� ���������� ������� ���������� ���� ����������.
inline bool is_alpha_numeric(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9');
}

#endif
