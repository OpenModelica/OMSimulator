.PHONY: pip

pip:
	@echo
	@echo "# make pip"
	@echo
	cd src/pip/install/ && python3 setup.py sdist
	@echo
	@echo "# All local packages:"
	@ls src/pip/install/dist/ -Art
	@echo
	@echo "# Run the following command to upload the package"
	@echo "> twine upload src/pip/install/dist/$(shell ls src/pip/install/dist/ -Art | tail -n 1)"
