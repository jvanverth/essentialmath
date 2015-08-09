release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Collision-01-Hierarchy' && $(MAKE) $(BUILD)
	cd 'Collision-02-SweepPrune' && $(MAKE) $(BUILD)

FORCE:


