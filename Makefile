# Makefile for CMake presets

# Variables
PROJECT_NAME = Sirkus
PRESETS := ninja-debug ninja-release ninja-multi xcode-debug xcode-release
BUILD_PRESETS := ninja-debug-vst3 ninja-release-vst3
CMAKE = /Users/jack/.asdf/shims/cmake
JUCE_DIR = $(shell git config --file .gitmodules --get submodule.libs/juce.path)
JUCE_VERSION = 8.0.6
MELATONIN_INSPECTOR_DIR = $(shell git config --file .gitmodules --get submodule.libs/melatonin_inspector.path)
MELATONIN_INSPECTOR_REF = main

STANDALONE_TARGET = $(PROJECT_NAME)_Standalone
AU_TARGET = $(PROJECT_NAME)_AU
VST3_TARGET = $(PROJECT_NAME)_VST3

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

configure: configure-ninja-debug configure-ninja-release
	@echo "Project configured successfully."

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

# Install JUCE submodule (initialize from .gitmodules)
.PHONY: install-juce
install-juce:
	@if [ ! -d "$(JUCE_DIR)" ]; then \
		echo "Initializing JUCE submodule from .gitmodules..."; \
		git submodule update --init --recursive $(JUCE_DIR); \
		cd $(JUCE_DIR) && git checkout tags/$(JUCE_VERSION); \
	else \
		echo "JUCE is already installed at $(JUCE_DIR)."; \
	fi

# Update JUCE submodule
update-juce:
	@if [ -d "$(JUCE_DIR)" ]; then \
		echo "Updating JUCE submodule..."; \
		git submodule update --remote --recursive $(JUCE_DIR); \
		cd $(JUCE_DIR) && git fetch --tags && git checkout tags/$(JUCE_VERSION); \
	else \
		echo "JUCE is not installed. Run 'make install-juce' first."; \
		exit 1; \
	fi

# Remove JUCE submodule
remove-juce:
	@if [ -d "$(JUCE_DIR)" ]; then \
		echo "Removing JUCE submodule..."; \
		rm -rf $(JUCE_DIR); \
		git submodule deinit -f $(JUCE_DIR); \
		git rm -f $(JUCE_DIR); \
		rm -rf .git/modules/$(JUCE_DIR); \
		echo "JUCE has been removed."; \
	else \
		echo "JUCE is not installed. Nothing to remove."; \
	fi

# Reinstall JUCE submodule
reinstall-juce: remove-juce install-juce
	@echo "JUCE has been reinstalled."

.PHONY: install-melatonin-inspector
install-melatonin-inspector:
	@if [ ! -d "$(MELATONIN_INSPECTOR_DIR)" ]; then \
		echo "Initializing melatonin_inspector submodule from .gitmodules..."; \
		git submodule update --init --recursive $(MELATONIN_INSPECTOR_DIR); \
		cd $(MELATONIN_INSPECTOR_DIR) && git checkout $(MELATONIN_INSPECTOR_REF); \
	else \
		echo "melatonin_inspector is already installed at $(MELATONIN_INSPECTOR_DIR)."; \
	fi

# Update melatonin_inspector submodule
update-melatonin-inspector:
	@if [ -d "$(MELATONIN_INSPECTOR_DIR)" ]; then \
		echo "Updating melatonin_inspector submodule..."; \
		git submodule update --remote --recursive $(MELATONIN_INSPECTOR_DIR); \
		cd $(MELATONIN_INSPECTOR_DIR) && git checkout $(MELATONIN_INSPECTOR_REF); \
	else \
		echo "melatonin_inspector is not installed. Run 'make install-melatonin-inspector' first."; \
		exit 1; \
	fi

# Remove melatonin_inspector submodule
remove-melatonin-inspector:
	@if [ -d "$(MELATONIN_INSPECTOR_DIR)" ]; then \
		echo "Removing melatonin_inspector submodule..."; \
		rm -rf $(MELATONIN_INSPECTOR_DIR); \
		git submodule deinit -f $(MELATONIN_INSPECTOR_DIR); \
		git rm -f $(MELATONIN_INSPECTOR_DIR); \
		rm -rf .git/modules/$(MELATONIN_INSPECTOR_DIR); \
		echo "melatonin_inspector has been removed."; \
	else \
		echo "melatonin_inspector is not installed. Nothing to remove."; \
	fi

# Reinstall melatonin_inspector submodule
reinstall-melatonin-inspector: remove-melatonin-inspector install-melatonin-inspector
	@echo "melatonin_inspector has been reinstalled."

.PHONY: update-melatonin-inspector remove-melatonin-inspector reinstall-melatonin-inspector

# Source update target
.PHONY: update-sources
update-sources:
	chmod +x scripts/cmake-update-source-files.sh
	./scripts/cmake-update-source-files.sh

# Clean target
.PHONY: clean
clean:
	rm -rf build/

# Initialize project
init: install-juce configure
	@echo "Project initialized successfully."

# Run commands
run-au:
	./utils/AudioPluginHost.sh $(PROJECT_NAME)_AU

run-standalone:
	open build/ninja-debug/$(PROJECT_NAME).app
