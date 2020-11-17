/*
Исайков Иван Александрович
БПИ 193-2
Вариант №7
Потоки
Вычислить прямое произведение множеств А1, А2, А3… Аn.
Входные данные: целое положительное число n, множества чисел А1, А2,
А3… Аn, мощности множеств равны между собой и мощность каждого
множества больше или равна 1. Количество потоков является входным
параметром.

Входные параметры: 1 - колво потоков 2 - колво файлов 3 - путь к входным файлам с множествами
В случае отклонения от условия задачи работоспособность программы не гарантируется...
*/
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
using namespace std;


fstream* fileThreads;
thread** threads;
mutex* fileMutexes;
mutex* threadMutexes;
mutex outputMutex;
int eltsize;

/// <summary>
/// Выводит один из элементов прямого произведения
/// </summary>
/// <param name="thrednumber"></param>
void func(int thrednumber)
{
	threadMutexes[thrednumber].lock();//блок на перезапись потока в main

	char buffer[10];
	string rezult = "(";
	int num = 0;
	//чтение из файловых потоков
	for (int i = 0; i < eltsize; i++)
	{
		if (i == 0)
			fileMutexes[i].lock();
		//если права были получены

		if (fileThreads[i].eof())
			return;//поток закончился => чисел нет => выход из потока

		fileThreads[i] >> num;
		_itoa_s(num, buffer, 10);
		rezult.append(buffer);

		if (i + 1 < eltsize)
		{
			rezult.append(", ");
			fileMutexes[i + 1].lock();//блокировка i+1 файлового потока для предотвращения колизий (если 1 поток не успеет его заблокировать после блокировки 1 то будет сбой т.к 2 прочитает и 1 занчение и 2)
		}
		else//подготовка к выводу
			outputMutex.lock();//блокировка прав нв вывод в консоль
		fileMutexes[i].unlock();//разблокировка прав на 1 файловый mutex
	}
	rezult.append(")");//конец элемента прямого (декартового) произведения
	cout << rezult << endl;

	outputMutex.unlock();//разблокировка прав на чтение
	threadMutexes[thrednumber].unlock(); //разблокировка на перезапись потока
	return;
}

int main(int argc, char** argv)
{
	try {
		char buffer[10];
		string fileway = argv[3];//full way to file without extention and intdex (../int/*Filename**index*.*extend*)
		string fileextention = ".txt";


		int n = atoi(((string)argv[1]).c_str());
		int numberOfThreads = atoi(((string)argv[2]).c_str());
		fileThreads = new fstream[n];
		fileMutexes = new mutex[n];
		eltsize = n;
		threads = new thread * [numberOfThreads];
		threadMutexes = new mutex[numberOfThreads];

		for (int i = 0; i < n; i++)//files init
		{
			string str = fileway;
			_itoa_s(i + 1, buffer, 10);
			str.append(buffer);
			str.append(fileextention);
			fileThreads[i] = fstream(str, ios::in);
			if (!fileThreads[i].is_open())
				throw runtime_error("Error! Failed to open input file!");
		}

		//запуск потоков
		int i = 0;//потоковый итератор
		do
		{
			if (threadMutexes[i].try_lock())
			{
				threadMutexes[i].unlock();
				//func(i);
				threads[i] = new thread(func, i);//запуск i потока
				i++;
				i %= numberOfThreads;//для перезапуска потокового цикла
			}
		} while (!fileThreads[0].eof());//пока не кончится первый файл...

		std::this_thread::sleep_for(2s);//так как программа может завершиться раньше чем потоки, спим 2 секунды (чтобы не убило мьютексы) (потоков может быть очень много)
	}catch (exception ex)
	{
		cout << ex.what() << endl;
		cin.get();
	}
	return 0;//завершение
}