#include "..\Headers\ParserEngine.h"

#include <shlobj.h>

ParserEngine::ParserEngine()
{
}


ParserEngine::~ParserEngine()
{
}

void ParserEngine::ParseFile()
{
	//������� ����� ����� � ����� ��� ������������� ������ �� ��������� ������
	cout << "������� ��� �����" << endl;
	cin >> FileName;
	cout << "������� ��� ����� ��� ������������� ������. ���� � ����� ������ ������������� �������� \ " << endl;
	cin >> Path;

	InputFile.open(FileName); // ������� ���� ��� ������
	if (!InputFile.is_open())					// ���� ���� �� ������
		cout << "���� �� ����� ���� ������!" << endl; // �������� �� ����
	else										// ��� ���� - ������ ����
	{
		if (ProceedString(Header))				// ������ ��������� �������. 
		{										// ���� ��� ������,
			cout << "��������� ���� ������" << endl;
			while (!InputFile.eof())			// ������ �� ����� �����:
			{
				ProceedString(CellBuffer);		// ������ ������
				ProceedZip();					// ���������� � � ��������������� zip-���� ����
				ProceedCity();					// ���������� � � ��������������� ������ ����
			}
			ProceedIndex();
		}
		else									// ���� �������� ������ � ���������,
		{
			cout << "������ ������� �����!" << endl;	// ������ ��������� �� ������ � ��������� ����
		}
		cout << "���� ������ ����������" << endl;
		InputFile.close(); // ��������� ����
	}
}

bool ParserEngine::ProceedString(ParserBuffer &Buffer)
{
	char buff;									// ����� ������ ����� (����������� ���������)

	bool EndString = false;						// ���� ����� ������ �������

	// ��������� ������� ������
	Buffer.CellPointer = 0;			
	// � ������� ������ � ���
	Buffer.Cells[Buffer.CellPointer] = "";		
	
	//���� �� ����� ������ ��� �����
	while (!(EndString || InputFile.eof()))
	{
		// ������ ����
		InputFile.get(buff);

		// ���� � ����� ����������� �������, �������� �� "," � �� "�����_������"
		// �������, ��� ��������� ���������� �������� ������� ������� ���� ��� �������
		if (!(buff == ',' || buff == '\r'))
		{
			//������ ������
			Buffer.Cells[Buffer.CellPointer] += buff;
		}
		// �������� �� ������ "," - ������ ��������� ������
		// ����������� ���� ������� �������� - ������ �� ������� � ������ �������
		else if (buff == ',')
		{
			// ���� ������ �� ����� 12 ����� � ������
			if (Buffer.CellPointer < 11)
			{
				// ��������� � ��������� ������ �������
				Buffer.CellPointer++;
				// � �������� � ����
				Buffer.Cells[Buffer.CellPointer] = "";
			}
			else // ���� ������ ����� 12 ����� � ������
			{

				// ��������� ���� � ������
				return false;
			}
		}
		// �������� �� ������ "�����_������" - ����� ������ � �����.
		// ������ �� ������� � ������ �������
		else if (buff == '\r')
		{
			// ���� ������ ����� 12 ����� � ������
			if (Buffer.CellPointer < 11)
				// ��������� ���� � ������
				return false;
			// ��������� ���� (� ������)
			EndString = true;
			//��������� � ������ ������
			Buffer.CellPointer = 0;
		}
	}
	// ������ ������� ���������
	return true;
}

void ParserEngine::ProceedZip()
{
	set <string>::iterator it = zips.find(CellBuffer.Cells[2]);
	// ������ ����� zip-���
	if (it == zips.end())
	{
		//��������� ��� � ����� �������� zip-�����
		zips.insert(CellBuffer.Cells[2]);

		// ������� ����� HTML-���� ��� ����
		ofstream tempOut(Path + CellBuffer.Cells[2] + ".html", ios_base::trunc);

		// ������ HTML-�����
		WriteHTMLHeader(tempOut, CellBuffer.Cells[2]);
		
		// ��� ������ �������
		tempOut << "<table class=\"brd\">\n";
		
		// ��������� �������
		WriteTableHeader(tempOut, Header);
		// ������ ������
		WriteTableLine(tempOut, CellBuffer);
		
		//��������� ����
		tempOut.close();
	}
	else // ����� zip-��� ��� ���� � ������ �������� �������
	{
		// ��������� ���� ����� zip-���� ��� �������� � �����
		ofstream tempOut(Path + CellBuffer.Cells[2] + ".html", ios_base::app);
		// ����� ������
		WriteTableLine(tempOut, CellBuffer);
		//��������� ����
		tempOut.close();
	}
}

void ParserEngine::ProceedCity()
{
	set <string>::iterator it = cities.find(CellBuffer.Cells[1]);
	// ������ ����� �����
	if (it == cities.end())
	{
		//��������� ��� � ����� �������� �������
		cities.insert(CellBuffer.Cells[1]);

		// ������� ����� HTML-���� ��� ���� (���� � ����� ��������� ���� - ��������������)
		ofstream tempOut(Path + CellBuffer.Cells[1] + ".html", ios_base::trunc);

		// ������ HTML-�����
		WriteHTMLHeader(tempOut, CellBuffer.Cells[1]);

		// ��� ������ �������
		tempOut << "<table class=\"brd\">\n";

		// ��������� �������
		WriteTableHeader(tempOut, Header);
		// ������ ������
		WriteTableLine(tempOut, CellBuffer);

		//��������� ����
		tempOut.close();
	}
	else // ����� ����� ��� ���� � ������ �������� �������
	{
		// ��������� ���� ����� ������ ��� �������� � �����
		ofstream tempOut(Path + CellBuffer.Cells[1] + ".html", ios_base::app);
		// ����� ������
		WriteTableLine(tempOut, CellBuffer);
		//��������� ����
		tempOut.close();
	}
}

void ParserEngine::ProceedIndex()
{
	// ������� ����� HTML-���� ��� ���� (���� � ����� ��������� ���� - ��������������)
	ofstream index(Path + "index.html", ios_base::trunc);

	// ������ HTML-�����
	WriteHTMLHeader(index, string("Index"));

	// ��������� ������ zip-�����
	index << "	<h1>ZIP-codes:</h1>\n";

	index << "	<p>\n";

	// ������ �� ��������� zip-�����
	while (zips.begin()!= zips.end())
	{
		// �������� ������ �� ���������� ��������� ���������
		index << "		<a href=\"" << *zips.begin() << ".html\">" << *zips.begin() << "</a>\n";
		
		//��������� HTML-����, ��������������� ����� ��������
		ofstream tempOut(Path + *zips.begin() + ".html", ios_base::app);
		tempOut << "</table>\n"
				<< "</body>\n"
				<< "</html>\n";
		// ��������� ����
		tempOut.close();
		// ������� ������ ������� ���������
		zips.erase(zips.begin());
	}

	index << "	</p>\n";
	
	index << "<hr>\n";

	// ��������� ������ �������
	index << "	<h1>Cities:</h1>\n";

	index << "	<p>\n";

	// ������ �� ��������� �������
	while (cities.begin() != cities.end())
	{
		// �������� ������ �� ���������� ��������� ���������
		index << "		<a href=\"" << *cities.begin() << ".html\">" << *cities.begin() << "</a>\n";

		//��������� HTML-����, ��������������� ����� ��������
		ofstream tempOut(Path + *cities.begin() + ".html", ios_base::app);
		tempOut << "</table>\n"
				<< "</body>\n"
				<< "</html>\n";
		// ��������� ����
		tempOut.close();
		// ������� ������ ������� ���������
		cities.erase(cities.begin());
	}
	index << "	</p>\n";

	// ��������� index-����
	index << "</body>\n";
	index << "</html>\n";
	// ��������� index-����
	index.close();
}

void ParserEngine::WriteHTMLHeader(ofstream &OutFile, string & Header)
{
	OutFile << "<!DOCTYPE html>\n" 
			<< "<html>\n"
			<< "<head>\n"
			<< "	<title>" << Header << "</title>\n"
			<< "</head>\n"
			<< "<body>\n";
}

void ParserEngine::WriteTableHeader(ofstream & OutFile, ParserBuffer & Line)
{
	// ���� �����, �� �������� �������, ��� for(int i=0, i<12, i++)...
	OutFile << "	<tr>\n"
		<< "		<th>" << Line.Cells[0] << "</th>\n"
		<< "		<th>" << Line.Cells[1] << "</th>\n"
		<< "		<th>" << Line.Cells[2] << "</th>\n"
		<< "		<th>" << Line.Cells[3] << "</th>\n"
		<< "		<th>" << Line.Cells[4] << "</th>\n"
		<< "		<th>" << Line.Cells[5] << "</th>\n"
		<< "		<th>" << Line.Cells[6] << "</th>\n"
		<< "		<th>" << Line.Cells[7] << "</th>\n"
		<< "		<th>" << Line.Cells[8] << "</th>\n"
		<< "		<th>" << Line.Cells[9] << "</th>\n"
		<< "		<th>" << Line.Cells[10] << "</th>\n"
		<< "		<th>" << Line.Cells[11] << "</th>\n"
		<< "	</tr>\n";
}

void ParserEngine::WriteTableLine(ofstream & OutFile, ParserBuffer &Line)
{
	// ���� �����, �� �������� �������, ��� for(int i=0, i<12, i++)...
	OutFile << "	<tr>\n"
			<< "		<td>" << Line.Cells[0] << "</td>\n"
			<< "		<td>" << Line.Cells[1] << "</td>\n"
			<< "		<td>" << Line.Cells[2] << "</td>\n"
			<< "		<td>" << Line.Cells[3] << "</td>\n"
			<< "		<td>" << Line.Cells[4] << "</td>\n"
			<< "		<td>" << Line.Cells[5] << "</td>\n"
			<< "		<td>" << Line.Cells[6] << "</td>\n"
			<< "		<td>" << Line.Cells[7] << "</td>\n"
			<< "		<td>" << Line.Cells[8] << "</td>\n"
			<< "		<td>" << Line.Cells[9] << "</td>\n"
			<< "		<td>" << Line.Cells[10] << "</td>\n"
			<< "		<td>" << Line.Cells[11] << "</td>\n"
			<< "	</tr>\n";
}
