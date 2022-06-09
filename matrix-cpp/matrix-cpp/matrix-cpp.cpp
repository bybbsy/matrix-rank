#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <random>

using namespace std;

const double EPS = 1E-9;

const int n = 10;
const int m = 4;

const int maxThreads = 2;
// shared_mutex shmut;

// mutex mut;

vector<thread> threads;

void displayMatrix(double mat[n][m]) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf(" %g ", mat[i][j]);
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
		//mut.lock();

		chunk += remainder;

		// Тут функция должна считать при помощи chunk и remainder необходимые участки матрицы

		//printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * (chunk - remainder), t * (chunk - remainder) + chunk);

		/*mut.lock();*/
		for (int k = t * (chunk - remainder); k < t * (chunk - remainder) + chunk; k++) {
			calcRows(k, n, p, q, mat);
		}

		//mut.unlock();
		 
	}
	// Если части получаются с остатков, распределяем этот остаток
	else {
		//shared_lock<shared_mutex> sl(shmut);
		//printf("THREAD CHUNK: FROM: %d, TO: %d\n", t * chunk, t * chunk + chunk);

		//mut.lock();

		for (int k = t * chunk; k < t * chunk + chunk; k++)
			calcRows(k, n, p, q, mat);

		//mut.unlock();
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

	//printf("RANK: %d\n", R);
}

void calcG(int t, int chunk, int i, int p, int q, int k, int mat[n][m]) {
	for (int j = t * chunk; j < t * chunk + chunk; j++) {
		mat[i][j] = mat[k][k] * mat[i][j] - mat[i][k] * mat[k][j];
	}
}
void calcRankThreads(int& R, int& z, int mat[n][m]) {
	checkZeros(z, mat);

	for (int k = 0; k < n - 1; k++) {
		for (int i = k + 1; i < n; i++) {

			int chunk = (m / maxThreads);
			int remainder = (m) % maxThreads;
			 
			if (remainder > 0) {

				chunk += remainder;


				int p = mat[k][k];
				int q = mat[i][k];

				if (p != 0) {
					for (int t = 0; t < maxThreads; t++) {
						threads.push_back(thread(calcG, t, chunk, i, p, q, k, mat));
					}
					for (auto& th : threads) {
						th.join();
					}
					
				}
				else {
					return;
				}
			}
			else {

			}

		}
	}

	calcRank(z, R, mat);
}

void calcRankStandart(int &R, int &z, int mat[n][m]) {
	int p, q;

	checkZeros(z, mat);

	for (int k = 0; k < n - 1; k++) {
		calcRows(k, n, p, q, mat);
	} 

	calcRank(z, R, mat);
}


int getLeadItem(int q, double mat[n][m]) {
	for (int j = 0; j < m; j++) {
		if(mat[q][j] != 0) {
			return j;
		}
	}

	return 0;
}


void replaceRows(double copy[n][m]) {
	for (int i = 1; i < n; i++) {
		if (i < n - 1) {
			for (int j = 0; j < m; j++) {
				//cout << "Here" << endl;
				if (copy[i][j] == 0 && copy[i + 1][j] != 0) {
					//printf("Rows");
					for (int q = i; q < i + 1; q++) {

						for (int b = 0; b < m; b++) {
							double old = copy[q][b];
							double newest = copy[q + 1][b];


							copy[q][b] = newest;
							copy[q + 1][b] = old;
						}
					}
				}
			}
		}
	}
}

void bubbleSort(double mat[n][m]) {
	for (int i = 1; i + 1 < n; i++) {
		for (int q = i + 1; q < n; q++) {
			for (int j = 0; j < m; j++) {
				if (mat[i][j] == 0 && mat[q][j] != 0) {

						for (int b = 0; b < m; b++) {
							double old = mat[i][b];
							double newest = mat[q][b];


							mat[i][b] = newest;
							mat[q][b] = old;
						}
					}
				
			}
		}
	}
}

int gauss(double copy[n][m]) {

	//auto copy = new int[n][m];

	// Копирует матрицу
	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			copy[i][j] = mat[i][j];
		}
	}*/

	// Преобразовывает первую строку
	if (copy[0][0] > 1 || copy[0][0] < -1) {
		double init = copy[0][0];

		for (int i = 1; i < n; i++) {
			if (copy[0][0] > copy[i][0]) {
				double mul = floor(copy[0][0] / copy[i][0]);

				double firstValue = copy[0][0] - mul * copy[i][0];

				//cout << "Less" << int(copy[0][0] / copy[i][0]) << endl;
				
				if (firstValue > 0 && firstValue < copy[0][0]) {
					for (int j = 0; j < m; j++) {
						copy[0][j] = copy[0][j] - mul * copy[i][j];
					}
				}
				break;
			}

			// Работает только если значение mat[0][0] = -1
			if (int(copy[i][0]) % int(copy[0][0]) == -1 && copy[0][0] > copy[i][0]) {
				int mul = copy[i][0] / copy[0][0];
				for (int j = 0; j < m; j++) {
					copy[0][j] = mul * copy[0][j] - copy[i][j];
				}
				break;
			}
			/*for (int j = 0; j < m; j++) {

			}*/
		}
		
		if (init == copy[0][0]) {
			for (int j = 0; j < m; j++) {
				copy[0][j] = copy[0][j] / init;
			}
		}
	}

	// Перемещает строку
	replaceRows(copy);

	//displayMatrix(copy);

	// Первый основной проход по рядам
	for (int i = 0; i < n; i++) {
		int k = getLeadItem(i, copy);

		//printf("ROW: %d LEAD: %d\n", i, k);
		// Сравнивает текущий ряд с нижними
		for (int q = i + 1; q < n; q++) {

			bool first = false;

			// Проходит по строкам
			for (int j = 0; j < m; j++) {

				//printf("%d %d\n", j, k);

				// Если текущий элемент равен наименьшему ведущему
				if (j == k && copy[q][k] != 0) {
					// скорее всего деление на ноль выкидвает ошибку

					//double first = copy[q][k];

					double div = round((copy[q][k] / copy[i][k]) * 100) / 100;

					printf("%g ", div);
					for (int h = k; h < m; h++) {
						double res = round((copy[q][h] - div * copy[i][h]) * 100) / 100;

						//printf("%g - %g * %g = %g\n", copy[q][h], div, copy[i][h], res);

						copy[q][h] = res;
					}
					//printf("first");

					first = true;
					 
					//printf("%g %g %g %g \n", copy[q][j], copy[q][0], copy[i][0], copy[i][j]);
					//break;
				}
				// Если текущий элемент больше ведущего, то нужно переместить строку
				//else if (j > k && first == false) {
				//	for (int u = q + 1; u < n; u++) {
				//		for (int t = 0; t < m; t++) {
				//			
				//			if (t == k && copy[u][k] != 0) {
				//				//printf("q: %d u: %d", q, u);
				//				  // printf("Before\n");
				//				  // displayMatrix(copy);
				//				for (int b = 0; b < m; b++) {
				//					int old = copy[q][b];
				//					int newest = copy[u][b];


				//					copy[q][b] = newest;
				//					copy[u][b] = old;
				//				}

				//				// printf("After\n");
				//				// displayMatrix(copy);

				//				double div = round((copy[q][k] / copy[i][k]) * 100) / 100;

				//				for (int h = k; h < m; h++) {
				//					double res = round((copy[q][h] - div * copy[i][h]) * 100) / 100;

				//					//printf("%g - %g * %g = %g\n", copy[q][h], div, copy[i][h], res);

				//					copy[q][h] = res;
				//				}

				//				  // printf("Nulled\n");
				//				  // displayMatrix(copy);
				//				break;
				//			}
				//		}
				//		break;
				//	}
				//}
				//printf("\n");
			}
			//printf("\n");
		}

		 // printf("Before replace\n");
		 // displayMatrix(copy);

		//for (int ii = 1; ii < n; ii++) {
		//	if (ii < n - 1) {
		//		for (int j = 0; j < m; j++) {
		//			//cout << "Here" << endl;
		//			if (copy[ii][j] == 0 && copy[ii + 1][j] != 0) {
		//				printf("Rows");
		//				for (int q = ii; q < ii + 1; q++) {

		//					for (int b = 0; b < m; b++) {
		//						double old = copy[q][b];
		//						double newest = copy[q + 1][b];


		//						copy[q][b] = newest;
		//						copy[q + 1][b] = old;
		//					}
		//				}
		//			}
		//		}
		//	}
		//}

		bubbleSort(copy);
		 // printf("After replace\n");
		 // displayMatrix(copy);


		//cout << "Rank: " << R << endl;

	//	//printf("\n");

		//cout << endl;
	}

	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			mat[i][j] = copy[i][j];
		}
	}*/

	//displayMatrix(copy);
	int R, z = 0;

	for (int i = 0; i < n; i++) {
		int c = 0;
		for (int j = 0; j < m; j++) {
			if (copy[i][j] == 0) {
				c++;
			}
		}
		if (c == m) {
			z++;
		}
	}

	R = n - z;

	return R;
}

