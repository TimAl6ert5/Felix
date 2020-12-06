/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef BASE_TEST_H
#define BASE_TEST_H

#include <iostream>
#include <string>
#include <sstream>

class BaseTest
{
protected:
	void LogInfo(std::string message);
	void LogError(std::string message);

	bool IsEquals(std::string note, std::string expected, std::string actual);
	bool IsEquals(std::string note, int expected, int actual);
};

#endif // BASE_TEST_H
