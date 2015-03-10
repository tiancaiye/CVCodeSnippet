#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<int> compute_next(const string& p)
{
	int N = p.size();
	vector<int> next(N, -1);
	int j = 0;
	int k = -1;

	while (j < N-1)
	{
		if (k == -1 || p[j] == p[k])
		{
			j++;
			k++;
			
			if (p[j] == p[k])
				next[j] = k;
			else
				next[j] = next[k];
		}
		else
			k = next[k];
	}

	return next;
}

int kmp(const string& s, const string& p)
{
	vector<int> next = compute_next(p);
	int i = 0;
	int j = 0;
	while (i < s.size())
	{
		if (j == -1 || s[i] == p[j])
		{
			i++;
			j++;
		}
		else
			j = next[j];

		if (j == int(p.size()))
			return i-j;
	}
	return -1;
}



int main(int argc, char const *argv[])
{
	string s = "abcaaaaaaaaaedfffffss";
	string p = "fsss";

	cout << kmp(s, p) << endl;
	return 0;
}