#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{	
	setlocale(LC_ALL, "Russian");

	if(argc != 3)
	{
		std::cout << "NLP1.exe [������� ����] [�������� ����]" << std::endl;
		return -1;
	}

	std::string const fnIn(argv[1]);
	std::string const fnOut(argv[2]);

	std::ifstream fin;
	std::ofstream fout;

	fin.open(fnIn, std::ios::in);
	fout.open(fnOut, std::ios::out);

	if(!fin)
	{
		std::cout << "�� ���� ������� ���� ��� ������: " << fnIn << std::endl;
		goto _runaway;
	}

	if(!fout)
	{
		std::cout << "�� ���� ������� ���� ��� ������: " << fnOut << std::endl;
		goto _runaway;
	}

	{
		std::cout << "����������� ����: " << fnIn << std::endl;
	}

	_runaway:
	fin.close();
	fout.close();
	return 0;
}
