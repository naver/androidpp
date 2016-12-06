# android++
Welcome to Android-like C++ application framework(android++).
The framework is an open-source project which provides Android-like development environment for platforms which use C++ as their native language.
By writing an application based on the framework, you will be able to import your program logics written Java for your Android application project much easier.

## Prerequisite
* CMake - 3.4 preferred.
* Visual Studio 2015 - On Windows.

## Getting Started
### Windows
With CMake installed, do like this:
```
mkdir build
cd build
cmake -G "Visual Studio 14 2015" -D LIBRARY_PRODUCT_DIR=<output-directory> ..
```
Then open androidpp.sln, Hit "Build Solution". or type `cmake --build .`

### Other Platforms
Not yet supported.

## Resources(Under construction)
* API documentation
* Quick tutorials

## Project Roadmap(Under construction)

## License
android++ is licensed under BSD license.

```
Copyright (c) 2016 NAVER Corp.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
```
