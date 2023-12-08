build: 
	cd src && make build
	cp src/simulation simulation
clean:
	rm -rf simulation
	cd src && make clean
run:
	./simulation