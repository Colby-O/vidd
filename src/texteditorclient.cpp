#include <vidd/texteditorclient.hpp>

#include <vidd/terminal.hpp>
#include <vidd/keybinds.hpp>
#include <vidd/display.hpp>
#include <vidd/tabarea.hpp>
#include <vidd/tab.hpp>
#include <vidd/fuzzywindow.hpp>
#include <vidd/log.hpp>
#include <vidd/vidd.hpp>
#include <vidd/timer.hpp>
#include <vidd/utils.hpp>
#include <vidd/theme.hpp>
#include <vidd/filesystem.hpp>
#include <vidd/filebrowserclient.hpp>
#include <vidd/logclient.hpp>
#include <vidd/procstream.hpp>
#include <vidd/virtualterminal.hpp>
#include <vidd/terminalclient.hpp>
#include <vidd/format.hpp>

#include <map>
#include <set>
#include <functional>
#include <string>

namespace {

#define EDITOR self->getEditor()

#define COMMAND(com, expect, code) \
	{ com, [](TextEditorClient* self, const std::vector<std::string_view>& params) { \
			if ((int)params.size() < expect) return; \
			code; \
		} \
	}, \

using CommandList = std::map<std::string_view, std::function<void(TextEditorClient*, const std::vector<std::string_view>&)>>;

const CommandList commandList = {
	COMMAND("w", 0, CLIENT->saveFile())
	COMMAND("number", 0, CLIENT->toggleLineNumbers())
	COMMAND("notabs", 0, EDITOR->setNoTabs())
	COMMAND("e", 1, CLIENT->editFile(params[0]))
	COMMAND("o", 1, CLIENT->openFile(params[0]))
	COMMAND("q", 0, CLIENT->tryClose())
	COMMAND("q!", 0, CLIENT->close())
	COMMAND("wq", 0, CLIENT->saveFile(); CLIENT->tryClose())
	COMMAND("theme", 1, Vidd::setTheme(Themes::getThemeByName(params[0])))
};

#define MOVEMENT_KEY_BINDS \
	KEYBIND(TextEditorClient, ({ 'h' }), EDITOR->cursorMoveX(-1)) \
	KEYBIND(TextEditorClient, ({ 'l' }), EDITOR->cursorMoveX(1)) \
	KEYBIND(TextEditorClient, ({ 'j' }), EDITOR->cursorMoveY(1)) \
	KEYBIND(TextEditorClient, ({ 'k' }), EDITOR->cursorMoveY(-1)) \
	KEYBIND(TextEditorClient, ({ Keys::Left }), EDITOR->cursorMoveX(-1)) \
	KEYBIND(TextEditorClient, ({ Keys::Right }), EDITOR->cursorMoveX(1)) \
	KEYBIND(TextEditorClient, ({ Keys::Down }), EDITOR->cursorMoveY(1)) \
	KEYBIND(TextEditorClient, ({ Keys::Up }), EDITOR->cursorMoveY(-1)) \
	KEYBIND(TextEditorClient, ({ 'J' }), EDITOR->viewScrollY(1)) \
	KEYBIND(TextEditorClient, ({ 'K' }), EDITOR->viewScrollY(-1)) \
	KEYBIND(TextEditorClient, ({ 'z' }), EDITOR->viewCenterAroundCursor()) \
	KEYBIND(TextEditorClient, ({ 'L' }), EDITOR->viewScrollX(1)) \
	KEYBIND(TextEditorClient, ({ 'H' }), EDITOR->viewScrollX(-1)) \
	KEYBIND(TextEditorClient, ({ 'z' }), EDITOR->viewCenterAroundCursor()) \
	KEYBIND(TextEditorClient, ({ 'Z' }), EDITOR->cursorMoveToViewCenter()) \
	KEYBIND(TextEditorClient, ({ Keys::ScrollUp }), EDITOR->viewScrollY(-2)) \
	KEYBIND(TextEditorClient, ({ Keys::ScrollDown }), EDITOR->viewScrollY(2)) \
	KEYBIND(TextEditorClient, ({ Keys::ScrollLeft }), EDITOR->viewScrollX(-2)) \
	KEYBIND(TextEditorClient, ({ Keys::ScrollRight }), EDITOR->viewScrollX(2)) \
	KEYBIND(TextEditorClient, ({ 'g', 'g' }), EDITOR->cursorMoveToFirstLine(); EDITOR->cursorMoveToLineStart();) \
	KEYBIND(TextEditorClient, ({ 'G' }), EDITOR->cursorMoveToLastLine(); EDITOR->cursorMoveToLineStart();) \
	KEYBIND(TextEditorClient, ({ '0' }), EDITOR->cursorMoveToLineStart()) \
	KEYBIND(TextEditorClient, ({ '^' }), EDITOR->cursorMoveToLineStart()) \
	KEYBIND(TextEditorClient, ({ '$' }), EDITOR->cursorMoveToLineEnd()) \
	KEYBIND(TextEditorClient, ({ 'w' }), EDITOR->cursorMoveNextWord()) \
	KEYBIND(TextEditorClient, ({ Keys::CtrlRight }), EDITOR->cursorMoveNextWord()) \
	KEYBIND(TextEditorClient, ({ 'b' }), EDITOR->cursorMovePrevWord()) \
	KEYBIND(TextEditorClient, ({ Keys::CtrlLeft }), EDITOR->cursorMovePrevWord()) \
	KEYBIND(TextEditorClient, ({ 'e' }), EDITOR->cursorMoveNextWordEnd()) \
	KEYBIND(TextEditorClient, ({ 'm' }), CLIENT->gotoMarker()) \
	KEYBIND(TextEditorClient, ({ 'M' }), CLIENT->setMarker()) \
	KEYBIND(TextEditorClient, ({ 'f' }), CLIENT->cursorMoveToNextChar()) \
	KEYBIND(TextEditorClient, ({ 'F' }), CLIENT->cursorMoveToPrevChar()) \
	KEYBIND(TextEditorClient, ({ '{' }), EDITOR->cursorMovePrevParagraph()) \
	KEYBIND(TextEditorClient, ({ '}' }), EDITOR->cursorMoveNextParagraph()) \
	KEYBIND(TextEditorClient, ({ '/' }), CLIENT->inputFindNext()) \
	KEYBIND(TextEditorClient, ({ '?' }), CLIENT->inputFindPrev()) \
	KEYBIND(TextEditorClient, ({ 'n' }), CLIENT->repeatFindNext()) \
	KEYBIND(TextEditorClient, ({ 'N' }), CLIENT->repeatFindPrev()) \
	KEYBIND(TextEditorClient, ({ '*' }), CLIENT->findNextUnderCursor()) \
	KEYBIND(TextEditorClient, ({ '#' }), CLIENT->findPrevUnderCursor()) \
	KEYBIND(TextEditorClient, ({ ' ', 'j' }), CLIENT->enterJumpMode()) \

const AliasBinds normalAliases = {
	WINDOW_ALIASES
	{ { 'd', 'd' }, { 'D' } },
	{ { 'd', 'j' }, { 'D', 'D' } },
	{ { 'd', 'k' }, { 'k', 'D', 'D' } },
	{ { 'd', 'a', 'w' }, { 'l', 'b', 'v', 'e', 'l', 'x' } },

	{ { 'c', 'a', 'w' }, { 'd', 'a', 'w', 'i' } },

	{ { 'y', 'y' }, { 'V', 'y', Keys::Escape } },
	{ { 'y', 'j' }, { 'V', 'j', 'y', 'k', Keys::Escape } },
	{ { 'y', 'k' }, { 'V', 'k', 'y', 'j', Keys::Escape } },
};

const KeyBinds normalKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
	WINDOW_KEY_BINDS(TextEditorClient)
	MOVEMENT_KEY_BINDS
	KEYBIND(TextEditorClient, ({ Keys::ctrl('m') }), Log::log("I hate you.")) \
	KEYBIND(TextEditorClient, ({ Keys::ctrl('y') }), CLIENT->copyEditorCopyBufferToSystemCopyBuffer()) \
	KEYBIND(TextEditorClient, ({ Keys::ctrl('c') }), CLIENT->tryClose())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('e') }), Log::log(std::to_string(std::size_t(EDITOR->getCursor().y))))
	KEYBIND(TextEditorClient, ({ Keys::ctrl('w') }), CLIENT->enterWindowMoveMode())
	KEYBIND(TextEditorClient, ({ Keys::CtrlBackslash }), CLIENT->duplicate())
	KEYBIND(TextEditorClient, ({ 31 }), CLIENT->inputReplace())
	KEYBIND(TextEditorClient, ({ 'u' }), EDITOR->undo())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('r') }), EDITOR->redo())
	KEYBIND(TextEditorClient, ({ 'q' }), CLIENT->toggleRecordingMacro())
	KEYBIND(TextEditorClient, ({ '@' }), CLIENT->runMacro())
	KEYBIND(TextEditorClient, ({ '~' }), EDITOR->toggleCaptailizationAtCursor(); EDITOR->cursorMoveX(1))
	KEYBIND(TextEditorClient, ({ '>' }), EDITOR->indentAtCursor())
	KEYBIND(TextEditorClient, ({ '<' }), EDITOR->deindentAtCursor())
	KEYBIND(TextEditorClient, ({ 'd' }), CLIENT->deleteMovement())
	KEYBIND(TextEditorClient, ({ 'y' }), CLIENT->copyMovement())
	KEYBIND(TextEditorClient, ({ 'Y' }), CLIENT->lineCopyMovement())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('n') }), EDITOR->toggleCommentAtCursor())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('v') }), EDITOR->paste())
	KEYBIND(TextEditorClient, ({ 'p' }), EDITOR->paste())
	KEYBIND(TextEditorClient, ({ 'P' }), EDITOR->pasteBack())
	KEYBIND(TextEditorClient, ({ ':' }), CLIENT->enterCommandMode())
	KEYBIND(TextEditorClient, ({ 'v' }), CLIENT->enterSelectMode())
	KEYBIND(TextEditorClient, ({ 'V' }), CLIENT->enterLineSelectMode())
	KEYBIND(TextEditorClient, ({ 'r' }), CLIENT->replaceChar())
	KEYBIND(TextEditorClient, ({ 'R' }), CLIENT->enterReplaceMode())
	KEYBIND(TextEditorClient, ({ 'i' }), CLIENT->enterInsertMode())
	KEYBIND(TextEditorClient, ({ 'a' }), CLIENT->enterInsertModeRight())
	KEYBIND(TextEditorClient, ({ 'o' }), CLIENT->enterInsertModeOnNewLineDown())
	KEYBIND(TextEditorClient, ({ 'O' }), CLIENT->enterInsertModeOnNewLineUp())
	KEYBIND(TextEditorClient, ({ 'x' }), EDITOR->deleteAtCursor())
	KEYBIND(TextEditorClient, ({ 'X' }), EDITOR->backspaceAtCursor())
	KEYBIND(TextEditorClient, ({ 'D' }), EDITOR->deleteLineAtCursor())
	KEYBIND(TextEditorClient, ({ Keys::Return }), TAB->swapSelectedWithMaster())
	KEYBIND(TextEditorClient, ({ ' ', 'l' }), CLIENT->openLogClient()) \
	KEYBIND(TextEditorClient, ({ ' ', 't' }), CLIENT->openTermianl())
	KEYBIND(TextEditorClient, ({ ' ', 'y' }), CLIENT->openFloatingTermianl())
	KEYBIND(TextEditorClient, ({ ' ', 'r' }), CLIENT->executeLine())
	KEYBIND(TextEditorClient, ({ ' ', 'e' }), CLIENT->fuzzyEditFile()) \
	KEYBIND(TextEditorClient, ({ ' ', 'o' }), CLIENT->fuzzyOpenFile()) \
	KEYBIND(TextEditorClient, ({ ' ', 'f' }), CLIENT->fuzzyOpenFloatingFile()) \
	KEYBIND(TextEditorClient, ({ ' ', 'g' }), CLIENT->fuzzyGoto()) \
	KEYBIND(TextEditorClient, ({ ' ', 'd' }), CLIENT->openDirectory()) \
};

