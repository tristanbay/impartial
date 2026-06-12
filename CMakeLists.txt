cmake_minimum_required(VERSION 4.3.0)

project(Impartial VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O2 -g -Wall)

set(CPM_VERSION 0.42.3)
set(CPM_FILEPATH "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_VERSION}.cmake")

if (NOT EXISTS ${CPM_FILEPATH})
	message("Downloading CPM to ${CPM_FILEPATH}...")
	file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake ${CPM_FILEPATH})
endif()

include(${CPM_FILEPATH})

CPMAddPackage(
	NAME JUCE
	GIT_REPOSITORY git@github.com:juce-framework/JUCE.git
	GIT_TAG 8.0.13
)

# CPMAddPackage(
#	NAME tuning-library 
#	GIT_REPOSITORY git@github.com:surge-synthesizer/tuning-library.git
#	GIT_TAG origin/main
# )

# juce_add_binary_data(Assets SOURCES
# 	assets/logo.svg
# )

juce_add_plugin(Impartial
	COMPANY_NAME "Tristan Bay"
	IS_SYNTH TRUE
	NEEDS_MIDI_INPUT TRUE
	NEEDS_MIDI_OUTPUT FALSE
	IS_MIDI_EFFECT FALSE
	EDITOR_WANTS_KEYBOARD_FOCUS FALSE
	COPY_PLUGIN_AFTER_BUILD FALSE
	PLUGIN_MANUFACTURER_CODE TBay
	PLUGIN_CODE Iprt
	FORMATS Standalone
	PRODUCT_NAME "Impartial"
	BUNDLE_ID com.tristanbay.impartial
	LV2URI http://github.com/tristanbay/impartial
)

juce_generate_juce_header(Impartial)

set(SourceFiles
	src/PluginProcessor.cpp
	src/voice.cpp
	src/fastpower.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

target_compile_definitions(${PROJECT_NAME}
	PUBLIC
		JUCE_USE_CURL=0
		JUCE_VST3_CAN_REPLACE_VST2=0
		JUCE_WEB_BROWSER=0
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		juce::juce_audio_basics
		juce::juce_audio_devices
		juce::juce_audio_formats
		juce::juce_audio_plugin_client
		juce::juce_audio_processors
		juce::juce_audio_utils
		juce::juce_core
		juce::juce_data_structures
		juce::juce_dsp
		juce::juce_events
		juce::juce_graphics
		juce::juce_gui_basics
		juce::juce_gui_extra
		juce::juce_recommended_config_flags
		juce::juce_recommended_lto_flags
		juce::juce_recommended_warning_flags
		# tuning-library
)
