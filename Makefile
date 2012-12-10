all:
	(cd CumulusLib/; make; cd -)
	(cd CumulusServer/; make; cd -)

clean:
	(cd CumulusLib/; make clean; cd -)
	(cd CumulusServer/; make clean; cd -)