const AliasBinds windowMoveAliases = {
	WINDOW_ALIASES
};

const KeyBinds windowMoveKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
	WINDOW_MOVE_KEY_BINDS(TextEditorClient)
	KEYBIND(TextEditorClient, ({ Keys::Escape }), CLIENT->exitWindowMoveMode())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('c') }), CLIENT->exitWindowMoveMode())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('w') }), CLIENT->exitWindowMoveMode())
};

const AliasBinds insertAliases = {
	{ { Keys::CtrlBackspace }, { Keys::Escape, 'l', 'd', 'b', 'i' } },
	{ { Keys::CtrlDelete }, { Keys::Escape, 'l', 'd', 'w', 'i' } },
};

const KeyBinds insertKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
	KEYBIND(TextEditorClient, ({ Keys::Escape }), CLIENT->exitInsertMode())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('v') }), EDITOR->paste())
	KEYBIND(TextEditorClient, ({ Keys::Return }), EDITOR->splitLineAtCursor())
	KEYBIND(TextEditorClient, ({ '\n' }), EDITOR->splitLineAtCursor())
	KEYBIND(TextEditorClient, ({ Keys::Backspace }), EDITOR->backspaceAtCursor())
	KEYBIND(TextEditorClient, ({ Keys::Left }), EDITOR->cursorMoveX(-1))
	KEYBIND(TextEditorClient, ({ Keys::Right }), EDITOR->cursorMoveX(1))
	KEYBIND(TextEditorClient, ({ Keys::Down }), EDITOR->cursorMoveY(1))
	KEYBIND(TextEditorClient, ({ Keys::Up }), EDITOR->cursorMoveY(-1))
	KEYBIND(TextEditorClient, ({ Keys::ScrollUp }), EDITOR->viewScrollY(-1)) \
	KEYBIND(TextEditorClient, ({ Keys::ScrollDown }), EDITOR->viewScrollY(1)) \
	KEYBIND(TextEditorClient, ({ Keys::CtrlRight }), EDITOR->cursorMoveNextWord()) \
	KEYBIND(TextEditorClient, ({ Keys::CtrlLeft }), EDITOR->cursorMovePrevWord()) \
};

