.PHONY: pip

pip:
	@echo
	@echo "# make pip"
	@echo
	cd install/ && python3 setup.py sdist
	@echo
	@echo "# All local packages:"
	@ls install/dist/ -Art
	@echo
	@echo "# Run the following command to upload the package"
	@echo "> twine upload src/pip/install/dist/$(shell ls install/dist/ -Art | tail -n 1)"
