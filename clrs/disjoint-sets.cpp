#include <iostream>
#include <string>

using namespace std;

const int MAXN = 100;
int pa[MAXN];
int rank[MAXN];

void make_set(int x)
{
	pa[x] = x;
	rank[x] = 0;
}

int find_set(int x)
{
	if (pa[x] != x)
		pa[x] = find_set(pa[x]);
	return pa[x];
}

void union_set(int x, int y)
{
	x = find_set(x);
	y = find_set(y);
	if (x == y) return;
	if (rank[x] > rank[y])
		pa[y] = x;
	else
	{
		pa[x] = y;
		if (rank[x] == rank[y])
			rank[y]++;
	}
}

int main(int argc, char const *argv[])
{
	
	return 0;
}