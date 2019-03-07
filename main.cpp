#include <iostream>
#include <fstream>
#include <string>
#include <set>

bool tokenize(std::ifstream& fin, std::set<std::string>& tokens)
{
	fin.seekg(0, std::ios::beg);
	while(true)
	{
		std::string token;
		fin >> token;
		if(fin.eof())
			break;
		tokens.insert(token);
	}
	return !tokens.empty();
}

bool save(std::ofstream& fout, std::set<std::string> const& tokens)
{
	fout.seekp(0, std::ios::end);
	std::set<std::string>::const_iterator iter = tokens.cbegin();
	std::set<std::string>::const_iterator const end = tokens.cend();
	while(iter != end)
	{
		fout << *iter << std::endl;
		++iter;
	}
	return true;
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	if(argc != 3)
	{
		std::cout << "NLP1.exe [������� ����] [�������� ����]" << std::endl;
		return -1;
	}

	std::set<std::string> tokens;

	{
		std::ifstream fin;
		fin.open(argv[1], std::ios::in);
		if(fin)
		{
			std::cout << "����������� ����: " << argv[1] << std::endl;
			tokenize(fin, tokens);
			fin.close();
		}
		else
		{
			std::cout << "�� ���� ������� ���� ��� ������: " << argv[1] << std::endl;
		}
	}

	{
		std::ofstream fout;
		fout.open(argv[2], std::ios::out | std::ios::app);

		if(fout)
		{
			save(fout, tokens);
			fout.close();
		}
		else
		{
			std::cout << "�� ���� ������� ���� ��� ������: " << argv[2] << std::endl;
		}
	}

	return 0;
}
