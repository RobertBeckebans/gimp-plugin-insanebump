.PHONY: clean All

All:
	@echo ----------Building project:[ InsaneBump - Release ]----------
	@"$(MAKE)" -f "InsaneBump.mk"
clean:
	@echo ----------Cleaning project:[ InsaneBump - Release ]----------
	@"$(MAKE)" -f "InsaneBump.mk" clean
