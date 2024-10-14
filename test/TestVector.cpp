#include "gtest/gtest.h"
#include "vec.h"

namespace NVec::NTest {

using namespace NVec;
    
TEST(TestVector, TestPrimitiveConstructors)
{
    {
        EXPECT_NO_THROW(Vector<int>());
        Vector<int> v;
        EXPECT_EQ(v.Size(), 0);
        EXPECT_EQ(v.Capacity(), 0);
        EXPECT_TRUE(v.Empty());
    }

    {
        EXPECT_NO_THROW(Vector<int>(8));
        Vector<int> v(8);
        EXPECT_EQ(v.Size(), 8);
        EXPECT_EQ(v.Capacity(), 8);
        EXPECT_FALSE(v.Empty());

        for (uint32_t i = 0; i < 8; ++i) {
            EXPECT_EQ(v[i], 0);
        }
    }

    {
        EXPECT_NO_THROW(Vector<int>(8, 1));
        Vector<int> v(8, 1);
        EXPECT_EQ(v.Size(), 8);
        EXPECT_EQ(v.Capacity(), 8);
        EXPECT_FALSE(v.Empty());

        for (uint32_t i = 0; i < 8; ++i) {
            EXPECT_EQ(v[i], 1);
        }
    }

    {
        EXPECT_NO_THROW(Vector<std::string>(8));
        Vector<std::string> v(8);
        EXPECT_EQ(v.Size(), 8);
        EXPECT_EQ(v.Capacity(), 8);
        EXPECT_FALSE(v.Empty());

        for (uint32_t i = 0; i < 8; ++i) {
            EXPECT_EQ(v[i], "");
        }
    }
}

TEST(TestVector, TestCapacityUpscaling)
{
    {
        EXPECT_NO_THROW(Vector<int>(7));
        Vector<int> v(7);
        EXPECT_EQ(v.Size(), 7);
        EXPECT_EQ(v.Capacity(), 8);
    }

    {
        EXPECT_NO_THROW(Vector<int>(8));
        Vector<int> v(8);
        EXPECT_EQ(v.Size(), 8);
        EXPECT_EQ(v.Capacity(), 8);
        EXPECT_FALSE(v.Empty());
    }

    {
        EXPECT_NO_THROW(Vector<int>(9));
        Vector<int> v(9);
        EXPECT_EQ(v.Size(), 9);
        EXPECT_EQ(v.Capacity(), 16);
    }

    {
        EXPECT_NO_THROW(Vector<int>(0));
        Vector<int> v(0);
        EXPECT_EQ(v.Size(), 0);
        EXPECT_EQ(v.Capacity(), 0);
    }

    {
        EXPECT_NO_THROW(Vector<int>());
        Vector<int> v;
        EXPECT_EQ(v.Size(), 0);
        EXPECT_EQ(v.Capacity(), 0);
    }
}

TEST(TestVector, TestOutOfBounds) 
{
    {
        Vector<int> v(8);
        EXPECT_DEATH(v[8], "Out of bounds");
        EXPECT_DEATH(v[-1], "Out of bounds");
        EXPECT_NO_THROW(v[0]);
    }
}

TEST(TestVector, TestIndexing)
{
    {
        Vector<int> v(8);
        for (uint32_t i = 0; i < 8; ++i) {
            v[i] = i;
        }
        for (uint32_t i = 0; i < 8; ++i) {
            EXPECT_EQ(v[i], i);
        }
        v[0] = 3;
        EXPECT_EQ(v[0], 3);
    }
}

TEST(TestVector, TestIterators)
{
    {
        Vector<int> v(8);
        for (uint32_t i = 0; i < 8; ++i) {
            v[i] = i;
        }
        uint32_t i = 0;
        for (auto it = v.Begin(); it != v.End(); ++it) {
            EXPECT_EQ(*it, i);
            ++i;
        }
        for (uint32_t i = 0; i < 8; i++) {
            v[7 - i] = i;
        }
        std::sort(v.Begin(), v.End());
        i = 0;
        for (auto it = v.Begin(); it != v.End(); ++it) {
            EXPECT_EQ(*it, i);
            ++i;
        }
    }
}

TEST(TestVector, TestCopyConstructor)
{
    {
        Vector<int> v(8);
        for (uint32_t i = 0; i < 8; ++i) {
            v[i] = i;
        }
        Vector<int> w(v);
        EXPECT_EQ(v, w);
    }
}

TEST(TestVector, TestConstructorTypeConversion)
{
    struct y {
        char a = 0;
    };

    struct x {
        int a = 0;
        x& operator=(const y& other) {
            a = other.a - 'a';
            return *this;
        }
    };

    {
        Vector<x> v(3, y{.a = 'b'});
        EXPECT_EQ(v.Size(), 3);
        EXPECT_EQ(v.Capacity(), 4);
        for (uint32_t i = 0; i < 3; ++i) {
            EXPECT_EQ(v[i].a, 1);
        }
    }

    {
        Vector<y> v(3, y{.a = 'b'});
        Vector<x> w(v);
        EXPECT_EQ(w.Size(), 3);
        EXPECT_EQ(w.Size(), v.Size());
        EXPECT_EQ(w.Capacity(), 4);
        for (uint32_t i = 0; i < 3; ++i) {
            EXPECT_EQ(w[i].a, 1);
        }
    }
}

TEST(TestVector, TestInitializerListConstructor)
{
    {
        Vector<int> v = {1, 2, 3, 4, 5};
        EXPECT_EQ(v.Size(), 5);
        EXPECT_EQ(v.Capacity(), 8);
        for (uint32_t i = 0; i < 5; ++i) {
            EXPECT_EQ(v[i], i + 1);
        }
    }

    {
        Vector<int> v = {};
        EXPECT_EQ(v.Size(), 0);
        EXPECT_EQ(v.Capacity(), 0);
        EXPECT_TRUE(v.Empty());
    }
}

TEST(TestVector, TestMoveConstructor)
{
    {
        Vector<int> v(8, 1);
        EXPECT_EQ(v.Size(), 8);
        EXPECT_EQ(v.Capacity(), 8);

        Vector<int> w(std::move(v));
        EXPECT_EQ(w.Size(), 8);
        EXPECT_EQ(w.Capacity(), 8);
        EXPECT_FALSE(w.Empty());
        
        EXPECT_EQ(v.Size(), 0);
        EXPECT_EQ(v.Capacity(), 0);
        EXPECT_TRUE(v.Empty());
    }

    {
        Vector<int> v(2, 3);
        v[1] = 5;
        Vector<int> w(std::move(v));

        EXPECT_EQ(w.Size(), 2);
        EXPECT_EQ(w.Capacity(), 2);
        EXPECT_EQ(w[0], 3);
        EXPECT_EQ(w[1], 5);
    }

    struct X {
        X() = default;
        X(const X&) {
            copyied = true;
        }
        X& operator=(const X&) {
            copyied = true;
            return *this;
        }

        bool copyied = false;
    };

    {
        Vector<X> v(1);
        EXPECT_FALSE(v[0].copyied);
        
        Vector<X> w(std::move(v));
        EXPECT_FALSE(w[0].copyied);
    }

    {
        Vector<float> v(3, 14.6f);
        Vector<int> w(std::move(v));

        EXPECT_EQ(w.Size(), 3);
        EXPECT_EQ(v.Size(), 0);
    }
}

TEST(TestVector, TestFront)
{
    {
        Vector<int> v(8, 1);
        EXPECT_EQ(v.Front(), 1);
    }

    {
        Vector<int> v(8, 1);
        v[1] = 3;
        EXPECT_EQ(v.Front(), 1);
        v[0] = 3;
        EXPECT_EQ(v.Front(), 3);
    }

    {
        Vector<int> v;
        EXPECT_DEATH(v.Front(), "Vector is empty");
    }
}

TEST(TestVector, TestBack)
{
    {
        Vector<int> v(8, 1);
        EXPECT_EQ(v.Back(), 1);
        v[7] = 3;
        EXPECT_EQ(v.Back(), 3);
    }

    {
        Vector<int> v(8, 1);
        EXPECT_EQ(v.Back(), 1);
    }

    {
        Vector<int> v;
        EXPECT_DEATH(v.Back(), "Vector is empty");
    }
}

TEST(TestVector, TestClear)
{
    {
        Vector<int> v(8, 1);
        v.Clear();
        EXPECT_EQ(v.Size(), 0);
        EXPECT_TRUE(v.Empty());
    }
}

TEST(TestVector, TestCopyAssign)
{
    {
        Vector<int> v(8, 1);
        v[1] = 3;
        v[2] = 5;
        v[3] = 7;

        Vector<int> w(1);
        w[0] = 1;
        w = v;

        EXPECT_EQ(w, v);
    }

    {
        Vector<int> v(8, 1);
        v[1] = 3;
        v[2] = 5;
        v[3] = 7;

        Vector<float> w(3, 4);
        w = v;

        EXPECT_EQ(w, v);
    }
}

TEST(TestVector, TestMoveAssign)
{
    {
        Vector<int> v(8, 1);
        v[1] = 3;
        v[2] = 5;
        Vector<int> w;
        w = std::move(v);

        EXPECT_EQ(w[0], 1);
        EXPECT_EQ(w[1], 3);
        EXPECT_EQ(w[2], 5);
        EXPECT_EQ(w.Size(), 8);

        EXPECT_EQ(v.Size(), 0);
        EXPECT_TRUE(v.Empty());
    }

    {
        Vector<int> v(8, 1);
        v[1] = 3;
        v[2] = 5;

        Vector<float> w(3, 4);
        w = std::move(v);

        EXPECT_EQ(w[0], 1);
        EXPECT_EQ(w[1], 3);
        EXPECT_EQ(w[2], 5);
        EXPECT_EQ(w.Size(), 8);
    }
}

TEST(TestVector, TestComparison)
{
    {
        Vector<int> v(8, 1);
        v[1] = 3;
        v[2] = 5;

        Vector<int> w(8, 1);
        w[1] = 3;
        w[2] = 5;

        EXPECT_EQ(v, w);
    }

    {
        Vector<int> v(8, 1);
        v[1] = 3;
        v[2] = 5;

        Vector<int> w(8, 1);
        w[1] = 3;
        w[2] = 6;

        EXPECT_NE(v, w);
    }

    {
        Vector<int> v(2, 1);
        v[0] = 3;
        v[1] = 5;

        Vector<int> w(3, 1);
        w[0] = 3;
        w[1] = 5;

        EXPECT_NE(v, w);
    }

    {
        Vector<float> v(2, 1);
        v[0] = 3;
        v[1] = 5;

        Vector<int> w(2, 1);
        w[0] = 3;
        w[1] = 5;

        EXPECT_EQ(v, w);
    }
}

TEST(TestVector, TestReserve)
{
    {
        Vector<int> t;
        t.Reserve(10);
        EXPECT_EQ(t.Capacity(), 16);
        EXPECT_EQ(t.Size(), 0);
    }

    {
        Vector<int> t(10, 1);
        EXPECT_EQ(t.Capacity(), 16);
        t.Reserve(10);
        EXPECT_EQ(t.Capacity(), 16);
        t.Reserve(17);
        EXPECT_EQ(t.Capacity(), 32);
        t.Reserve(10);
        EXPECT_EQ(t.Capacity(), 32);
    }

    struct nonMovable {
        nonMovable() = default;
        nonMovable(nonMovable&&) = delete;
        nonMovable& operator=(nonMovable&&) = delete;
        nonMovable(const nonMovable&) {
            copyied = true;
        }
        nonMovable& operator=(const nonMovable&) {
            copyied = true;
            return *this;
        }

        bool copyied = false;
    };

    {
        Vector<nonMovable> t(10);
        EXPECT_EQ(t.Capacity(), 16);
        EXPECT_FALSE(t[0].copyied);
        t.Reserve(17);
        EXPECT_EQ(t.Capacity(), 32);
        EXPECT_TRUE(t[0].copyied);
    }
}

TEST(TestVector, TestResize)
{
    {
        Vector<int> t(10, 1);
        EXPECT_EQ(t.Capacity(), 16);
        EXPECT_EQ(t.Size(), 10);
        t.Resize(17);
        EXPECT_EQ(t.Capacity(), 32);
        EXPECT_EQ(t.Size(), 17);
        t.Resize(10);
        EXPECT_EQ(t.Capacity(), 32);
        EXPECT_EQ(t.Size(), 10);
    }
}

TEST(TestVector, TestPushBack)
{
    {
        Vector<int> t(10, 1);
        EXPECT_EQ(t.Capacity(), 16);
        EXPECT_EQ(t.Size(), 10);
        t.PushBack(1);
        EXPECT_EQ(t.Size(), 11);
    }

    {
        Vector<int> t;
        EXPECT_EQ(t.Capacity(), 0);
        EXPECT_EQ(t.Size(), 0);
        t.PushBack(1);
        EXPECT_EQ(t.Size(), 1);
        EXPECT_EQ(t.Capacity(), 1);
        t.PushBack(2);
        EXPECT_EQ(t.Size(), 2);
        EXPECT_EQ(t.Capacity(), 2);
        t.PushBack(3);
        EXPECT_EQ(t.Size(), 3);
        EXPECT_EQ(t.Capacity(), 4);
    }

    {
        Vector<int> t;
        t.PushBack(14.6);
        EXPECT_EQ(t.Size(), 1);
    }

    struct y {
        char a = 0;
    };

    struct x {
        int a = 0;
        x& operator=(const y& other) {
            a = other.a - 'a';
            return *this;
        }
    };

    {
        Vector<x> t;
        t.PushBack(y{'a'});
        EXPECT_EQ(t.Size(), 1);

        y y{'b'};
        t.PushBack(std::move(y));
        EXPECT_EQ(t.Size(), 2);
    }
}

TEST(TestVector, TestPopBack)
{
    {
        Vector<int> t(3, 1);
        t.PopBack();
        EXPECT_EQ(t.Size(), 2);
        t.PopBack();
        EXPECT_EQ(t.Size(), 1);
        t.PopBack();
        EXPECT_EQ(t.Size(), 0);
        EXPECT_TRUE(t.Empty());
        EXPECT_DEATH(t.PopBack(), "Vector is empty");
    }
}

TEST(TestVector, TestEmplaceBack)
{
    {
        Vector<std::pair<int, std::string>> t;
        t.EmplaceBack(1, "hello");
        EXPECT_EQ(t.Size(), 1);
        EXPECT_EQ(t[0].first, 1);
        EXPECT_EQ(t[0].second, "hello");
    }
}

TEST(TestVector, TestSwap)
{
    {
        Vector<int> t(3, 1);
        Vector<int> w(2, 2);
        t.Swap(w);
        EXPECT_EQ(t.Size(), 2);
        EXPECT_EQ(w.Size(), 3);
        EXPECT_EQ(t[0], 2);
        EXPECT_EQ(w[0], 1);
    }
}

TEST(TestVector, TestInsert)
{
    {
        Vector<int> t(1);
        t.Insert(0, 1);
        EXPECT_EQ(t.Size(), 2);
        EXPECT_EQ(t[0], 1);
        EXPECT_EQ(t[1], 0);

        t.Insert(1, 2);
        EXPECT_EQ(t.Size(), 3);
        EXPECT_EQ(t[0], 1);
        EXPECT_EQ(t[1], 2);
        EXPECT_EQ(t[2], 0);

        t.Insert(t.Size(), 4);
        EXPECT_EQ(t.Size(), 4);
        EXPECT_EQ(t[0], 1);
        EXPECT_EQ(t[1], 2);
        EXPECT_EQ(t[2], 0);
        EXPECT_EQ(t[3], 4);

        EXPECT_DEATH(t.Insert(t.Size() + 1, 5), "Out of bounds");
    }
}

TEST(TestVector, TestErase)
{
    {
        Vector<int> t(3, 0);
        t[0] = 3;
        t.Erase(0);
        EXPECT_EQ(t.Size(), 2);
        EXPECT_EQ(t[0], 0);
        EXPECT_EQ(t[1], 0);

        t.Erase(1);
        EXPECT_EQ(t.Size(), 1);
        EXPECT_EQ(t[0], 0);

        t.Erase(0);
        EXPECT_EQ(t.Size(), 0);
        EXPECT_TRUE(t.Empty());

        EXPECT_DEATH(t.Erase(0), "Out of bounds");
    }

}

}  // namespace NVec::NTest
