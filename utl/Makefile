########################################################################################################################
### ssh hostnames per target
# HPC Class TUM HPC Class A
hpcClARoot = hpc01
hpcClANode1 = hpc02
hpcClANode2 = hpc03
hpcClANode3 = hpc04
hpcClANode4 = hpc05

# HPC Class TUM HPC Class B
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

# HPC Class Nuc
nucRoot = marco
nucNode1 = polo

# HPC Class Rpi
rpiRoot = rpinode1
rpiNode1 = rpinode2
rpiNode2 = rpinode3
rpiNode3 = rpinode4

# Remote test utilities
_prepareTarget:
	./_prepareTarget.sh $(host) $(runtype)

_testTarget:
	./_testTarget.sh $(root) $(nruns) $(nproca) $(nprocb) $(maketarget)

# Shell scripted local test execution
_testHpcClassATarget:
	$(MAKE) _preTest
	./hpcclassa/_testHpcClassATarget.sh $(nruns) $(nproca) $(nprocb)
	$(MAKE) _postTest

_testHpcClassBTarget:
	$(MAKE) _preTest
	./hpcclassb/_testHpcClassBTarget.sh $(nruns) $(nproca) $(nprocb)
	$(MAKE) _postTest

_testHpcClassMixedTarget:
	$(MAKE) _preTest
	./hpcclassmixed/_testHpcClassMixedTarget.sh $(nruns) $(nproca) $(nprocb)
	$(MAKE) _postTest

_testNucTarget:
	$(MAKE) _preTest
	./nuc/_testNucTarget.sh $(nruns) $(nproca) $(nprocb)
	$(MAKE) _postTest
	
_testRpiTarget:
	$(MAKE) _preTest
	./rpi/_testRpiTarget.sh $(nruns) $(nproca) $(nprocb)
	$(MAKE) _postTest

# Retrieve latest measurements from server, check data set integrity
_preTest:
	./_preTest.sh

# Synchronize measurement files, to upload latest results
_postTest:
	./_postTest.sh

# HPC Class TUM HPC A
_prepareHpcClassA:
	# preferred: pure make approach
	$(MAKE) _prepareTarget host=$(hpcClANode1) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode2) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode3) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode4) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClARoot) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClARoot) runtype=rebuild
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) _prepareHpcClassA root=$(hpcClARoot) node1=$(hpcClANode1) ...

_testHpcClassA:
	$(MAKE) _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testHpcClassATarget

# HPC Class TUM HPC B
_prepareHpcClassB:
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
	$(MAKE) _prepareTarget host=$(hpcClBRoot) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBRoot) runtype=rebuild
	# alternative: bash script would allow parallel execution, useless here due to NAS home directory
	# $(MAKE) _prepareHpcClassB root=$(hpcClBRoot) node1=$(hpcClBNode1) ...

_testHpcClassB:
	$(MAKE) _testTarget root=$(hpcClBRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testHpcClassBTarget

# HPC Class TUM HPC Mixed
_prepareHpcClassMixed:
	# preferred: pure make approach
	$(MAKE) _prepareTarget host=$(hpcClANode1) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode2) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode3) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClANode4) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClARoot) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode1) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode2) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode3) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode4) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode5) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode6) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode7) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode8) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBNode9) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClBRoot) runtype=init
	$(MAKE) _prepareTarget host=$(hpcClARoot) runtype=rebuild # TODO: figure out, why mixed class fails with build on class B target but works when building on A

_testHpcClassMixed:
	$(MAKE) _testTarget root=$(hpcClARoot) nruns=$(nruns) nproca=$(nproca) nprocb=$(nprocb) maketarget=_testHpcClassMixedTarget

# HPC Class Nuc
_prepareNuc:
	# preferred: bash script allows fully parallel execution of make targets
	./nuc/prepareNuc.sh $(nucRoot) $(nucNode1)
	# alternative: pure make approach
	# $(MAKE) _prepareTarget host=$(nucRoot) runtype=build
	# $(MAKE) _prepareTarget host=$(nucNode1) runtype=build

_testNuc:
	$(MAKE) _testTarget root=$(nucRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testNucTarget

# HPC Class Rpi
_prepareRpi:
	# preferred: bash script allows fully parallel execution of make targets
	./rpi/prepareRpi.sh $(rpiRoot) $(rpiNode1) $(rpiNode2) $(rpiNode3)
	# alternative: pure make approach
	# $(MAKE) _prepareTarget host=$(RPIRoot) runtype=build
	# $(MAKE) _prepareTarget host=$(RPINode1) runtype=build

_testRpi:
	$(MAKE) _testTarget root=$(rpiRoot) nruns=$(nruns) nproca=$(nproc) nprocb=0 maketarget=_testRpiTarget

# HPC Class Local
_testLocal:
	$(MAKE) _preTest
	./local/_testLocalTarget.sh $(nruns) $(nproca)
	$(MAKE) _postTest

# HPC Class CI
_testCI:
	$(MAKE) _preTest
	./ci/_testCI.sh
	$(MAKE) _postTest
