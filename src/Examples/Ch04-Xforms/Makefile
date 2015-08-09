release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Transforms-01-Interaction' && $(MAKE) $(BUILD)
	cd 'Transforms-02-Centered' && $(MAKE) $(BUILD)
	cd 'Transforms-03-Separate' && $(MAKE) $(BUILD)
	cd 'Transforms-04-Tank' && $(MAKE) $(BUILD)
	cd 'Transforms-05-SceneGraph' && $(MAKE) $(BUILD)

FORCE:


