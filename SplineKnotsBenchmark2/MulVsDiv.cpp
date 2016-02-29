#include "stdafx.h"
#include "MulVsDiv.h"
#include <ostream>
#include <iostream>
#include <locale>
#include <vector>
#include <numeric>
#include <tchar.h>
#include <windows.h>
#include "StopWatch.h"
#include <forward_list>

void MulVsDiv::ResetArrays(const int length, double* a, double* b, double& 
	ignoreit)
{
	ignoreit += a[(rand() % (int)(length))] + b[(rand() % (int)(length))];
	for (size_t i = 0; i < length; i++)
	{
		a[i] = 6 * ((double)rand() / (RAND_MAX)) + 2;
		b[i] = 2 * ((double)rand() / (RAND_MAX)) + 1;
	}
	ignoreit += a[(rand() % (int)(length))] + b[(rand() % (int)(length))];
	ignoreit = 1/ignoreit;
}

void MulVsDiv::ResetMatrix(const int rows, const int columns, double** matrix, double& ignoreit)
{
	ignoreit += matrix[(rand() % (int)(rows))][(rand() % (int)(columns))];
	
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < columns; j++)
		{
			matrix[i][j]= 2 * ((double)rand() / (RAND_MAX)) + 1;
		}
	}
	ignoreit += matrix[(rand() % (int)(rows))][(rand() % (int)(columns))];
	ignoreit = 1 / ignoreit;
}

void MulVsDiv::ResetList(const int length,std::forward_list<double>& list, double& ignoreit)
{
	list.clear();
	for (size_t i = 0; i < length; i++)
	{
		list.push_front(6 * ((double)rand() / (RAND_MAX)) + 2);
	}
	ignoreit += list.front();
	ignoreit = 1 / ignoreit;
}

void MulVsDiv::Loop()
{
	StopWatch sw;

	const int length = 512;
	const int loops = 1e6/2;
	std::cout << "Simple loop:\n---" << std::endl;
	double a[length], b[length], c[length];
	/*double d[length], e[length], f[length], g[length];
	double d1[length], e1[length], f1[length], g1[length],x[length];*/
	auto ignoreit = 0.0;

	ResetArrays(length, a, b, ignoreit);
	/*ResetArrays(length, d, e, ignoreit);
	ResetArrays(length, f, g, ignoreit);
	ResetArrays(length, g, x, ignoreit);*/
	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
		// MSVC cannot vectorize this loop (message 1300).
		// However, if this loop will not be nested in, autovectorization will 
		//happen. Same condition apply for mul/div/rcp loops

		// ICL does not have this issue, but to provide both vectorized and 
		// nonvectorized comparison i specifically disabled vectorization in 
		// this method
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			c[i] = a[i] + b[i];// +d[i] + e[i] + f[i] + g[i] + d1[i] + e1[i] + f1[i] + g1[i] + x[i];
		}
	}
	sw.Stop();
	auto add_time = sw.EllapsedTime();
	std::cout << "Addition: " << add_time << std::endl;
	ignoreit -= c[(rand() % static_cast<int>(length))];
	ResetArrays(length, a, b, ignoreit);

	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			a[i] = c[i] * b[i];
		}
	}
	sw.Stop();
	auto mul_time = sw.EllapsedTime();
	std::cout << "Multiplication: " << mul_time << std::endl;
	ignoreit -= a[(rand() % static_cast<int>(length))];
	ResetArrays(length, a, b, ignoreit);

	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			b[i] = a[i] / c[i];
		}
	}
	sw.Stop();
	auto div_time = sw.EllapsedTime();
	ignoreit -= c[(rand() % static_cast<int>(length))];
	ignoreit += a[(rand() % static_cast<int>(length))] + b[(rand() % 
		static_cast<int>(length))];
	std::cout << "Division: " << div_time << std::endl;
	std::cout << "Addition faster than multiplication: " << static_cast<double>
		(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>
		(div_time) / static_cast<double>(mul_time) << std::endl;
	std::cout << "Just ignore it: " << ignoreit << std::endl << std::endl;
}

