#pragma once

class Test
{
public:
	Test(int* pInt);
	Test(Test&& other) noexcept;

	int* m_pInt;
};

