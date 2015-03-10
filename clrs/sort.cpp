#include <iostream>
#include <algorithm>
#include <limits>
#include <climits>
#include <cfloat>

using namespace std;


// insertion sort
void insertion_sort(int a[], int n)
{
	for (int i = 0; i < n; ++i)
	{
		int key = a[i];
		int j = i - 1;

		// don't use swap here, not efficient
		while (j >= 0 && a[j] > key)
		{
			a[j+1] = a[j];
			j--;
		}
		a[j+1] = key;
	}
}


// merge sort
void merge(int a[], int p, int q, int r)
{
	int left[q-p+1];
	int right[r-q+1];

	copy(a+p, a+q, left);
	copy(a+q, a+r, right);

	left[q-p] = INT_MAX;
	right[r-q] = INT_MAX;

	int i = 0, j = 0;
	for (int k = p; k < r; ++k)
	{
		if (left[i] < right[j])
			a[k] = left[i++];
		else
			a[k] = right[j++];
	}
}

void merge_sort(int a[], int p, int r)
{
	if (r - p <= 1)
		return;
	int q = (p+r)/2;
	merge_sort(a, p, q);
	merge_sort(a, q, r);
	merge(a, p, q, r);
}

// heap sort
int left(int k)
{
	return 2*(k+1)-1;
}

int right(int k)
{
	return 2*(k+1);
}

int parent(int k)
{
	return (k+1)/2-1;
}

void max_heapify(int a[], int n, int k)
{
	int ileft = left(k);
	int iright = right(k);
	int imax = k;
	if (ileft < n && a[ileft] > a[k])
		imax = ileft;
	if (iright < n && a[iright] > a[imax])
		imax = iright;
	if (imax != k)
	{
		swap(a[k], a[imax]);
		max_heapify(a, n, imax);
	}
}

void build_max_heap(int a[], int n)
{
	for (int i = parent(n-1); i >= 0; --i)
	{
		max_heapify(a, n, i);
	}
}

void heap_sort(int a[], int n)
{
	build_max_heap(a, n);
	for (int i = n-1; i >0; --i)
	{
		swap(a[i], a[0]);
		max_heapify(a, i, 0);
	}
}

// quick sort
int partition(int a[], int p, int r)
{
	int key = a[r-1];
	int i = 0, j = 0;
	while (i < r-1)
	{
		if (a[i] < key)
		{
			swap(a[i], a[j++]);
		}
		i++;
	}
	swap(a[j], a[r-1]);
	return j;
}

void quick_sort(int a[], int p, int r)
{
	if (r-p <= 1) return;
	int q = partition(a, p ,r);
	quick_sort(a, p, q);
	quick_sort(a, q+1, r);
}

// select the kth element in average linear time
int select(int a[], int p, int r, int k)
{
	if (r - p <= 1) return a[p];
	int q = partition(a, p, r);
	int rank = q - p;
	if (rank == k)
		return a[q];

	if (rank < k)
		return select(a, q, r, k-rank);
	else
		return select(a, p, q, k);
}

void print_array(int a[], size_t size)
{
	for (int i = 0; i < size; ++i)
		cout << a[i] << ",";
	cout << endl;
}

int main(int argc, char const *argv[])
{
	int a[] = { 8, 9, 10, 20, 4, 5, 0, 100, 21};
	int N = sizeof(a) / sizeof(int);
	//insertion_sort(a, N);
	//merge_sort(a, 0, N);
	//heap_sort(a, N);
	cout << select(a, 0, N, 2) << endl;
	quick_sort(a, 0, N);
	print_array(a, N);

	return 0;
}