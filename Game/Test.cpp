#include "pch.h"
#include "Test.h"

Test::Test(int* pInt)
	: m_pInt{ pInt }
{
}

Test::Test(Test&& other) noexcept
	: m_pInt{ other.m_pInt }
{
	other.m_pInt = nullptr;
}