void MulVsDiv::LoopVectorized()
{
	const int length = 256;
	const int loops = 1e6;
	std::cout << "Vectorized loop:\n---" << std::endl;
	double a[length], b[length],c[length];
	auto ignoreit = 0.0;

	ResetArrays(length, a, b, ignoreit);
	int l = 0;
	auto start = clock();
add:
	for (int i = 0; i < length; i++)
	{
		b[i] = a[i] + c[i];
	}
	// MSVC doesn't vectorize nested loops (message 1300 - too little 
	// computation to vectorize) as mentioned on line 23 in function 'Loop'.
	// However if nested loop is replaced with this nasty workaround SIMD 
	// vectorization will happen. Same condition apply for mul/div/rcp loops
	while (l < loops)
	{
		++l;
		goto add;
	}
	auto add_time = clock() - start;
	std::cout << "Addition: " << add_time << std::endl;
	ignoreit -= c[(rand() % (int)(length))];
	ResetArrays(length, a, b, ignoreit);

	l = 0;
	start = clock();
mul:
	for (int i = 0; i < length; ++i)
	{
		c[i] = a[i] * b[i];
	}
	while (l < loops)
	{
		++l;
		goto mul;
	}
	auto mul_time = clock() - start;
	std::cout << "Multiplication: " << mul_time << std::endl;
	ignoreit -= c[(rand() % (int)(length))];
	ResetArrays(length, a, b, ignoreit);

	l = 0;
	start = clock();
div:
	for (int i = 0; i < length; ++i)
	{
		a[i] = c[i] / b[i];
	}
	while (l < loops)
	{
		++l;
		goto div;
	}
	auto div_time = clock() - start;
	ignoreit -= c[(rand() % (int)(length))];
	ignoreit += a[(rand() % (int)(length))] + b[(rand() % (int)(length))];
	std::cout << "Division: " << div_time << std::endl;
	std::cout << "Addition faster than multiplication: " << static_cast<double>
		(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>
		(div_time) / static_cast<double>(mul_time) << std::endl;
	std::cout << "Just ignore it: " << ignoreit << std::endl << std::endl;
}

void MulVsDiv::DynamicArrayLoop()
{
	StopWatch sw;
	const int length = 512;
	const int loops = 1e6/2;
	std::cout << "Dynamic array loop:\n---" << std::endl;
	std::vector<double> av(length), bv(length), cv(length);
	//std::vector<double> dv(length), ev(length), fv(length), gv(length);
	//std::vector<double> d1v(length), e1v(length), f1v(length), g1v(length), xv(length);
	double *a = &av.front(), *b = &bv.front(), *c = &cv.front();
	//double *d = &dv.front(), *e = &ev.front(), *f = &fv.front(), *g = &gv.front();
	//double *d1 = &d1v.front(), *e1 = &e1v.front(), *f1 = &f1v.front(), *g1 = &g1v.front(),*x=&xv.front();
	auto ignoreit = 0.0;

	ResetArrays(length, a, b, ignoreit);
	//ResetArrays(length, d, e, ignoreit);
	//ResetArrays(length, f, g, ignoreit);
	//ResetArrays(length, g, x, ignoreit);

	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
		// MSVC doesn't vectorize nested loops (message 1300 - too little 
		// computation to vectorize) as mentioned on line 23 in function 'Loop'.
		// However if nested loop is replaced with this nasty workaround SIMD 
		// vectorization will happen. Same condition apply for mul/div/rcp loops

		// ICL does not have this issue, but to provide both vectorized and 
		// nonvectorized comparison i specifically disabled vectorization in 
		// this method
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			c[i] = a[i] + b[i];// +d[i] + e[i] + f[i] + g[i] + d1[i] + e1[i] + f1[i] + g1[i] + x[i];
		}
	}
	sw.Stop();
	auto add_time = sw.EllapsedTime();
	std::cout << "Addition: " << add_time << std::endl;

	ResetArrays(length, a, b, ignoreit);
	ignoreit /= c[(rand() % (int)(length))];

	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			c[i] = a[i] * b[i];// *d[i] * e[i] * f[i] * g[i] * d1[i] * e1[i] * f1[i] * g1[i] * x[i];
		}
	}
	sw.Stop();
	auto mul_time = sw.EllapsedTime();
	std::cout << "Multiplication: " << mul_time << std::endl;

	ResetArrays(length, a, b, ignoreit);
	ignoreit -= c[(rand() % (int)(length))];
	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			c[i] = b[i] / a[i];// / d[i] / e[i] / f[i] / g[i] / d1[i] / e1[i] / f1[i] / g1[i] / x[i];
		}
	}
	sw.Stop();
	auto div_time = sw.EllapsedTime();
	ignoreit += a[(rand() % (int)(length))] + b[(rand() % (int)(length))];
	ignoreit /= c[(rand() % (int)(length))];
	std::cout << "Division: " << div_time << std::endl;
	std::cout << "Addition faster than multiplication: " << static_cast<double>
		(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>
		(div_time) / static_cast<double>(mul_time) << std::endl;
	std::cout << "Just ignore it: " << ignoreit << std::endl << std::endl;
}

