setupToolchain:
	./setupToolchain.sh

init:
	./init.sh

preTest:
	./preTest.sh

postTest:
	./postTest.sh

testX:
	./testX.sh $(nruns)

_prepareTarget:
	./_prepareTarget.sh $(host) $(runtype)

prepareNuc:
	./nuc/prepareNuc.sh $(root) $(node1)
	
prepareRPI:
	./rpi/prepareRpi.sh $(root) $(node1) $(node2) $(node3)

_testTarget:
	./_testTarget.sh $(root) $(nruns) $(nproca) $(nprocb) $(maketarget)

_testHpcClassATarget:
	./hpcclassa/_testHpcClassATarget.sh $(nruns) $(nproca) $(nprocb)

_testHpcClassBTarget:
	./hpcclassb/_testHpcClassBTarget.sh $(nruns) $(nproca) $(nprocb)

_testHpcClassMixedTarget:
	./hpcclassmixed/_testHpcClassMixedTarget.sh $(nruns) $(nproca) $(nprocb)

_testNucTarget:
	./nuc/_testNucTarget.sh $(nruns) $(nproca) $(nprocb)
	
_testRpiTarget:
	./rpi/_testRpiTarget.sh $(nruns) $(nproca) $(nprocb)
