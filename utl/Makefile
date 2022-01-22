# Remote test utilities
_prepareTarget:
	./_prepareTarget.sh $(host) $(runtype)

_testTarget:
	./_testTarget.sh $(root) $(nruns) $(nproca) $(nprocb) $(maketarget)

# Shell scripted target preparation for parallelized runs
_prepareNuc:
	./nuc/prepareNuc.sh $(root) $(node1)

_prepareRpi:
	./rpi/prepareRpi.sh $(root) $(node1) $(node2) $(node3)

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