const AliasBinds replaceAliases = {
};

const KeyBinds replaceKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
	KEYBIND(TextEditorClient, ({ Keys::Escape }), CLIENT->exitReplaceMode())
};

const AliasBinds selectAliases = {
	{ { Keys::ctrl('x') }, { Keys::ctrl('c'), 'x' } },
};

const KeyBinds selectKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
	WINDOW_KEY_BINDS(TextEditorClient)
	MOVEMENT_KEY_BINDS
	KEYBIND(TextEditorClient, ({ Keys::ctrl('y') }), CLIENT->copyEditorCopyBufferToSystemCopyBuffer()) \
	KEYBIND(TextEditorClient, ({ Keys::Escape }), CLIENT->exitSelectMode())
//    KEYBIND(TextEditorClient, ({ Keys::ctrl('c') }), EDITOR->exitSelectMode(); CLIENT->setNormalBinds(); EDITOR->restrictCursor();)
	KEYBIND(TextEditorClient, ({ Keys::ctrl('c') }), EDITOR->copySelection())
	KEYBIND(TextEditorClient, ({ Keys::ctrl('n') }), EDITOR->toggleCommentAtSelection())
	KEYBIND(TextEditorClient, ({ 'y' }), EDITOR->copySelection())
	KEYBIND(TextEditorClient, ({ 'd' }), EDITOR->deleteSelection(); CLIENT->exitSelectMode();)
	KEYBIND(TextEditorClient, ({ 'c' }), EDITOR->deleteSelection(); CLIENT->exitSelectMode(); CLIENT->enterInsertMode();)
	KEYBIND(TextEditorClient, ({ 'x' }), EDITOR->deleteSelection(); CLIENT->exitSelectMode();)
	KEYBIND(TextEditorClient, ({ Keys::Backspace }), EDITOR->deleteSelection(); CLIENT->exitSelectMode();)
	KEYBIND(TextEditorClient, ({ 's' }), EDITOR->swapCursorSelectionEnd())
	KEYBIND(TextEditorClient, ({ '<' }), EDITOR->deindentSelection())
	KEYBIND(TextEditorClient, ({ '>' }), EDITOR->indentSelection())
	KEYBIND(TextEditorClient, ({ 'v' }), CLIENT->enterSelectMode())
//    KEYBIND(TextEditorClient, ({ 'V' }), CLIENT->switchToLineSelectMode())
	KEYBIND(TextEditorClient, ({ Keys::ScrollUp }), EDITOR->viewScrollY(-1)) \
	KEYBIND(TextEditorClient, ({ Keys::ScrollDown }), EDITOR->viewScrollY(1)) \
};

const AliasBinds commandAliases = {
};

const KeyBinds commandKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
};

const AliasBinds noAliases = {
};

const KeyBinds noKeyBinds = {
	GLOBAL_KEY_BINDS(TextEditorClient)
};

}; // namespace

bool TextEditorClient::sIsRecordingMacro = false;
std::vector<Key> TextEditorClient::sMacroBuffer;

TextEditorClient::TextEditorClient(Tab* tab, Input input)
: Client(tab),
  mEditor(std::move(input)),
  mCodeView(&mEditor),
  mScrollBar(
	ScrollBar::Side::Right,
	[this](void) {
		int lines = mEditor.getLineCount();
		int y = mEditor.getViewPort().y;
		int height = mEditor.getViewPort().height;

		ScrollBar::SizeDetails details;
		details.pos = std::clamp((double)y / (double)lines, 0.0, 1.0);
		details.size = std::clamp((double)height / (double)lines, 0.0, 1.0);
		return details;
	},
	[this](double pos) {
		int lines = mEditor.getLineCount();
		mEditor.viewScrollToY(pos * lines);
	}
  )
{
	if (Vidd::hasFlag("-hln")) {
		mShowLineNumbers = false;
	}
	if (Vidd::hasFlag("-hsb")) {
		mShowStatusBar = false;
	}
	setNormalBinds();
	setSelectable(true);
}

void TextEditorClient::onAttach(void) {
	addChild(&mCodeView);
	addChild(&mScrollBar);
}

void TextEditorClient::saveFile(void) {
	mStatus = StatusMessage {
		.type = StatusMessageType::FileWrite,
		.text = Format::format("saving file '{}'", mEditor.getFileName()),
	};
	getDisplay()->delay(1.0, [this] {
		setNoStatus();
	}, std::size_t(this));
	mEditor.saveFile();
}

void TextEditorClient::editFile(std::string_view file) {
	mEditor.editFile(file);
}

void TextEditorClient::openFile(std::string_view file) {
	getTab()->addAndSelectClient<TextEditorClient>(Input(file));
}

void TextEditorClient::openFloatingFile(std::string_view file) {
	Client* client = getTab()->addAndSelectClient<TextEditorClient>(Input(file));
	client->setFloating();
	Component* parent = client->getParent();
	Vec2 tabSize = getTab()->getSize();
	Vec2 windowSize = tabSize * 0.8;
	parent->setSize(windowSize);
	parent->setPos((tabSize - windowSize) / 2);
}

void TextEditorClient::tryClose(void) {
	if (mAutoSave) {
		saveFile();
		close();
	} else if (mEditor.hasUnsavedChanges() && mStatus.type != StatusMessageType::UnsavedChanges) {
		mStatus = StatusMessage {
			.type = StatusMessageType::UnsavedChanges,
			.text = "unsaved changes",
		};
		getDisplay()->delay(0.8, [this] {
			setNoStatus();
		}, std::size_t(this));
		requireSelfRedraw();
	} else {
		close();
	}
}

void TextEditorClient::setNoStatus(void) {
	mStatus = StatusMessage{};
	requireSelfRedraw();
}

void TextEditorClient::duplicate(void) {
	TextEditorClient* client = getTab()->addAndSelectClient<TextEditorClient>(Input(mEditor.getFileName()));
	Cursor cur = mEditor.getCursor();
	client->getEditor()->cursorMoveTo(cur.x, cur.y->number);
}

WString TextEditorClient::getTitle(void) {
	return FileSystem::getFileName(mEditor.getFileName());
}

void TextEditorClient::resizeEditor(void) {
	const Theme* theme = Vidd::getTheme();
	int leftOffset = 0;
	if (mShowLineNumbers) {
		mOldLineCount = mEditor.getLineCount();
		int lineCountWidth = Utils::countDigits(mOldLineCount);
		leftOffset = WString(Format::vformat(theme->numberFormat, mOldLineCount, lineCountWidth)).length();
	}
	int bottomOffset = mShowStatusBar ? 1 : 0;
	Vec2 size = mSize - Vec2(leftOffset, bottomOffset) - Vec2(1, 0);
	Vec2 pos = Vec2(leftOffset, 0);
	mEditor.resize(size);
	mCodeView.setSize(size);
	mCodeView.setPos(pos);
}

void TextEditorClient::toggleLineNumbers(void) {
	mShowLineNumbers = !mShowLineNumbers;
	resizeEditor();
}

void TextEditorClient::setNormalBinds(void) {
	mKeyBinds = &normalKeyBinds;
	mAliases = &normalAliases;
}

void TextEditorClient::setInsertBinds(void) {
	mKeyBinds = &insertKeyBinds;
	mAliases = &insertAliases;
}

void TextEditorClient::setReplaceBinds(void) {
	mKeyBinds = &replaceKeyBinds;
	mAliases = &replaceAliases;
}

void TextEditorClient::setWindowMoveBinds(void) {
	mKeyBinds = &windowMoveKeyBinds;
	mAliases = &windowMoveAliases;
}

void TextEditorClient::setSelectBinds(void) {
	mKeyBinds = &selectKeyBinds;
	mAliases = &selectAliases;
}

void TextEditorClient::setCommandBinds(void) {
	mKeyBinds = &commandKeyBinds;
	mAliases = &commandAliases;
}

void TextEditorClient::setNoBinds(void) {
	mKeyBinds = &noKeyBinds;
	mAliases = &noAliases;
}

void TextEditorClient::enterInsertMode(void) {
	mMode = EditMode::Insert;
	mEditor.setLineOverflow(true);
	setInsertBinds();
	requireRedraw();
}

void TextEditorClient::enterInsertModeRight(void) {
	enterInsertMode();
	mEditor.cursorMoveX(1);
}

void TextEditorClient::enterInsertModeOnNewLineDown(void) {
	enterInsertMode();
	mEditor.insertLineDownFromCursor();
	mEditor.cursorMoveY(1);
}

void TextEditorClient::enterInsertModeOnNewLineUp(void) {
	enterInsertMode();
	mEditor.insertLineUpFromCursor();
	mEditor.cursorMoveY(-1);
}

void TextEditorClient::exitInsertMode(void) {
	mMode = EditMode::Normal;
	mEditor.cursorMoveX(-1);
	mEditor.setLineOverflow(false);
	setNormalBinds();
	requireRedraw();
}

void TextEditorClient::enterReplaceMode(void) {
	mMode = EditMode::Replace;
	mEditor.setLineOverflow(true);
	setReplaceBinds();
	requireRedraw();
}

void TextEditorClient::exitReplaceMode(void) {
	mMode = EditMode::Normal;
	mEditor.cursorMoveX(-1);
	mEditor.setLineOverflow(false);
	setNormalBinds();
	requireRedraw();
}

void TextEditorClient::replaceChar(void) {
	WChar key = getKey();
	if (isVisibleWChar(key)) {
		mEditor.replaceCharAtCursor(key);
	}
}

void TextEditorClient::enterSelectMode(void) {
	mMode = EditMode::Select;
	setSelectBinds();
	mEditor.startSelection();
	mEditor.setLineOverflow(true);
	requireRedraw();
}

void TextEditorClient::enterLineSelectMode(void) {
	mMode = EditMode::LineSelect;
	setSelectBinds();
	mEditor.startLineSelection();
	mEditor.setLineOverflow(true);
	requireRedraw();
}

