all: build rebuild

build:
	@mkdir build && mkdir bin && cd build && cmake .. && cd ..

rebuild:
	@cd build && make && cd ..

clean:
	@rm -r build && rm -r bin

update:
	git pull origin master
	make

test:
	make && bin/testing

.PHONY: test
