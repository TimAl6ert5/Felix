/*
 *    Due Date: 2020-11-15
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 3-2
 */

#include "mesh.h"

//#define MESH_TEST

#ifdef MESH_TEST

#define PASS 0
#define FAIL 1

int TestLoadValidLightSphere();
int TestLoadValidDollHighChair();

int main() {
	int result = PASS;

	result += TestLoadValidLightSphere();
	result += TestLoadValidDollHighChair();

	if (result) {
		std::cout << "FAIL" << std::endl;
	} else {
		std::cout << "PASS" << std::endl;
	}
	exit(result);
}

int TestLoadValidLightSphere() {
	std::cout << "Begin - TestLoadValidLightSphere" << std::endl;
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
		std::cout << "Error" << std::endl;
		result = FAIL;
	}

	return result;
}


int TestLoadValidDollHighChair() {
	std::cout << "Begin - TestLoadValidLightSphere" << std::endl;
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
		std::cout << "Error" << std::endl;
		result = FAIL;
	}

	return result;
}


#endif // MESH_TEST