void TextEditorClient::enterWordSelectMode(void) {
	mMode = EditMode::WordSelect;
	setSelectBinds();
	mEditor.startWordSelection();
	mEditor.setLineOverflow(true);
	requireRedraw();
}

void TextEditorClient::exitSelectMode(void) {
	mMode = EditMode::Normal;
	mEditor.stopSelection();
	mEditor.setLineOverflow(false);
	setNormalBinds();
	requireRedraw();
}

void TextEditorClient::enterCommandMode(void) {
	mMode = EditMode::Prompt;
	setCommandBinds();
	mPrompt = TextPrompt(":", {
		.change = [this] {
			requireSelfRedraw();
		},
		.submit = [this] {
			exitCommandMode();

			std::vector<std::string_view> params;

			std::string_view data(mPrompt.get());

			auto comSplit = data.find_first_of(" \r\n\0\t");
			if (comSplit == std::string_view::npos) comSplit = data.length();
			std::string_view cmd = data.substr(0, comSplit);
			if (comSplit < data.length()) {
				std::string_view paramString = data.substr(comSplit + 1);
		
				while (paramString.length() > 0) {
					auto split = paramString.find_first_of(" \r\n\0\t");
					if (split == std::string_view::npos) split = paramString.length();
					params.push_back(paramString.substr(0, split));
					if (split >= paramString.length()) break;
					paramString = paramString.substr(split + 1);
				}
			}

			if (commandList.find(cmd) != commandList.end()) {
				commandList.at(cmd)(this, params);
			} else {
				int y = std::atoi(std::string(cmd).c_str());
				if (y != 0) {
					mEditor.cursorMoveTo(0, y - 1);
				}
			}

			requireRedraw();
		},
		.exit = [this] {
			exitCommandMode();
		}
	});
	requireRedraw();
}

void TextEditorClient::exitCommandMode(void) {
	mMode = EditMode::Normal;
	setNormalBinds();
	requireRedraw();
}

using namespace std::string_literals;

std::vector<std::pair<std::string, Vec2>> TextEditorClient::generateJumpKeys(std::vector<Vec2> locations) {
	static const std::vector<char> leftKeys = {
		'f', 'a', 'g', 'e', 'w', 'x', 'c', 't', 's', 'r', 'd', 'q', 'b', 'z', 'v',
	};

	static const std::vector<char> rightKeys = {
		'm', 'j', 'u', 'n', 'l', 'i', 'h', '8', 'k', 'y', 'p',
	};

	static const std::vector<char> numberKeys = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	};

	std::vector<std::pair<std::string, Vec2>> pairs;

	std::set<std::uint32_t> matches;

	std::size_t i = 0;
	std::size_t max = 0;

	auto applyOrientation = [&](const std::vector<char>& aKeys, const std::vector<char>& bKeys) {
		matches.clear();
		max += aKeys.size() * bKeys.size();
		for (; i < locations.size() && i < max; i++) {
			std::uint32_t aIdx, bIdx, id;
			do {
				aIdx = std::rand() % aKeys.size();
				bIdx = std::rand() % bKeys.size();
				id = aIdx | (bIdx << 5);
			} while (matches.count(id) != 0);
			matches.insert(id);
	
			pairs.push_back(std::make_pair(""s + aKeys[aIdx] + bKeys[bIdx], locations[i]));
		}
	};

	applyOrientation(leftKeys, rightKeys);
	applyOrientation(rightKeys, leftKeys);
	applyOrientation(leftKeys, leftKeys);
	applyOrientation(rightKeys, rightKeys);
	applyOrientation(rightKeys, numberKeys);
	applyOrientation(leftKeys, numberKeys);
	applyOrientation(numberKeys, rightKeys);
	applyOrientation(numberKeys, leftKeys);

	return pairs;
}

void TextEditorClient::enterJumpMode(void) {
	mMode = EditMode::Jump;
	setNoBinds();
	std::vector<Vec2> words = mEditor.indexWordsLocations();
	mJumpPairs = generateJumpKeys(words);
	mCodeView.enterJumpMode(mJumpPairs);
	requireRedraw();
}

void TextEditorClient::exitJumpMode(void) {
	mMode = EditMode::Normal;
	mCodeView.exitJumpMode();
	mJumpInput.clear();
	setNormalBinds();
	requireRedraw();
}

void TextEditorClient::exitPrompt(void) {
	mMode = EditMode::Normal;
	setNormalBinds();
	requireRedraw();
}

void TextEditorClient::enterWindowMoveMode(void) {
	mMode = EditMode::WindowMove;
	setWindowMoveBinds();
	requireRedraw();
}

void TextEditorClient::exitWindowMoveMode(void) {
	mMode = EditMode::Normal;
	setNormalBinds();
	requireRedraw();
}

Key TextEditorClient::getKey(void) {
	Key key = Terminal::getKey();
	if (sIsRecordingMacro) {
		sMacroBuffer.push_back(key);
	}
	return key;
}

void TextEditorClient::cursorMoveToNextChar(void) {
	char chr = getKey();
	if (CharSets::all.contains(chr)) {
		Cursor cursor = mEditor.getCursor();
		std::size_t pos = cursor.y->data.findNextIndex(chr, cursor.x);
		if (pos < cursor.y->data.length()) {
			mEditor.cursorMoveToX(pos);
		}
	}
}

void TextEditorClient::cursorMoveToPrevChar(void) {
	char chr = getKey();
	if (CharSets::all.contains(chr)) {
		Cursor cursor = mEditor.getCursor();
		mEditor.cursorMoveToX(cursor.y->data.findPrevIndex(chr, cursor.x));
	}
}

void TextEditorClient::deleteMovement(void) {
	enterSelectMode();

	do getDisplay()->nextEvent();
	while (keysBuffered());

	mEditor.deleteSelection();
	exitSelectMode();
	setNormalBinds();
}

void TextEditorClient::copyMovement(void) {
	Cursor saved = mEditor.getCursor();

	enterSelectMode();

	do getDisplay()->nextEvent();
	while (keysBuffered());

	mEditor.copySelection();
	exitSelectMode();

	setNormalBinds();

	mEditor.cursorMoveTo(saved);
}

void TextEditorClient::lineCopyMovement(void) {
	Cursor saved = mEditor.getCursor();

	enterLineSelectMode();

	do getDisplay()->nextEvent();
	while (keysBuffered());

	mEditor.copySelection();
	exitSelectMode();

	setNormalBinds();

	mEditor.cursorMoveTo(saved);
}

void TextEditorClient::copyEditorCopyBufferToSystemCopyBuffer(void) {
	std::string data;
	const CopyBuffer& cbuf = mEditor.getCopyBuffer();
	for (WChar chr : cbuf.data) {
		if (chr == '\'') data.push_back('\\');
		if (chr == '\\') data.push_back('\\');
		data.append(chr.view().begin(), chr.view().end());
	}
	std::string cmd = Format::format("echo -n $'{}' | xclip -selection c", data);
	std::FILE* fp = ::popen(cmd.c_str(), "r");
	::pclose(fp);
}

void TextEditorClient::inputFindNext(void) {
	mMode = EditMode::Prompt;
	setNoBinds();
	mPrompt = TextPrompt("/", {
		.change = [this] {
			requireSelfRedraw();
		},
		.submit = [this] {
			exitPrompt();
			WString find(mPrompt.get());
			mLastFind = find;
			mEditor.cursorMoveTo(mEditor.findNextFromCursor(find));
			requireRedraw();
		},
		.exit = [this] {
			exitPrompt();
		}
	});
	requireSelfRedraw();
}

void TextEditorClient::inputFindPrev(void) {
	mMode = EditMode::Prompt;
	setNoBinds();
	requireSelfRedraw();
	mPrompt = TextPrompt("?", {
		.change = [this] {
			requireSelfRedraw();
		},
		.submit = [this] {
			exitPrompt();
			WString find(mPrompt.get());
			mLastFind = find;
			mEditor.cursorMoveTo(mEditor.findPrevFromCursor(find));
			requireRedraw();
		},
		.exit = [this] {
			exitPrompt();
		}
	});
}

void TextEditorClient::inputReplace(void) {
	mMode = EditMode::Prompt;
	setNoBinds();
	requireSelfRedraw();
	mPrompt = TextPrompt("s/", {
		.change = [this] {
			requireSelfRedraw();
		},
		.submit = [this] {
			exitPrompt();
			requireRedraw();
			std::string_view data = mPrompt.get();

			if (data.length() == 0) return;
			if (data[0] == '/') return;

			std::string_view find;

			std::size_t i = 0;
			for (; i < data.length(); i++) {
				if (data[i] == '/' && data[i-1] != '\\') {
					find = data.substr(0, i);
					break;
				}
			}

			if (i >= data.length()) return;

			std::string_view replace;

			std::size_t start = i + 1;
			for (i = start; i < data.length(); i++) {
				if (data[i] == '/' && data[i-1] != '\\') {
					replace = data.substr(start, i - start);
					break;
				}
			}

			if (i == data.length()) {
				Log::log(Format::format("0: '{}' : '{}'", find, replace));
				mEditor.replaceNextFromCursor(WString(std::string(find)), WString(std::string(replace)));
			}

			i++;

			if (i >= data.length()) {
				Log::log(Format::format("0: '{}' : '{}'", find, replace));
				mEditor.replaceNextFromCursor(WString(std::string(find)), WString(std::string(replace)));
			} else if (data[i] == 'g') {
				Log::log(Format::format("g: '{}' : '{}'", find, replace));
			} else {
				Log::log(Format::format("0: '{}' : '{}'", find, replace));
				mEditor.replaceNextFromCursor(WString(std::string(find)), WString(std::string(replace)));
			}
		},
		.exit = [this] {
			exitPrompt();
		}
	});
}

void TextEditorClient::repeatFindNext(void) {
	mEditor.cursorMoveTo(mEditor.findNextFromCursor(mLastFind));
}

void TextEditorClient::repeatFindPrev(void) {
	mEditor.cursorMoveTo(mEditor.findPrevFromCursor(mLastFind));
}

void TextEditorClient::findNextUnderCursor(void) {
	mLastFind = mEditor.getWordUnderCursor();
	mEditor.cursorMoveTo(mEditor.findNextFromCursor(mLastFind));
}

void TextEditorClient::findPrevUnderCursor(void) {
	mLastFind = mEditor.getWordUnderCursor();
	mEditor.cursorMoveTo(mEditor.findPrevFromCursor(mLastFind));
}

void TextEditorClient::setMarker(void) {
	char chr = getKey();
	if (CharSets::all.contains(chr) == false) return;

	mMarkers[chr] = mEditor.getCursorPosition();
}

void TextEditorClient::gotoMarker(void) {
	char chr = getKey();
	if (CharSets::all.contains(chr) == false) return;
	auto it = mMarkers.find(chr);
	if (it != mMarkers.end()) {
		mEditor.cursorMoveTo(it->second);
	}
}

void TextEditorClient::openFuzzy(const std::string& title, const std::vector<std::string> data, std::function<void(std::string)> callback) {
	Vec2 size = getTab()->getSize() * 0.8;

	FuzzyWindow* fw = new FuzzyWindow(title, data, size, [callback](FuzzyWindow* fw, std::string result) {
		callback(result);
		delete fw;
	});
	getTabArea()->addChild(fw);
	getDisplay()->setSelected(fw);
}

void TextEditorClient::fuzzyEditFile(void) {
	openFuzzy(
		"edit",
		FileSystem::getAllSubFilesAndDirectories("./"),
		[this](std::string result) {
			if (result.length() > 0) {
				editFile(result);
			}
		}
	);
}

void TextEditorClient::fuzzyOpenFile(void) {
	openFuzzy(
		"open",
		FileSystem::getAllSubFilesAndDirectories("./"),
		[this](std::string result) {
			if (result.length() > 0) {
				openFile(result);
			}
		}
	);
}

void TextEditorClient::fuzzyOpenFloatingFile(void) {
	openFuzzy(
		"float",
		FileSystem::getAllSubFilesAndDirectories("./"),
		[this](std::string result) {
			if (result.length() > 0) {
				openFloatingFile(result);
			}
		}
	);
}

