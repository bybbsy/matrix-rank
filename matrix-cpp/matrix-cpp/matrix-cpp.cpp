#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <future>x`

#define stand
using namespace std;

const double EPS = 1E-9;

const int n = 500;
const int m = 00;

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

void calcRank(int &z, int &R, double mat[n][m]) {
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

//void calcRankThreadsImproved(int &R, int &z, double mat[n][m]) {
//	int N = 0, p = 0, q = 0;
//
//	for (int t = 0; t < maxThreads; t++) {
//		threads.push_back(thread(mult, t, ref(p), ref(q), ref(z), ref(R), ref(N), mat));
//	}
//
//	for (auto& th : threads) {
//		th.join();
//	}
//	 
//	calcRank(z, R, mat);
//
//	//printf("RANK: %d\n", R);
//}
//
//void calcG(int t, int chunk, int i, int p, int q, int k, int mat[n][m]) {
//	for (int j = t * chunk; j < t * chunk + chunk; j++) {
//		mat[i][j] = mat[k][k] * mat[i][j] - mat[i][k] * mat[k][j];
//	}
//}
//void calcRankThreads(int& R, int& z, int mat[n][m]) {
//	checkZeros(z, mat);
//
//	for (int k = 0; k < n - 1; k++) {
//		for (int i = k + 1; i < n; i++) {
//
//			int chunk = (m / maxThreads);
//			int remainder = (m) % maxThreads;
//			 
//			if (remainder > 0) {
//
//				chunk += remainder;
//
//
//				int p = mat[k][k];
//				int q = mat[i][k];
//
//				if (p != 0) {
//					for (int t = 0; t < maxThreads; t++) {
//						threads.push_back(thread(calcG, t, chunk, i, p, q, k, mat));
//					}
//					for (auto& th : threads) {
//						th.join();
//					}
//					
//				}
//				else {
//					return;
//				}
//			}
//			else {
//
//			}
//
//		}
//	}
//
//	calcRank(z, R, mat);
//}
//
//void calcRankStandart(int &R, int &z, int mat[n][m]) {
//	int p, q;
//
//	checkZeros(z, mat);
//
//	for (int k = 0; k < n - 1; k++) {
//		calcRows(k, n, p, q, mat);
//	} 
//
//	calcRank(z, R, mat);
//}

int getLeadItem(int q, double mat[n][m]) {
	for (int j = 0; j < m; j++) {
		if(mat[q][j] != 0) {
			return j;
		}
	}

	return 0;
}


void replaceRows(double mat[n][m]) {
	for (int i = 1; i < n; i++) {
		if (i < n - 1) {
			for (int j = 0; j < m; j++) {
				//cout << "Here" << endl;
				if (mat[i][j] == 0 && mat[i + 1][j] != 0) {
					//printf("Rows");
					for (int q = i; q < i + 1; q++) {

						for (int b = 0; b < m; b++) {
							double old = mat[q][b];
							double newest = mat[q + 1][b];


							mat[q][b] = newest;
							mat[q + 1][b] = old;
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

// OUTER: 1802x + 901y + 901x = 3608
// INNER: 

// Inner row: 600x + 300y + 
void gauss(double mat[n][m]) {
	// Первый основной проход по рядам
	for (int i = 0; i < n; i++) {
		int k = getLeadItem(i, mat);

		// Сравнивает текущий ряд с нижними
		for (int q = i + 1; q < n; q++) {
			// Проходит по строкам
			for (int j = 0; j < m; j++) {
				// Если текущий элемент равен наименьшему ведущему
				if (j == k && mat[q][k] != 0) {
					double div = round((mat[q][k] / mat[i][k]) * 100) / 100;
					for (int h = k; h < m; h++) {
						double res = round((mat[q][h] - div * mat[i][h]) * 100) / 100;

						mat[q][h] = res;
					}
				}
			}
		}

		bubbleSort(mat);

		//printf("ROW: %d and matrix: \n", i);
		//displayMatrix(mat);
	}
}


int getLeadItem_test(int q, int& f, double mat[n][m]) {
	f += 2;
	for (int j = 0; j < m; j++) {
		f += 3;
		if (mat[q][j] != 0) {
			f++;
			return j;
		}
	}

	return 0;
}

void bubbleSort_test(int& f, double mat[n][m]) {
	f += 2;
	for (int i = 1; i + 1 < n; i++) {
		for (int q = i + 1; q < n; q++) {
			for (int j = 0; j < m; j++) {
				if (mat[i][j] == 0 && mat[q][j] != 0) {

					for (int b = 0; b < m; b++) {
						double old = mat[i][b];
						double newest = mat[q][b];


						mat[i][b] = newest;
						mat[q][b] = old;

						f += 6;
					}

					f += 5;
				}
				f += 2;

			}
			f += 5;
		}

		f += 5;
	}
}

void gauss_test(double mat[n][m]) {
	int f = 2;
	// Первый основной проход по рядам
	for (int i = 0; i < n; i++) {
		int k = getLeadItem_test(i, f, mat);

		//printf("ROW: %d LEAD: %d\n", i, k);
		// Сравнивает текущий ряд с нижними
		
		for (int q = i + 1; q < n; q++) {
			
			// Проходит по строкам
			for (int j = 0; j < m; j++) {

				//printf("%d %d\n", j, k);

				// Если текущий элемент равен наименьшему ведущему
				if (j == k && mat[q][k] != 0) {
					f += 3;

					double div = round((mat[q][k] / mat[i][k]) * 100) / 100;
					f += 6;
					for (int h = k; h < m; h++) {
						double res = round((mat[q][h] - div * mat[i][h]) * 100) / 100;

						mat[q][h] = res;
						f += 9;
					}

					f += 3;
				}
				f += 5;
			}
			f += 5;
		}
		bubbleSort_test(f, mat);

	}


	printf("f: %d", f);
}
void fineMatrix(double mat[n][m]) {
	if (mat[0][0] > 1 || mat[0][0] < -1) {
		double init = mat[0][0];

		for (int i = 1; i < n; i++) {
			if (mat[0][0] > mat[i][0]) {
				double mul = floor(mat[0][0] / mat[i][0]);

				double firstValue = mat[0][0] - mul * mat[i][0];

				//cout << "Less" << int(mat[0][0] / mat[i][0]) << endl;

				if (firstValue > 0 && firstValue < mat[0][0]) {
					for (int j = 0; j < m; j++) {
						mat[0][j] = mat[0][j] - mul * mat[i][j];
					}
				}
				break;
			}

			// Работает только если значение mat[0][0] = -1
			if (int(mat[i][0]) % int(mat[0][0]) == -1 && mat[0][0] > mat[i][0]) {
				int mul = mat[i][0] / mat[0][0];
				for (int j = 0; j < m; j++) {
					mat[0][j] = mul * mat[0][j] - mat[i][j];
				}
				break;
			}
		}

		if (init == mat[0][0]) {
			for (int j = 0; j < m; j++) {
				mat[0][j] = mat[0][j] / init;
			}
		}
	}

	// Перемещает строку
	replaceRows(mat);
}


void gauss_calc_rows(int i, int k, int t, int chunk, int remainder,  double mat[n][m]) {
	double div = 0, res = 0;
	 int from = 0, to = 0;
	if (remainder > 0) {
		chunk += remainder;
		from = t * chunk + i + 1;
		to = t * (chunk) + chunk + i;

		if (to + 1 < n) {
			to++;
		}

		for (int q = from; q < to; q++) {
			for (int j = 0; j < m; j++) {
				if (j == k && mat[q][k] != 0) {

					div = round((mat[q][k] / mat[i][k]) * 100) / 100;

					for (int h = k; h < m; h++) {
						res = round((mat[q][h] - div * mat[i][h]) * 100) / 100;
						mat[q][h] = res;
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
				if (j == k && mat[q][k] != 0) {

					div = round((mat[q][k] / mat[i][k]) * 100) / 100;
					//printf("%d", div);
					for (int h = k; h < m; h++) {
						res = round((mat[q][h] - div * mat[i][h]) * 100) / 100;

						//printf("%g - %g * %g = %g\n", mat[q][h], div, mat[i][h], res);

						mat[q][h] = res;
					}
				}
			}
		}
	}

	//mut.unlock();
}

void gauss_thread(double mat[n][m]) {
	thread* t1 = new thread;
	//thread* t2 = new thread;
	int chunk, remainder;
	 
	int from = 0, to = 0;
	// Первый основной проход по рядам
	for (int i = 0; i < n; i++) {
		int k = getLeadItem(i, mat);

		chunk = (n - i - 1) / maxThreads;
		remainder = (n - i - 1) % maxThreads;

		if (chunk >= 2) 
		{
			*t1 = thread(gauss_calc_rows, i, k, 0, chunk, remainder, mat);
			//*t2 = thread(gauss_calc_rows, i, k, 1, chunk, remainder, mat);
			
			gauss_calc_rows(i, k, 1, chunk, remainder, mat);
			
			t1->join();
			//t2->join();
		}
		else if(chunk == 1) {
			*t1 = thread(gauss_calc_rows, i, k, 0, chunk, remainder, mat);
			t1->join();
		}

		bubbleSort(mat);
	}
}

void createRandMatrix() {
	
		ofstream out;
		out.open("./arrTest.txt");
	
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> distr(0, 5);
	
		if (out.is_open()) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < m; j++) {
					out << distr(gen) << " ";
				}
				out << "\n";
			}
		}
}


void writeMatrix(double mat[n][m]) {
	ofstream out;
	out.open("./out100Th.txt");

	if (out.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				out << mat[i][j] << " ";
			}
			out << "\n";
		}
	}
}

int main() {

	//auto testMat = new double[1000];
	//
	//fstream in;
	//in.open("./arrTest.txt");
	//
	//if (in.is_open()) {
	//	for (int i = 0; i < 1000; i++) { 
	//		in >> testMat[i]; 
	//	}
	//}

	//in.close();

	//int sum = 0;
	//int sum_a = 0;
	//int sum_b = 0;

	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//
	//async(launch::async, [&]() {
	//	for (int i = 0; i < 500; i++)
	//		sum_a += testMat[i];
	//	});

	//async(launch::async, [&]() {
	//	for (int i = 500; i < 1000; i++)
	//		sum_b += testMat[i];
	//	});

	//sum = sum_a + sum_b;
	///*std::thread t1([&]() {
	//	for (int i = 0; i < 500; i++)
	//		sum_a += testMat[i];
	//});

	//std::thread t2([&]() {
	//	for (int i = 500; i < 1000; i++)
	//		sum_b += testMat[i];
	//	});

	//t1.detach();
	//t2.detach();

	//sum = sum_a + sum_b;*/

	///*for (int i = 0; i < 1000; i++) {
	//	sum += testMat[i];
	//}*/

	// 
	// 
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//
	//auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	//printf("sum %d : runtime: %d", sum, timeDiff);

	srand(time(0));

	//createRandMatrix();

	auto testMat = new double[n][m];

	fstream in;
	in.open("./arrTest.txt");

	if (in.is_open()) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				in >> testMat[i][j];
			}
		}
	}

	in.close();

	fineMatrix(testMat);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#ifdef stand
	gauss(testMat);
#else
	gauss_thread(testMat);
#endif
	 
	 
	//printf("Init matrix[3][3]\n");
	//displayMatrix(testMat);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	int z = 0, rank = 0;

	writeMatrix(testMat);
	calcRank(z, rank, testMat);
	
	/*ofstream out;
	out.open("./resul.txt");


	if (out.is_open()) {
		out << rank << " " << timeDiff;
	}

	out.close();*/


	//printf("Result:\n");
	//displayMatrix(testMat);

	printf("Matrix[%d][%d] rank = %d\n[RUNTIME]: %dms", n, m, rank, timeDiff);
}

//
//int main()
//{
//
//	//int matrix[][C] = {
//	//    {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12} // 3 : 3
//	//};
//
//	//int matrix_v2[][C] = {
//	//    {11, 2, 3, 4}, {1, 22, 3, 4}, {1, 123, 3, 4}, {1, 2, 3, 41 } // 4 : 3
//	//};
//
//	//int matrix_v3[][C] = {
//	//     {3, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 }, {2, 2, -1, -5} // 3 : 
//	//};
//
//	/*double testMat[n][m] = {
//	    {2., 1.0, 3.0, 0.0}, {1.0, 4.0, 2.0, 1.0}, {3.0, 2.0, 2.0, 6.0}, {8.0, 5.0, 7.0, 12.0}
//	};*/
//
//	/*int testMat[n][m] = {
//	    {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12}
//	};*/
//
//	 
//
//
//	/*auto testMat = new double[n][m];
//
//	fstream in;
//	in.open("./arr100.txt");
//
//	if (in.is_open()) {
//		for (int i = 0; i < n; i++) {
//			for (int j = 0; j < m; j++) {
//				in >> testMat[i][j];
//			}
//		}
//	}
//
//	in.close();*/
//
//	//auto testMat = new double[n][m];
//
//	/*ofstream out;
//	out.open("./arr100.txt");
//
//	random_device rd;
//	mt19937 gen(rd());
//	uniform_int_distribution<> distr(1, 10);
//
//	if (out.is_open()) {
//		for (int i = 0; i < n; i++) {
//			for (int j = 0; j < m; j++) {
//				out << distr(gen) << " ";
//			}
//			out << "\n";
//		}
//	}*/
//
//	//int R, rank = 0, z = 0;
//
//
//	//displayMatrix(testMat);
//
//	// OK
//	//double testMat[n][m] = {
//	//	{11, 2, 3, 4}, {1, 22, 3, 4}, {1, 123, 3, 4}, {1, 2, 3, 41 } // 4 :4
//	//};
//
//	//double testMat[n][m] = {
//	//	{1, 2, 3}, {2, 4, 5}, {7, 8, 9} // 4 :4
//	//};
//
//	//double testMat[n][m] = {
//	//	{-2, 3, 1, 0, -4}, {0, 0, 0, 5, 6}, {4, -11, -5, 12, 18}, {-9, 6, 0, -2, 21}, {-5, 5, 1, 1, 1} // 4 :4
//	//};
//	
//	// OK
//	//double testMat[n][m] = {
//	//	{-2, 3, 1, 0, -4}, {0, 0, 0, 5, 6}, {4, -11, -5, 12, 18}, {-9, 6, 0, -2, 21}, {-5, 5, 1, 1, 1} // 4 :4
//	//};
//
//	double testMat[n][m] = {
//		{1, 1, 1}, {1, 2, 3}, {1, 4, 9} // 4 :4
//	};
//
//	//double testMat[n][m] = {
//	//	{11.0, -13.0, 61.0, 10.0, -11.0}, {2, -2, 11, 2, -2}, {-3, 5, -17, -2, 3}, {4, 0, 24, 7, -8} // 4 :4
//	//};
//
//	//displayMatrix(testMat);
//
//	
//
//	//double testMat[n][m] = {
//	//	{1, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 } // 3 : 3
//	//};
//
//	//double testmat[n][m] = {
//	//	 {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12} // 3 : 4
//	//};
//
//	//double testMat[n][m] = {
//	//	{3, 7, -1, -2 }, {7, 1, -2, -1 }, { 11, 1, -3, 0 }, {2, 2, -1, -5} // 3 : 4
//	//};
//	//
//
//	/*double testMat[n][m] = {
//		{1, 7, -1, -2 },
//		{1, 1, -3, 10 },
//		{2, 2, -1, -5},
//		{1, 4, 2, 1},
//		{3, 7, -1, -2 },
//		{5, 4, 2, 1},
//		{7, 1, -2, -1},
//		{1, 5, 1, 1},
//		{10, 5, -2, -1},
//		{12, 1, -2, 10}
//	};*/
//
//	//displayMatrix(testMat);
//
//	fineMatrix(testMat);
//
//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//	//calcRankStandart(rank, z, testMat);
//	
//
//	/*double test[n][m] = {
//		{1, 2, 3, 4},
//		{0, 2, 1, 5},
//		{0, 5, 6, 7},
//		{11,5, 6, 7},
//		{0, 1, 2, 3}
//	};
//
//	displayMatrix(test);
//	bubbleSort(test);
//
//	printf("Sorted\n");
//	displayMatrix(test);*/
//	
//	//gauss_thread(testMat);
//
//	//displayMatrix(testMat);
//
//	//int rank = gauss_thread(testMat);
//	//calcRankThreads(testMat);
//
//	//calcRankStandart(rank, z, testMat);
//
//	//calcRankThreadsImproved(rank, z, testMat);
//	 
//	printf("Init matrix[3][3]\n");
//	displayMatrix(testMat);
//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//
//	auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
//
//	int z = 0, rank = 0;
//
//	gauss(testMat);
//	calcRank(z, rank, testMat);
//	
//	ofstream out;
//	out.open("./out100Th.txt");
//	
//	if (out.is_open()) {
//		for (int i = 0; i < n; i++) {
//			for (int j = 0; j < m; j++) {
//				out << testMat[i][j] << " ";
//			}
//			out << "\n";
//		}
//	}
//	printf("Result:\n");
//	displayMatrix(testMat);
//
//	//printf("Matrix[%d][%d] rank = %d\n[THREADS]: %d\n[RUNTIME]: %dms", n, m, 531, maxThreads, timeDiff);
//	printf("Matrix[%d][%d] rank = %d\n[RUNTIME]: %dms", n, m, rank, timeDiff);
//}
