#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>
#include <shared_mutex>

using namespace std;

const double EPS = 1E-9;

const int n = 5;
const int m = 5;

const int maxThreads = 4;
shared_mutex shmut;

mutex mut;

vector<thread> threads;

void displayMatrix(int mat[n][m]) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf(" %d ", mat[i][j]);
		}
		printf("\n");
	}
}


void calcRows(int k, int to, int& p, int& q, int mat[n][m]) {
	
	
	for (int i = k + 1; i < to; i++) {
		

		//shared_lock<shared_mutex> sl(shmut);
		p = mat[k][k];
		q = mat[i][k];

		//lock_guard<mutex> ul(mut);

		if (p != 0) {
			for (int j = 0; j < m; j++) {

				mat[i][j] = p * mat[i][j] - q * mat[k][j];
			}
		}
		else {
			return;
		}
	}
}

void calcRank(int &z, int &R, int mat[n][m]) {
	z = 0;
	// Ранг считается тут
	for (int i = 0; i < n; i++) {
		int c = 0;
		for (int j = 0; j < m; j++) {
			if (mat[i][j] == 0) {
				c++;
			}
		}
		if (c == m) {
			z++;
		}
	}

	R = n - z;
}

void checkZeros(int &z, int mat[n][m]) {
	int i;

	if (mat[0][0] == 0) {
		for (int j = 0; j < m; j++) {
			z = -1;
			for (i = 1; i < n; i++) {
				if (mat[i][j] != 0) {
					z = i;
					break;
				}
			}
			if (z != -1) {
				for (int v = 0; v < m; v++) {
					int t = mat[0][v];
					mat[0][v] = mat[i][v];
					mat[i][v] = t;
				}
				break;
			}
		}
	}
}

void mult(int t, int &p, int &q, int &z, int &R, int &N, int mat[n][m]) { 

	int chunk = (n / maxThreads);
	int remainder = (n) % maxThreads;

	// Если части получаются целые
	if (remainder > 0) {
		//shared_lock<shared_mutex> sl(shmut);
		mut.lock();

		chunk += remainder;

		// Тут функция должна считать при помощи chunk и remainder необходимые участки матрицы

		printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * (chunk - remainder), t * (chunk - remainder) + chunk);

		/*mut.lock();*/
		for (int k = t * (chunk - remainder); k < t * (chunk - remainder) + chunk; k++) {
			calcRows(k, n, p, q, mat);
		}

		mut.unlock();
		 
	}
	// Если части получаются с остатков, распределяем этот остаток
	else {
		//shared_lock<shared_mutex> sl(shmut);
		printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * chunk, t * chunk + chunk);

		mut.lock();

		for (int k = t * chunk; k < t * chunk + chunk; k++)
			calcRows(k, n, p, q, mat);

		mut.unlock();
	}
	 
}

void calcRankThreadsImproved(int &R, int &z, int mat[n][m]) {
	int N = 0, p = 0, q = 0;

	for (int t = 0; t < maxThreads; t++) {
		threads.push_back(thread(mult, t, ref(p), ref(q), ref(z), ref(R), ref(N), mat));
	}

	for (auto& th : threads) {
		th.join();
	}
	 
	calcRank(z, R, mat);

	printf("RANK: %d\n", R);
}

void calcRankStandart(int &R, int &z, int mat[n][m]) {
	int p, q;

	checkZeros(z, mat);

	for (int k = 0; k < n - 1; k++) {
		calcRows(k, n, p, q, mat);
	} 

	calcRank(z, R, mat);
}


bool getLeadItem(int kMin, int q, int mat[n][m]) {
	for (int j = 0; j < m; j++) {
		if(mat[q][j] != 0 && j == 0) {
			return true;
		}
	}

	return false;
}

