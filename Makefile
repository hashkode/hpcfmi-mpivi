########################################################################################################################
############################                MPI-VI Project for HPCfMI at TUM                ############################
########################################################################################################################
### misc. settings
# enable multicore make, taken from: https://stackoverflow.com/a/51149133
NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

# name of tarball generated with make pack
tarname = hpcmi-gw.tar.gz

########################################################################################################################
### General make targets
all:
	@echo "This is a dummy to prevent running make without explicit target!"

# Setup minimum target toolchain, install packages
setupToolchain:
	./utl/setupToolchain.sh

# Setup complete host toolchain, install packages
setupHostToolchain: init
	./utl/setupHostToolchain.sh

# Initialize the data set on the host machine
init:
	$(MAKE) clean
	git submodule update --init --recursive
	./utl/init.sh

# Remove generated files, build output and related files
.SILENT: clean
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
	$(MAKE) -C utl/ _postTest

generateDoxygen: clean
	doxygen Doxyfile >/dev/null

documentation: clean
	$(MAKE) -C rep/ _generatePuml
	$(MAKE) -C utl/ _preTest
	$(MAKE) -C rep/ _generatePlots
	git add rep/gen/
	git add rep/data/
	$(MAKE) -C rep/ report.pdf

pack: clean
	tar -czf $(tarname) * .git .clang-format .gitignore .gitmodules .mailmap

unpack:
	tar -xzf $(tarname)

########################################################################################################################
### Remote test make targets for all implemented targets
# Execute all TUM HPC standard tests
runAllHpcTests:
	$(MAKE) -C utl/ _prepareHpcClassA
	$(MAKE) -C utl/ _testHpcClassA nruns=1 nproc=10
	$(MAKE) -C utl/ _prepareHpcClassB
	$(MAKE) -C utl/ _testHpcClassB nruns=1 nproc=20
	$(MAKE) -C utl/ _prepareHpcClassMixed
	$(MAKE) -C utl/ _testHpcClassMixed nruns=1 nproca=10 nprocb=20

# Execute TUM HPC Class A standard tests
runHpcATests:
	$(MAKE) -C utl/ _prepareHpcClassA
	$(MAKE) -C utl/ _testHpcClassA nruns=1 nproc=10

# Execute TUM HPC Class B standard tests
runHpcBTests:
	$(MAKE) -C utl/ _prepareHpcClassB
	$(MAKE) -C utl/ _testHpcClassB nruns=1 nproc=40

# Execute TUM HPC Class Mixed standard tests
runHpcMixedTests:
	$(MAKE) -C utl/ _prepareHpcClassMixed
	$(MAKE) -C utl/ _testHpcClassMixed nruns=1 nproca=10 nprocb=20

# Execute all NUC standard tests
runNucTests:
	$(MAKE) -C utl/ _prepareNuc
	$(MAKE) -C utl/ _testNuc nruns=1 nproc=3

# Execute all Raspberry Pi standard tests
runRpiTests:
	$(MAKE) -C utl/ _prepareRpi
	$(MAKE) -C utl/ _testRpi nruns=1 nproc=16

# Execute all Local standard tests
runLocalTests: rebuild
	$(MAKE) -C utl/ _testLocal nruns=1 nproca=4
