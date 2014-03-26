release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Viewing-01-LookAt' && $(MAKE) $(BUILD)
	cd 'Viewing-02-Rotation' && $(MAKE) $(BUILD)
	cd 'Viewing-03-Perspective' && $(MAKE) $(BUILD)
	cd 'Viewing-04-Stereo' && $(MAKE) $(BUILD)
	cd 'Viewing-05-Orthographic' && $(MAKE) $(BUILD)
	cd 'Viewing-06-Oblique' && $(MAKE) $(BUILD)
	cd 'Viewing-07-Clipping' && $(MAKE) $(BUILD)
	cd 'Viewing-08-Picking' && $(MAKE) $(BUILD)

FORCE:


