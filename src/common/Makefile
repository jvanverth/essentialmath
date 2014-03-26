release: BUILD = release
release: IvCommon

debug: BUILD = debug
debug: IvCommon

clean: BUILD = clean
clean: IvCommon

IvCommon: FORCE
	cd IvCollision && $(MAKE) $(BUILD)
	cd IvCurves && $(MAKE) $(BUILD)
	cd IvEngine && $(MAKE) $(BUILD)
	cd IvEngine/OGL && $(MAKE) $(BUILD)
	cd IvMath && $(MAKE) $(BUILD)
	cd IvScene && $(MAKE) $(BUILD)
	cd IvUtility && $(MAKE) $(BUILD)
	cd IvGraphics && $(MAKE) $(BUILD)
	cd IvGraphics/OGL && $(MAKE) $(BUILD)
	cd IvRandom && $(MAKE) $(BUILD)

FORCE:


