/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

#include "test_config.h"
#include "mesh_test.h"

class TestSuite {
 public:
	int Run();

 private:
	MeshTest mesh_test_;

};

#endif // TEST_SUITE_H_
