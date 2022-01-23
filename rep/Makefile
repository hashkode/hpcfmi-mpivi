# Clean temp files
.SILENT: clean
clean:
	rm -f *.o errors.err
	rm -f *.aux *.bbl *.blg *.log *.out
	rm -f *.synctex.gz
	@echo

# Adapted from: https://tex.stackexchange.com/a/596785
# Generate PDF
%.pdf: %.tex
	@echo '.........: pdflatex running pass 1...'
	pdflatex $< -o $@ 2>&1 | tee errors.err
	@echo '.........: bibtex running...'
	bibtex $(basename $<) 2>&1 | tee errors.err
	@echo '.........: pdflatex running pass 2...'
	pdflatex $< -o $@ 2>&1 | tee errors.err
	@echo '.........: pdflatex and bibtex run finished.'

_generatePuml:
	./_generatePuml.sh

_generatePlots:
	./_generatePlots.py