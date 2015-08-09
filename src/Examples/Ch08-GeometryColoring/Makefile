release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Geometry-01-ColorRemapping' && $(MAKE) $(BUILD)
	cd 'Geometry-02-BasicDrawing' && $(MAKE) $(BUILD)
	cd 'Geometry-03-BasicShading' && $(MAKE) $(BUILD)
	cd 'Geometry-04-UniformColors' && $(MAKE) $(BUILD)
	cd 'Geometry-05-VertexColors' && $(MAKE) $(BUILD)
	cd 'Geometry-06-SharpEdges' && $(MAKE) $(BUILD)
	cd 'Geometry-07-BasicTexturing' && $(MAKE) $(BUILD)
	cd 'Geometry-08-TextureAddressing' && $(MAKE) $(BUILD)

FORCE:


