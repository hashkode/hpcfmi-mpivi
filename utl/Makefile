setupToolchain:
	./setupToolchain.sh

init:
	./init.sh

preTest:
	./preTest.sh

postTest:
	./postTest.sh

testX:
	./testX.sh $(nRuns)

prepareTarget:
	./prepareTarget.sh $(host) $(runtype)

prepareNuc:
	./nuc/prepareNuc.sh $(root) $(node1)

testTarget:
	./testTarget.sh $(root) $(nruns) $(nproc) $(maketarget)

_testHpcClassATarget:
	./hpcclassa/_testHpcClassATarget.sh $(nruns) $(nproc)

_testHpcClassBTarget:
	./hpcclassa/_testHpcClassBTarget.sh $(nruns) $(nproc)

_testNucTarget:
	./nuc/_testNucTarget.sh $(nruns) $(nproc)

.PHONY: build
build:
	./qbuild.sh
