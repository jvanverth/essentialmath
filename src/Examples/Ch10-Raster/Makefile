release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Raster-01-DepthBuffering' && $(MAKE) $(BUILD)
	cd 'Raster-02-TextureFilter' && $(MAKE) $(BUILD)
	cd 'Raster-03-Mipmapping' && $(MAKE) $(BUILD)
	cd 'Raster-04-Blending' && $(MAKE) $(BUILD)

FORCE:


