#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <set>


using namespace std;

struct ParserBuffer													// Буфер считывания строки из файла
{
	string Cells[12];												// Хранит 12 ячеек читаемой строки
	int CellPointer;												// Указатель на текущую ячейку
};


class ParserEngine
{
public:
	ParserEngine();													// Конструктор
	~ParserEngine();												// Деструктор

	void ParseFile();												// Главная функция парсера

private:
	ParserBuffer			CellBuffer,								// Буфер считываемой из файла строки
							Header;									// и заголовка таблицы

	set <string>			zips,									// Буферы найденых уникальных zip-кодов
							cities;									// и городов
		
	ifstream				InputFile;								// Файл базы данных

	string					FileName,								// Имя обрабатываемого файла
							Path;									// Папка для результирющих файлов


	bool ProceedString(ParserBuffer &Buffer);						// Чтение строки из таблицы
	void ProceedZip();												// Запись строки в HTML-файл по ZIP-коду
	void ProceedCity();												// Запись строки в HTML-файл по городу
	void ProceedIndex();											// Запись строки index-файла
	void WriteHTMLHeader(ofstream &OutFile, string &Header);		// Запись заголовка HTML
	void WriteTableHeader(ofstream &OutFile, ParserBuffer &Line);	// Запись заголовка таблицы
	void WriteTableLine(ofstream &OutFile, ParserBuffer &Line);		// Запись строки таблицы

};

