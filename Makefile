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

# Remote test utilities
_prepareTarget:
	$(MAKE) -C utl/ _prepareTarget host=$(host) runtype=$(runtype)

_testTarget:
	$(MAKE) -C utl/ _testTarget root=$(root) nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb) maketarget=$(maketarget)

# HPC Class A
prepareHpcClassA: clean
	# preferred: pure make approach
	$(MAKE) _prepareTarget host=$(hpcClANode1) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode2) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode3) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode4) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClARoot) runtype=build
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) -C utl/ prepareHpcClassA root=$(hpcClARoot) node1=$(hpcClANode1) ...

testHpcClassA:
	$(MAKE) _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testHpcClassATarget

_testHpcClassATarget:
	$(MAKE) -C utl/ _testHpcClassATarget nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb)

# HPC Class B
prepareHpcClassB: clean
	# preferred: pure make approach
	$(MAKE) _prepareTarget host=$(hpcClBNode1) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode2) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode3) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode4) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode5) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode6) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode7) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode8) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode9) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBRoot) runtype=build
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) -C utl/ prepareHpcClassB root=$(hpcClBRoot) node1=$(hpcClBNode1) ...

testHpcClassB:
	$(MAKE) _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testHpcClassBTarget

_testHpcClassBTarget:
	$(MAKE) -C utl/ _testHpcClassBTarget nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb)

# HPC Class Mixed
prepareHpcClassMixed: clean
	$(MAKE) prepareHpcClassA
	$(MAKE) prepareHpcClassB

testHpcClassMixed:
	$(MAKE) _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb) maketarget=_testHpcClassMixedTarget

_testHpcClassMixedTarget:
	$(MAKE) -C utl/ _testHpcClassMixedTarget nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb)

# HPC Class Nuc
prepareNuc: clean
	# preferred: bash script allows fully parallel execution of make targets
	$(MAKE) -C utl/ prepareNuc root=$(nucRoot) node1=$(nucNode1)
	# alternative: pure make approach
	# $(MAKE) _prepareTarget host=$(nucRoot) runtype=build
	# $(MAKE) _prepareTarget host=$(nucNode1) runtype=build

testNuc:
	$(MAKE) _testTarget root=$(nucRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testNucTarget

_testNucTarget:
	$(MAKE) -C utl/ _testNucTarget nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb)

# HPC Class Rpi
prepareRpi: clean
	# preferred: bash script allows fully parallel execution of make targets
	$(MAKE) -C utl/ prepareRPI root=$(rpiRoot) node1=$(rpiNode1) node2=$(rpiNode2) node3=$(rpiNode3)
	# alternative: pure make approach
	# $(MAKE) _prepareTarget host=$(RPIRoot) runtype=build
	# $(MAKE) _prepareTarget host=$(RPINode1) runtype=build

testRpi:
	$(MAKE) _testTarget root=$(rpiRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testRpiTarget

_testRpiTarget:
	$(MAKE) -C utl/ _testRpiTarget nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb)

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

