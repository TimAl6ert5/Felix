/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#include "mesh_test.h"

int MeshTest::Run() {
	int test_result = PASS;

	test_result += TestLoadValidLightSphere();
	test_result += TestLoadValidDollHighChair();

	return test_result;
}

int MeshTest::TestLoadValidLightSphere() {
	LogInfo("Begin TestLoadValidLightSphere");
	int result = PASS;

	std::string testObjectFilename = "LightSphere.obj";

	Mesh mesh;

	try {

		if(!mesh.LoadOBJ(testObjectFilename)) {
			result = FAIL;
		}

		std::cout << "X " << mesh.GetXMin() << " ... " << mesh.GetXMax() << std::endl;
		std::cout << "Y " << mesh.GetYMin() << " ... " << mesh.GetYMax() << std::endl;
		std::cout << "Z " << mesh.GetZMin() << " ... " << mesh.GetZMax() << std::endl;

	} catch(...) {
		LogError("Exception occurred");
		result = FAIL;
	}

	LogInfo("End TestLoadValidLightSphere");
	return result;
}

int MeshTest::TestLoadValidDollHighChair() {
	LogInfo("Begin TestLoadValidDollHighChair");
	int result = PASS;

	std::string testObjectFilename = "DollHighChair03.obj";

	Mesh mesh;

	try {

		if(!mesh.LoadOBJ(testObjectFilename)) {
			result = FAIL;
		}

		std::cout << "X " << mesh.GetXMin() << " ... " << mesh.GetXMax() << std::endl;
		std::cout << "Y " << mesh.GetYMin() << " ... " << mesh.GetYMax() << std::endl;
		std::cout << "Z " << mesh.GetZMin() << " ... " << mesh.GetZMax() << std::endl;

	} catch(...) {
		LogError("Exception occurred");
		result = FAIL;
	}

	LogInfo("End TestLoadValidDollHighChair");
	return result;
}

