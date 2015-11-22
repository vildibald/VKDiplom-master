#include "stdafx.h"
#include "MulVsDiv.h"
#include <ostream>
#include <iostream>
#include <locale>

void MulVsDiv::Loop()
{
	const int length = 512;
	const int loops = 10e6;
	std::cout << "Loop: " << std::endl;
	double a[length], b[length],c[length];
	for (size_t i = 0; i < length; i++)
	{
		a[i] = rand() % 2048;
		b[i] = rand() % 2048;
		c[i] = rand() % 2048;
	}

	auto start = clock();
	for (size_t l = 0; l < loops; l++)
	{
		// MSVC cannot vectorize this loop (message 1300).
		// However, if this loop will not be nested in, autovectorization will happen.
		// Same condition apply for mul/div/rcp loops

		// ICL does not have this issue, but to provide both vectorized and nonvectorized comparison 
		// i specifically disabled vectorization in this method
#pragma novector
		for (int i = 0; i < length; ++i)
		{
			c[i] = a[i] + b[i];
		}
	}
	auto add_time = clock() - start;
	std::cout << "Addition: " << add_time << std::endl;

	start = clock();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
		for (int i = 0; i < length; ++i)
		{
			c[i] = a[i] * b[i];
		}
	}
	auto mul_time = clock() - start;
	std::cout << "Multiplication: " << mul_time << std::endl;

	start = clock();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
		for (int i = 0; i < length; ++i)
		{
			c[i] = a[i] / b[i];
		}
	}
	auto div_time = clock() - start;
	std::cout << "Division: " << div_time << std::endl;

	start = clock();
	for (size_t l = 0; l < loops; l++)
	{
#pragma novector
		for (int i = 0; i < length; i++)
		{
			c[i] = 1 / a[i];
		}
	}
	auto rcp_time = clock() - start;
	std::cout << "Reciprocal: " << rcp_time << std::endl << std::endl;

	std::cout << "Addition faster than multiplication: " << static_cast<double>(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>(div_time) / static_cast<double>(mul_time) << std::endl ;
	std::cout << "Multiplication faster than reciprocal: " << static_cast<double>(rcp_time) / static_cast<double>(mul_time) << std::endl << std::endl;
	std::cout << "Just ignore it: " << c[7] + a[8] + b[1] << std::endl << std::endl;
}

void MulVsDiv::LoopVectorized()
{
	const int length = 512;
	const int loops = 10e6;
	std::cout << "Vectorized loop: " << std::endl;
	double a[length], b[length],c[length];
	for (size_t i = 0; i < length; i++)
	{
		a[i] = rand() % 2048;
		b[i] = rand() % 2048;
		c[i] = rand() % 2048;
	}

	int l = 0;
	auto start = clock();
add:
	for (int i = 0; i < length; i++)
	{
		c[i] = a[i] + b[i];
	}
	// MSVC doesn't vectorize nested loops (message 1300 - too little computation to vectorize) as mentioned on line 23 in function 'Loop'.
	// However if nested loop is replaced with this nasty workaround SIMD vectorization will happen.
	// Same condition apply for mul/div/rcp loops
	while (l < loops)
	{
		++l;
		goto add;
	}
	auto add_time = clock() - start;
	std::cout << "Addition: " << add_time << std::endl;

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
	std::cout << "Division: " << div_time << std::endl;

	l = 0;
	start = clock();
rcp:
	for (int i = 0; i < length; i++)
	{
		c[i] = 1 / a[i];
	}
	while (l < loops)
	{
		++l;
		goto rcp;
	}

	auto rcp_time = clock() - start;
	std::cout << "Reciprocal: " << rcp_time << std::endl << std::endl;

	std::cout << "Addition faster than multiplication: " << static_cast<double>(mul_time) / static_cast<double>(add_time) << std::endl;
	std::cout << "Multiplication faster than division: " << static_cast<double>(div_time) / static_cast<double>(mul_time) << std::endl;
	std::cout << "Multiplication faster than reciprocal: " << static_cast<double>(rcp_time) / static_cast<double>(mul_time) << std::endl << std::endl;
	std::cout << "Just ignore it: " << c[7] + a[8] + b[1] << std::endl << std::endl;
}


void MulVsDiv::BenchAll()
{
	Loop();
	LoopVectorized();
}

MulVsDiv::MulVsDiv()
{
}


MulVsDiv::~MulVsDiv()
{
}
