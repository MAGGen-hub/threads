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

// c++ main.cpp -fopenmp //for correct omp work (this file is only for c++ compiller not for vs)
#include <iostream>
#include <fstream>
#include <mutex>
#include <omp.h>
using namespace std;

fstream* fileThreads;

int main(int argc, char** argv)
{
	try{
		string fileway = argv[3];//full way to file without extention and intdex (../int/*Filename**index*.*extend*)
		string fileextention = ".txt";
		

		int n = atoi(((string)argv[1]).c_str());
		int numberOfThreads = atoi(((string)argv[2]).c_str());
		fileThreads = new fstream[n];

		#pragma omp parallel num_threads(numberOfThreads)
		{
			bool check = true;
			#pragma omp for
			for (int i = 0; i < n; i++)//files init
			{

				auto num = omp_get_thread_num();
				//#pragma omp critical //for test
				//{
				//	std::cout << " thread " << num << std::endl;
				//}
				char buffer[100];
				string str = fileway;
				_itoa(i + 1, buffer, 10);
				str.append(buffer);
				str.append(fileextention);
				fileThreads[i] = fstream(str, ios::in);

				if (!fileThreads[i].is_open())
					check = false;
			}
			if (!check)
				throw runtime_error("Error! Failed to open input file!");
		}

		omp_set_num_threads(numberOfThreads);
		#pragma omp parallel
		{
			do
			{
				char buffer[100];
				string rezult = "(";
				int* num = new int[n];
				bool temp = true;
				//чтение из файловых потоков
				#pragma omp critical (nm)
				{
					for (int j = 0; j < n; j++)
					{	
						if (fileThreads[j].eof())
							temp = false;
						fileThreads[j] >> num[j];
					}
				}
				for (int j = 0; j < n; j++)
				{
					_itoa(num[j], buffer, 10);
					rezult.append(buffer);

					if (j + 1 < n)
						rezult.append(", ");
				}
				rezult.append(")");//конец элемента прямого (декартового) произведения
				#pragma omp critical (cout)
				{
					if(temp)
					cout << omp_get_thread_num() << ": " << rezult << endl;
				}
			} while (!fileThreads[0].eof());
		}
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	return 0;//завершение
}