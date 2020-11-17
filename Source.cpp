/*
������� ���� �������������
��� 193-2
������� �7
������
��������� ������ ������������ �������� �1, �2, �3� �n.
������� ������: ����� ������������� ����� n, ��������� ����� �1, �2,
�3� �n, �������� �������� ����� ����� ����� � �������� �������
��������� ������ ��� ����� 1. ���������� ������� �������� �������
����������.

������� ���������: 1 - ����� ������� 2 - ����� ������ 3 - ���� � ������� ������ � �����������
� ������ ���������� �� ������� ������ ����������������� ��������� �� �������������...
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
/// ������� ���� �� ��������� ������� ������������
/// </summary>
/// <param name="thrednumber"></param>
void func(int thrednumber)
{
	threadMutexes[thrednumber].lock();//���� �� ���������� ������ � main

	char buffer[10];
	string rezult = "(";
	int num = 0;
	//������ �� �������� �������
	for (int i = 0; i < eltsize; i++)
	{
		if (i == 0)
			fileMutexes[i].lock();
		//���� ����� ���� ��������

		if (fileThreads[i].eof())
			return;//����� ���������� => ����� ��� => ����� �� ������

		fileThreads[i] >> num;
		_itoa_s(num, buffer, 10);
		rezult.append(buffer);

		if (i + 1 < eltsize)
		{
			rezult.append(", ");
			fileMutexes[i + 1].lock();//���������� i+1 ��������� ������ ��� �������������� ������� (���� 1 ����� �� ������ ��� ������������� ����� ���������� 1 �� ����� ���� �.� 2 ��������� � 1 �������� � 2)
		}
		else//���������� � ������
			outputMutex.lock();//���������� ���� �� ����� � �������
		fileMutexes[i].unlock();//������������� ���� �� 1 �������� mutex
	}
	rezult.append(")");//����� �������� ������� (�����������) ������������
	cout << rezult << endl;

	outputMutex.unlock();//������������� ���� �� ������
	threadMutexes[thrednumber].unlock(); //������������� �� ���������� ������
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

		//������ �������
		int i = 0;//��������� ��������
		do
		{
			if (threadMutexes[i].try_lock())
			{
				threadMutexes[i].unlock();
				//func(i);
				threads[i] = new thread(func, i);//������ i ������
				i++;
				i %= numberOfThreads;//��� ����������� ���������� �����
			}
		} while (!fileThreads[0].eof());//���� �� �������� ������ ����...

		std::this_thread::sleep_for(2s);//��� ��� ��������� ����� ����������� ������ ��� ������, ���� 2 ������� (����� �� ����� ��������) (������� ����� ���� ����� �����)
	}catch (exception ex)
	{
		cout << ex.what() << endl;
		cin.get();
	}
	return 0;//����������
}