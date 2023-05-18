/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#include "test_suite.h"

int TestSuite::Run() {
	int test_result = PASS;

#ifdef MESH_TEST
	test_result += mesh_test_.Run();
#endif

	return test_result;
}
