package main

import (
	"fmt"
	"math"
	"sync"
	"time"
)

const n = 10
const m = 4
const maxThreads = 2

var mut sync.Mutex

var mat = [][]float64{
	{1.0, 7.0, -1.0, -2.0},
	{1.0, 1.0, -3.0, 10.0},
	{2.0, 2.0, -1.0, -5.0},
	{1.0, 4.0, 2.0, 1.0},
	{3.0, 7.0, -1.0, -2.0},
	{5.0, 4.0, 2.0, 1.0},
	{7.0, 1.0, -2.0, -1.0},
	{1.0, 5.0, 1.0, 1.0},
	{10.0, 5.0, -2.0, -1.0},
	{12.0, 1.0, -2.0, 10.0}}

// var mat [][]float64

func displayMatrix(mat *[][]float64) {
	for i := 0; i < n; i++ {
		for j := 0; j < m; j++ {
			fmt.Printf("%.2f ", (*mat)[i][j])
		}
		fmt.Printf("\n")
	}
}

func replaceRows(copy *[][]float64) {
	for i := 1; i < n; i++ {
		if i < n-1 {
			for j := 0; j < m; j++ {
				//cout << "Here" << endl;
				if (*copy)[i][j] == 0 && (*copy)[i+1][j] != 0 {
					//printf("Rows");
					for q := i; q < i+1; q++ {

						for b := 0; b < m; b++ {
							old := (*copy)[q][b]
							newest := (*copy)[q+1][b]

							(*copy)[q][b] = newest
							(*copy)[q+1][b] = old
						}
					}
				}
			}
		}
	}
}

func bubbleSort(mat *[][]float64) {
	for i := 1; i+1 < n; i++ {
		for q := i + 1; q < n; q++ {
			for j := 0; j < m; j++ {
				if (*mat)[i][j] == 0 && (*mat)[q][j] != 0 {

					for b := 0; b < m; b++ {
						old := (*mat)[i][b]
						newest := (*mat)[q][b]

						(*mat)[i][b] = newest
						(*mat)[q][b] = old
					}
				}

			}
		}
	}
}

func getRank(copy *[][]float64) int {
	R := 0
	z := 0

	for i := 0; i < n; i++ {
		c := 0
		for j := 0; j < m; j++ {
			if (*copy)[i][j] == 0 {
				c++
			}
		}
		if c == m {
			z++
		}
	}

	R = n - z

	return R
}

func getLeadItem(q int, mat *[][]float64) int {
	for j := 0; j < m; j++ {
		if (*mat)[q][j] != 0 {
			return j
		}
	}

	return 0
}

func gauss(copy *[][]float64) int {

	if (*copy)[0][0] > 1 || (*copy)[0][0] < -1 {
		init := (*copy)[0][0]

		for i := 1; i < n; i++ {
			if (*copy)[0][0] > (*copy)[i][0] {
				mul := math.Floor((float64((*copy)[0][0])/float64((*copy)[i][0]))*100) / 100

				firstValue := float64((*copy)[0][0]) - float64(mul)*float64((*copy)[i][0])

				if firstValue > 0 && firstValue < (*copy)[0][0] {
					for j := 0; j < m; j++ {
						(*copy)[0][j] = math.Round((float64((*copy)[0][j])-float64(mul)*float64((*copy)[i][j]))*100) / 100
					}
				}
				break
			}

			// Работает только если значение (*mat)[0][0] = -1
			if int((*copy)[i][0])%int((*copy)[0][0]) == -1 && (*copy)[0][0] > (*copy)[i][0] {
				mul := (*copy)[i][0] / (*copy)[0][0]
				for j := 0; j < m; j++ {
					(*copy)[0][j] = math.Round(((float64(mul)*float64((*copy)[0][j]) - float64((*copy)[i][j])) * 100) / 100)
				}
				break
			}
		}

		if init == (*copy)[0][0] {
			for j := 0; j < m; j++ {
				(*copy)[0][j] = math.Round((float64((*copy)[0][j])/float64(init))*100) / 100
			}
		}
	}

	replaceRows(copy)
	for i := 0; i < n; i++ {
		k := getLeadItem(i, copy)
		for q := i + 1; q < n; q++ {
			for j := 0; j < m; j++ {
				if j == k && (*copy)[q][k] != 0 {
					div := math.Round((float64((*copy)[q][k])/float64((*copy)[i][k]))*100) / 100

					// fmt.Printf("%g", div)
					for h := k; h < m; h++ {
						res := math.Round((float64((*copy)[q][h])-float64(div)*float64((*copy)[i][h]))*100) / 100
						(*copy)[q][h] = float64(res)
					}
				}
			}
		}

		bubbleSort(copy)
	}

	//displayMatrix(copy);
	R := 0
	z := 0

	for i := 0; i < n; i++ {
		c := 0
		for j := 0; j < m; j++ {
			if (*copy)[i][j] == 0 {
				c++
			}
		}
		if c == m {
			z++
		}
	}

	R = n - z

	return R
}

func gauss_thread(copy *[][]float64, wg *sync.WaitGroup) {
	// mut.Lock()

	if (*copy)[0][0] > 1 || (*copy)[0][0] < -1 {
		init := (*copy)[0][0]

		for i := 1; i < n; i++ {
			if (*copy)[0][0] > (*copy)[i][0] {
				mul := math.Floor((float64((*copy)[0][0])/float64((*copy)[i][0]))*100) / 100

				firstValue := float64((*copy)[0][0]) - float64(mul)*float64((*copy)[i][0])

				if firstValue > 0 && firstValue < (*copy)[0][0] {
					for j := 0; j < m; j++ {
						(*copy)[0][j] = math.Round((float64((*copy)[0][j])-float64(mul)*float64((*copy)[i][j]))*100) / 100
					}
				}
				break
			}

			// Работает только если значение (*mat)[0][0] = -1
			if int((*copy)[i][0])%int((*copy)[0][0]) == -1 && (*copy)[0][0] > (*copy)[i][0] {
				mul := (*copy)[i][0] / (*copy)[0][0]
				for j := 0; j < m; j++ {
					(*copy)[0][j] = math.Round(((float64(mul)*float64((*copy)[0][j]) - float64((*copy)[i][j])) * 100) / 100)
				}
				break
			}
		}

		if init == (*copy)[0][0] {
			for j := 0; j < m; j++ {
				(*copy)[0][j] = math.Round((float64((*copy)[0][j])/float64(init))*100) / 100
			}
		}
	}

	replaceRows(copy)

	for i := 0; i < n; i++ {
		k := getLeadItem(i, copy)

		chunk := (n - i - 1) / maxThreads
		remainder := (n - i - 1) % maxThreads

		wg.Add(2)
		for t := 0; t < maxThreads; t++ {

			go gauss_calc_rows(i, k, t, chunk, remainder, copy, wg)
			// go gauss_calc_rows(i, k, 1, chunk, remainder, copy, wg)
		}

		wg.Wait()
		bubbleSort(copy)
	}

	// fmt.Printf("\nBan%g\n", (*copy)[2][1])

}

func gauss_calc_rows(i int, k int, t int, chunk int, remainder int, copy *[][]float64, wg *sync.WaitGroup) {
	defer wg.Done()
	if remainder > 0 {
		chunk += remainder
		from := t*chunk + i + 1
		to := t*chunk + chunk + i

		fmt.Printf("From: %g To: %g\n", from, to)

		if to+1 < n {
			to++
		}

		for q := from; q < to; q++ {
			for j := 0; j < m; j++ {
				if j == k && (*copy)[q][k] != 0 {
					div := math.Round((float64((*copy)[q][k])/float64((*copy)[i][k]))*100) / 100

					for h := k; h < m; h++ {
						res := math.Round((float64((*copy)[q][h])-float64(div)*float64((*copy)[i][h]))*100) / 100
						// fmt.Printf("Res: %g\n", div)
						(*copy)[q][h] = float64(res)
					}
				}
			}
		}
	} else {
		from := t*chunk + i + 1
		to := t*chunk + chunk + i
		fmt.Printf("From: %g To: %g\n", from, to)
		for q := from; q < to; q++ {
			for j := 0; j < m; j++ {
				if j == k && (*copy)[q][k] != 0 {
					div := math.Round((float64((*copy)[q][k])/float64((*copy)[i][k]))*100) / 100

					// fmt.Printf("%g", div)
					for h := k; h < m; h++ {
						res := math.Round((float64((*copy)[q][h])-float64(div)*float64((*copy)[i][h]))*100) / 100
						(*copy)[q][h] = float64(res)
					}
				}
			}
		}
	}

}

func main() {
	// f, err := os.Open("arr100.txt")

	// if err != nil {
	// 	log.Fatal(err)
	// }

	// defer f.Close()

	// scanner := bufio.NewScanner(f)

	// var mat [][]float64

	// for scanner.Scan() {
	// 	records := strings.Fields(scanner.Text())
	// 	line := make([]float64, len(records))
	// 	mat = append(mat, line)
	// 	for i := range records {
	// 		line[i], err = strconv.ParseFloat(records[i], 64)
	// 		if err != nil {
	// 			panic(err)
	// 		}
	// 	}
	// }
	// if err = scanner.Err(); err != nil {
	// 	panic(err)
	// }

	// fmt.Println(mat[500][0])
	var wg sync.WaitGroup

	t := time.Now()
	gauss_thread(&mat, &wg)

	rank := getRank(&mat)

	// rank := gauss(&mat)
	displayMatrix(&mat)
	fmt.Printf("Rank: %d Runtime: %d\n", rank, time.Since(t).Milliseconds())
}
