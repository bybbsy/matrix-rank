package main

import (
	"fmt"
	"math"
)

const n = 10
const m = 4

func displayMatrix(mat[n][m] float64) {
	for i := 0; i < n; i++ {
		for j := 0; j < m; j++ {
			fmt.Sprintf("%.2f", mat[i][j])
		}
		fmt.Printf("\n")
	}
}

func replaceRows(copy[n][m] float64) {
	for i := 1; i < n; i++ {
		if (i < n - 1) {
			for j := 0; j < m; j++ {
				//cout << "Here" << endl;
				if (copy[i][j] == 0 && copy[i + 1][j] != 0) {
					//printf("Rows");
					for q := i; q < i + 1; q++ {

						for b := 0; b < m; b++ {
							old := copy[q][b];
							newest := copy[q + 1][b];


							copy[q][b] = newest;
							copy[q + 1][b] = old;
						}
					}
				}
			}
		}
	}
}

func bubbleSort(mat[n][m] float64) {
	for i := 1; i + 1 < n; i++ {
		for q := i + 1; q < n; q++ {
			for j := 0; j < m; j++ {
				if (mat[i][j] == 0 && mat[q][j] != 0) {

						for b := 0; b < m; b++ {
							old := mat[i][b];
							newest := mat[q][b];


							mat[i][b] = newest;
							mat[q][b] = old;
						}
					}
				
			}
		}
	}
}

func getLeadItem(q int, mat[n][m] float64) int {
	for j := 0; j < m; j++ {
		if(mat[q][j] != 0) {
			return j;
		}
	}

	return 0;
}


func gauss(copy[n][m] float64) int {
	 

	if (copy[0][0] > 1 || copy[0][0] < -1) {
		init := copy[0][0];

		for i := 1; i < n; i++ {
			if (copy[0][0] > copy[i][0]) {
				mul := math.Floor((float64(copy[0][0]) / float64(copy[i][0])) * 100 )/100;

				firstValue := float64(copy[0][0]) - float64(mul) * float64(copy[i][0]);
				
				if (firstValue > 0 && firstValue < copy[0][0]) {
					for j := 0; j < m; j++ {
						copy[0][j] = math.Round( (float64(copy[0][j]) - float64(mul) * float64(copy[i][j])) * 100 ) / 100;
					}
				}
				break;
			}

			// Работает только если значение mat[0][0] = -1
			if (int(copy[i][0]) % int(copy[0][0]) == -1 && copy[0][0] > copy[i][0]) {
				mul := copy[i][0] / copy[0][0];
				for j := 0; j < m; j++ {
					copy[0][j] = math.Round(((float64( mul) * float64(copy[0][j]) - float64(copy[i][j])) / 100 ) * 100 );
				}
				break;
			}
		}
		
		if (init == copy[0][0]) {
			for j := 0; j < m; j++ {
				copy[0][j] = math.Round((float64(copy[0][j]) / float64(init) / 100)) * 100;
			}
		}
	}

	replaceRows(copy);
	for i := 0; i < n; i++ {
		k := getLeadItem(i, copy);
		for q := i + 1; q < n; q++ {
			for j := 0; j < m; j++ {
				if (j == k && copy[q][k] != 0) {
					div := math.Round((float64(copy[q][k]) / float64(copy[i][k])) / 100) * 100;

					fmt.Printf("%g", div)
					for h := k; h < m; h++ {
						res := math.Round((float64(copy[q][h]) - float64(div) * float64(copy[i][h])) / 100 ) * 100;
						copy[q][h] = float64(res);
					}
				}
			}
		}

		bubbleSort(copy);
	}

	for i := 0; i < n; i++ {
		for j := 0; j < m; j++ {
			fmt.Printf("mat[%d][%d] = %d\n", i,j, copy[i][j])
		}
	}
	//displayMatrix(copy);
	R := 0
	z := 0;

	for i := 0; i < n; i++ {
		c := 0;
		for j := 0; j < m; j++ {
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


func main() {

	

	var mat = [n][m]float64{
		{1.0, 7.0, -1.0, -2.0 },
		{1.0, 1.0, -3.0, 10.0 },
		{2.0, 2.0, -1.0, -5.0},
		{1.0, 4.0, 2.0, 1.0},
		{3.0, 7.0, -1.0, -2.0 },
		{5.0, 4.0, 2.0, 1.0},
		{7.0, 1.0, -2.0, -1.0},
		{1.0, 5.0, 1.0, 1.0},
		{10.0, 5.0, -2.0, -1.0},
		{12.0, 1.0, -2.0, 10.0}}
	
	
	rank := gauss(mat);

	 
	
	fmt.Printf("%d", rank);
}