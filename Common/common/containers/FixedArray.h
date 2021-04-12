#pragma once
#include "common/Typedefs.h"

//Wraps fixed size c-arrays via a pointer to their start. Supports range based for loops. Ex: for (auto& element : someArray)
template<class T>
class FixedArray
{
public:
    FixedArray() = default;
    FixedArray(T*& arrayStart, int capacity, int NumberOfElements_) : arrayStart_(arrayStart), capacity_(capacity), numberOfElements_(NumberOfElements_)
    {
        initialized_ = true;
    }

    void Init(T*& arrayStart, int capacity, int numberOfElements, std::string name) //Needed for arrays who's locations are only known at runtime.
    {
        arrayStart_ = arrayStart;
        capacity_ = capacity;
        numberOfElements_ = numberOfElements;
        name_ = name;
        initialized_ = true;
    }
    T& operator[](int index)
    {
        if (!initialized_)
        {
            throw std::runtime_error("Error! Tried accessing" + name_ + "before it was initialized. You can use .Initialized() to check if this container is initialized before using it.");
        }
        return arrayStart_[index];
    }

    [[nodiscard]] int Size() const { return numberOfElements_; }
    [[nodiscard]] int Length() const { return numberOfElements_; }
    [[nodiscard]] int Capacity() const { return capacity_; }
    [[nodiscard]] bool Initialized() const { return initialized_; }
    [[nodiscard]] T* GetRawPointer() const { return arrayStart_; };

    T* begin() const { return arrayStart_; }
    T* end() const { return arrayStart_ + numberOfElements_; };

private:
    T* arrayStart_ = nullptr;
    int capacity_ = 0;
    int numberOfElements_ = 0;
    bool initialized_ = false;
    string name_ = "Default";
};