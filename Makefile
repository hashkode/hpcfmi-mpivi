# enable multicore make, taken from: https://stackoverflow.com/a/51149133
NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

tarname = hpcmi-gw.tar.gz

nucRoot = marco
nucNode1 = polo

rpiRoot = rpinode1
rpiNode1 = rpinode2
rpiNode2 = rpinode3
rpiNode3 = rpinode4

hpcClARoot = hpc01
hpcClANode1 = hpc02
hpcClANode2 = hpc03
hpcClANode3 = hpc04
hpcClANode4 = hpc05

hpcClBRoot = hpc06
hpcClBNode1 = hpc07
hpcClBNode2 = hpc08
hpcClBNode3 = hpc09
hpcClBNode4 = hpc10
hpcClBNode5 = hpc11
hpcClBNode6 = hpc12
hpcClBNode7 = hpc13
hpcClBNode8 = hpc14
hpcClBNode9 = hpc15

.SILENT: clean

all:
	@echo "This is a dummy to prevent running make without explicit target!"

clean:
	rm -rf doc
	rm -rf cmake-build-debug
	rm -rf build* debug* release*
	rm -f CMakeLists.txt.user
	@# Is created when opening the CMake Project with Visual Studio, blows up the size
	@# of the directories for sharing the code due to VS database
	rm -rf .vs/
	$(MAKE) -C rep/ clean

# Get rid of everything that might be left for whatever reason, then compile from scratch
# Not elegant but failsafe
rebuild: clean
	doxygen Doxyfile
	mkdir -p build/
	cd build/ && cmake -DCMAKE_BUILD_TYPE=Release ..
	$(MAKE) -C build/

.PHONY: build
build:
	mkdir -p build/
	cd build/ && cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null
	$(MAKE) -C build/ >/dev/null

preTest:
	$(MAKE) -C utl/ preTest

postTest:
	$(MAKE) -C utl/ postTest

test:
	$(MAKE) testX nruns=1

testX: preTest build
	$(MAKE) -C utl/ testX nruns=$(nruns)
	$(MAKE) postTest

prepareTarget:
	$(MAKE) -C utl/ prepareTarget host=$(host) runtype=$(runtype)

testTarget:
	$(MAKE) -C utl/ testTarget root=$(root) nruns=$(nruns) nproc=$(nproc) maketarget=$(maketarget)

#test-hpc-class1:
#	$(MAKE) prepareTarget host=hpc04 runtype=init
#	$(MAKE) prepareTarget host=hpc03 runtype=init
#	$(MAKE) prepareTarget host=hpc02 runtype=init
#	$(MAKE) prepareTarget host=hpc01 runtype=rebuild
#	#make init auf allen ranks
#	#make preTest auf allen ranks
#	#make rebuild auf einem rank einer gruppe (hier einer)

prepareHpcClassA: clean
	# preferred: pure make approach
	$(MAKE) prepareTarget host=$(hpcClANode1) runtype=init
	$(MAKE) prepareTarget host=$(hpcClANode2) runtype=init
	$(MAKE) prepareTarget host=$(hpcClANode3) runtype=init
	$(MAKE) prepareTarget host=$(hpcClANode4) runtype=init
	$(MAKE) prepareTarget host=$(hpcClARoot) runtype=rebuild
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) -C utl/ prepareHpcClassA root=$(hpcClARoot) node1=$(hpcClANode1) ...

testHpcClassA:
	$(MAKE) testTarget root=$(hpcClARoot) nruns=$(nruns) nproc=$(nproc) maketarget=_testHpcClassATarget

_testHpcClassATarget:
	$(MAKE) -C utl/ _testHpcClassATarget nruns=$(nruns) nproc=$(nproc)

prepareHpcClassB: clean
	# preferred: pure make approach
	$(MAKE) prepareTarget host=$(hpcClBNode1) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode2) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode3) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode4) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode5) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode6) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode7) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode8) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBNode9) runtype=init
	$(MAKE) prepareTarget host=$(hpcClBRoot) runtype=rebuild
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) -C utl/ prepareHpcClassB root=$(hpcClBRoot) node1=$(hpcClBNode1) ...

testHpcClassB:
	$(MAKE) testTarget root=$(hpcClARoot) nruns=$(nruns) nproc=$(nproc) maketarget=_testHpcClassBTarget

_testHpcClassBTarget:
	$(MAKE) -C utl/ _testHpcClassBTarget nruns=$(nruns) nproc=$(nproc)

prepareNuc: clean
	# preferred: bash script allows fully parallel execution of make targets
	$(MAKE) -C utl/ prepareNuc root=$(nucRoot) node1=$(nucNode1)
	# alternative: pure make approach
	# $(MAKE) prepareTarget host=$(nucRoot) runtype=rebuild
	# $(MAKE) prepareTarget host=$(nucNode1) runtype=rebuild

testNuc:
	$(MAKE) testTarget root=$(nucRoot) nruns=$(nruns) nproc=$(nproc) maketarget=_testNucTarget
	#$(MAKE) _testNucTarget root=$(nucRoot) nruns=$(nruns) nproc=$(nproc)

_testNucTarget:
	$(MAKE) -C utl/ _testNucTarget nruns=$(nruns) nproc=$(nproc)

prepareRpi: clean
	# preferred: bash script allows fully parallel execution of make targets
	$(MAKE) -C utl/ prepareRPI root=$(rpiRoot) node1=$(rpiNode1) node2=$(rpiNode2) node3=$(rpiNode3)
	# alternative: pure make approach
	# $(MAKE) prepareTarget host=$(RPIRoot) runtype=rebuild
	# $(MAKE) prepareTarget host=$(RPINode1) runtype=rebuild

testRpi:
	$(MAKE) testTarget root=$(rpiRoot) nruns=$(nruns) nproc=$(nproc) maketarget=_testRpiTarget
	#$(MAKE) _testNucTarget root=$(RPIRoot) nruns=$(nruns) nproc=$(nproc)

_testRpiTarget:
	$(MAKE) -C utl/ _testRpiTarget nruns=$(nruns) nproc=$(nproc)

report:
	$(MAKE) -C rep/ report.pdf

pack: clean
	tar -czf $(tarname) * .git .clang-format .gitignore .gitmodules .mailmap

unpack:
	tar -xzf $(tarname)

init:
	git submodule update --init --recursive
	$(MAKE) -C utl/ init

setupToolchain:
	$(MAKE) -C utl/ setupToolchain

