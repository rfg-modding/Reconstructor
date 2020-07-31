#pragma once
#include "Vector.h"

class matrix //3x3
{
	//friend std::ostream& operator<<(std::ostream& os, const matrix& obj)
	//{
	//	return os
	//		<< "rvec: " << obj.rvec
	//		<< "uvec: " << obj.uvec
	//		<< "fvec: " << obj.fvec;
	//}

public:
	matrix()
	{
		rvec.SetAll(0.0f);
		uvec.SetAll(0.0f);
		fvec.SetAll(0.0f);
	}
	matrix(const matrix& CopyMatrix)
	{
		rvec = CopyMatrix.rvec;
		uvec = CopyMatrix.uvec;
		fvec = CopyMatrix.fvec;
	}
	matrix(float InitialValue)
	{
		rvec.SetAll(InitialValue);
		uvec.SetAll(InitialValue);
		fvec.SetAll(InitialValue);
	}
	matrix(vector Right, vector Up, vector Forward)
	{
		rvec = Right;
		uvec = Up;
		fvec = Forward;
	}
	matrix operator+(const matrix& B)
	{
		return matrix(rvec + B.rvec, uvec + B.uvec, fvec + B.fvec);
	}
	matrix operator-(const matrix& B)
	{
		return matrix(rvec - B.rvec, uvec - B.uvec, fvec - B.fvec);
	}
	bool operator==(const matrix& B)
	{
		return(rvec == B.rvec && uvec == B.uvec && fvec == B.fvec);
	}
	bool operator!=(const matrix& B)
	{
		return !(*this == B);
	}
	matrix operator=(const matrix& B)
	{
		rvec = B.rvec;
		uvec = B.uvec;
		fvec = B.fvec;
		return *this;
	}
	matrix Abs()
	{
		return matrix(rvec.Abs(), uvec.Abs(), fvec.Abs());
	}
	matrix SetAll(float Value)
	{
		rvec.SetAll(Value);
		uvec.SetAll(Value);
		fvec.SetAll(Value);
		return *this;
	}
	//std::string ToString(bool Parentheses, bool Labels)
	//{
	//	return fmt::format("rvec: {}\nuvec: {}\nfvec: {}", rvec.GetDataString(Parentheses, Labels), uvec.GetDataString(Parentheses, Labels), fvec.GetDataString(Parentheses, Labels));
	//}

	vector rvec;
	vector uvec;
	vector fvec;
};