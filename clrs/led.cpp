#include <iostream>
#include <vector>
#include <string>

using namespace std;

const vector<string> numstrlist = {
	"1111011", "1111111", "1110000",
	"1011111", "1011011", "0110011",
	"1111001", "1101101", "0110000",
	"1111110"
};

int main(int argc, char const *argv[])
{

	vector<bitset<7>> numlist(numstrlist.size());
	for (int i = 0; i < numstrlist.size(); ++i)
		numlist[i] = stoi(numstrlist[i], NULL, 2);

	int T;
	cin >> T;
	for (int casenum = 0; casenum < T; ++casenum)
	{
		int N;
		cin >> N;

		vector<char> nums(N);
		for (int i = 0; i < N; ++i)
		{
			string s;
			cin >> s;
			nums[i] = stoi(s, NULL, 2);
		}

		char res = -1;
		for (int i = 0; i < numlist.size(); ++i)
		{
			int j = 0;
			char mask = 0x7f;
			for (; j < nums.size(); ++j)
			{
				if ((~numlist[(i+j) % 10]) & nums[j])
					break;
				mask &= nums[j];
			}
			if (j == nums.size())
			{
				if (res == -1)
					res = numlist[(i+j) % 10] & mask;
				else
				{
					res = -1;
					break;
				}
			}

		}
		if (res == -1)
		{
			cout << "Case #" << casenum << ""
		}
	}
	return 0;
}