void gauss_calc_rows(int i, int k, int t, int chunk, int remainder, double copy[n][m]) {
	double div = 0, res = 0;
	int from = 0, to = 0;

	//mut.lock();
	//displayMatrix(copy);
	//printf("[i]:%d [chunk]:%d [TH]:%d", i, chunk, t);
	if (remainder > 0) {
		chunk += remainder;
		from = t * chunk + i + 1;
		to = t * (chunk) + chunk + i;

		if (to + 1 < n) {
			to++;
		}

		for (int q = from; q < to; q++) {
			//printf("> 0 THREAD:%d CHUNK: FROM: %d, TO: %d\n", t, from, to);
			// Проходит по строкам
			for (int j = 0; j < m; j++) {

				// Если текущий элемент равен наименьшему ведущему
				if (j == k && copy[q][k] != 0) {

					div = round((copy[q][k] / copy[i][k]) * 100) / 100;

					for (int h = k; h < m; h++) {
						res = round((copy[q][h] - div * copy[i][h]) * 100) / 100;

						//printf("%g - %g * %g = %g\n", copy[q][h], div, copy[i][h], res);

						copy[q][h] = res;
					}
				}
			}
		}
	}
	else {
		from = t * chunk + i + 1;
		to = t * chunk + chunk + i + 1;
		for (int q = from; q < to; q++) {
			//printf("= 0 THREAD:%d CHUNK: FROM: %d, TO: %d\n",t, from, to);
			// Проходит по строкам
			for (int j = 0; j < m ; j++) {

				// Если текущий элемент равен наименьшему ведущему
				if (j == k && copy[q][k] != 0) {

					div = round((copy[q][k] / copy[i][k]) * 100) / 100;
					//printf("%d", div);
					for (int h = k; h < m; h++) {
						res = round((copy[q][h] - div * copy[i][h]) * 100) / 100;

						//printf("%g - %g * %g = %g\n", copy[q][h], div, copy[i][h], res);

						copy[q][h] = res;
					}
				}
			}
		}
	}

	//mut.unlock();
}

int gauss_thread(double copy[n][m]) {
	
	
	if (copy[0][0] > 1 || copy[0][0] < -1) {
		double init = copy[0][0];

		for (int i = 1; i < n; i++) {
			if (copy[0][0] > copy[i][0]) {
				double mul = floor(copy[0][0] / copy[i][0]);

				double firstValue = copy[0][0] - mul * copy[i][0];

				//cout << "Less" << int(copy[0][0] / copy[i][0]) << endl;

				if (firstValue > 0 && firstValue < copy[0][0]) {
					for (int j = 0; j < m; j++) {
						copy[0][j] = copy[0][j] - mul * copy[i][j];
					}
				}
				break;
			}

			// Работает только если значение mat[0][0] = -1
			if (int(copy[i][0]) % int(copy[0][0]) == -1 && copy[0][0] > copy[i][0]) {
				int mul = copy[i][0] / copy[0][0];
				for (int j = 0; j < m; j++) {
					copy[0][j] = mul * copy[0][j] - copy[i][j];
				}
				break;
			}
		}

		if (init == copy[0][0]) {
			for (int j = 0; j < m; j++) {
				copy[0][j] = copy[0][j] / init;
			}
		}
	}

	// Перемещает строку
	replaceRows(copy);


	int chunk, remainder;

	// Первый основной проход по рядам
	for (int i = 0; i < n; i++) {
		int k = getLeadItem(i, copy);

		chunk = (n - i - 1) / maxThreads;
		remainder = (n - i - 1) % maxThreads;

		//printf("ROW: %d LEAD: %d\n", i, k);
		// Сравнивает текущий ряд с нижними

		for (int t = 0; t < maxThreads; t++) {
			threads.push_back(thread(gauss_calc_rows, i, k, t, chunk, remainder, copy));
		}

		 
		for (auto& th : threads) {
			th.join();
		}



		threads.clear();
		
		//thread t1(gauss_calc_rows, i, k, 0, chunk, remainder, copy);
		//thread t2(gauss_calc_rows, i, k, 1, chunk, remainder, copy);

		//t1.join();
		//t2.join();

		bubbleSort(copy);
	}


	//displayMatrix(copy);

	int R, z = 0;

	for (int i = 0; i < n; i++) {
		int c = 0;
		for (int j = 0; j < m; j++) {
			if (copy[i][j] == 0) {
				c++;
			}
		}
		if (c == m) {
			z++;
		}
	}

	R = n - z;

	return R;
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

	/*double testMat[n][m] = {
	    {2., 1.0, 3.0, 0.0}, {1.0, 4.0, 2.0, 1.0}, {3.0, 2.0, 2.0, 6.0}, {8.0, 5.0, 7.0, 12.0}
	};*/

	/*int testMat[n][m] = {
	    {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12}
	};*/

	srand(time(0));


	/*auto testMat = new double[n][m];

	fstream in;
	in.open("./arr100.txt");

	if (in.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				in >> testMat[i][j];
			}
		}
	}

	in.close();*/

	//auto testMat = new double[n][m];

	/*ofstream out;
	out.open("./arr100.txt");

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distr(1, 10);

	if (out.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				out << distr(gen) << " ";
			}
			out << "\n";
		}
	}*/

	//int R, rank = 0, z = 0;


	//displayMatrix(testMat);

	// OK
	//double testMat[n][m] = {
	//	{11, 2, 3, 4}, {1, 22, 3, 4}, {1, 123, 3, 4}, {1, 2, 3, 41 } // 4 :4
	//};

	//double testMat[n][m] = {
	//	{1, 2, 3}, {2, 4, 5}, {7, 8, 9} // 4 :4
	//};

	//double testMat[n][m] = {
	//	{-2, 3, 1, 0, -4}, {0, 0, 0, 5, 6}, {4, -11, -5, 12, 18}, {-9, 6, 0, -2, 21}, {-5, 5, 1, 1, 1} // 4 :4
	//};
	
	// OK
	//double testMat[n][m] = {
	//	{-2, 3, 1, 0, -4}, {0, 0, 0, 5, 6}, {4, -11, -5, 12, 18}, {-9, 6, 0, -2, 21}, {-5, 5, 1, 1, 1} // 4 :4
	//};

	//double testMat[n][m] = {
	//	{1, 1, 1}, {1, 2, 3}, {1, 4, 9} // 4 :4
	//};

	//double testMat[n][m] = {
	//	{11.0, -13.0, 61.0, 10.0, -11.0}, {2, -2, 11, 2, -2}, {-3, 5, -17, -2, 3}, {4, 0, 24, 7, -8} // 4 :4
	//};

	//displayMatrix(testMat);

	

	//double testMat[n][m] = {
	//	{1, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 } // 3 : 3
	//};

	//double testmat[n][m] = {
	//	 {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12} // 3 : 4
	//};

	//double testMat[n][m] = {
	//	{3, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 }, {2, 2, -1, -5} // 3 : 4
	//};
	//

	double testMat[n][m] = {
		{1, 7, -1, -2 },
		{1, 1, -3, 10 },
		{2, 2, -1, -5},
		{1, 4, 2, 1},
		{3, 7, -1, -2 },
		{5, 4, 2, 1},
		{7, 1, -2, -1},
		{1, 5, 1, 1},
		{10, 5, -2, -1},
		{12, 1, -2, 10}
	};

	//displayMatrix(testMat);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//calcRankStandart(rank, z, testMat);
	

	/*double test[n][m] = {
		{1, 2, 3, 4},
		{0, 2, 1, 5},
		{0, 5, 6, 7},
		{11,5, 6, 7},
		{0, 1, 2, 3}
	};

	displayMatrix(test);
	bubbleSort(test);

	printf("Sorted\n");
	displayMatrix(test);*/
	
	int rank = gauss(testMat);

	displayMatrix(testMat);

	//int rank = gauss_thread(testMat);
	//calcRankThreads(testMat);

	//calcRankStandart(rank, z, testMat);

	//calcRankThreadsImproved(rank, z, testMat);
	 

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	ofstream out;
	out.open("./out100Th.txt");
	
	if (out.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				out << testMat[i][j] << " ";
			}
			out << "\n";
		}
	}
	//displayMatrix(testMat);

	printf("Matrix[%d][%d] rank = %d\n[THREADS]: %d\n[RUNTIME]: %dms", n, m, rank, maxThreads, timeDiff);
}