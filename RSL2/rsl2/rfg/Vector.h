#pragma once
#include <sstream>

//Todo: Move this into the String:: helper namespace
namespace rfg
{
	template <typename T>
	std::string to_string_precise(const T Value, const int Precision = 1)
	{
		std::ostringstream Out;
		Out.precision(Precision);
		Out << std::fixed << Value;
		return Out.str();
	}
}

class vector
{
	//friend std::ostream& operator<<(std::ostream& os, const vector& obj)
	//{
	//	return os
	//		<< "x: " << obj.x
	//		<< "y: " << obj.y
	//		<< "z: " << obj.z;
	//}

public:
	vector()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};
	vector(const vector& CopyVector)
	{
		x = CopyVector.x;
		y = CopyVector.y;
		z = CopyVector.z;
	}
	vector(float InitialValue) : x(InitialValue), y(InitialValue), z(InitialValue) { }
	vector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) { }
	vector operator+(const vector& B)
	{
		return vector(x + B.x, y + B.y, z + B.z);
	}
	vector operator-(const vector& B)
	{
		return vector(x - B.x, y - B.y, z - B.z);
	}
	float operator*(const vector& B) //Dot product
	{
		return (x * B.x) + (y * B.y) + (z * B.z);
	}
	vector& operator+=(const vector& B)
	{
		x += B.x;
		y += B.y;
		z += B.z;
		return *this;
	}
	bool operator==(const vector& B)
	{
		return (x == B.x && y == B.y && z == B.z);
	}
	bool operator!=(const vector& B)
	{
		return !(*this == B);
	}
	vector& operator=(const vector& B)
	{
		x = B.x;
		y = B.y;
		z = B.z;
		return *this;
	}
	vector Cross(const vector& B)
	{
		vector C;
		C.x = (y * B.z) - (B.y * z);
		C.y = (B.x * z) - (y * B.z);
		C.z = (x * B.y) - (B.x * y);
		return C;
	}
	float Magnitude()
	{
		return sqrtf((x * x) + (y * y) + (z * z));
	}
	vector Scale(const float& Multiplier)
	{
		x *= Multiplier;
		y *= Multiplier;
		z *= Multiplier;
		return *this;
	}
	vector SetAll(float Value)
	{
		x = Value;
		y = Value;
		z = Value;
		return *this;
	}
	vector UnitVector()
	{
		vector UnitVec = *this;
		float Magnitude = this->Magnitude();
		UnitVec.Scale(1.0f / Magnitude);
		return UnitVec;
	}
	vector Midpoint(vector& B)
	{
		vector Mid;
		Mid.x = (this->x + B.x) / 2.0f;
		Mid.y = (this->y + B.y) / 2.0f;
		Mid.z = (this->z + B.z) / 2.0f;
		return Mid;
	}
	vector Lerp(vector& B, float parameter)
	{
		vector NewPos;
		NewPos.x = this->x + parameter * (B.x - this->x);
		NewPos.y = this->y + parameter * (B.y - this->y);
		NewPos.z = this->z + parameter * (B.z - this->z);
		return NewPos;
	}
	vector Abs()
	{
		return vector(abs(x), abs(y), abs(z));
	}
	std::string GetDataString(bool Parentheses, bool Labels)
	{
		std::string String;
		if (Parentheses)
		{
			String += "(";
		}
		if (Labels)
		{
			String += "x: ";

		}
		String += rfg::to_string_precise(x, 2);
		String += ", ";
		if (Labels)
		{
			String += "y: ";

		}
		String += rfg::to_string_precise(y, 2);
		String += ", ";
		if (Labels)
		{
			String += "z: ";
		}
		String += rfg::to_string_precise(z, 2);
		if (Parentheses)
		{
			String += ")";
		}
		return String;
	}

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};