#pragma once

//Todo: Make sure the member functions of base_array<T> are working properly
template<class T>
class base_array
{
public:
    T& operator[](int Index)
    {
        return Elements[Index];
    }
    int Size() { return NumberOfElements; }
    int Length() { return NumberOfElements; }
    int Capacity() { return ArraySize; }

    //Returns pointer to start of elements
    T* begin() { return Elements; }

    //Returns pointer to start of elements
    const T* begin() const { return Elements; }

    //Returns pointer to end of elements
    //To get the last member, use back() instead.
    T* end() { return Elements + ArraySize; }

    //Returns pointer to end of elements
    //To get the last member, use back() instead.
    const T* end() const { return Elements + ArraySize; }

    //Returns reference to first member of elements
    T& front() { return *Elements; }

    //Returns reference to first member of elements
    const T& front() const { return *Elements; }

    //Returns reference to last member of elements
    T& back() { return Elements[ArraySize - 1]; }

    //Returns reference to last member of elements
    const T& back() const { return Elements[ArraySize - 1]; }

private:
    void* VirtualFunctionPointer;
    T* Elements;
    int ArraySize;
    int NumberOfElements;
};

//Todo: Reverse the constructor for this to make sure this is correct.
//Todo: Make sure that the index operator [] is working for this.
template <class T, unsigned int InitialSize>
class farray : public base_array<T>
{
public:
    T data[InitialSize];
};

template <class T>
class Array
{
public:
    const T& operator[](int Index)
    {
        return Data[Index];
    }
    unsigned int GetSize() { return Size; }
private:
    T* Data;
    unsigned int Size;
};

template <class T>
class pool_list : base_array<T>
{
    bool DynamicInit;
    unsigned int MinUsed;
};

template <class T, unsigned int U>
class circ_array
{
    T data[U];
    int num = U;
    int offset = 0;
};

template<class T>
class object_stream_resource
{
    T m_data;
    bool m_loaded = false;
};

template<class T>
class linked_list_p
{
    T* m_head_p;
    int m_elem_count;
};


//et_sized_native_pointer
template<class T>
class et_sized_native_pointer_storage_type_pointer_type
{
    T* m_pointer;
    void* m_pointer_padding;
};

template<class T>
union et_sized_native_pointer_storage_type
{
    et_sized_native_pointer_storage_type_pointer_type<T> m_pointer_type;
    unsigned __int64 m_pointer_type_padding;
};

template<class T>
class et_sized_native_pointer
{
    et_sized_native_pointer_storage_type<T> m_storage;
};