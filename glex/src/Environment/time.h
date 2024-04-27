#pragma once
#include "comhdr.h"
#include <iostream>

namespace glex
{
	class Time
	{
		GLEX_STATIC_CLASS(Time)
	private:
		inline static double s_time;
		inline static float s_deltaTime;
	public:
		static void Startup() {
			s_deltaTime = 0.0f;
			s_time = glfwGetTime() * 1000.0;
		}
		static void Update() {
			double time = glfwGetTime() * 1000.0;
			s_deltaTime = time - s_time;
			s_time = time;
		}
		static double Current() {
			return s_time;
		}
		static float DeltaTime() {
			return s_deltaTime;
		}
	};
}