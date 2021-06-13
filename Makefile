all:
	@if [ -d "build" ]; then \
		cd build && make -j4; \
	else \
		mkdir build; \
		cd build && cmake .. && make -j4; \
	fi

doc:
	@doxygen Doxyfile

%:
	@if [ -d "build" ]; then \
		cd build && make $@ -j4; \
	else \
		mkdir build; \
		cd build && cmake .. && make $@ -j4; \
	fi

.PHONY: clean distclean
clean:
	@if [ -d "build" ]; then \
		cd build && make clean;\
	fi

distclean:
	rm build -fr
	rm lib -fr
	rm html -fr