/*
 * Copyright 2013 Przemysław Buczkowski <przemub@przemub.pl>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "SpiderWindow.h"

#include "SpiderView.h"

#include <Alert.h>
#include <Application.h>
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <MenuBar.h>
#include <MenuItem.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "BeSpider"


SpiderWindow::SpiderWindow(BRect frame, const char* title)
	:
	BWindow(frame, title, B_DOCUMENT_WINDOW,
	B_QUIT_ON_WINDOW_CLOSE)
{
	fView = new SpiderView();
	fDiffSet = new BInvoker(new BMessage(kDiffChosenMessage), this);
	
	SetPulseRate(500000);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 1)
		.Add(_CreateMenuBar())
		.Add(fView);
}


void SpiderWindow::FrameResized(float newWidth, float newHeight)
{
	fView->Resize(newWidth, newHeight);
}


void SpiderWindow::MessageReceived(BMessage* message)
{
	BAlert* question;
	int32 response;

	switch (message->what) {
	case kNewGameMessage:
		fView->NewGame();
		break;
	case kDifficultyMessage:
		question = new BAlert("DiffAlert", B_TRANSLATE("Choose difficulty level."),
			B_TRANSLATE("Easy (1 color)"), B_TRANSLATE("Medium (2 colors)"),
			B_TRANSLATE("Hard (4 colors)"),
			B_WIDTH_AS_USUAL, B_IDEA_ALERT);
		question->Go(fDiffSet);
		break;
	case kDiffChosenMessage:
		message->FindInt32("which", &response);
		fView->ChangeDifficulty(response);
		break;
	case kHintMessage:
		fView->Hint();
		break;
	case 'DATA':
		if(message->WasDropped()) {
			fView->MouseUp(message->DropPoint());
		} else {
			BWindow::MessageReceived(message);
		}
		break;
	default:
		BWindow::MessageReceived(message);
	}
}


BMenuBar* SpiderWindow::_CreateMenuBar()
{
	BMenuBar* menuBar = new BMenuBar("MenuBar");
	BMenu* mGame = new BMenu(B_TRANSLATE("Game"));
	BMenu* mOptions = new BMenu(B_TRANSLATE("Options"));
	BMenuItem* menuItem;
	menuBar->AddItem(mGame);
	menuBar->AddItem(mOptions);
	
	menuItem = new BMenuItem(B_TRANSLATE("New game"), new BMessage(kNewGameMessage));
	menuItem->SetShortcut('N', B_COMMAND_KEY);
	mGame->AddItem(menuItem);
	
	menuItem = new BMenuItem(B_TRANSLATE("Change difficulty"), new BMessage(kDifficultyMessage));
	menuItem->SetShortcut('D', B_COMMAND_KEY);
	mOptions->AddItem(menuItem);
	
	menuItem = new BMenuItem(B_TRANSLATE_CONTEXT("Hint", "Menu bar"), new BMessage(kHintMessage));
	menuItem->SetShortcut('H', B_COMMAND_KEY);
	mGame->AddItem(menuItem);
	mGame->AddSeparatorItem();

	BMenuItem* about = new BMenuItem(B_TRANSLATE_CONTEXT("About" B_UTF8_ELLIPSIS, "Menu bar"),
		new BMessage(B_ABOUT_REQUESTED));
	about->SetTarget(be_app);
	mGame->AddItem(about);
	mGame->AddSeparatorItem();
	
	menuItem = new BMenuItem(B_TRANSLATE_CONTEXT("Quit", "Menu bar"), new BMessage(B_QUIT_REQUESTED));
	menuItem->SetShortcut('Q', B_COMMAND_KEY);
	mGame->AddItem(menuItem);

	return menuBar;
}
