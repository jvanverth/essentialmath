release: BUILD = release
release: Book

debug: BUILD = debug
debug: Book

clean: BUILD = clean
clean: Book

Book: FORCE
	cd 'Ch04-Xforms' && $(MAKE) $(BUILD)
	cd 'Ch05-Orientation' && $(MAKE) $(BUILD)
	cd 'Ch06-Viewing' && $(MAKE) $(BUILD)
	cd 'Ch07-Interpolation' && $(MAKE) $(BUILD)
	cd 'Ch08-GeometryColoring' && $(MAKE) $(BUILD)
	cd 'Ch09-Lighting' && $(MAKE) $(BUILD)
	cd 'Ch10-Raster' && $(MAKE) $(BUILD)
	cd 'Ch11-RandomNumbers' && $(MAKE) $(BUILD)
	cd 'Ch12-Collision' && $(MAKE) $(BUILD)
	cd 'Ch13-Simulation' && $(MAKE) $(BUILD)

FORCE:


