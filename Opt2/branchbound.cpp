
#include "branchbound.h"

BranchBound::BranchBound(int row, int col, std::ifstream &fin)
{
	this->row = row;
	this->col = col;
	std::cout << "reading from file row: " << row << ", col:" << col << std::endl;

	matrix = new double*[row];
	for (int i = 0; i < row; i++)
	{
		// read b and variables
		matrix[i] = new double[col];
		for (int j = 0; j < col; j++)
		{
			fin >> matrix[i][j];
		}
	}

	lowBound = -INFINITY;
}

bool BranchBound::solve(double &opt, std::vector<double> &vars)
{
	DualSimplex *simplex0 = new DualSimplex();
	simplex0->setMatrix(row, col, matrix);
	simplexs.push(simplex0);

	while (!simplexs.empty())
	{
		DualSimplex *curSimplex = simplexs.top();
		simplexs.pop();

		if (!curSimplex->solveMinProblemWithDual(targetVal, varval) || targetVal <= lowBound)
		{
			delete curSimplex;
			continue;
		}

		noIntIndex = findNoIntIndex(varval);
		if (noIntIndex == -1)
		{
			delete curSimplex;
			lowBound = targetVal;
			candicateVar = varval;
		}
		else
		{
			double b = varval[noIntIndex];
			//左分支
			DualSimplex *leftSimplex = new DualSimplex(*curSimplex, noIntIndex, (int)b, true);
			//右分支
			DualSimplex *rightSimplex = new DualSimplex(*curSimplex, noIntIndex, (int)b + 1, false);
			simplexs.push(rightSimplex);
			simplexs.push(leftSimplex);
			delete curSimplex;
		}
	}
	if (candicateVar.size() != 0)
	{
		opt = lowBound;
		vars = candicateVar;
		return true;
	}
	else
	{
		return false;
	}
}


//返回第一个非整数下标  没有则返回-1
int BranchBound::findNoIntIndex(std::vector<double> &vec)
{
	int total = vec.size();
	for (int i = 0; i < total; i++)
	{
		double val = vec[i];
		if (abs(round(val) - val) > 0.0001)
		{
			return i;
		}
	}
	return -1;
}