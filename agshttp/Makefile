CXX = g++
CXXFLAGS = -std=c++0x -g -O2 -Wall -fPIC -D_REENTRANT  
LD = g++
LDFLAGS = -L/usr/lib/x86_64-linux-gnu 
DEFINES = 

all: libagshttp.so

clean:
	rm -f libagshttp.so *.o test

libagshttp.so: agshttp.o
	$(LD) $(LDFLAGS) -shared -o $@ $^

libagshttp32.so: agshttp32.o
	$(LD) $(LDFLAGS) -shared -m32 -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) -o $@ -c $^

%32.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) -m32 -o agshttp32.o -c $^
