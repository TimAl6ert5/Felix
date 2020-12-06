/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
 */

#ifndef TEST_CONFIG_H_
#define TEST_CONFIG_H_

#define PASS 0
#define FAIL 1

//#define RUN_TESTS // Run any defined tests

#ifdef RUN_TESTS
// Select tests to run
#define MESH_TEST
#endif // RUN_TESTS

#endif /* TEST_CONFIG_H_ */
