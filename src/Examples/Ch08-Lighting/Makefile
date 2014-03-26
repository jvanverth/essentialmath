release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Lighting-01-DirectionalLight' && $(MAKE) $(BUILD)
	cd 'Lighting-02-PointLight' && $(MAKE) $(BUILD)
	cd 'Lighting-03-SpotLight' && $(MAKE) $(BUILD)
	cd 'Lighting-04-LightingComponents' && $(MAKE) $(BUILD)
	cd 'Lighting-05-MultipleLights' && $(MAKE) $(BUILD)
	cd 'Lighting-06-PerFragmentLighting' && $(MAKE) $(BUILD)
	cd 'Lighting-07-TexturesAndLighting' && $(MAKE) $(BUILD)
	cd 'Lighting-08-NormalMapping' && $(MAKE) $(BUILD)

FORCE:


