release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Random-01-SphereDisc' && $(MAKE) $(BUILD)
	cd 'Random-02-Perlin' && $(MAKE) $(BUILD)

FORCE:


