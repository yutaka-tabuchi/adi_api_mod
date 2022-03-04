DOCS=README.md \
	awg.md \
	qube-unit.md \
	qubeunit-table.md 

all:
	${XVFB} pandoc ${DOCS} -f markdown -t html5 -s -c github.css -o README.pdf
