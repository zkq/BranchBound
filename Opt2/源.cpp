
#include<fstream>
#include<iostream>
#include<vector>
#include<unordered_map>
#include<ctime>
#include<iomanip>

#include "simplex.h"
#include "branchbound.h"

using namespace std;

#define V 500
#define E 500
#define inf 99999999

int vis[V];
int dist[V];
int pre[V];

struct Edge{
	int u, v, c, cost;
}edge[E];

vector<int> edgeIn[V];
vector<int> edgeOut[V];

int cnt = 0;
void addCenterEdge(int u, int v, int c, int cost)
{
	edge[cnt].u = u; edge[cnt].v = v; edge[cnt].cost = cost; edge[cnt].c = c;
	edgeIn[v].push_back(cnt);
	edgeOut[u].push_back(cnt);
	cnt++;

	edge[cnt].u = v; edge[cnt].v = u; edge[cnt].cost = cost; edge[cnt].c = c;
	edgeIn[u].push_back(cnt);
	edgeOut[v].push_back(cnt);
	cnt++;
}

unordered_map<int, int> neighborToEdge;
void addConsumerEdge(int u, int demand)
{
	edge[cnt].c = demand;
	neighborToEdge.insert(pair<int, int>(u, cnt));
	cnt++;
}


void foutArray(ofstream &fout, int vec[], int cnt)
{
	for (int i = 0; i < cnt; i++)
	{
		fout << vec[i] << " ";
	}
}

void setArray(int array[], vector<int> &vec, int var)
{
	for (vector<int>::iterator i = vec.begin(); i != vec.end(); i++)
	{
		array[*i] = var;
	}
}


int main()
{
	/*
	ifstream fin("graph.txt");
	ofstream fout("opt.txt");

	int vCnt, eCnt, cCnt;
	fin >> vCnt >> eCnt >> cCnt;

	int sCost;
	fin >> sCost;

	//输出节点数 服务器价格 边数量
	//fout << vCnt << " " << eCnt * 2 << endl;

	//内部网络
	int from, to, cap, cost;
	for (int i = 0; i < eCnt; i++)
	{
		fin >> from >> to >> cap >> cost;
		addCenterEdge(from, to, cap, cost);
	}
	//fout << endl;

	//边缘网络
	int cId, vId, demand;
	int totalDemand = 0;
	for (int i = 0; i < cCnt; i++)
	{
		fin >> cId >> vId >> demand;
		totalDemand += demand;
		addConsumerEdge(vId, demand);
	}

	//构造系数
	//首先是目标函数系数  前2*eCnt为边流量f的系数 后vCnt为节点是否为服务器s的系数
	fout << 0 << " ";
	for (int i = 0; i < 2 * eCnt; i++)
	{
		fout << edge[i].cost << " ";
	}
	for (int i = 0; i < vCnt; i++)
	{
		fout << sCost << " ";
	}
	fout << endl;

	int totalVar = 2 * eCnt + vCnt;
	int M = totalDemand;
	
	int A[2 * E + V] = { 0 };
	//容量约束
	for (int i = 0; i < 2 * eCnt; i++)
	{
		fout << edge[i].c << " ";
		A[i] = 1;
		foutArray(fout, A, totalVar);
		A[i] = 0;
		fout << endl;
	}
	

	//顶点0 1约束
	for (int i = 0; i < vCnt; i++)
	{
		fout << 1 << " ";
		A[i+2*eCnt] = 1;
		foutArray(fout, A, totalVar);
		A[i + 2 * eCnt] = 0;
		fout << endl;
	}
	

	//顶点流量平衡
	for (int i = 0; i < vCnt; i++)
	{
		fout << M << " ";
		setArray(A, edgeIn[i], 1);
		A[2 * eCnt + i] = M;
		foutArray(fout, A, totalVar);
		fout << endl;
		//memset(A, 0, sizeof(A));

		unordered_map<int, int>::iterator ite = neighborToEdge.find(i);
		int b = ite != neighborToEdge.end() ? edge[ite->second].c : 0;

		fout << b << " ";
		setArray(A, edgeIn[i], 1);
		setArray(A, edgeOut[i], -1);
		A[2 * eCnt + i] = -M;
		foutArray(fout, A, totalVar);
		fout << endl;
		//memset(A, 0, sizeof(A));

		fout << -b << " ";
		setArray(A, edgeOut[i], 1);
		setArray(A, edgeIn[i], -1);
		A[2 * eCnt + i] = -M;
		foutArray(fout, A, totalVar);
		fout << endl;
		memset(A, 0, sizeof(A));
	}

	fin.close();
	fout.close();*/



	//算法开始
	/*ifstream matrixFile("opt.txt", ios::in | ios::binary);
	if (!matrixFile.is_open())
		cout << "error open file" << endl;

	vector<double> variables;
	DualSimplex mySimplex;
	// row = constraint + 1 (objective)
	mySimplex.readMatrix(203, 119, matrixFile);
	// mySimplex.outputMatrix();

	double optimized = 0;
	if (!mySimplex.solveMinProblemWithDual(2000, optimized, variables))
	{
		cout << "can't find best" << endl;
		return 0;
	}
	cout << "optimization value is " << optimized << endl;
	cout << "variables is "<<endl;
	for (int i = 0; i < variables.size(); i++)
	{
		cout <<setw(15)<< variables[i];
	}
	cout << endl;*/

	ifstream fin2("opt.txt");
	BranchBound branchBound(203, 119, fin2);
	double optimized;
	vector<double> variables;
	if (!branchBound.solve(optimized, variables))
	{
		cout << "can't find best" << endl;
		return 0;
	}
	cout << "optimization value is " << optimized << endl;
	cout << "variables is " << endl;
	for (int i = 0; i < variables.size(); i++)
	{
		cout << setw(15) << variables[i];
	}
	cout << endl;

	return 0;
}

