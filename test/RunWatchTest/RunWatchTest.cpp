// RunWatchTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void highly_optimised_subroutine()
{
raven::set::cRunWatch runwatch("highly_optimised_subroutine");
Sleep( 2 );
}

void badly_written_function()
{
raven::set::cRunWatch runwatch("badly_written_function");
for (int k = 1; k < 1000; k++ )
{
highly_optimised_subroutine();
}
}

void well_written_function()
{
raven::set::cRunWatch runwatch("well_written_function");
for (int k = 1; k < 10; k++ )
{
highly_optimised_subroutine();
}
}

void func5()
{
	raven::set::cRunWatch runwatch("func5");
}
void func4()
{
	raven::set::cRunWatch runwatch("func4");
	func5();
}
void func3()
{
	raven::set::cRunWatch runwatch("func3");
	func4();
}
void func2()
{
	raven::set::cRunWatch runwatch("func2");
	func3();
}
void func1()
{
	raven::set::cRunWatch runwatch("func1");
	func2();
}

int _tmain(int argc, _TCHAR* argv[])
{
raven::set::cRunWatch::StartDaisyChain();

//well_written_function();
//badly_written_function();

{
raven::set::cRunWatch runwatch("main");
func1();
func3();
}

raven::set::cRunWatch::Report();

raven::set::cRunWatch::Report( L"runwatch_test.log");


return 0;
}

