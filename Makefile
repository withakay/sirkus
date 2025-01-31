# Makefile for CMake presets

# Default target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  Setup targets:"
	@echo "    install-juce              - Install JUCE framework from git"
	@echo ""
	@echo "  Source targets:"
	@echo "    update-sources            - Update SourceFiles.cmake with current source files"
	@echo ""
	@echo "  Configure targets:"
	@echo "    configure-ninja-debug      - Configure debug build using Ninja"
	@echo "    configure-ninja-release    - Configure release build using Ninja"
	@echo "    configure-ninja-multi      - Configure multi-config build using Ninja"
	@echo "    configure-xcode-debug      - Configure debug build using Xcode"
	@echo "    configure-xcode-release    - Configure release build using Xcode"
	@echo ""
	@echo "  Build targets:"
	@echo "    vst3-debug                - Build VST3 plugin (Debug)"
	@echo "    vst3-release              - Build VST3 plugin (Release)"
	@echo "    au-debug                  - Build AU plugin (Debug)"
	@echo "    au-release                - Build AU plugin (Release)"
	@echo "    standalone-debug          - Build Standalone app (Debug)"
	@echo "    standalone-release        - Build Standalone app (Release)"
	@echo ""
	@echo "  Clean targets:"
	@echo "    clean                     - Remove build directory"

# Configure targets
.PHONY: configure-ninja-debug configure-ninja-release configure-ninja-multi configure-xcode-debug configure-xcode-release

configure-ninja-debug:
	cmake --preset ninja-debug

configure-ninja-release:
	cmake --preset ninja-release

configure-ninja-multi:
	cmake --preset ninja-multi

configure-xcode-debug:
	cmake --preset xcode-debug

configure-xcode-release:
	cmake --preset xcode-release

# Build targets
.PHONY: vst3-debug vst3-release au-debug au-release standalone-debug standalone-release

vst3-debug: configure-ninja-debug
	cmake --build --preset ninja-debug-vst3

vst3-release: configure-ninja-release
	cmake --build --preset ninja-release-vst3

au-debug: configure-ninja-debug
	cmake --build --preset ninja-debug-au

au-release: configure-ninja-release
	cmake --build --preset ninja-release-au

standalone-debug: configure-ninja-debug
	cmake --build --preset ninja-debug-standalone

standalone-release: configure-ninja-release
	cmake --build --preset ninja-release-standalone

# Setup targets
.PHONY: install-juce
install-juce:
	git submodule update --init --recursive libs/juce

# Source update target
.PHONY: update-sources
update-sources:
	chmod +x scripts/cmake-update-source-files.sh
	./scripts/cmake-update-source-files.sh

# Clean target
.PHONY: clean
clean:
	rm -rf build/