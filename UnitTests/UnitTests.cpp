#include "pch.h"
#include "CppUnitTest.h"
#include "../imgui_test/Engine.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace UnitTests
{
	TEST_CLASS(AnimationTest)
	{
	public:
		
		TEST_METHOD(Constructor)
		{
			Animation anim("resources/animations/floor/water.png", 10, 100, 48, 48);
			Assert::AreEqual(anim.file_data.w, 244);
			Assert::AreEqual(anim.file_data.h, 97);
		}
		TEST_METHOD(Next_Frame)
		{
			Animation anim("resources/animations/floor/water.png", 10, 100, 48, 48);
			anim.NextFrame();
			Assert::AreEqual(anim.GetFrame().x, 49);
		}
	};
}
