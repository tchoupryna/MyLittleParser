#include "ParserEngine.h"

using namespace std;

ParserEngine Parser;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus"); // ���������� ����������� ���������

	Parser.ParseFile();

	system("pause");

	return 0;
}