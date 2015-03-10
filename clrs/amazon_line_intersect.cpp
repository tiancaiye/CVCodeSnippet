#include <vector>
#include <iostream>
#include <algorithm>
#include <climits>
#include <string>
using namespace std;

struct Point
{
	int x;
	int y;
};

struct Line
{
	Point a;
	Point b;
};

bool intersect(const Line& m, const Line& n)
{
	int tmp0 = (m.b.y - m.a.y)*(n.a.x - m.a.x) - (n.a.y - m.a.y)*(m.b.x - m.a.x);
	int tmp1 = (m.b.y - m.a.y)*(n.b.x - m.a.x) - (n.b.y - m.a.y)*(m.b.x - m.a.x);
	if (tmp0 * tmp1 > 0)
		return false;
	
	int tmp2 = (n.b.y - n.a.y)*(m.a.x - n.a.x) - (m.a.y - n.a.y)*(n.b.x - n.a.x);
	int tmp3 = (n.b.y - n.a.y)*(m.b.x - n.a.x) - (m.b.y - n.a.y)*(n.b.x - n.a.x);
	if (tmp2 * tmp3 > 0)
		return false;

	return true;
}

struct UnionSet
{
	int* p;
	int* rank;

	void init_set(int N)
	{
		p = new int[N];
		rank = new int[N];
		for (int i = 0; i < N; ++i)
		{
			p[i] = i;
			rank[i] = 1;
		}
	}

	int find_set(int x)
	{
		if (p[x] == x) return x;
		p[x] = find_set(p[x]);
		return p[x];
	}

	void union_set(int x, int y)
	{
		int setx = find_set(x);
		int sety = find_set(y);
		if (setx == sety)
			return;

		if (rank[setx] > rank[sety])
		{
			p[sety] = setx;
		}
		else
		{
			p[setx] = sety;
			if (rank[setx] == rank[sety])
				rank[sety]++;
		}
	}
};

double line_len(const Line& l)
{
	return sqrt(pow(double(l.a.x - l.b.x), 2) + pow(double(l.a.y - l.b.y), 2));
}

int main(int, char**)
{
	int N;
	cin >> N;
	char tmp;
	vector<Line> lines(N);
	for (int i = 0; i < N; ++i)
		cin >> lines[i].a.x >> tmp >>
		lines[i].a.y >> tmp >> lines[i].b.x >> tmp >> lines[i].b.y;
	
	UnionSet uset;
	uset.init_set(N);

	for (int i = 0; i < N; ++i)
	{
		for (int j = i + 1; j < N; ++j)
		{
			if (intersect(lines[i], lines[j]))
				uset.union_set(i, j);
		}
	}

	vector<double> len(N, 0.0);
	for (int i = 0; i < N; i++)
	{
		int p = uset.find_set(i);
		double l = line_len(lines[i]);
		len[p] += l;
	}

	cout << int(*(max_element(len.begin(), len.end()))) << endl;

	return 0;
}
