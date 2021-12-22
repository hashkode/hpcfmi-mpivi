TARNAME = hpcmi-gw.tar.gz
HOST = hpc12

all:
	@echo "This is a dummy to prevent running make without explicit target!"

clean:
	$(MAKE) -C lib/ clean
	$(MAKE) -C rep/ clean
	rm -rf doc
	rm -rf build* debug* release*
	rm -f CMakeLists.txt.user
	@# Is created when opening the CMake Project with Visual Studio, blows up the size 
	@# of the directories for sharing the code due to VS database
	rm -rf .vs/

# Get rid of everything that might be left for whatever reason, then compile from scratch
# Not elegant but failsafe
compile: clean
	doxygen Doxyfile
	mkdir -p build/
	cd build/ && cmake -DCMAKE_BUILD_TYPE=Release ..
	$(MAKE) -C build/

test: compile
	cd build/
	$(MAKE) -C utl/ test
	
report:
	$(MAKE) -C rep/ report.pdf

pack: clean
	rm -f $(TARNAME)
	tar -czf $(TARNAME) backend/ cpp_backend/ tests/ Makefile main.py

unpack:
	tar -xzf $(TARNAME)

send: pack
	scp $(TARNAME) $(HOST):~/Projects/hpcmi/

init:
	git submodule update --init --recursive
	$(MAKE) -C utl/ init
