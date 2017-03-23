#include "simplex.h"
using namespace std;


DualSimplex::DualSimplex(const DualSimplex &oldSimplex, int varIndex, int b, bool less)
{
	num_row = oldSimplex.num_row + 1;
	num_variable = oldSimplex.num_variable;
	num_col = oldSimplex.num_col + 1; // b + variable + relaxation

	//申请空间
	matrix = new double*[num_row];
	for (int i = 0; i < num_row; i++)
	{
		matrix[i] = new double[num_col];
	}

	//把父问题的矩阵复制过来
	for (int i = 0; i < oldSimplex.num_row; i++)
	{
		for (int j = 0; j < oldSimplex.num_col; j++)
		{
			matrix[i][j] = oldSimplex.matrix[i][j];
		}
	}
	//添加约束和松弛变量
	int lastRow = num_row - 1;
	int lastCol = num_col - 1;
	// last col is 0
	for (int i = 0; i < lastRow; i++)
	{
		matrix[i][lastCol] = 0;//relax
	}
	// last row is 0
	for (int i = 0; i < lastCol; i++)
	{
		matrix[lastRow][i] = 0;
	}

	matrix[lastRow][0] = less ? b : -b;
	matrix[lastRow][varIndex + 1] = less ? 1 : -1;
	matrix[lastRow][lastCol] = 1; //relax

	//把基变量编号复制过来
	bv = oldSimplex.bv;
	bv.push_back(num_col - 2);

	//基变量冲突吗？
	int totalBv = bv.size();
	for (int i = 0; i < totalBv; i++)
	{
		if (bv[i] == varIndex)
		{
			//有冲突  把约束的相应变量消除
			if (less)
			for (int j = 0; j < num_col; j++)
				matrix[lastRow][j] -= matrix[i + 1][j];
			else
			for (int j = 0; j < num_col; j++)
				matrix[lastRow][j] += matrix[i + 1][j];
			break;
		}
	}

	//outputMatrix();
}




void DualSimplex::setMatrix(int row, int col, double **data)
{
	clear();
	num_row = row;
	num_variable = col - 1;

	num_col = 1 + num_variable + num_row - 1; //b + variable + relaxation
	cout << "reading from data matrix: " << num_row << ", col:" << col << endl;

	matrix = new double*[num_row];
	for (int i = 0; i < num_row; i++)
	{
		// read b and variables
		matrix[i] = new double[num_col];
		for (int j = 0; j < col; j++)
		{
			matrix[i][j] = data[i][j];
		}
	}
	//设置基变量
	for (int i = num_variable; i < num_col - 1; i++)
	{
		bv.push_back(i);
	}
	// add relexation
	addRelaxVars();
}

void DualSimplex::readMatrix(int row, int col, std::ifstream & file)
{
	num_row = row;
	num_variable = col - 1;

	num_col = 1 + num_variable + num_row - 1; // b + variable + relaxation
	cout << "reading from file row: " << row << ", col:" << col << endl;

	matrix = new double*[num_row];
	//matrix = (double**)malloc(num_col*num_row*sizeof(double));
	for (int i = 0; i < num_row; i++)
	{
		// read b and variables
		matrix[i] = new double[num_col];
		for (int j = 0; j < col; j++)
		{
			file >> matrix[i][j];
		}
	}

	// add relexation
	addRelaxVars();
}

void DualSimplex::outputMatrix(std::ofstream & file)
{
	for (int i = 0; i < num_row; i++)
	{
		// read b and variables
		for (int j = 0; j < num_col; j++)
		{
			file << matrix[i][j] << "\t";
		}
		file << endl;
	}

}

void DualSimplex::outputMatrix()
{
	for (int i = 0; i < num_row; i++)
	{
		// read b and variables
		for (int j = 0; j < num_col; j++)
		{
			cout << matrix[i][j] << "\t";
		}
		cout << endl;
	}
}

