/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#include "base_test.h"

void BaseTest::LogInfo(std::string message) {
	std::cout << "[INFO] " << message << std::endl;
}

void BaseTest::LogError(std::string message) {
	std::cout << "[ERROR] " << message << std::endl;
}

bool BaseTest::IsEquals(std::string note, std::string expected, std::string actual) {
	std::stringstream ss;
	ss << "Test " << note << "\n\tExpected: " << expected << "\n\tActual: " << actual;

	if (expected == actual) {
		LogInfo(ss.str());
		return true;
	}
	else {
		LogError(ss.str());
		return false;
	}
}

bool BaseTest::IsEquals(std::string note, int expected, int actual) {
	std::stringstream ss;
	ss << "Test " << note << "\n\tExpected: (" << expected << ")\n\tActual: (" << actual << ").";

	if (expected == actual) {
		LogInfo(ss.str());
		return true;
	}
	else {
		LogError(ss.str());
		return false;
	}
}
