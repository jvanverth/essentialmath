release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Simulation-01-Force' && $(MAKE) $(BUILD)
	cd 'Simulation-02-Integration' && $(MAKE) $(BUILD)
	cd 'Simulation-03-Torque' && $(MAKE) $(BUILD)
	cd 'Simulation-04-Tank' && $(MAKE) $(BUILD)
	cd 'Simulation-05-LinCollision' && $(MAKE) $(BUILD)
	cd 'Simulation-06-AngCollision' && $(MAKE) $(BUILD)

FORCE:


