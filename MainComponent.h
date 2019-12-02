#pragma once

#include <assert.h>
#include <thread>
#include <atomic>

#include "IPTable.h"
#include "TimerTextBox.h"
#include "settings.hpp"

//#include "../JuceLibraryCode/JuceHeader.h"

constexpr int max_amount_of_labels = 20;
constexpr int window_width = 1000;
constexpr int window_height = 400;
constexpr int margin_top = 5;
constexpr int margin_left = 5;
constexpr int default_height = 20;

#define width(x) x.getBounds().getWidth()
#define height(x) x.getBounds().getHeight()
#define Y(x) x.getBounds().getY()
#define X(x) x.getBounds().getX()

#define GLUER(x,y, z) x##y##z
#define progress_bar(x) double GLUER(x,Valu,e){ 0.0 }; std::atomic_flag GLUER(x, Lc, k) = ATOMIC_FLAG_INIT; ProgressBar x{ GLUER(x,Valu,e),  GLUER(x, Lc, k) }
#define text_box(x) String GLUER(x,Valu,e){ 0.0 }; std::atomic_flag GLUER(x, Lc, k) = ATOMIC_FLAG_INIT; TimerTextBox x{ GLUER(x,Valu,e),  GLUER(x, Lc, k) }

class MainComponent   : public Component
{
public:

    MainComponent();
    ~MainComponent();

	int add_std_label(const std::wstring& caption, int pos_x, int pos_y, int width = -1, int height = -1);
	void configure_txt_editor(TextEditor& src, const int idx, const std::wstring& txt);
	void update_specific(const std::wstring&);
	void msg_box(const std::wstring& msg) { NativeMessageBox::showMessageBoxAsync(AlertWindow::AlertIconType::NoIcon, L"", msg.c_str()); }

	Label labels[max_amount_of_labels];
	int amount_of_labels = 0;

	const Font std_font = Font( "Consolas", 15.0, Font::FontStyleFlags::plain );

	//Globals
	progress_bar(globalProcessor);
	progress_bar(globalRAM);
	progress_bar(globalDisk);

	std::unique_ptr<std::thread> global_refresher{ nullptr };
	std::atomic_flag global_run = ATOMIC_FLAG_INIT;
	
	//Specify
	std::unique_ptr<TableDemoComponent> tbl;
	std::vector<str> addresses;
	std::atomic_flag mtx_addresses = ATOMIC_FLAG_INIT;

	TextButton add_address;
	TextButton rem_address;
	TextButton sync_files;

	text_box(specProcModel);
	text_box(specProcCores);
	progress_bar(specProcessorUsage);
	text_box(specRamCapacity);
	progress_bar(specRamUsage);
	text_box(specOS);
	text_box(specOSVersion);
	text_box(specOSSPVersion);
	text_box(specIP);
	ComboBox specDisks;
	text_box(specDiskCapacity);
	progress_bar(specDiskFree);

	std::unique_ptr < std::thread > specify_refresher{ nullptr };
	std::atomic_flag specify_run = ATOMIC_FLAG_INIT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
