
all:	documentation binaries

documentation:
	cd doxygen && doxygen doxyfile

binaries:
	cd NVT && $(MAKE) $(MFLAGS);
	cd makeconfig && $(MAKE) $(MFLAGS);
	cd config2eps && $(MAKE) $(MFLAGS)

clean:
	cd NVT && $(MAKE) clean ;
	cd makeconfig && $(MAKE) clean ;
	cd config2eps && $(MAKE) clean ;
	cd doxygen && rm -rf html doxygen.log