void MulVsDiv::DynamicListLoop()
{
	StopWatch sw;
	const int length = 512;
	const int loops = 1e6/2;
	std::cout << "Dynamic list loop:\n---" << std::endl;
	std::vector<double> av(length), bv(length);
	double *a = &av.front(), *b = &bv.front();
	std::forward_list<double> cl;
	auto ignoreit = 0.0;

	ResetArrays(length, a, b, ignoreit);
	ResetList(length, cl, ignoreit);
	
	
	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
		// MSVC doesn't vectorize nested loops (message 1300 - too little 
		// computation to vectorize) as mentioned on line 23 in function 'Loop'.
		// However if nested loop is replaced with this nasty workaround SIMD 
		// vectorization will happen. Same condition apply for mul/div/rcp loops

		// ICL does not have this issue, but to provide both vectorized and 
		// nonvectorized comparison i specifically disabled vectorization in 
		// this method
#pragma novector
#pragma loop( no_vector )
		int i = 0;
		for (auto& c : cl)
		{
			c = a[i] + b[i];
			++i;
		}
	}
	sw.Stop();
	auto add_time = sw.EllapsedTime();
	std::cout << "Addition: " << add_time << std::endl;

	ResetArrays(length, a, b, ignoreit);
	auto it = cl.begin();
	std::advance(it, (rand() % (int)(length)));
	ignoreit /= *it;

	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		int i = 0;
		for (auto& c : cl)
		{
			c = a[i] * b[i];
			++i;
		}
	}
	sw.Stop();
	auto mul_time = sw.EllapsedTime();
	std::cout << "Multiplication: " << mul_time << std::endl;

	ResetArrays(length, a, b, ignoreit);
	it = cl.begin();
	std::advance(it, (rand() % (int)(length)));
	ignoreit /= *it;
	sw.Start();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		int i = 0;
		for (auto& c : cl)
		{
			c = b[i] / a[i];
			++i;
		}
	}
	sw.Stop();
	auto div_time = sw.EllapsedTime();
	ignoreit += a[(rand() % (int)(length))] + b[(rand() % (int)(length))];
	it = cl.begin();
	std::advance(it, (rand() % (int)(length)));
	ignoreit /= *it;
	std::cout << "Division: " << div_time << std::endl;
	std::cout << "Addition faster than multiplication: " << static_cast<double>
		(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>
		(div_time) / static_cast<double>(mul_time) << std::endl;
	std::cout << "Just ignore it: " << ignoreit << std::endl << std::endl;
}

