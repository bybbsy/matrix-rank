#include <iostream>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>

using namespace std;

const double EPS = 1E-9;

const int n = 300;
const int m = 400;

const int maxThreads = 10;

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
		p = mat[k][k];
		q = mat[i][k];

		for (int j = 0; j < m; j++)
			mat[i][j] = p * mat[i][j] - q * mat[k][j];
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

void mult(int t, int &p, int &q, int z, int &R, int &N, int mat[n][m]) {
	int chunk = (n - 1) / maxThreads;
	int remainder = (n - 1) % maxThreads;

	// Если части получаются целые
	if (remainder > 0) {
		mut.lock();

		chunk += remainder;

		// Тут функция должна считать при помощи chunk и remainder необходимые участки матрицы

		printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * (chunk - remainder), t * (chunk - remainder) + chunk);

		int to = t * (chunk - remainder) + chunk;

		/*mut.lock();*/
		for (int k = t * (chunk - remainder); k < to; k++) {
			calcRows(k, to, p, q, mat);
		}

		mut.unlock();
		 
	}
	// Если части получаются с остатков, распределяем этот остаток
	else {
		//printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * chunk, t * chunk + chunk);
		mut.lock();

		int to = t * chunk + chunk;

		for (int k = t * chunk; k < to; k++)
			calcRows(k, to, p, q, mat);

		mut.unlock();
	}
}

void calcRankThreadsImproved(int &R, int mat[n][m]) {
	int N = 0, z = 0, p = 0, q = 0;

	checkZeros(z, mat);

	for (int t = 0; t < maxThreads; t++) {
		threads.push_back(thread(mult, t, ref(p), ref(q), z, ref(R), ref(N), mat));
	}

	for (auto& th : threads) {
		th.join();
	}

	calcRank(z, R, mat);

	printf("RANK: %d\n", R);

}

void calcRankThreads(int mat[n][m]) {
	int i, j, r, N, z, R, v;

	int p = 0, q = 0;


	for (int t = 0; t < maxThreads; t++) {
		// Если потоков меньше рядов
		// Часть которую нужно посчитать
		int chunk = (n - 1) / maxThreads;
		int remainder = (n - 1) % maxThreads;

		printf("%d\n", remainder);

		// Если части получаются целые
		if (remainder > 0) {
			mut.lock();

			chunk += remainder;

			// Тут функция должна считать при помощи chunk и remainder необходимые участки матрицы

			//printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * (chunk - remainder), t * (chunk - remainder) + chunk);

			/*mut.lock();*/
			for (int k = t * (chunk - remainder); k < t * (chunk - remainder) + chunk; k++)
				threads.push_back(thread(calcRows, k, n, ref(p), ref(q), mat));

			mut.unlock();
		}
		// Если части получаются с остатков, распределяем этот остаток
		else {
			//printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * chunk, t * chunk + chunk);
			mut.lock();

			for (int k = t * chunk; k < t * chunk + chunk; k++)
				threads.push_back(thread(calcRows, k, n, ref(p), ref(q), mat));

			mut.unlock();
		}
	}


	for (auto& th : threads) {
		th.join();
	}

	printf("SIZE: %d\n", threads.size());

	z = 0;

	// Ранг считается тут
	for (i = 0; i < n; i++) {
		int c = 0;
		for (j = 0; j < m; j++) {
			if (mat[i][j] == 0) {
				c++;
			}
		}
		if (c == m) {
			z++;
		}
	}

	R = n - z;

	cout << "Rank = " << R << endl;
}

void calcRankStandart(int &R, int mat[n][m]) {
	int z, p, q;

	checkZeros(z, mat);

	for (int k = 0; k < n - 1; k++) {
		calcRows(k, n, p, q, mat);
	}

	calcRank(z, R, mat);
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

	//int mat_v2[n][m] = {
	//    //{2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12}
	//};

	srand(time(0));

	int rank;

	auto testMat = new int[n][m];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			testMat[i][j] = (rand() % 5) + 1;
		}
	}

	//displayMatrix(testMat);

	//int mat_v3[n][m] = {
	//   {1, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 } // 4 : 3
	//};


	//displayMatrix(testMat);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	//calcRankStandart(rank, testMat);

	//calcRankThreads(testMat);

	calcRankThreadsImproved(rank, testMat);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	//displayMatrix(testMat);

	printf("Matrix[%d][%d] rank = %d\n[THREADS]: %d\n[RUNTIME]: %dms", n, m, rank, maxThreads, timeDiff);
}