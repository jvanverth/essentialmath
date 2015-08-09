release: BUILD = release
release: Chapter

debug: BUILD = debug
debug: Chapter

clean: BUILD = clean
clean: Chapter

Chapter: FORCE
	cd 'Curves-01-Linear' && $(MAKE) $(BUILD)
	cd 'Curves-02-Hermite' && $(MAKE) $(BUILD)
	cd 'Curves-03-AutoHermite' && $(MAKE) $(BUILD)
	cd 'Curves-04-Catmull' && $(MAKE) $(BUILD)
	cd 'Curves-05-Kochanek' && $(MAKE) $(BUILD)
	cd 'Curves-06-Bezier' && $(MAKE) $(BUILD)
	cd 'Curves-07-B-Spline' && $(MAKE) $(BUILD)
	cd 'Curves-08-Euler' && $(MAKE) $(BUILD)
	cd 'Curves-09-LerpSlerp' && $(MAKE) $(BUILD)
	cd 'Curves-10-SpeedControl' && $(MAKE) $(BUILD)
	cd 'Curves-11-CameraControl' && $(MAKE) $(BUILD)

FORCE:


