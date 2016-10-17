#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <set>


using namespace std;

struct ParserBuffer													// ����� ���������� ������ �� �����
{
	string Cells[12];												// ������ 12 ����� �������� ������
	int CellPointer;												// ��������� �� ������� ������
};


class ParserEngine
{
public:
	ParserEngine();													// �����������
	~ParserEngine();												// ����������

	void ParseFile();												// ������� ������� �������

private:
	ParserBuffer			CellBuffer,								// ����� ����������� �� ����� ������
							Header;									// � ��������� �������

	set <string>			zips,									// ������ �������� ���������� zip-�����
							cities;									// � �������
		
	ifstream				InputFile;								// ���� ���� ������

	string					FileName,								// ��� ��������������� �����
							Path;									// ����� ��� ������������� ������


	bool ProceedString(ParserBuffer &Buffer);						// ������ ������ �� �������
	void ProceedZip();												// ������ ������ � HTML-���� �� ZIP-����
	void ProceedCity();												// ������ ������ � HTML-���� �� ������
	void ProceedIndex();											// ������ ������ index-�����
	void WriteHTMLHeader(ofstream &OutFile, string &Header);		// ������ ��������� HTML
	void WriteTableHeader(ofstream &OutFile, ParserBuffer &Line);	// ������ ��������� �������
	void WriteTableLine(ofstream &OutFile, ParserBuffer &Line);		// ������ ������ �������

};

