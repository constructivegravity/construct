all: build rebuild

build:
	@mkdir build && mkdir bin && cd build && cmake .. && cd .. && ln -s $PWD/bin/construct /usr/local/bin/construct

rebuild:
	@cd build && make && cd ..

clean:
	@rm -r build && rm -r bin && rm /usr/local/bin/construct

update:
	git pull origin master
	make

link:
	@ln -s $PWD/bin/construct /usr/local/bin/construct

test:
	make && bin/testing

.PHONY: test
