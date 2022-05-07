#include <iostream>
#include <vector>
#include <utility>

using namespace std;


const double EPS = 1E-9;

void displayMatrix(vector<vector<double>> matrix) {

    for (vector<vector<double>>::iterator row = matrix.begin(); row != matrix.end(); ++row) {
        for (vector<double>::iterator col = row->begin(); col != row->end(); ++col) {
            cout << *col << " ";
        }
        cout << endl;
    }
}

int compute_rank(vector<vector<double>> A) {
    int n = A.size();
    int m = A[0].size();

    printf("M: %d N: %d\n", m, n);

    int rank = 0;
    vector<bool> row_selected(n, false);

    for (int i = 0; i < m; ++i) {
        int j;
        for (j = 0; j < n; ++j) {
            if (!row_selected[j] && abs(A[j][i]) > EPS)
                break;
        }

        if (j != n) {
            ++rank;
            row_selected[j] = true;
            for (int p = i + 1; p < m; ++p)
                A[j][p] /= A[j][i];
            for (int k = 0; k < n; ++k) {
                if (k != j && abs(A[k][i]) > EPS) {
                    for (int p = i + 1; p < m; ++p)
                        A[k][p] -= A[j][p] * A[k][i];
                }
            }
        }
    }
    displayMatrix(A);
    return rank;
}

int factorial(int i) {
    if (i == 0) return 1;
    else return i * factorial(i - 1);
}

/// <summary>
/// Возвращает количество миноров k-порядка
/// </summary>
/// <param name="n"></param>
/// <param name="p"></param>
/// <param name="k"></param>
/// <returns></returns>
int getMinors(int n, int p, int k) {
    int cP, cN;

    cP = (factorial(p)) / (factorial(k) * factorial(p - k));
    cN = (factorial(p)) / (factorial(n) * factorial(p - n));

    return cP * cN;
}


pair<int, int> getMatrixSize(vector<vector<double>> matrix) {
    
    pair <int, int> matrixSize (0, 0);
    
    for (vector<vector<double>>::iterator row = matrix.begin(); row != matrix.end(); ++row) {

        matrixSize.first++;

        for (vector<double>::iterator col = row->begin(); col != row->end(); ++col) {
            if(matrixSize.first == 1) {
                matrixSize.second++;
            }
            else {
                break;
            }
        }
    }

    return matrixSize;

}

vector<vector<double>> calcMinors(vector<vector<double>> A, int k, int maxMinors) {
    int maxShift = k;
    int startRows = 0;
    int startCols = 0;

    vector<vector<vector<double>>> result;

    for (vector<vector<double>>::iterator row = matrix.begin(); row != matrix.end(); row += maxShift) {
        vector<vector<double>> subMatrix;

        for (vector<double>::iterator col = row->begin(); col != row->end(); col += maxShift) {
            cout << *col << " ";
        }
        cout << endl;
    }
}

int calcRank(vector<vector<double>> A, int n, int p) {
    int k = 2;
    int kMax = 3;
    int minor;

    cout << n << " " << p << endl;


    for (int i = k; i <= kMax; i++) {
        minor = getMinors(n, p, i);

    }

    return k;
}

int main()
{
    //n is inner vector
    vector<vector<double>> matrix = { {11, 2, 3, 4}, {1, 22, 3, 4}, {1, 123, 3, 4}, {1, 2, 3, 41 } };

    vector<vector<double>> matrixGauss = { {2, 1, 3, 0}, {1, 4, 2, 1}, {3, 2, 2, 6}, {8, 5, 7, 12} };

    vector<vector<double>> matrixMinors = { {3, 7, -1, -2, 8 }, {7, 1, -2, -1, 12}, { 11, 1, -3, 0, 16}, {2, 2, -1, -5, 12} };

    vector<vector<double>> matrixMinors_v2 = { {3, 7, -1, -2 }, {7, 1, -2, -1}, { 11, 1, -3, 0} };

    //vector<vector<double>> matrixXZ = { {1, 1, 1, 1, 0 }, {1, 2, 1, 1, 0}, { 1, 1, 3, 1, 0 }, {1, 1, 1, 4, 0}, {1, 2, 3, 4, 0 } };
    displayMatrix(matrixMinors);

    pair <int, int> matrixSize(getMatrixSize(matrixMinors));

    calcRank(matrixMinors, matrixSize.first, matrixSize.second);


    //std::cout << "rank: " << rank << std::endl;
}