//============================================================================
// Name        : compute_feature.cpp
// Author      : Tiancai
// Version     :
// Copyright   : Copyright 2014 @ Tiancai Ye
// Description : Hello World in C, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"
using namespace std;
using namespace cv;
using namespace os;
using namespace os::path;

string srcdir;
string desdir;

bool init_args(int argc, char** argv)
{
	string helpinfo = "usage : compute_feature -s srcdir -d desdir";
	int ch;
	while ((ch = getopt(argc, argv, "s:d:n:l:a")) != -1)
	{
		switch (ch)
		{
		case 's':
			srcdir = optarg;
			break;
		case 'd':
			desdir = optarg;
			break;
		default:
			cout << helpinfo << endl;
			return false;
		}
	}
	if (srcdir == "" || desdir == "")
	{
		cout << helpinfo << endl;
		return false;
	}

	if (!exists(srcdir) || !exists(desdir))
	{
		cout << "directory doesn't exists!" << endl;
		return false;
	}

	return true;
}

void write_orb_fea(ofstream& ofs, const Mat& desc)
{
	assert(desc.cols == 32); // ORB has 256/8 = 32 char
	for (int i = 0; i < desc.rows; i++)
		ofs.write(desc.ptr<char>(i), desc.cols * sizeof(char));
}

void write_list_info(ofstream& ofs, const string& name, size_t nfeatures)
{
	ofs << name << endl;
	ofs << nfeatures << endl;
}

void write_keypoint(ofstream& ofs, const vector<KeyPoint>& keypoints)
{
	for (size_t i = 0; i < keypoints.size(); ++i)
		ofs << keypoints[i].angle << " " << keypoints[i].octave << endl;
}

void process_sub_dir(const string& _srcdir, const string& _desdir)
{
	try
	{
		auto srcdir = normpath(_srcdir);
		auto desdir = normpath(_desdir);

		auto names = listdir(srcdir, LIST_FILE);
		sort(names.begin(), names.end());


		auto write_path = join(desdir, basename(srcdir));
		if (!exists(write_path))
			mkdir(write_path);

		auto orb_path = join(write_path, "fea.orb");
		auto list_path = join(write_path, "fea.list");
		auto kps_path = join(write_path, "fea.kps");

		ofstream ofs_orb(orb_path, ios::binary);
		ofstream ofs_list(list_path);
		ofstream ofs_kps(kps_path);

		if (!ofs_orb.is_open() || !ofs_list.is_open() || !ofs_kps.is_open())
			throw OSError("can not write results to disk!");

		ORB orb;

		for (size_t i = 0; i < names.size(); ++i)
		{
			string imgpath = join(srcdir, names[i]);
			cout << "processing " << imgpath << "..." << endl;
			Mat im = imread(imgpath, CV_LOAD_IMAGE_GRAYSCALE);
			if (im.empty())
			{
				cerr << "read img error : " << imgpath << " : Skipped!"<< endl;
				continue;
			}
			vector<KeyPoint> keypoints;
			Mat descriptors;
			orb(im, Mat(), keypoints, descriptors);

			if (!descriptors.empty())
			{
				if (keypoints.size() != uint32_t(descriptors.rows))
				{
					cerr << "ORB Error! Skipped!" << endl;
					continue;
				}
				write_orb_fea(ofs_orb, descriptors);
				write_list_info(ofs_list, names[i], keypoints.size());
				write_keypoint(ofs_kps, keypoints);
			}
		}
	}
	catch(const OSError& err)
	{
		cerr << err.what() << endl;
		return;
	}
	catch(const Exception& cv_err)
	{
		cerr << cv_err.what() << endl;
		return;
	}
	catch(...)
	{
		cerr << "unexpected Exception! Exit!" << endl;
		return;
	}
}

int main(int argc, char** argv)
{
	if (!init_args(argc, argv)) return 1;

	auto subdirs = listdir(srcdir, LIST_DIR);

#pragma omp parallel for
	for (size_t i = 0; i < subdirs.size(); ++i)
	{
		process_sub_dir(join(srcdir, subdirs[i]), desdir);
	}

	return 0;
}