int gauss(int mat[n][m]) {
	int k = m - 1;
	int kMin = 0;

	auto copy = new int[n][m];

	// Копирует матрицу
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			copy[i][j] = mat[i][j];
		}
	}

	// Преобразовывает первую строку
	if (mat[0][0] > 1 || mat[0][0] < -1) {
		 
		for (int i = 1; i < n; i++) {
			/*if (mat[0][0] % mat[i][0] == 1 && mat[0][0] > mat[i][0]) {
				int mul = mat[0][0] / mat[i][0];
				for (int j = 0; j < m; j++) {
					copy[0][j] = mat[0][j] - mul * mat[i][j];
				}
			}*/

			// Работает только если значение mat[0][0] = -1
			if (mat[i][0] % mat[0][0] == -1 && mat[0][0] > mat[i][0]) {
				int mul = mat[i][0] / mat[0][0];
				for (int j = 0; j < m; j++) {
					copy[0][j] = mul * mat[0][j] - mat[i][j];
				}
				break;
			}
			/*for (int j = 0; j < m; j++) {

			}*/
		}
	}

	for (int i = 0; i < n - 1; i++) {
		for (int q = i + 1; q < n; q++) {

			bool lead = getLeadItem(kMin, q, mat);

			//printf("k: %d", lead);
			if (lead == true) {
				printf("%d \n", mat[q][0]);

				for (int j = 0; j < m; j++) {
					// скорее всего деление на ноль выкидвает ошибку
					//int res = mat[q][j] - ((mat[q][0] / mat[i][0]) * mat[i][j]);


					copy[q][j] = 0;
					//printf("RES: %d ", res);
				}
			}

			/*for (int j = 0; j < m; j++) {

				int res = mat[q][j] - ((mat[q][0] / mat[i][0]) * mat[i][j]);
				printf("RES: %d ", res);
			}*/

			//printf("\n");
		}

	//	//printf("\n");

		cout << "Copy" << endl;
		displayMatrix(copy);

		cout << endl;
	}

	return 1;
}

int main()
{

	//int matrix[][C] = {
	//    {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12} // 3 : 3
	//};

	//int matrix_v2[][C] = {
	//    {11, 2, 3, 4}, {1, 22, 3, 4}, {1, 123, 3, 4}, {1, 2, 3, 41 } // 4 : 3
	//};

	//int matrix_v3[][C] = {
	//     {3, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 }, {2, 2, -1, -5} // 3 : 
	//};

	//int mat[n][m] = {
	//    {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12}
	//};

	/*int testMat[n][m] = {
	    {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12}
	};*/

	srand(time(0));


	/*auto testMat = new int[n][m];


	fstream in;
	in.open("./arr100.txt");

	if (in.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				in >> testMat[i][j];
			}
		}
	}

	in.close();

	int rank, z = 0;*/


	//displayMatrix(testMat);

	//int testMat[n][m] = {
	//	{11, 2, 3, 4}, {1, 22, 3, 4}, {1, 123, 3, 4}, {1, 2, 3, 41 } // 4 :4
	//};

	//int testMat[n][m] = {
	//	{1, 2, 3}, {2, 4, 5}, {7, 8, 9} // 4 :4
	//};

	int testMat[n][m] = {
		{-2, 3, 1, 0, -4}, {0, 0, 0, 5, 6}, {4, -11, -5, 12, 18}, {-9, 6, 0, -2, 21}, {-5, 5, 1, 1, 1} // 4 :4
	};
	
	displayMatrix(testMat);

	int rank = gauss(testMat);

	//int testMat[n][m] = {
	//	{1, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 } // 3 : 3
	//};

	//int testMat[n][m] = {
	//	 {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12} // 3 : 4
	//};

	//int testMat[n][m] = {
	//	{3, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 }, {2, 2, -1, -5} // 3 : 4
	//};
	 
	/*int testMat[n][m] = {
		{1, 7, -1, -2 },
		{11, 1, -3, 0 },
		{2, 2, -1, -5},
		{1, 4, 2, 1},
		{3, 7, -1, -2 },
		{5, 4, 2, 1},
		{7, 1, -2, -1},
		{1, 5, 1, 1},
		{10, 5, -2, -1},
		{12, 1, -2, 10}
	};*/

	displayMatrix(testMat);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//calcRankStandart(rank, z, testMat);
	 

	//calcRankThreads(testMat);

	//calcRankStandart(rank, z, testMat);

	//calcRankThreadsImproved(rank, z, testMat);
	 
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	/*ofstream out;
	out.open("./out100Th.txt");
	
	if (out.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				out << testMat[i][j] << " ";
			}
			out << "\n";
		}
	}*/
	//displayMatrix(testMat);

	printf("Matrix[%d][%d] rank = %d\n[THREADS]: %d\n[RUNTIME]: %dms", n, m, rank, maxThreads, timeDiff);
}