void MulVsDiv::DynamicArrayLoopVectorized()
{
	const int length = 256;
	const int loops = 1e6;
	std::cout << "Vectorized dynamic array loop:\n---" << std::endl;
	std::vector<double> av(length), bv(length), cv(length);
	double *a = &av.front(), *b = &bv.front(), *c = &bv.front();
	auto ignoreit = 0.0;

	ResetArrays(length, a, b, ignoreit);

	int l = 0;
	auto start = clock();
add:
	for (int i = 0; i < length; i++)
	{
		c[i] = a[i] + b[i];
	}
	// MSVC doesn't vectorize nested loops (message 1300 - too little 
	// computation to vectorize) in function 'DynamicArrayLoop'. However if 
	// nested loop is replaced with this nasty workaround SIMD vectorization 
	// will happen. Same condition apply for mul/div/rcp loops
	while (l < loops)
	{
		++l;
		goto add;
	}
	auto add_time = clock() - start;
	std::cout << "Addition: " << add_time << std::endl;

	ResetArrays(length, a, b, ignoreit);
	ignoreit /= c[(rand() % (int)(length))];
	l = 0;
	start = clock();
mul:
	for (int i = 0; i < length; ++i)
	{
		c[i] = a[i] * b[i];
	}
	while (l < loops)
	{
		++l;
		goto mul;
	}
	auto mul_time = clock() - start;
	std::cout << "Multiplication: " << mul_time << std::endl;

	ResetArrays(length, a, b, ignoreit);
	ignoreit /= c[(rand() % (int)(length))];
	l = 0;
	start = clock();
div:
	for (int i = 0; i < length; ++i)
	{
		c[i] = a[i] / b[i];
	}
	while (l < loops)
	{
		++l;
		goto div;
	}
	auto div_time = clock() - start;
	ignoreit /= c[(rand() % (int)(length))];
	ignoreit += a[(rand() % (int)(length))] + b[(rand() % (int)(length))];
	std::cout << "Division: " << div_time << std::endl;

	std::cout << "Addition faster than multiplication: " << static_cast<double>
		(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>
		(div_time) / static_cast<double>(mul_time) << std::endl;
	std::cout << "Just ignore it: " << ignoreit << std::endl << std::endl;
}

void MulVsDiv::CsabaDynamicArrayLoop()
{
	const int length = 256;//1024 *1024* 8;
	const int loops = 1e6;//1e3 / 2;
	std::cout << "Csaba loop:\n---" << std::endl;
	std::vector<double> av(length), bv(length);
	double *a = &av.front(), *b = &bv.front();
	auto ignoreit = 0.0;

	ResetArrays(length, a, b, ignoreit);
	auto idx1 = (rand() % static_cast<int>(length)), idx2 = (rand() % 
		static_cast<int>(length-1));
	auto start = clock();
	for (size_t l = 0; l < loops; l++)
	{
		// MSVC cannot vectorize this loop (message 1300).
		// However, if this loop will not be nested in, autovectorization will 
		// happen. Same condition apply for mul/div/rcp loops

		// ICL does not have this issue, but to provide both vectorized and 
		// nonvectorized comparisoni specifically disabled vectorization in 
		// this method
#pragma novector
#pragma loop( no_vector )
		for (int i = 1; i < length-1; ++i)
		{
			a[i] = b[idx1] + b[idx2];
		}
	}
	auto add_time = clock() - start;
	std::cout << "Addition: " << add_time << std::endl;

	ignoreit += a[(rand() % static_cast<int>(length))] + b[(rand() % 
		static_cast<int>(length))];

	idx1 = (rand() % static_cast<int>(length)), idx2 = (rand() % 
		static_cast<int>(length));

	start = clock();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length-1; ++i)
		{
			a[i] =b[idx1] * b[idx2];
		}
	}
	auto mul_time = clock() - start;
	std::cout << "Multiplication: " << mul_time << std::endl;

	ignoreit += a[(rand() % static_cast<int>(length))] + b[(rand() % 
		static_cast<int>(length))];
	idx1 = (rand() % static_cast<int>(length)), idx2 = (rand() % 
		static_cast<int>(length));
	start = clock();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
