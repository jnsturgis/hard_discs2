
all:	binaries documentation

documentation:
	cd doxygen && doxygen doxyfile;
	cd doxygen/latex && $(MAKE) $(MFLAGS);

binaries: 
	cd Classes && $(MAKE) $(MFLAGS);
	cd NVT && $(MAKE) $(MFLAGS);
	cd makeconfig && $(MAKE) $(MFLAGS);
	cd config2eps && $(MAKE) $(MFLAGS);
	cd shrinkconfig && $(MAKE) $(MFLAGS)

clean:
	cd NVT && $(MAKE) clean ;
	cd makeconfig && $(MAKE) clean ;
	cd config2eps && $(MAKE) clean ;
	cd shrinkconfig && $(MAKE) clean;
	cd Classes && $(MAKE) clean ;
	cd doxygen && rm -rf html doxygen.log


