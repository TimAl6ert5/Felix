# Change Log


## [0.0.2] - Not yet released

CR video starting @ 7:30

### Tasks

- [x] Refactor classes to separate source
- [x] Consistent code formatting (prefer https://google.github.io/styleguide/cppguide.html)
	- [x] Curly brackets
	- [x] Classes & Functions upper camel case
	- [x] variables use snake case
	- [x] member variables end with underscore
	- [x] file names prefer snake case
- [x] Implement test cases for Mesh loader
- [x] Fix bad variable names (camera yaw/pitch/radius, mouse pressed flag, 'value' timer function)
- [x] Move mesh programs to files outside of source code
- [x] Accept subject mesh and texture from command line arguments
- [ ] Encapsulate the application (object-oriented rather than structured)
- [ ] Encapsulate the configuration
- [ ] Review comments
- [ ] Refactor string split function
- [ ] OBJ file validation
- [ ] Handle errors in OBJ files
- [ ] Review error handling of SOIL2 image loading
- [ ] Automatically adjust camera and lighting for the size of the subject


### Added

* Changelog and Readme documentation.
* Added min/max dimensions for loaded Mesh object.
* Implemented test cases for Mesh class.
* Commandline arguments to set the subject mesh and texture files.

### Changed

* Refactored the following classes into their own respective source files.
	* Camera
	* Mesh
	* ShaderProgram
	* Texture2D
* More consistent code format (following https://google.github.io/styleguide/cppguide.html)
	* Source file names prefer snake case
	* Consistent curly braces
	* Classes and Function names are upper camel case
	* variables are snake case
	* member variables end with underscore
* Modified mesh class for testability. Made initBuffers method required by the caller.
* Moved shader source to separate files and modifed application to load the source from files.


### Removed

### Fixed


## [0.0.1] - 2020-06-21
Initial implementation.
