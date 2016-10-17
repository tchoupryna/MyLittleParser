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
	//Получае имена файла и папки для результирющих файлов из командной строки
	cout << "Введите имя файла" << endl;
	cin >> FileName;
	cout << "Введите имя папки для результирющих файлов. Путь к папке должен заканчиваться символом \ " << endl;
	cin >> Path;

	InputFile.open(FileName); // открыли файл для чтения
	if (!InputFile.is_open())					// Если файл не открыт
		cout << "Файл не может быть открыт!" << endl; // сообщить об этом
	else										// Все норм - читаем файл
	{
		if (ProceedString(Header))				// Читаем заголовок таблицы. 
		{										// Если нет ошибок,
			cout << "Обработка базы данных" << endl;
			while (!InputFile.eof())			// читаем до конца файла:
			{
				ProceedString(CellBuffer);		// Читаем строку
				ProceedZip();					// Записываем её в соответствующий zip-коду файл
				ProceedCity();					// Записываем её в соответствующий городу файл
			}
			ProceedIndex();
		}
		else									// Если возникли ошибки в заголовке,
		{
			cout << "Ошибка формата файла!" << endl;	// выдаем сообщение об ошибке и закрываем файл
		}
		cout << "База данных обработана" << endl;
		InputFile.close(); // закрываем файл
	}
}

bool ParserEngine::ProceedString(ParserBuffer &Buffer)
{
	char buff;									// буфер чтения файла (выполняется побайтово)

	bool EndString = false;						// Флаг конца строки таблицы

	// Установим нулевую ячейку
	Buffer.CellPointer = 0;			
	// и обнулим строку в ней
	Buffer.Cells[Buffer.CellPointer] = "";		
	
	//Пока не конец строки или файла
	while (!(EndString || InputFile.eof()))
	{
		// Читаем файл
		InputFile.get(buff);

		// Чаще в файле встречаются символы, отличные от "," и от "конец_строки"
		// Поэтому, для сокращени количества проверок впервую очередь ищем эти символы
		if (!(buff == ',' || buff == '\r'))
		{
			//Читаем данные
			Buffer.Cells[Buffer.CellPointer] += buff;
		}
		// Проверка на символ "," - начало следующей ячейки
		// Встречается реже обычных символов - второй на очереди в поиске символа
		else if (buff == ',')
		{
			// Если считно не более 12 ячеек в строке
			if (Buffer.CellPointer < 11)
			{
				// Переходим к следующей ячейке таблицы
				Buffer.CellPointer++;
				// и обнуляем её буер
				Buffer.Cells[Buffer.CellPointer] = "";
			}
			else // Если считно более 12 ячеек в строке
			{

				// Завершаем цикл с ошбкой
				return false;
			}
		}
		// Проверка на символ "конец_строки" - самый редкий в файле.
		// Третья на очередь в поиске символа
		else if (buff == '\r')
		{
			// Если считно более 12 ячеек в строке
			if (Buffer.CellPointer < 11)
				// Завершаем цикл с ошбкой
				return false;
			// Завершаем цикл (и строку)
			EndString = true;
			//Переходим к первой ячейке
			Buffer.CellPointer = 0;
		}
	}
	// Строка считана правильно
	return true;
}

void ParserEngine::ProceedZip()
{
	set <string>::iterator it = zips.find(CellBuffer.Cells[2]);
	// Найден новый zip-код
	if (it == zips.end())
	{
		//Добавляем его в буфер найденых zip-кодов
		zips.insert(CellBuffer.Cells[2]);

		// Создаем новый HTML-файл для него
		ofstream tempOut(Path + CellBuffer.Cells[2] + ".html", ios_base::trunc);

		// Начало HTML-файла
		WriteHTMLHeader(tempOut, CellBuffer.Cells[2]);
		
		// Тег начала таблицы
		tempOut << "<table class=\"brd\">\n";
		
		// Заголовок таблицы
		WriteTableHeader(tempOut, Header);
		// Первая строка
		WriteTableLine(tempOut, CellBuffer);
		
		//Закрываем файл
		tempOut.close();
	}
	else // Такой zip-код уже есть в буфере найденых городов
	{
		// открываем файл этого zip-кода для дозаписи в конец
		ofstream tempOut(Path + CellBuffer.Cells[2] + ".html", ios_base::app);
		// Пишем строку
		WriteTableLine(tempOut, CellBuffer);
		//Закрываем файл
		tempOut.close();
	}
}

