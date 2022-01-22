# enable multicore make, taken from: https://stackoverflow.com/a/51149133
NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

tarname = hpcmi-gw.tar.gz

########################################################################################################################
# ssh hostnames per target
## HPC Class TUM HPC Class A
hpcClARoot = hpc01
hpcClANode1 = hpc02
hpcClANode2 = hpc03
hpcClANode3 = hpc04
hpcClANode4 = hpc05

## HPC Class TUM HPC Class B
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

## HPC Class Nuc
nucRoot = marco
nucNode1 = polo

## HPC Class Rpi
rpiRoot = rpinode1
rpiNode1 = rpinode2
rpiNode2 = rpinode3
rpiNode3 = rpinode4

########################################################################################################################
### General make targets
.SILENT: clean
all:
	@echo "This is a dummy to prevent running make without explicit target!"

# Setup toolchain, install packages
setupToolchain:
	./utl/setupToolchain.sh

# Initialize the data set on the host machine
init:
	git submodule update --init --recursive
	./utl/init.sh

# Remove generated files, build output and related files
clean:
	rm -rf doc
	rm -rf cmake-build-debug
	rm -rf build* debug* release*
	rm -f CMakeLists.txt.user
	@# Is created when opening the CMake Project with Visual Studio, blows up the size
	@# of the directories for sharing the code due to VS database
	rm -rf .vs/
	$(MAKE) -C rep/ clean

# Run a clean build/rebuild of the project
rebuild: clean
	mkdir -p build/
	cd build/ && cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null
	$(MAKE) -C build/ >/dev/null

# Run an incremental build of the project
.PHONY: build
build:
	mkdir -p build/
	cd build/ && cmake -DCMAKE_BUILD_TYPE=Release .. >/dev/null
	$(MAKE) -C build/ >/dev/null

########################################################################################################################
### Development make targets for local development and tests
test:
	$(MAKE) testX nruns=1

testX: build
	./utl/testX.sh $(nruns)
	$(MAKE) postTest

documentation:
	doxygen Doxyfile
	$(MAKE) -C rep/ report.pdf

pack: clean
	tar -czf $(tarname) * .git .clang-format .gitignore .gitmodules .mailmap

unpack:
	tar -xzf $(tarname)

########################################################################################################################
### Remote test make targets for all implemented targets
# Execute all TUM HPC standard tests
runHpcTests: init
	$(MAKE) prepareHpcClassA
	$(MAKE) testHpcClassA nruns=5 nproc=10
	$(MAKE) prepareHpcClassB
	$(MAKE) testHpcClassB nruns=5 nproc=20
	$(MAKE) prepareHpcClassMixed
	$(MAKE) testHpcClassMixed nruns=5 nproca=10 nprocb=20
	$(MAKE) -C utl/ _preTest

# Execute all NUC standard tests
runNucTests: init
	$(MAKE) prepareNuc
	$(MAKE) testNuc nruns=5 nproc=4
	$(MAKE) -C utl/ _preTest

# Execute all Raspberry Pi standard tests
runRpiTests: init
	$(MAKE) prepareRpi
	$(MAKE) testRpi nruns=5 nproc=16
	$(MAKE) -C utl/ _preTest

# HPC Class TUM HPC A
prepareHpcClassA: clean
	# preferred: pure make approach
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode1) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode2) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode3) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode4) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClARoot) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClARoot) runtype=rebuild
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) -C utl/ prepareHpcClassA root=$(hpcClARoot) node1=$(hpcClANode1) ...

testHpcClassA:
	$(MAKE) -C utl/ _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testHpcClassATarget

# HPC Class TUM HPC B
prepareHpcClassB: clean
	# preferred: pure make approach
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode1) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode2) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode3) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode4) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode5) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode6) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode7) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode8) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode9) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBRoot) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBRoot) runtype=rebuild
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) -C utl/ prepareHpcClassB root=$(hpcClBRoot) node1=$(hpcClBNode1) ...

testHpcClassB:
	$(MAKE) -C utl/ _testTarget root=$(hpcClBRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testHpcClassBTarget

# HPC Class TUM HPC Mixed
prepareHpcClassMixed: clean
	# preferred: pure make approach
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode1) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode2) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode3) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClANode4) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClARoot) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode1) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode2) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode3) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode4) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode5) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode6) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode7) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode8) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBNode9) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClBRoot) runtype=init
	$(MAKE) -C utl/ _prepareTarget host=$(hpcClARoot) runtype=rebuild # TODO: figure out, why mixed class fails with build on class B target but works when building on A

testHpcClassMixed:
	$(MAKE) -C utl/ _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb) maketarget=_testHpcClassMixedTarget

# HPC Class Nuc
prepareNuc: clean
	# preferred: bash script allows fully parallel execution of make targets
	$(MAKE) -C utl/ _prepareNuc root=$(nucRoot) node1=$(nucNode1)
	# alternative: pure make approach
	# $(MAKE) -C utl/ _prepareTarget host=$(nucRoot) runtype=build
	# $(MAKE) -C utl/ _prepareTarget host=$(nucNode1) runtype=build

testNuc:
	$(MAKE) -C utl/ _testTarget root=$(nucRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testNucTarget

# HPC Class Rpi
prepareRpi: clean
	# preferred: bash script allows fully parallel execution of make targets
	$(MAKE) -C utl/ _prepareRPI root=$(rpiRoot) node1=$(rpiNode1) node2=$(rpiNode2) node3=$(rpiNode3)
	# alternative: pure make approach
	# $(MAKE) -C utl/ _prepareTarget host=$(RPIRoot) runtype=build
	# $(MAKE) -C utl/ _prepareTarget host=$(RPINode1) runtype=build

testRpi:
	$(MAKE) -C utl/ _testTarget root=$(rpiRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testRpiTarget
