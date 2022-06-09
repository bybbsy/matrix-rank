using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace matrix_cs
{
    class Program
    {
		const int n = 300;
		const int m = 50;

		private Mutex mutex = new Mutex();

		const int maxThreads = 2;

        public static void displayMatrix(double[,] mat)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    Console.Write(String.Format(" ${0} ", mat[i, j]));
                }
                Console.WriteLine("\n");
            }
        }

        public static int getLeadItem(int q, double[,] mat)
		{
			for (int j = 0; j < m; j++)
			{
				if (mat[q, j] != 0)
				{
					return j;
				}
			}

			return 0;
		}

		public static void replaceRows(double[,] copy)
		{
			for (int i = 1; i < n; i++)
			{
				if (i < n - 1)
				{
					for (int j = 0; j < m; j++)
					{
						//cout << "Here" << endl;
						if (copy[i, j] == 0 && copy[i + 1, j] != 0)
						{
							//printf("Rows");
							for (int q = i; q < i + 1; q++)
							{

								for (int b = 0; b < m; b++)
								{
									double old = copy[q, b];
									double newest = copy[q + 1, b];


									copy[q, b] = newest;
									copy[q + 1, b] = old;
								}
							}
						}
					}
				}
			}
		}

		public static void bubbleSort(double[,] mat)
		{
			for (int i = 1; i + 1 < n; i++)
			{
				for (int q = i + 1; q < n; q++)
				{
					for (int j = 0; j < m; j++)
					{
						if (mat[i, j] == 0 && mat[q, j] != 0)
						{

							for (int b = 0; b < m; b++)
							{
								double old = mat[i, b];
								double newest = mat[q, b];


								mat[i, b] = newest;
								mat[q, b] = old;
							}
						}

					}
				}
			}
		}

		public static int gauss(double[,] copy)
		{
			//displayMatrix(copy);
			// Преобразовывает первую строку
			if (copy[0, 0] > 1 || copy[0, 0] < -1)
			{
				double init = copy[0, 0];

				for (int i = 1; i < n; i++)
				{
					if (copy[0, 0] > copy[i, 0])
					{
						double mul = Math.Floor(copy[0, 0] / copy[i, 0]);

						double firstValue = copy[0, 0] - mul * copy[i, 0];

						//cout << "Less" << int(copy[0][0] / copy[i][0]) << endl;

						if (firstValue > 0 && firstValue < copy[0, 0])
						{
							for (int j = 0; j < m; j++)
							{
								copy[0, j] = copy[0, j] - mul * copy[i, j];
							}
						}
						break;
					}	

					// Работает только если значение mat[0][0] = -1
					if (Convert.ToInt32(copy[i, 0]) % Convert.ToInt32(copy[0, 0]) == -1 && copy[0, 0] > copy[i, 0])
					{
						int mul = Convert.ToInt32(copy[i, 0] / copy[0, 0]);
						for (int j = 0; j < m; j++)
						{
							copy[0, j] = mul * copy[0, j] - copy[i, j];
						}
						break;
					}
				}

				if (init == copy[0, 0])
				{
					for (int j = 0; j < m; j++)
					{
						copy[0, j] = copy[0, j] / init;
					}
				}
			}

			// Перемещает строку
			replaceRows(copy);


			// Первый основной проход по рядам
			for (int i = 0; i < n; i++)
			{
				int k = getLeadItem(i, copy);

				//printf("ROW: %d LEAD: %d\n", i, k);
				// Сравнивает текущий ряд с нижними
				//Console.WriteLine(i);
				for (int q = i + 1; q < n; q++)
				{
					// Проходит по строкам
					for (int j = 0; j < m; j++)
					{
						// Если текущий элемент равен наименьшему ведущему
						if (j == k && copy[q, k] != 0)
						{
							double div = Math.Round((copy[q, k] / copy[i, k]), 2);

							for (int h = k; h < m; h++)
							{
								double res = Math.Round((copy[q, h] - div * copy[i, h]) , 2);


								copy[q, h] = res;
							}
						}

					}

				}
				bubbleSort(copy);
			}

			//displayMatrix(copy);
			int R, z = 0;

			for (int i = 0; i < n; i++)
			{
				int c = 0;
				for (int j = 0; j < m; j++)
				{
					if (copy[i, j] == 0)
					{
						c++;
					}
				}
				if (c == m)
				{
					z++;
				}
			}

			//displayMatrix(copy);
			R = n - z;

			return R;
		}


		public void gauss_calc_rows(int i, int k, int t, int chunk, int remainder, double[,] copy)
		{
            //this.mutex.WaitOne();

            double div = 0, res = 0;
			int from = 0, to = 0;

            //Console.WriteLine(String.Format("${0} ${1} ${2}", i, k, t));
            //mut.lock();
            //displayMatrix(copy);
            //printf("[i]:%d [chunk]:%d [TH]:%d", i, chunk, t);
            if (remainder > 0)
			{
				chunk += remainder;
				from = t * chunk + i + 1;
				to = t * (chunk) + chunk + i;

				if (to + 1 < n)
				{
					to++;
				}

				for (int q = from; q < to; q++)
				{
					//Console.WriteLine(from);
					//printf("> 0 THREAD:%d CHUNK: FROM: %d, TO: %d\n", t, from, to);
					// Проходит по строкам
					for (int j = 0; j < m; j++)
					{

						// Если текущий элемент равен наименьшему ведущему
						if (j == k && copy[q, k] != 0)
						{

							div = Math.Round((copy[q, k] / copy[i, k]), 2);

							for (int h = k; h < m; h++)
							{
								res = Math.Round((copy[q, h] - div * copy[i, h]), 2);

								//printf("%g - %g * %g = %g\n", copy[q][h], div, copy[i][h], res);

								copy[q, h] = res;
							}
						}
					}
				}
			}
			else
			{

				from = t * chunk + i + 1; 
				to = t * chunk + chunk + i + 1;
				for (int q = from; q < to; q++)
				{
					//printf("= 0 THREAD:%d CHUNK: FROM: %d, TO: %d\n",t, from, to);
					// Проходит по строкам
					for (int j = 0; j < m; j++)
					{

						// Если текущий элемент равен наименьшему ведущему
						if (j == k && copy[q, k] != 0)
						{

							div = Math.Round((copy[q, k] / copy[i, k]), 2);
							//printf("%d", div);
							for (int h = k; h < m; h++)
							{
								res = Math.Round((copy[q, h] - div * copy[i, h]), 2);

								//printf("%g - %g * %g = %g\n", copy[q][h], div, copy[i][h], res);

								copy[q, h] = res;
							}
						}
					}
				}
			}

			//Console.WriteLine("Inside");
			//displayMatrix(copy);
			//Console.WriteLine("");

            //this.mutex.ReleaseMutex();
            //mut.unlock();
        }

		public int gauss_thread(double[,] copy)
		{
			//List<Task> tasks = new List<Task>();

			if (copy[0, 0] > 1 || copy[0, 0] < -1)
			{
				double init = copy[0, 0];

				for (int i = 1; i < n; i++)
				{
					if (copy[0, 0] > copy[i, 0])
					{
						double mul = Math.Floor(copy[0, 0] / copy[i, 0]);

						double firstValue = copy[0, 0] - mul * copy[i, 0];

						//cout << "Less" << int(copy[0][0] / copy[i][0]) << endl;

						if (firstValue > 0 && firstValue < copy[0, 0])
						{
							for (int j = 0; j < m; j++)
							{
								copy[0, j] = copy[0, j] - mul * copy[i, j];
							}
						}
						break;
					}

					// Работает только если значение mat[0][0] = -1
					if (Convert.ToInt32(copy[i, 0]) % Convert.ToInt32(copy[0, 0]) == -1 && copy[0, 0] > copy[i, 0])
					{
						int mul = Convert.ToInt32(copy[i, 0] / copy[0, 0]);
						for (int j = 0; j < m; j++)
						{
							copy[0, j] = mul * copy[0, j] - copy[i, j];
						}
						break;
					}
				}

				if (init == copy[0, 0])
				{
					for (int j = 0; j < m; j++)
					{
						copy[0, j] = copy[0, j] / init;
					}
				}
			}




			// Перемещает строку
			replaceRows(copy);

			int chunk, remainder;





			

			// Первый основной проход по рядам
			for (int i = 0; i < n; i++)
			{
				int k = getLeadItem(i, copy);

				chunk = (n - i - 1) / maxThreads;
				remainder = (n - i - 1) % maxThreads;

				//printf("ROW: %d LEAD: %d\n", i, k);
				// Сравнивает текущий ряд с нижними

				System.Threading.Tasks.Parallel.For(0, maxThreads, x =>
				{
					gauss_calc_rows(i, k, x, chunk, remainder, copy);
				});

				//for (int t = 0; t < maxThreads; t++)
				//{
				//    System.Threading.Tasks.Parallel.For(0, maxThreads, x =>
				//    {
				//        gauss_calc_rows(i, k, t, chunk, remainder, copy);
				//    });
				//    //ts.RunSynchronously();
				//}

				//System.Threading.Tasks.Parallel.For(0, maxThreads, x =>
				//{
				//	gauss_calc_rows(i, k, x, chunk, remainder, copy);
				//});

				// Работает
				//for(int t = 0; t < maxThreads; t++)
				//            {
				//	Action a = () => gauss_calc_rows(i, k, t, chunk, remainder, copy);
				//                //a.Invoke();

				//                Console.WriteLine(i);
				//}

				//tasks.Add(Task.Factory.StartNew(() => gauss_calc_rows(i, k, i, chunk, remainder, copy)));

				//actions.Add(a);

				//a.Invoke();
				//if (i + 1 < n)
				//            {
				//	Action a_2 = () => gauss_calc_rows(i, k, i + 1, chunk, remainder, copy);
				//	actions.Add(a_2);
				//	a_2.Invoke();
				//}



				//for (int t = 0; t < maxThreads; t++)
				//{
				//                //Thread th = new Thread(new ThreadStart(() => gauss_calc_rows(i, k, t, chunk, remainder, copy)));
				//                tasks[i] = Task.Run(() => gauss_calc_rows(i, k, t, chunk, remainder, copy));
				//                //threads.push_back(thread(gauss_calc_rows, i, k, t, chunk, remainder, copy));
				//                //th.Start();
				//}


				//for (auto & th : threads)
				//{
				//	th.join();
				//}



				//threads.clear();

				//mutex.WaitOne();
				bubbleSort(copy);
                //mutex.ReleaseMutex();
            }


			//Task.WaitAll(tasks.ToArray());
			//Task.WaitAll(tasks.ToArray());




			//Console.WriteLine("Result");
			//displayMatrix(copy);
			//Console.WriteLine("");

			int R, z = 0;

			for (int i = 0; i < n; i++)
			{
				int c = 0;
				for (int j = 0; j < m; j++)
				{
					if (copy[i, j] == 0)
					{
						c++;
					}
				}
				if (c == m)
				{
					z++;
				}
			}

			R = n - z;

			return R;
		}


		void run()
        {
            String file = File.ReadAllText("./arr100.txt");

            double[,] mat = new double[n, m];

            int i = 0, j = 0;

            foreach (var row in file.Split('\n'))
            {

                j = 0;
                foreach (var col in row.Trim().Split(' '))
                {
                    if (i < n && j < m)
                    {
                        mat[i, j] = int.Parse(col.Trim());
                    }
                    j++;
                }
                i++;
            }

			//Console.WriteLine("Base");
			//displayMatrix(mat);
			//Console.WriteLine("");
			var watch = System.Diagnostics.Stopwatch.StartNew();

			int rank = gauss_thread(mat);

			watch.Stop();

			var ms = watch.ElapsedMilliseconds;

			String msg = String.Format("Rank: ${0}\n runtime: ${1} ms", rank, ms);
            Console.WriteLine(msg);


        }

		static void Main(string[] args)
        {
			Program p = new Program();
			p.run();
        }
    }
}
