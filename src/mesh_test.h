/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef MESH_TEST_H_
#define MESH_TEST_H_

#include "mesh.h"
#include "test_config.h"
#include "base_test.h"

class MeshTest : public BaseTest {
public:
	int Run();

private:
	int TestLoadValidLightSphere();
	int TestLoadValidDollHighChair();

};

#endif // MESH_TEST_H_
