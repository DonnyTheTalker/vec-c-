#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace NVec {

namespace NDetails {

template <typename T>
constexpr bool IsSupported = (
    std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> && std::is_default_constructible_v<T>
);

template <typename T, typename V>
constexpr bool IsConstructable = requires (T a, V b)
{
    a = b;
};

template <typename T, typename V>
constexpr bool IsMoveConstructable = requires (T a, V b)
{
    a = std::move(b);
};

template <typename T, typename V>
constexpr bool IsComparable = requires (T a, V b)
{
    a == b;
};

uint32_t NextPowerOfTwo(uint32_t x) {
    return std::bit_ceil(x);
}

} // namespace NDetails

template <typename T>
requires NDetails::IsSupported<T>
class Vector
{
private:
    template <typename V>
    requires NDetails::IsSupported<V>
    friend class Vector;

public:
    Vector();
    explicit Vector(uint32_t size);
    template <typename V>
    requires NDetails::IsConstructable<T, V>
    Vector(uint32_t size, const V& value);

    template <typename V>
    requires NDetails::IsConstructable<T, V>
    explicit Vector(const Vector<V>& other);

    explicit Vector(const Vector& other);

    template<typename V>
    requires NDetails::IsConstructable<T, V>
    Vector(std::initializer_list<V> other);

    template <typename V>
    requires NDetails::IsMoveConstructable<T, V>
    Vector(Vector<V>&& other) noexcept;

    Vector(Vector<T>&& other) noexcept;

    template <typename V>
    requires NDetails::IsConstructable<T, V>
    Vector& operator=(const Vector<V>& other);

    Vector& operator=(const Vector& other);

    template <typename V>
    requires NDetails::IsMoveConstructable<T, V>
    Vector& operator=(Vector<V>&& other) noexcept;

    Vector& operator=(Vector&& other) noexcept;

    ~Vector();

public:
    uint32_t Size() const;
    uint32_t Capacity() const;
    bool Empty() const;
    void Clear();
    void Reserve(uint32_t size);
    void Resize(uint32_t size);

public:
    template <typename V>
    requires NDetails::IsConstructable<T, V>
    void PushBack(const V& value);

    template <typename V>
    requires NDetails::IsMoveConstructable<T, V>
    void PushBack(V&& value);

    void PopBack();

    template <typename... Args>
    void EmplaceBack(Args&&... args);
    void Swap(Vector& other);

    template <typename V>
    requires NDetails::IsConstructable<T, V>
    void Insert(uint32_t index, const V& value);
    void Erase(uint32_t index);

public:
    T& operator[](uint32_t index);
    const T& operator[](uint32_t index) const;

    T* Begin();
    const T* Begin() const;
    T* End();
    const T* End() const;

    T& Front();
    const T& Front() const;
    T& Back();
    const T& Back() const;

public:
    template <typename V>
    requires NDetails::IsComparable<T, V>
    bool operator==(const Vector<V>& other) const;

    template <typename V>
    requires NDetails::IsComparable<T, V>
    bool operator!=(const Vector<V>& other) const;

private:
    void ReserveIfNeeded();

private:
    uint32_t Size_ = 0;
    uint32_t Capacity_ = 0;
    T* buffer = nullptr;
};


template <typename T>
requires NDetails::IsSupported<T>
Vector<T>::Vector()
    : Size_(0)
    , Capacity_(0)
    , buffer(nullptr)
{
}