void ParserEngine::ProceedCity()
{
	set <string>::iterator it = cities.find(CellBuffer.Cells[1]);
	// Найден новый город
	if (it == cities.end())
	{
		//Добавляем его в буфер найденых городов
		cities.insert(CellBuffer.Cells[1]);

		// Создаем новый HTML-файл для него (если с таким названием есть - перезаписываем)
		ofstream tempOut(Path + CellBuffer.Cells[1] + ".html", ios_base::trunc);

		// Начало HTML-файла
		WriteHTMLHeader(tempOut, CellBuffer.Cells[1]);

		// Тег начала таблицы
		tempOut << "<table class=\"brd\">\n";

		// Заголовок таблицы
		WriteTableHeader(tempOut, Header);
		// Первая строка
		WriteTableLine(tempOut, CellBuffer);

		//Закрываем файл
		tempOut.close();
	}
	else // Такой город уже есть в буфере найденых городов
	{
		// открываем файл этого города для дозаписи в конец
		ofstream tempOut(Path + CellBuffer.Cells[1] + ".html", ios_base::app);
		// Пишем строку
		WriteTableLine(tempOut, CellBuffer);
		//Закрываем файл
		tempOut.close();
	}
}

void ParserEngine::ProceedIndex()
{
	// Создаем новый HTML-файл для него (если с таким названием есть - перезаписываем)
	ofstream index(Path + "index.html", ios_base::trunc);

	// Начало HTML-файла
	WriteHTMLHeader(index, string("Index"));

	// Заголовок списка zip-кодов
	index << "	<h1>ZIP-codes:</h1>\n";

	index << "	<p>\n";

	// Проход по множеству zip-кодов
	while (zips.begin()!= zips.end())
	{
		// Выбираем первый из оставшихся элементов множества
		index << "		<a href=\"" << *zips.begin() << ".html\">" << *zips.begin() << "</a>\n";
		
		//Завершаем HTML-файл, соответствующий этому элементу
		ofstream tempOut(Path + *zips.begin() + ".html", ios_base::app);
		tempOut << "</table>\n"
				<< "</body>\n"
				<< "</html>\n";
		// Закрываем файл
		tempOut.close();
		// Удаляем первый элемент множества
		zips.erase(zips.begin());
	}

	index << "	</p>\n";
	
	index << "<hr>\n";

	// Заголовок списка городов
	index << "	<h1>Cities:</h1>\n";

	index << "	<p>\n";

	// Проход по множеству городов
	while (cities.begin() != cities.end())
	{
		// Выбираем первый из оставшихся элементов множества
		index << "		<a href=\"" << *cities.begin() << ".html\">" << *cities.begin() << "</a>\n";

		//Завершаем HTML-файл, соответствующий этому элементу
		ofstream tempOut(Path + *cities.begin() + ".html", ios_base::app);
		tempOut << "</table>\n"
				<< "</body>\n"
				<< "</html>\n";
		// Закрываем файл
		tempOut.close();
		// Удаляем первый элемент множества
		cities.erase(cities.begin());
	}
	index << "	</p>\n";

	// Завершаем index-файл
	index << "</body>\n";
	index << "</html>\n";
	// Закрываем index-файл
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
	// Кода много, но работает быстрее, чем for(int i=0, i<12, i++)...
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
	// Кода много, но работает быстрее, чем for(int i=0, i<12, i++)...
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
