#include "stdafx.h"
#include "ReducedCurveDeboorGenerator.h"
#include "utils.h"


splineknots::ReducedCurveDeboorKnotsGenerator::ReducedCurveDeboorKnotsGenerator(const splineknots::MathFunction& function): CurveDeboorKnotsGenerator(function)
{
}

splineknots::ReducedCurveDeboorKnotsGenerator::ReducedCurveDeboorKnotsGenerator(const splineknots::InterpolativeMathFunction& function): CurveDeboorKnotsGenerator(function)
{
}

splineknots::KnotVector splineknots::ReducedCurveDeboorKnotsGenerator::RightSide(const splineknots::KnotVector& function_values, double h, double dfirst, double dlast)
{
	int n1, n2, N = function_values.size();
	n1 = (N % 2 == 0) ? (N - 2) / 2 : (N - 3) / 2;
	n2 = (N % 2 == 0) ? (N - 2) : (N - 3);

	splineknots::KnotVector rhs(n1);
	double mu1 = 3.0 / h;
	double mu2 = 4.0 * mu1;
	int k = -1;
	for (int i = 2; i < n2; i+=2)
	{
		++k;
		rhs[k] = mu1 * (function_values[i + 1] - function_values[i - 2]) - mu2 * (function_values[i + 1] - function_values[i - 1]);
	}
	rhs[0] -= dfirst;
	rhs[n1 - 1] -= dlast;
	return rhs;
}

splineknots::KnotVector splineknots::ReducedCurveDeboorKnotsGenerator::GenerateKnots(const splineknots::SurfaceDimension& dimension)
{
	splineknots::KnotVector knots(dimension.knot_count);
	InitializeKnots(dimension, knots);
	auto dfirst = Function().Dx()(dimension.min, 0);
	auto dlast = Function().Dx()(dimension.max, 0);
	auto h = abs(dimension.max - dimension.min) / (dimension.knot_count - 1);
	auto rhs = RightSide(knots, h, dfirst, dlast);

	utils::SolveDeboorTridiagonalSystem(1, -14, 1, &rhs.front(), rhs.size(), -15);

	splineknots::KnotVector result(knots.size());
	result[0] = dfirst;
	result[result.size() - 1] = dlast;

	for (int i = 0; i < rhs.size(); i++)
	{
		result[2 * (i + 1)] = rhs[i];
	}
	auto mu1 = 3.0 / h;
	for (int i = 1; i < result.size(); i += 2)
	{
		result[i] = 0.25 * (mu1 * (knots[i + 1] - knots[i - 1]) - result[i - 1] - result[i + 1]);
	}

	return result;
}
