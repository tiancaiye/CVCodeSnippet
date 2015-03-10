//============================================================================
// Name        : bin_kmeans.cpp
// Author      : Tiancai
// Version     :
// Copyright   : Copyright 2014 @ Tiancai Ye
// Description : Hello World in C, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <random>
#include <unistd.h>
#include <omp.h>

#include "utils.h"
#include "bin_feat.h"

using namespace std;
using namespace os;
using namespace os::path;


typedef BinFeat<256> OrbFeat;

string srcdir;
string desfile;

bool init_args(int argc, char** argv)
{
	string helpinfo = "usage : bin_kmeans -s srcdir -d desfile";
	int ch;
	while ((ch = getopt(argc, argv, "s:d:")) != -1)
	{
		switch (ch)
		{
		case 's':
			srcdir = optarg;
			break;
		case 'd':
			desfile = optarg;
			break;
		default:
			cout << helpinfo << endl;
			return false;
		}
	}

	if (srcdir.empty() || desfile.empty())
	{
		cout << helpinfo << endl;
		return false;
	}

	return true;
}

template<size_t N>
vector<size_t> bin_kmeans(const vector<BinFeat<N>>& dataset, size_t K)
{
	vector<size_t> centers = pickup_centers(dataset, K);
	vector<size_t> flags(dataset.size());

	int times = 0;
	while (times++ < 999999)
	{
		vector<size_t> old_centers = centers;
#pragma omp parallel for
		for (size_t i = 0; i < dataset.size(); ++i)
		{
			size_t min_distance = numeric_limits<size_t>::max();
			size_t which_one = 0;
			for (size_t j = 0; j < K; ++j)
			{
				size_t distance = hamming_distance(dataset[i], dataset[centers[j]]);
				if (min_distance > distance)
				{
					min_distance = distance;
					which_one = j;
				}
			}
			flags[i] = which_one;
		}

		vector<size_t> dist(dataset.size(), 0);
#pragma omp parallel for
		for (size_t i = 0; i < dataset.size(); ++i)
		{
			for (size_t j = 0; j < dataset.size(); ++j)
				if (flags[j] == flags[i])
					dist[i] += hamming_distance(dataset[i], dataset[j]);
		}

		for (size_t i = 0; i < dataset.size(); ++i)
		{
			if (dist[i] < dist[centers[flags[i]]])
				centers[flags[i]] = i;
		}

		bool converge = true;
		for (size_t i = 0; i < centers.size(); ++i)
		{
			if (centers[i] != old_centers[i])
			{
				converge = false;
				break;
			}
		}

		if (converge)
		{
			cout << "converge when times == " << times << endl;
			break;
		}
	}

	vector<size_t> avg_dist(K, 0);
	vector<size_t> num(K, 0);
	for (size_t i = 0; i < dataset.size(); ++i)
	{
		avg_dist[flags[i]] += hamming_distance(dataset[i], dataset[centers[flags[i]]]);
		num[flags[i]]++;
	}

	size_t max_dist = 0;
	size_t min_dist = numeric_limits<size_t>::max();
	size_t sum_dist = 0;
	for (size_t i = 0; i < avg_dist.size(); ++i)
	{
		avg_dist[i] /= num[i];
		max_dist = max(max_dist, avg_dist[i]);
		min_dist = min(min_dist, avg_dist[i]);
		sum_dist += avg_dist[i];
	}

	cout << "max cluster avg dist : " << max_dist << endl;
	cout << "min cluster avg dist : " << min_dist << endl;
	cout << "avg cluster avg dist : " << sum_dist/K << endl;


	return centers;
}

template<size_t N>
vector<size_t> pickup_centers(const vector<BinFeat<N>>& dataset, size_t K)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, dataset.size()-1);

	vector<size_t> centers;
	centers.push_back(dis(gen));

	while (centers.size() < K)
	{
		const int MAX_THREADS = omp_get_max_threads();
		vector<size_t> max_distance(MAX_THREADS, 0);
		vector<size_t> which_one(MAX_THREADS, 0);

#pragma omp parallel for
		for (size_t i = 0; i < dataset.size(); ++i)
		{
			size_t distance = numeric_limits<size_t>::max();
			for (size_t j = 0; j < centers.size(); ++j)
			{
				distance = min(distance, hamming_distance(dataset[i], dataset[centers[j]]));
			}

			const int CUR_ID = omp_get_thread_num();
			if (distance > max_distance[CUR_ID])
			{
				max_distance[CUR_ID] = distance;
				which_one[CUR_ID] = i;
			}
		}

		size_t the_one = 0;
		for (size_t i = 0; i < max_distance.size(); ++i)
		{
			if (max_distance[i] > max_distance[the_one])
			{
				the_one = i;
			}
		}

		centers.push_back(which_one[the_one]);
		cout << centers.size() << endl;
	}
	return centers;
}

int main(int argc, char** argv)
{
	if (!init_args(argc, argv)) return 1;

	vector<OrbFeat> dataset;

	auto dirs = listdir(srcdir, LIST_DIR);

	cout << "Loading Features......" << endl;
	for (size_t i = 0; i < dirs.size(); ++i)
	{
		ifstream input(join(srcdir, dirs[i], "fea.orb"), ios::binary);
		while (!input.eof())
		{
			OrbFeat feat;
			input >> feat;
			dataset.push_back(feat);
		}
	}
	cout << "Features Loaded Successfully!" << endl;

	cout << "Begin Kmeans!" << endl;
	auto centers = bin_kmeans(dataset, 2000);
	cout << "Kmeans Ends!" << endl;

	cout << "Writing Centers to File......" << endl;

	ofstream ofs(desfile);
	for (size_t i = 0; i < centers.size(); ++i)
	{
		ofs << dataset[centers[i]];
	}

	cout << "Writing File Completed!" << endl;

	return EXIT_SUCCESS;
}
