
binaries: 
	cd Classes && $(MAKE) $(MFLAGS);
	cd NVT && $(MAKE) $(MFLAGS);
	cd makeconfig && $(MAKE) $(MFLAGS);
	cd config2eps && $(MAKE) $(MFLAGS);
	cd shrinkconfig && $(MAKE) $(MFLAGS);
	cd test && $(MAKE) $(MFLAGS);
	cd analysis && $(MAKE) $(MFLAGS)

all:	binaries documentation

documentation:
	cd doxygen && doxygen doxyfile;
	cd doxygen/latex && $(MAKE) $(MFLAGS);

clean:
	cd NVT && $(MAKE) clean ;
	cd makeconfig && $(MAKE) clean ;
	cd config2eps && $(MAKE) clean ;
	cd shrinkconfig && $(MAKE) clean;
	cd Classes && $(MAKE) clean ;
	cd test && $(MAKE) clean;
	cd analysis && $(MAKE) clean;
	cd doxygen && rm -rf html doxygen.log

test:	binaries
	cd test && $(MAKE) test