template <typename T>
requires NDetails::IsSupported<T>
Vector<T>::Vector(uint32_t size)
    : Size_(size)
    , Capacity_(0 == size ? 0 : NDetails::NextPowerOfTwo(size))
    , buffer(0 == Size_ ? nullptr : new T[Size_])
{
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsConstructable<T, V>
Vector<T>::Vector(uint32_t size, const V& value)
    : Size_(size)
    , Capacity_(0 == size ? 0 : NDetails::NextPowerOfTwo(size))
    , buffer(0 == Size_ ? nullptr : new T[Size_])
{
    for (uint32_t i = 0; i < size; ++i) {
        buffer[i] = value;
    }
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsConstructable<T, V>
Vector<T>::Vector(const Vector<V>& other)
    : Size_(other.Size_)
    , Capacity_(other.Capacity_)
    , buffer(0 == Size_ ? nullptr : new T[Size_])
{
    for (uint32_t i = 0; i < Size_; ++i) {
        buffer[i] = other.buffer[i];
    }
}

template <typename T>
requires NDetails::IsSupported<T>
Vector<T>::Vector(const Vector& other)
    : Size_(other.Size_)
    , Capacity_(other.Capacity_)
    , buffer(0 == Size_ ? nullptr : new T[Size_])
{
    for (uint32_t i = 0; i < Size_; ++i) {
        buffer[i] = other.buffer[i];
    }
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsConstructable<T, V>
Vector<T>::Vector(std::initializer_list<V> other)
    : Size_(other.size())
    , Capacity_(0 == other.size() ? 0 : NDetails::NextPowerOfTwo(other.size()))
    , buffer(0 == Size_ ? nullptr : new T[Size_])
{
    uint32_t i = 0;
    for (const auto& value : other) {
        buffer[i++] = value;
    }
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsMoveConstructable<T, V>
Vector<T>::Vector(Vector<V>&& other) noexcept
    : Size_(other.Size_)
    , Capacity_(other.Capacity_)
    , buffer(0 == Size_ ? nullptr : new T[Size_])
{
    for (size_t i = 0; i < Size_; ++i) {
        buffer[i] = std::move(other.buffer[i]);
    }
    other.Clear();
}

template <typename T>
requires NDetails::IsSupported<T>
Vector<T>::Vector(Vector<T>&& other) noexcept
    : Size_(other.Size_)
    , Capacity_(other.Capacity_)
    , buffer(other.buffer)
{
    other.Size_ = 0;
    other.Capacity_ = 0;
    other.buffer = nullptr;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsConstructable<T, V>
Vector<T>& Vector<T>::operator=(const Vector<V>& other)
{
    Clear();
    Size_ = other.Size_;
    Capacity_ = other.Capacity_;
    buffer = new T[Size_];
    for (uint32_t i = 0; i < Size_; ++i) {
        buffer[i] = other.buffer[i];
    }
    return *this;
} 

template <typename T>
requires NDetails::IsSupported<T>
Vector<T>& Vector<T>::operator=(const Vector& other)
{
    if (this != &other) {
        Clear();
        Size_ = other.Size_;
        Capacity_ = other.Capacity_;
        buffer = new T[Size_];
        for (uint32_t i = 0; i < Size_; ++i) {
            buffer[i] = other.buffer[i];
        }
    }
    return *this;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsMoveConstructable<T, V>
Vector<T>& Vector<T>::operator=(Vector<V>&& other) noexcept
{
    Clear();
    Size_ = other.Size_;
    Capacity_ = other.Capacity_;
    buffer = new T[Size_];
    for (uint32_t i = 0; i < Size_; ++i) {
        buffer[i] = std::move(other.buffer[i]);
    }
    other.Clear();
    return *this;
}

template <typename T>
requires NDetails::IsSupported<T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept
{
    if (this != &other) {
        Clear();
        Size_ = other.Size_;
        Capacity_ = other.Capacity_;
        buffer = other.buffer;
        other.buffer = nullptr;
        other.Clear();
    }
    return *this;
}

template <typename T>
requires NDetails::IsSupported<T>
uint32_t Vector<T>::Size() const
{
    return Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
uint32_t Vector<T>::Capacity() const
{
    return Capacity_;
}

template <typename T>
requires NDetails::IsSupported<T>
bool Vector<T>::Empty() const
{
    return 0 == Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::Reserve(uint32_t newCapacity)
{
    if (newCapacity > Capacity_) {
        newCapacity = NDetails::NextPowerOfTwo(newCapacity);
        T* newBuffer = new T[newCapacity];
        for (uint32_t i = 0; i < Size_; ++i) {
            if constexpr (std::is_move_assignable_v<T>) {
                newBuffer[i] = std::move(buffer[i]);
            } else {
                newBuffer[i] = buffer[i];
            }
        }
        delete[] buffer;
        buffer = newBuffer;
        Capacity_ = newCapacity;
    }
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::Resize(uint32_t newSize)
{
    if (newSize > Capacity_) {
        Reserve(NDetails::NextPowerOfTwo(newSize));
    }
    Size_ = newSize;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsConstructable<T, V>
void Vector<T>::PushBack(const V& value)
{
    ReserveIfNeeded();
    buffer[Size_++] = value;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsMoveConstructable<T, V>
void Vector<T>::PushBack(V&& value)
{
    ReserveIfNeeded();
    buffer[Size_++] = std::forward<V>(value);
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::PopBack()
{
    assert(Size_ > 0 && "Vector is empty");
    --Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename... Args>
void Vector<T>::EmplaceBack(Args&&... args)
{
    ReserveIfNeeded();
    buffer[Size_++] = T(std::forward<Args>(args)...);
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::Swap(Vector<T>& other)
{
    std::swap(Size_, other.Size_);
    std::swap(Capacity_, other.Capacity_);
    std::swap(buffer, other.buffer);
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsConstructable<T, V>
void Vector<T>::Insert(uint32_t index, const V& value)
{
    assert(index <= Size_ && "Out of bounds");

    ReserveIfNeeded();
    for (int i = Size_; i > index; --i) {
        buffer[i] = buffer[i - 1];
    }
    buffer[index] = value;
    ++Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::Erase(uint32_t index)
{
    assert(index < Size_ && "Out of bounds");
    for (uint32_t i = index; i < Size_ - 1; ++i) {
        buffer[i] = buffer[i + 1];
    }
    --Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
T& Vector<T>::operator[](uint32_t index)
{
    assert(index < Size_ && "Out of bounds");
    return buffer[index];
}

template <typename T>
requires NDetails::IsSupported<T>
const T& Vector<T>::operator[](uint32_t index) const
{
    assert(index < Size_ && "Out of bounds");
    return buffer[index];
}

template <typename T>
requires NDetails::IsSupported<T>
T* Vector<T>::Begin()
{
    return buffer;
}

template <typename T>
requires NDetails::IsSupported<T>
const T* Vector<T>::Begin() const
{
    return buffer;
}

template <typename T>
requires NDetails::IsSupported<T>
T* Vector<T>::End()
{
    return buffer + Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
const T* Vector<T>::End() const
{
    return buffer + Size_;
}

template <typename T>
requires NDetails::IsSupported<T>
T& Vector<T>::Front()
{
    assert(Size_ > 0 && "Vector is empty");
    return buffer[0];
}

template <typename T>
requires NDetails::IsSupported<T>
const T& Vector<T>::Front() const
{
    assert(Size_ > 0 && "Vector is empty");
    return buffer[0];
}

template <typename T>
requires NDetails::IsSupported<T>
T& Vector<T>::Back()
{
    assert(Size_ > 0 && "Vector is empty");
    return buffer[Size_ - 1];
}

template <typename T>
requires NDetails::IsSupported<T>
const T& Vector<T>::Back() const
{
    assert(Size_ > 0 && "Vector is empty");
    return buffer[Size_ - 1];
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::Clear()
{
    Size_ = 0;
    delete[] buffer;
    buffer = nullptr;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsComparable<T, V>
bool Vector<T>::operator==(const Vector<V>& other) const
{
    if (Size_ != other.Size_) {
        return false;
    }
    for (uint32_t i = 0; i < Size_; ++i) {
        if (buffer[i] != other[i]) {
            return false;
        }
    }
    return true;
}

template <typename T>
requires NDetails::IsSupported<T>
template <typename V>
requires NDetails::IsComparable<T, V>
bool Vector<T>::operator!=(const Vector<V>& other) const
{
    return !(*this == other);
}

template <typename T>
requires NDetails::IsSupported<T>
Vector<T>::~Vector()
{
    Clear();
}

template <typename T>
requires NDetails::IsSupported<T>
void Vector<T>::ReserveIfNeeded()
{
    if (Size_ == Capacity_) {
        Reserve(NDetails::NextPowerOfTwo(Size_ + 1));
    }
}

} // namespace NVec