#pragma loop( no_vector )
		for (int i = 0; i < length; ++i)
		{
			a[i] = b[idx1] / b[idx2];
		}
	}
	auto div_time = clock() - start;
	ignoreit += a[(rand() % static_cast<int>(length))] + b[(rand() % 
		static_cast<int>(length))];
	std::cout << "Division: " << div_time << std::endl;
	std::cout << "Addition faster than multiplication: " << 
		static_cast<double>(mul_time) / static_cast<double>(add_time) << 
		std::endl;
	std::cout << "Multiplication faster than division: " << 
		static_cast<double>(div_time) / static_cast<double>(mul_time) << 
		std::endl;
	std::cout << "Just ignore it: " << ignoreit << std::endl << std::endl;
}

void MulVsDiv::DependendDynamicArrayLoop()
{
	const int length = 512;
	const int loops = 1e6;
	std::cout << "Dependend Dynamic loop:\n---" << std::endl;
	std::vector<double> av(length), bv(length);
	double *a = &av.front(), *b = &bv.front();
	
	std::vector<long> add_times;
	std::vector<long> mul_times;
	std::vector<long> div_times;
	add_times.reserve(loops);
	mul_times.reserve(loops);
	div_times.reserve(loops);
	unsigned long long ignoreit[]{0,0,0};
	
	for (size_t i = 0; i < length; i++)
	{
		av[i] = 8 * (static_cast<double>(rand()) / (RAND_MAX)) + 1;
		bv[i] = DBL_MIN;
	}

	//Division
	
	for (size_t i = 0; i < loops; i++)
	{
		auto start = clock();
		for (size_t j = 0; j < length-1; j++)
		{
			b[j] = a[j + 1] / a[j];
		}
		auto finish = clock();
		div_times.push_back(finish - start);
		
		ignoreit[0] += std::accumulate(bv.begin(), bv.end(), 0);
	}
	
	auto div_time = static_cast<double>(std::accumulate(div_times.begin(),
		div_times.end(), 0));
	std::cout << "Division:\t\t" << div_time << std::endl;
	//Multiplication
	
	for (size_t i = 0; i < loops; i++)
	{
		auto start = clock();
		for (size_t j = 0; j < length - 1; j++)
		{
			b[j] = a[j + 1] * a[j];
		}
		auto finish = clock();
		mul_times.push_back(finish - start);
		ignoreit[1] += std::accumulate(bv.begin(),bv.end(), 0);
	}
	
	auto mul_time = static_cast<double>(std::accumulate(mul_times.begin(),
		mul_times.end(), 0));
	std::cout << "Multiplication:\t\t" << mul_time << std::endl;
	//Addition
	
	for (size_t i = 0; i < loops; i++)
	{
		auto start = clock();
		for (size_t j = 0; j < length - 1; j++)
		{
			b[j] = a[j + 1] + a[j];
		}
		auto finish = clock();
		add_times.push_back(finish - start);
		ignoreit[2] += std::accumulate(bv.begin(), bv.end(), 0);
	}
	
	auto add_time = static_cast<double>(std::accumulate(add_times.begin(), 
		add_times.end(), 0));
	std::cout << "Addition:\t\t" << add_time << std::endl;

	std::cout << "Addition faster than multiplication:\t" << 
		static_cast<double>(mul_time) / static_cast<double>(add_time) << 
		std::endl;
	std::cout << "Multiplication faster than division:\t" << 
		static_cast<double>(div_time) / static_cast<double>(mul_time) << 
		std::endl;
	std::cout << "Just ignore it: " << ignoreit[0]<< ignoreit[1]<< ignoreit[2] 
		<< std::endl << std::endl;
}


void MulVsDiv::BenchAll()
{
	Loop();
	//LoopVectorized();
	DynamicArrayLoop();
	DynamicListLoop();
	//DynamicArrayLoopVectorized();
	//DependendDynamicArrayLoop();
	//CsabaDynamicArrayLoop();
}

MulVsDiv::MulVsDiv()
{
}


MulVsDiv::~MulVsDiv()
{
}