// add relax variables for less constraint
void DualSimplex::addRelaxVars()
{
	// the first row for the relaxation is 0
	for (int j = 1; j < num_row; j++)
	{
		matrix[0][num_variable + j] = 0;
	}

	// num_relaxation + num_variable + 1 = num_col (num_row = num_relaxation + 1)
	// from the second row to last row: add relaxation (the first row is objective, all constraints is less)
	for (int i = 1; i < num_row; i++)
	{
		// from the column with index num_variable + 1, to num_col: add relaxation (the first colunm is b)
		for (int j = 1; j < num_row; j++)
		{
			matrix[i][num_variable + j] = (i == j);
		}
	}
}

// find the row to pivot (most negative row)
int DualSimplex::findPivotRow()
{
	// find the most negative one in b
	double lowest = 0;
	int pivot_row = -1;
	for (int i = 1; i < num_row; i++)
	{
		if (matrix[i][0] < lowest)
		{
			lowest = matrix[i][0];
			pivot_row = i;
		}
	}

	// promption
	if (pivot_row == -1)
	{
		//cout << "can not find pivot row" << endl;
	}
	else
	{
		//cout << "pivot row is " << pivot_row << " and its b value is " << lowest << endl;
	}

	return pivot_row;
}

// find the column to pivot (ratio with least abs value)
int DualSimplex::findPivotCol(int row)
{
	double largest = -INFINITY;
	int pivot_col = -1;
	double now_ratio = 0;
	// the first col is b
	for (int j = 1; j < num_col; j++)
	{
		// try to make b to be positive, so choose the negative col && the first row must be non-negative
		if (matrix[row][j] >= 0)
			continue;

		now_ratio = matrix[0][j] / matrix[row][j];
		// choose the negative col with min abs ratio, max negative
		if (now_ratio > largest)
		{
			largest = now_ratio;
			pivot_col = j;
		}
	}

	// promption
	if (pivot_col == -1) // no negative variable
	{
		//cout << "can not find pivot col for row " << row << endl;
	}
	else
	{
		//cout << "pivot col is " << pivot_col << " and its ratio value is " << largest << endl;
	}

	return pivot_col;
}

// execute pivotion
void DualSimplex::pivot(int row, int col)
{
	bv[row - 1] = col - 1;
	double cofficiency = matrix[row][col];

	// cast the cofficiency of pivot into 1
	for (int j = 0; j < num_col; j++)
		matrix[row][j] /= cofficiency;

	// cast cofficiency of pivot of the remained row into 0
	double multiplier;
	for (int i = 0; i < num_row; i++) {
		if (i == row)
			continue;
		// update each row 
		multiplier = matrix[i][col];
		for (int j = 0; j < num_col; j++)
		{
			matrix[i][j] -= multiplier * matrix[row][j];
		}
	}
}

// get optimization variables and value
bool DualSimplex::getOptimization(double &best, vector<double>& variableValue)
{
	variableValue.clear();
	for (int i = 1; i < num_col; i++)
	{
		variableValue.push_back(0);
	}
	int totalBv = bv.size();
	for (int i = 0; i < totalBv; i++)
	{
		int bvNum = bv[i];
		variableValue[bvNum] = matrix[i + 1][0];
	}
	best = matrix[0][0];

	return true;
}

//  execute solve (after readMatrix or setMatrix)
bool DualSimplex::solveMinProblemWithDual(double &best, vector<double>& variableValue)
{
	// iteration at most loop times
	int pivotRow, pivotCol;

	while (1)
	{
		//step1: find pivot row
		pivotRow = findPivotRow();
		// no negative b value, already the best solution
		if (pivotRow == -1)
		{
			// get optimized solution
			getOptimization(best, variableValue);
			return true;
		}

		pivotCol = findPivotCol(pivotRow);
		// exist some row that can never have positive b value
		if (pivotCol == -1)
			return false;

		//step2 do pivot
		pivot(pivotRow, pivotCol);
		//outputMatrix();

		//step3 判断是否可行
		/*for (int i = 1; i < num_row; i++)
		{
		if (matrix[i][0] < 0)// b 为负数
		{
		bool pos = true;
		for (int j = 1; j < num_col; j++)
		{
		if (matrix[i][j] < 0)
		{
		pos = false;
		break;
		}
		}
		if (pos)//变量系数全为>=0
		return false;
		}
		}*/
	}
}