#include<iostream>
#include<vector>
#include<set>
#include<algorithm>
using namespace std;

vector<vector<int>> graph;
set<int> vertices;
set<int> ::iterator it;
vector<bool> visited;

bool solved = false;
bool ansFirstTime = true;

struct verticeInfo
{
	int vertice;
	vector<int> domain;
	int currentColor;
	verticeInfo(int v, vector<int> d, int cc = 0) :vertice(v), domain(d), currentColor(cc) {}
};

vector<verticeInfo> vCP;
vector<verticeInfo> answer;

vector<int> removeFromDomain(int removeValue, vector<int> domain)
{
	vector<int> newDomain;

	for (int i = 0; i < domain.size(); i++)
	{
		if (domain[i] != removeValue)
		{
			newDomain.push_back(domain[i]);
		}
	}

	return newDomain;
}

bool isGoalReached(vector<verticeInfo> v)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < graph[i].size(); j++)
		{
			if (v[i].currentColor == v[graph[i][j]].currentColor || v[i].currentColor == 0)
			{
				return false;
			}
		}
	}

	return true;
}

int MRV_Index(vector<verticeInfo> v)
{
	int mrvIndex = 0;
	int minDomSize = 1e12;

	for (int i = 0; i < v.size(); i++)
	{
		if (v[i].domain.size() < minDomSize && !visited[v[i].vertice])
		{
			minDomSize = v[i].domain.size();
			mrvIndex = i;
		}
	}

	return mrvIndex;
}

vector<int> LCV(vector<verticeInfo> v, int vert)
{
	vector<int> sortedLCVDomain;
	while (v[vert].domain.size() != 0)
	{
		int maxSize = -1e9;
		int lcv = 0;
		for (int i = 0; i < v[vert].domain.size(); i++)
		{
			int color = v[vert].domain[i];
			int size = 0;
			for (int j = 0; j < graph[vert].size(); j++)
			{
				for (int k = 0; k < v[graph[vert][j]].domain.size(); k++)
				{
					if (color != v[graph[vert][j]].domain[k])
					{
						size++;
					}
				}
			}
			if (size > maxSize)
			{
				maxSize = size;
				lcv = color;
			}
		}
		sortedLCVDomain.push_back(lcv);
		v[vert].domain = removeFromDomain(lcv, v[vert].domain);
	}

	return sortedLCVDomain;
}

//CP function (ac-3)
void Propagate(int node, int color)
{
	for (int i = 0; i < graph[node].size(); i++)
	{
		if (vCP[graph[node][i]].currentColor == 0)
		{
			vCP[graph[node][i]].domain = removeFromDomain(color, vCP[graph[node][i]].domain);
		}
	}
	for (int i = 0; i < graph[node].size(); i++)
	{
		if (vCP[graph[node][i]].domain.size() == 1 && vCP[graph[node][i]].currentColor == 0)
		{
			vCP[graph[node][i]].currentColor = vCP[graph[node][i]].domain[0];
			Propagate(graph[node][i], vCP[graph[node][i]].domain[0]);
		}
	}
}

bool colorGraphDFS(vector<verticeInfo> v, int vert)
{
	if (solved)
	{
		return true;
	}

	if (isGoalReached(v))
	{
		solved = true;
		if (ansFirstTime) // no need but for safety
		{
			answer = v;
			ansFirstTime = false;
		}
		return true;
	}

	visited[vert] = true;

	//Sort domain by LCV Heuristic
	vector<int> dom = LCV(v, vert);

	for (int i = 0; i < dom.size(); i++)
	{
		v[vert].currentColor = dom[i];

		//Constraint propagation
		vCP = v;
		Propagate(vert, dom[i]); //ac-3 function
		v = vCP;

		//forward checking
		for (int j = 0; j < vertices.size(); j++)
		{
			if (j != vert && v[j].domain.size() == 0)
			{
				visited[vert] = false;
				return false;
			}
		}

		//MRV Heuristic
		int mrvVertice = MRV_Index(v);
		if (mrvVertice != vert)
		{
			colorGraphDFS(v, mrvVertice);
		}
		else
		{
			if (vert + 1 < vertices.size())
			{
				colorGraphDFS(v, vert + 1);
			}
		}
	}

	//could not find solution from here, returning back
	visited[vert] = false;
	return solved; //solved is always false here
}

int main()
{
	int Colors;
	int from, to;
	graph.resize(100000);

	//input: terminated with (-1, -1)
	cin >> Colors;
	while (true)
	{
		scanf_s("%d,%d", &from, &to);
		if (from == -1 && to == -1)
		{
			break;
		}
		graph[from].push_back(to);
		graph[to].push_back(from);
		vertices.insert(from);
		vertices.insert(to);
	}
	
	graph.resize(vertices.size());

	vector<verticeInfo> vColors;

	vector<int> colorDomain;
	for (int i = 1; i <= Colors; i++)
	{
		colorDomain.push_back(i);
	}
	
	for (int i = 0; i < vertices.size(); i++)
	{
		vColors.push_back(verticeInfo(i, colorDomain));
		visited.push_back(false);
	}

	//Start the algorithm
	colorGraphDFS(vColors, 0);

	if (solved)
	{
		cout << "Solution found" << endl;
		for (int i = 0; i < answer.size(); i++)
		{
			cout << "Vertex " << answer[i].vertice << ": Color " << answer[i].currentColor << endl;
		}
	}
	else
	{
		cout << "Not possible to color the graph" << endl;
	}
}