void TextEditorClient::fuzzyGoto(void) {
	std::vector<Client*> clients = getTabArea()->getAllClients();
	std::vector<std::string> clientNames;
	for (std::size_t i = 0; i < clients.size(); i++) {
		clientNames.push_back(Format::format("{}: {}", i, clients[i]->getTitle().string()));
	}

	openFuzzy(
		"goto",
		clientNames,
		[clients](std::string result) {
			if (result.length() > 0) {
				int idx = std::stoi(result.substr(0, result.find(':')));
				Client* c = clients[idx];
				Tab* tab = c->getTab();
				c->getTabArea()->setActive(tab);
				tab->setSelected(c);
			}
		}
	);
}

void TextEditorClient::toggleRecordingMacro(void) {
	if (sIsRecordingMacro) {
		sMacroBuffer.pop_back();
	} else {
		sMacroBuffer.clear();
	}
	sIsRecordingMacro = !sIsRecordingMacro;
	requireSelfRedraw();
}

void TextEditorClient::runMacro(void) {
	if (sIsRecordingMacro) {
		sMacroBuffer.pop_back();
		return;
	}

	std::size_t top = Terminal::stagedEventsLeft();

	for (auto it = sMacroBuffer.rbegin(); it != sMacroBuffer.rend(); it++) {
		Terminal::stageEvent(Event(EventType::Key, KeyEvent(*it)));
	}

	while (Terminal::stagedEventsLeft() != top) {
		getDisplay()->nextEvent();
	}
}

void TextEditorClient::openLogClient(void) {
	getTab()->addClient<LogClient>();
}

Vec2 TextEditorClient::getCursor(void) {
	auto cursor = Terminal::CursorStyle::SteadyUnderline;
	switch (mMode) {
	case EditMode::Select:
	case EditMode::LineSelect:
	case EditMode::WordSelect:
	case EditMode::Insert: {
		cursor = Terminal::CursorStyle::SteadyBar; break;
	} break;
	default: break;
	}

	Terminal::setCursor(cursor);

	if (mMode == EditMode::Prompt) {
		return getRealPos(Vec2(mPrompt.getCursor(), mSize.y - 1));
	} else {
		return mCodeView.getRealPos(mEditor.getCursorRelativePosition());
	}
}

void TextEditorClient::onRightMouseButtonDown(Vec2 pos) {
	if (mMode == EditMode::WindowMove) {
		onRightMouseButtonDrag(pos);
	} else {
		mEditor.cursorMoveTo(mEditor.relativeToAbsolutePosition(getPosInOtherComponent(pos, &mCodeView)));
	}
}

void TextEditorClient::onRightMouseButtonDrag(Vec2 pos) {
	if (mMode == EditMode::WindowMove) {
		Vec2 windowResizeDelta = pos - mSize;
		windowResizeX(windowResizeDelta.x);
		windowResizeY(windowResizeDelta.y);
	} else {
		onLeftMouseButtonDrag(pos);
	}
}

void TextEditorClient::onLeftMouseButtonDown(Vec2 pos) {
	if (mMode == EditMode::WindowMove) {
		mWindowDragLatch = pos;
	} else {
		if (mEditor.isSelecting()) {
			exitSelectMode();
		}
		if (mMode != EditMode::Insert) {
			enterInsertMode();
		}
		mEditor.cursorMoveTo(mEditor.relativeToAbsolutePosition(getPosInOtherComponent(pos, &mCodeView)));
	}
}

void TextEditorClient::onLeftMouseButtonDrag(Vec2 pos) {
	if (mMode == EditMode::WindowMove) {
		Vec2 windowMoveDelta = pos - mWindowDragLatch;
		mWindowDragLatch = pos - windowMoveDelta;
		windowMoveX(windowMoveDelta.x);
		windowMoveY(windowMoveDelta.y);
	} else {
		if (mEditor.isSelecting() == false) {
			enterSelectMode();
		}
		mEditor.cursorMoveTo(mEditor.relativeToAbsolutePosition(getPosInOtherComponent(pos, &mCodeView)));
	}
}

void TextEditorClient::onLeftMouseButtonDoubleDown(Vec2 pos) {
	enterWordSelectMode();
	mEditor.cursorMoveToWordStart();
}

void TextEditorClient::onLeftMouseButtonDoubleDrag(Vec2 pos) {
	mEditor.cursorMoveTo(mEditor.relativeToAbsolutePosition(getPosInOtherComponent(pos, &mCodeView)));
	mEditor.cursorMoveToWordStart();
}

void TextEditorClient::onLeftMouseButtonTripleDown(Vec2 pos) {
	enterLineSelectMode();
}

void TextEditorClient::onLeftMouseButtonTripleDrag(Vec2 pos) {
	mEditor.cursorMoveTo(mEditor.relativeToAbsolutePosition(getPosInOtherComponent(pos, &mCodeView)));
}

void TextEditorClient::onResize(void) {
	resizeEditor();
}

void TextEditorClient::onSelect(void) {
	getTab()->setSelected(this);
	requireRedraw();
}

void TextEditorClient::onDeselect(void) {
	requireRedraw();
}

void TextEditorClient::onPaste(WStringView data) {
	switch (mMode) {
	case EditMode::Insert: {
		for (auto c : data) {
			mEditor.insertCharAtCursor(c);
		}
	} break;
	case EditMode::Normal: {
		enterInsertModeRight();
		for (auto c : data) {
			mEditor.insertCharAtCursor(c);
		}
		exitInsertMode();
	} break;
	default: {
	} break;
	}
}

void TextEditorClient::unhandledKey(Key key) {
	switch (mMode) {
	case EditMode::Insert: {
		mEditor.insertCharAtCursor(key);
	} break;
	case EditMode::Replace: {
		mEditor.replaceCharAtCursor(key);
		mEditor.cursorMoveX(1);
	} break;
	case EditMode::Jump: {
		if (!CharSets::letters.contains(key) && !CharSets::numbers.contains(key)) {
			exitJumpMode();
			break;
		}

		mJumpInput.push_back(key);
		if (mJumpInput.size() == 2) {
			auto it = std::find_if(
				mJumpPairs.begin(),
				mJumpPairs.end(),
				[this](const auto& pair) { return pair.first == mJumpInput; }
			);
			if (it == mJumpPairs.end()) {
				exitJumpMode();
			} else {
				mEditor.cursorMoveTo(it->second);
				exitJumpMode();
			}
		}
	} break;
	case EditMode::Prompt: {
		mPrompt.input(key);
	} break;
	default: break;
	}
}

void TextEditorClient::onKeyDown(Key key) {
	requireSelfRedraw();
	if (sIsRecordingMacro) {
		Log::log(Format::format("{}: {}", (int)mMode, key));
		sMacroBuffer.push_back(key);
	}
	interpret(key);
}

void TextEditorClient::onPrerender(void) {
	if (mEditor.viewHasChanged()) {
		if (mEditor.getLineCount() != mOldLineCount) {
			resizeEditor();
		}
		mEditor.unsetViewChanged();
		requireRedraw();
	}
}

void TextEditorClient::renderStatusBar(void) {
	if (mStatus.type != StatusMessageType::None) {
		renderStatus();
		return;
	}

	const Theme* theme = Vidd::getTheme();
	if (isSelected()) {
		Draw::style(theme->barActive);
	} else {
		Draw::style(theme->barInactive);
	}

	Vec2 pos = Vec2(0, mSize.y - 1);

	drawFilledBox(pos, Vec2(mSize.x, 1), ' ');

	if (sIsRecordingMacro) {
		drawText(pos, WString("@"));
		pos.x += 1;
	}

	switch (mMode) {
	case EditMode::Normal: {
		drawText(pos, WString("[NORMAL]"));
	} break;
	case EditMode::Insert: {
		drawText(pos, WString("[INSERT]"));
	} break;
	case EditMode::Replace: {
		drawText(pos, WString("[REPLACE]"));
	} break;
	case EditMode::Select: {
		drawText(pos, WString("[SELECT]"));
	} break;
	case EditMode::LineSelect: {
		drawText(pos, WString("[LINE-SELECT]"));
	} break;
	case EditMode::WordSelect: {
		drawText(pos, WString("[WORD-SELECT]"));
	} break;
	case EditMode::WindowMove: {
		drawText(pos, WString("[WINDOW-MOVE]"));
	} break;
	case EditMode::Jump: {
		drawText(pos, WString("[JUMP]"));
	} break;
	case EditMode::Prompt: {
		drawText(pos, WString(mPrompt.getLine()));
	} break;
	default: break;
	}

	drawTextReverse(Vec2(mSize.x - 1, pos.y), WString(" "));

	std::string fileName = mEditor.getFileName();
	if (1.5 * fileName.length() > mSize.x) {
		fileName = FileSystem::getFileName(fileName);
	}
	fileName = Format::format("[{}]", fileName);
	drawTextReverse(WString(fileName));

	const Cursor& cursor = mEditor.getCursor();
	WString cursorPos = WString(Format::format("[{},{}]", cursor.x + 1, cursor.y->number + 1));

	drawTextReverse(cursorPos);
}

void TextEditorClient::renderStatus(void) {
	const Theme* theme = Vidd::getTheme();
	if (isSelected()) {
		Draw::style(theme->barActive);
	} else {
		Draw::style(theme->barInactive);
	}

	Vec2 pos = Vec2(0, mSize.y - 1);
	drawFilledBox(pos, Vec2(mSize.x, 1), ' ');
	drawText(pos, WString(mStatus.text));
}

void TextEditorClient::renderLineNumbers(void) {
	const Theme* theme = Vidd::getTheme();
	const ViewPort& view = mEditor.getViewPort();
	int y = mEditor.getCursor().y->get(view.y)->number + 1;
	int cursorLine = mEditor.getCursor().y->number + 1;
	int lineCount = mEditor.getLineCount();
	int lineCountWidth = Utils::countDigits(lineCount);

	Draw::style(theme->numberStyle);

	int i = 0;
	for (; i < view.height && y <= lineCount; i++) {
		if (y == cursorLine) {
			Style st = theme->numberStyle;
			st.fg = theme->important;
			Draw::style(st);
		}
		drawText(Vec2(0, i), WString(Format::vformat(theme->numberFormat, y, lineCountWidth)));
		if (y == cursorLine) {
			Draw::style(theme->numberStyle);
		}
		y++;
	}

	Draw::style(theme->emptyLineStyle);

	for (; i < view.height; i++) {
		drawText(Vec2(0, i), WString(Format::vformat(theme->emptyLineFormat, ' ', lineCountWidth)));
		y++;
	}
}

void TextEditorClient::openDirectory(void) {
	std::string path = mEditor.getFileName();
	getTab()->replaceClientWithNew<FileBrowserClient>(this, FileSystem::getParentDirectory(path), FileSystem::getFileName(path));
}

void TextEditorClient::openTermianl(void) {
	getTab()->addAndSelectClient<TerminalClient>();
}

void TextEditorClient::openFloatingTermianl(void) {
	Client* client = getTab()->addAndSelectClient<TerminalClient>();
	client->setFloating();
	Component* parent = client->getParent();
	Vec2 tabSize = getTab()->getSize();
	Vec2 windowSize = tabSize * 0.8;
	parent->setSize(windowSize);
	parent->setPos((tabSize - windowSize) / 2);
}

void TextEditorClient::executeLine(void) {
	Process proc({ mEditor.getCursor().y->data.string(), "2>&1" });

	Timer fullTimer;
	fullTimer.start();

	Timer hangTimer;
	hangTimer.start();

	enterInsertModeOnNewLineDown();
	while (proc.isOpen()) {
		if (fullTimer.now() > 10.0) {
			break;
		}

		if (proc.readReady() == false) {
			if (hangTimer.now() > 1.0) break;
			continue;
		}
		hangTimer.reset();

		for (auto c : proc.read(128)) {
			mEditor.insertCharAtCursor(c);
		}
	}
	exitInsertMode();
}

void TextEditorClient::render(void) {
	if (mShowStatusBar) {
		renderStatusBar();
	}
	if (mShowLineNumbers) {
		renderLineNumbers();
	}
}
