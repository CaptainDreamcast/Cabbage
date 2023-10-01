#include "gamescreen.h"

#include <prism/soundeffect.h>

#include "bookscreen.h"

class GameScreen
{
public:
	std::set<std::string> mGlobalFlags;

	GameScreen() {
		loadFiles();
		loadPositions();
		loadEntities();
		loadSelectScreenEntities();
		loadVNDialogs();
		playVNDialog("intro2");
		streamMusicFile("music/sailing.ogg");
	}

	int godEntity;
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	MugenSounds mSounds;
	void loadFiles()
	{
		mSprites = loadMugenSpriteFileWithoutPalette("game/GAME.sff");
		mAnimations = loadMugenAnimationFile("game/GAME.air");
		mSounds = loadMugenSoundFile("game/GAME.snd");
	}

	std::set<std::string> mObjects[2];
	void loadPositions()
	{
		mObjects[0].emplace("you");
		mObjects[0].emplace("wolf");
		mObjects[0].emplace("goat");
		mObjects[0].emplace("cabbage");
	}

	std::string crossingAnimal = "";
	void updateBackground()
	{
		if (mObjects[0].find("you") != mObjects[0].end())
		{
			changeBlitzMugenAnimation(mBGEntity, 100);

			setBlitzEntityPosition(mWolfEntity, Vector3D(32, 202, 2));
			setBlitzEntityPosition(mGoatEntity, Vector3D(126, 123, 3));
			setBlitzEntityPosition(mCabbageEntity, Vector3D(202, 193, 4));
		}
		else
		{
			changeBlitzMugenAnimation(mBGEntity, 120);

			setBlitzEntityPosition(mWolfEntity, Vector3D(320-32, 202, 2));
			setBlitzEntityPosition(mGoatEntity, Vector3D(320-126, 123, 3));
			setBlitzEntityPosition(mCabbageEntity, Vector3D(320-202, 193, 4));
		}

		auto currentShore = getActiveShore();
		auto& object = mObjects[currentShore];

		if (object.find("wolf") != object.end())
		{
			setBlitzMugenAnimationVisibility(mWolfEntity, 1);
		}
		if (object.find("goat") != object.end())
		{
			setBlitzMugenAnimationVisibility(mGoatEntity, 1);
		}
		if (object.find("cabbage") != object.end())
		{
			setBlitzMugenAnimationVisibility(mCabbageEntity, 1);
		}

	}

	std::vector<int> currentStory = { 0,0,0,0 };
	std::vector<int> maxStory = { 2,4,2,2 };
	void startCrossing(const std::string& animal)
	{
		int storyIndex;
		std::string storyName;
		crossingAnimal = animal;
		if (crossingAnimal == "you") crossingAnimal = "";

		if (crossingAnimal == "")
		{
			storyName = "nobody";
			storyIndex = 3;
		}
		else if (crossingAnimal == "wolf")
		{
			storyName = "wolf";
			storyIndex = 0;
		}
		else if (crossingAnimal == "goat")
		{
			storyName = "goat";
			storyIndex = 1;
		}
		else if (crossingAnimal == "cabbage")
		{
			storyName = "cabbage";
			storyIndex = 2;
		}

		currentStory[storyIndex] = std::min(currentStory[storyIndex] + 1, maxStory[storyIndex]);
		std::stringstream ss;
		ss << storyName << "_crossing_" << currentStory[storyIndex];
		setBackgroundLake();
		playVNDialog(ss.str());

		streamMusicFile("music/sailing.ogg");
		checkLossConditions();
	}

	void checkLossConditions()
	{
		auto objectCopy = mObjects[getActiveShore()];
		objectCopy.erase("you");
		
		if (crossingAnimal != "")
		{
			objectCopy.erase(crossingAnimal);
		}

		if (objectCopy.size() == 2) {

			if (objectCopy.find("wolf") != objectCopy.end() && objectCopy.find("goat") != objectCopy.end())
			{
				setBookName("goat_dies_ending");
				setNewScreen(getBookScreen());
			}
			else if (objectCopy.find("cabbage") != objectCopy.end() && objectCopy.find("goat") != objectCopy.end())
			{
				setBookName("cabbage_dies_ending");
				setNewScreen(getBookScreen());
			}
		}

		if (objectCopy.size() == 3 && (objectCopy.find("cabbage") != objectCopy.end() && objectCopy.find("goat") != objectCopy.end() && objectCopy.find("wolf") != objectCopy.end()))
		{
			setBookName("abandonment_ending");
			setNewScreen(getBookScreen());
		}

	}

	void finishCrossing()
	{
		if (mObjects[0].find("you") != mObjects[0].end())
		{
			mObjects[0].erase("you");
			mObjects[1].emplace("you");
			if (crossingAnimal != "")
			{
				mObjects[0].erase(crossingAnimal);
				mObjects[1].emplace(crossingAnimal);
			}
		}
		else
		{
			mObjects[1].erase("you");
			mObjects[0].emplace("you");
			if (crossingAnimal != "")
			{
				mObjects[1].erase(crossingAnimal);
				mObjects[0].emplace(crossingAnimal);
			}
		}
		updateBackground();
		checkVictoryCondition();
	}

	void checkVictoryCondition()
	{
		auto& objects = mObjects[1];
		if (objects.size() == 4)
		{
			playVNDialog("final1");
		}
	}

	void setBackgroundLake()
	{
		if (crossingAnimal == "")
		{
			changeBlitzMugenAnimation(mBGEntity, 110);
		}
		else if (crossingAnimal == "wolf")
		{
			changeBlitzMugenAnimation(mBGEntity, 111);
		}
		else if (crossingAnimal == "goat")
		{
			changeBlitzMugenAnimation(mBGEntity, 112);
		}
		else if (crossingAnimal == "cabbage")
		{
			changeBlitzMugenAnimation(mBGEntity, 113);
		}

		setBlitzMugenAnimationVisibility(mWolfEntity, 0);
		setBlitzMugenAnimationVisibility(mGoatEntity, 0);
		setBlitzMugenAnimationVisibility(mCabbageEntity, 0);
	}

	std::string activeDialogName = "";
	int currentLoadedVoiceSoundEffectId = -1;
	int currentPlayingVoiceSoundEffectId = -1;
	void playVoiceClip()
	{
		std::stringstream ss;
		ss << "voice/" << activeDialogName << "/" << currentDialogStage << ".ogg";
		if (isFile(ss.str()))
		{
			currentLoadedVoiceSoundEffectId = loadSoundEffect(ss.str().c_str());
			currentPlayingVoiceSoundEffectId = playSoundEffect(currentLoadedVoiceSoundEffectId);
		}
	}

	int mBGEntity;
	int mWolfEntity;
	int mGoatEntity;
	int mCabbageEntity;
	int mCharacterEntity;
	int mSpeechboxEntity;
	int mSpeechboxArrowEntity;
	int mNameText;
	int mTextText;
	Vector3D arrowBasePosition = Vector3D(300, 230, 20);
	Vector3D selectArrowBasePosition[2] = { Vector3D(40, 160, 20) , Vector3D(280, 160, 20) };

	void loadEntities()
	{
		mBGEntity = addBlitzEntity(Vector3D(0, 0, 1));
		addBlitzMugenAnimationComponent(mBGEntity, &mSprites, &mAnimations, 100);

		mWolfEntity = addBlitzEntity(Vector3D(32, 202, 2));
		addBlitzMugenAnimationComponent(mWolfEntity, &mSprites, &mAnimations, 101);

		mGoatEntity = addBlitzEntity(Vector3D(126, 123, 3));
		addBlitzMugenAnimationComponent(mGoatEntity, &mSprites, &mAnimations, 102);

		mCabbageEntity = addBlitzEntity(Vector3D(202, 193, 4));
		addBlitzMugenAnimationComponent(mCabbageEntity, &mSprites, &mAnimations, 103);

		godEntity = addBlitzEntity(Vector3D(153, 8, 4));
		addBlitzMugenAnimationComponent(godEntity, &mSprites, &mAnimations, 601);
		setBlitzMugenAnimationVisibility(godEntity, 0);


		mCharacterEntity = addBlitzEntity(Vector3D(160, 240, 6));
		addBlitzMugenAnimationComponent(mCharacterEntity, &mSprites, &mAnimations, -1);
		setBlitzMugenAnimationVisibility(mCharacterEntity, 0);

		mSpeechboxEntity = addBlitzEntity(Vector3D(160, 240, 10));
		addBlitzMugenAnimationComponent(mSpeechboxEntity, &mSprites, &mAnimations, 300);
		setBlitzMugenAnimationVisibility(mSpeechboxEntity, 0);

		mSpeechboxArrowEntity = addBlitzEntity(arrowBasePosition);
		addBlitzMugenAnimationComponent(mSpeechboxArrowEntity, &mSprites, &mAnimations, 301);
		setBlitzMugenAnimationVisibility(mSpeechboxArrowEntity, 0);

		mNameText = addMugenTextMugenStyle("NAME", Vector3D(30, 174, 15), Vector3DI(2, 0, 1));
		setMugenTextVisibility(mNameText, 0);

		mTextText = addMugenTextMugenStyle("TEXT", Vector3D(30, 190, 15), Vector3DI(2, 0, 1));
		setMugenTextTextBoxWidth(mTextText, 260);
		setMugenTextVisibility(mTextText, 0);
	}

	int mSelectBG;
	int mSelectNameId;
	int mSelectTextId;
	int mSelectCharacterId;
	int mSelectArrowIds[2];
	int mSelectBringButton;
	void loadSelectScreenEntities()
	{
		mSelectBG = addBlitzEntity(Vector3D(160, 0, 10));
		addBlitzMugenAnimationComponent(mSelectBG, &mSprites, &mAnimations, 500);

		mSelectNameId = addMugenTextMugenStyle("NAME", Vector3D(160, 14, 15), Vector3DI(2, 0, 0));

		mSelectTextId = addMugenTextMugenStyle("TEXT", Vector3D(30, 27, 15), Vector3DI(2, 0, 1));
		setMugenTextTextBoxWidth(mSelectTextId, 260);

		mSelectCharacterId = addBlitzEntity(Vector3D(160, 310, 9));
		addBlitzMugenAnimationComponent(mSelectCharacterId, &mSprites, &mAnimations, -1);

		mSelectArrowIds[0] = addBlitzEntity(selectArrowBasePosition[0]);
		addBlitzMugenAnimationComponent(mSelectArrowIds[0], &mSprites, &mAnimations, 501);

		mSelectArrowIds[1] = addBlitzEntity(selectArrowBasePosition[1]);
		addBlitzMugenAnimationComponent(mSelectArrowIds[1], &mSprites, &mAnimations, 502);

		mSelectBringButton = addBlitzEntity(Vector3D(280, 220, 10));
		addBlitzMugenAnimationComponent(mSelectBringButton, &mSprites, &mAnimations, 503);
	}

	int mSelectedAnimal;
	void setSelectScreenActivity(int isActive)
	{
		setBlitzMugenAnimationVisibility(mSelectBG, isActive);
		setMugenTextVisibility(mSelectNameId, isActive);
		
		setMugenTextVisibility(mSelectTextId, isActive);

		setBlitzMugenAnimationVisibility(mSelectCharacterId, isActive);

		setBlitzMugenAnimationVisibility(mSelectArrowIds[0], isActive);
		setBlitzMugenAnimationVisibility(mSelectArrowIds[1], isActive);

		setBlitzMugenAnimationVisibility(mSelectBringButton, isActive);

		if (isActive)
		{
			mSelectedAnimal = 0;
			mCanSelect = 0;
			updateSelection(0);
			streamMusicFile("music/select.ogg");
		}
	}

	struct VNDialogPart
	{
		std::string speaker;
		std::string text;
		int bg;
	};

	struct VNDialog {
		std::vector<VNDialogPart> mDialogParts;
	};

	std::map<std::string, VNDialog> mDialogs;
	VNDialog* mActiveDialog;

	void loadVNDialogs()
	{
		MugenDefScript script;
		loadMugenDefScript(&script, "game/DIALOGS.def");

		auto group = script.mFirstGroup;
		while (group)
		{
			loadSingleVNDialogFromGroup(group);
			group = group->mNext;
		}

		unloadMugenDefScript(&script);
	}

	void loadSingleVNDialogFromGroup(MugenDefScriptGroup* group)
	{
		auto& conversation = mDialogs[group->mName];
		auto iterator = list_iterator_begin(&group->mOrderedElementList);
		while (iterator)
		{
			VNDialogPart part;
			MugenDefScriptGroupElement* element = (MugenDefScriptGroupElement*)list_iterator_get(iterator);
			part.speaker = element->mName;
			part.text = getSTLMugenDefStringVariableAsElementForceAddWhiteSpaces(element);
			conversation.mDialogParts.push_back(part);

			if (!list_has_next(iterator))
			{
				iterator = nullptr;
			}
			else {
				list_iterator_increase(&iterator);
			}
		}
	}

	int currentDialogStage;
	void playVNDialog(const std::string& dialogName)
	{
		if (mDialogs.find(dialogName) == mDialogs.end())
		{
			logErrorFormat("Unable to find dialog: %s", dialogName.c_str());
			assert(0);
		}

		activeDialogName = dialogName;
		mActiveDialog = &mDialogs[dialogName];

		changeBlitzMugenAnimation(mCharacterEntity, -1);
		setSelectScreenActivity(0);
		setVNScreenActivity(1);
		currentDialogStage = -1;
		playNextDialogStage();
	}

	int isVNScreenActive = 0;
	void setVNScreenActivity(int isActive)
	{
		
		setBlitzMugenAnimationVisibility(mSpeechboxArrowEntity, isActive);
		if (!isFightActive)
		{
			setMugenTextVisibility(mNameText, isActive);
			setBlitzMugenAnimationVisibility(mCharacterEntity, isActive);
			setBlitzMugenAnimationVisibility(mSpeechboxEntity, isActive);
		}
		else
		{
			setMugenTextVisibility(mNameText, false);
			setBlitzMugenAnimationVisibility(mCharacterEntity, false);
			setBlitzMugenAnimationVisibility(mSpeechboxEntity, false);
		}
		setMugenTextVisibility(mTextText, isActive);

		isVNScreenActive = isActive;
	}

	int getAnimFromSpeaker(const std::string& name)
	{
		if (name == "wolf") return 200;
		if (name == "goat") return 201;
		if (name == "cabbage") return 202;
		if (name == "god") return 203;
		if (name == "nobody") return -1;
		return -2;
	}

	int isFightActive = 0;
	void startFight()
	{
		changeBlitzMugenAnimation(mWolfEntity, -1);
		changeBlitzMugenAnimation(mGoatEntity, -1);
		changeBlitzMugenAnimation(mCabbageEntity, -1);

		changeBlitzMugenAnimation(mBGEntity, 600);
		setBlitzMugenAnimationVisibility(godEntity, 1);

		isFightActive = 1;
		streamMusicFile("music/jrpg.ogg");
	}

	void playNextDialogStage()
	{
		if (currentPlayingVoiceSoundEffectId != -1)
		{
			stopSoundEffect(currentPlayingVoiceSoundEffectId);
			currentPlayingVoiceSoundEffectId = -1;
		}
		if (currentLoadedVoiceSoundEffectId != -1)
		{

			unloadSoundEffect(currentLoadedVoiceSoundEffectId);
			currentLoadedVoiceSoundEffectId = -1;
		}

		currentDialogStage++;
		if (currentDialogStage >= mActiveDialog->mDialogParts.size())
		{
			setVNScreenActivity(0);
			setSelectScreenActivity(1);
			return;
		}

		auto& dialogPart = mActiveDialog->mDialogParts[currentDialogStage];

		if (dialogPart.speaker == "setglobalvar")
		{
			mGlobalFlags.insert(dialogPart.text);
			playNextDialogStage();
		}
		else if (dialogPart.speaker == "cross")
		{
			finishCrossing();
			playNextDialogStage();
		}
		else if (dialogPart.speaker == "fight")
		{
			startFight();
			playVNDialog("jrpg_fight");
		}
		else if (dialogPart.speaker == "victory")
		{
			setBookName("final2");
			setNewScreen(getBookScreen());
		}
		else
		{
			playVoiceClip();
			std::string speaker = dialogPart.speaker;
			speaker[0] = toupper(speaker[0]);

			int anim = getAnimFromSpeaker(dialogPart.speaker);
			if (anim != -2)
			{
				changeBlitzMugenAnimation(mCharacterEntity, anim);
			}

			changeMugenText(mNameText, speaker.c_str());
			changeMugenText(mTextText, dialogPart.text.c_str());
			setMugenTextBuildup(mTextText, 1);
		}
	}

	void update() {
		updateVNScreen();
		updateSelectScreen();
		updateGod();
	}

	void updateVNScreen()
	{
		if (!isVNScreenActive) return;

		updateVNArrow();
		updateVNInput();
	}

	void updateSelectScreen()
	{
		if (isVNScreenActive) return;

		updateSelectArrows();
		updateSelectInput();
	}

	int getActiveShore()
	{
		return (mObjects[0].find("you") == mObjects[0].end());
	}

	void updateSelection(int delta)
	{
		auto activeShore = getActiveShore();
		std::vector<std::string> animals = std::vector<std::string>(mObjects[activeShore].begin(), mObjects[activeShore].end());

		mSelectedAnimal = (mSelectedAnimal + animals.size() + delta) % animals.size();
		std::string animal = animals[mSelectedAnimal];
		if (animal == "you") animal = "nobody";

		int whichText = randfromInteger(1, 3);
		std::stringstream ss;
		ss << animal << "_" << whichText;

		auto dialog = mDialogs[ss.str()].mDialogParts[0].text;
		std::string animalText = animal;
		animalText[0] = toupper(animalText[0]);
		changeMugenText(mSelectNameId, animalText.c_str());
		changeMugenText(mSelectTextId, dialog.c_str());
		setMugenTextBuildup(mSelectTextId, 1);

		int anim = getAnimFromSpeaker(animal.c_str());
		changeBlitzMugenAnimation(mSelectCharacterId, anim);
	}

	std::string getSelectedAnimal()
	{
		auto activeShore = getActiveShore();
		std::vector<std::string> animals = std::vector<std::string>(mObjects[activeShore].begin(), mObjects[activeShore].end());
		return animals[mSelectedAnimal];
	}

	int mCanSelect = 0;
	void updateSelectInput()
	{
		if (!mCanSelect)
		{
			mCanSelect = 1;
			return;
		}
		if (hasPressedLeftFlank())
		{
			updateSelection(-1);
		}
		else if (hasPressedRightFlank())
		{
			updateSelection(1);
		}

		if (hasPressedAFlank())
		{
			startCrossing(getSelectedAnimal());
		}
	}

	int arrowTicks;
	void updateSelectArrows()
	{
		arrowTicks++;
		setBlitzEntityPositionX(mSelectArrowIds[0], selectArrowBasePosition[0].x + sin(arrowTicks * 0.1) * 3);
		setBlitzEntityPositionX(mSelectArrowIds[1], selectArrowBasePosition[1].x - sin(arrowTicks * 0.1) * 3);
	}

	void updateVNArrow()
	{
		arrowTicks++;
		setBlitzEntityPositionX(mSpeechboxArrowEntity, arrowBasePosition.x + sin(arrowTicks * 0.1) * 3);
	}

	Vector3D hsv2rgb(const Vector3D& in)
	{
		double hh, p, q, t, ff;
		long i;
		Vector3D out;

		if (in.y <= 0.0) {
			out.x = in.z;
			out.y = in.z;
			out.z = in.z;
			return out;
		}
		hh = in.x;
		if (hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = in.z * (1.0 - in.y);
		q = in.z * (1.0 - (in.y * ff));
		t = in.z * (1.0 - (in.y * (1.0 - ff)));

		switch (i)
		{
		case 0:
			out.x = in.z;
			out.y = t;
			out.z = p;
			break;
		case 1:
			out.x = q;
			out.y = in.z;
			out.z = p;
			break;
		case 2:
			out.x = p;
			out.y = in.z;
			out.z = t;
			break;

		case 3:
			out.x = p;
			out.y = q;
			out.z = in.z;
			break;
		case 4:
			out.x = t;
			out.y = p;
			out.z = in.z;
			break;
		case 5:
		default:
			out.x = in.z;
			out.y = p;
			out.z = q;
			break;
		}
		return out;
	}

	double hue = 0;
	void updateGod()
	{
		if (!isFightActive) return;
		hue += 10;
		if (hue >= 360.0) hue -= 360.0;
		const auto rgb = hsv2rgb(Vector3D(hue, 1.0, 1.0));
		setBlitzMugenAnimationColor(godEntity, rgb.x, rgb.y, rgb.z);
	}



	void updateVNInput()
	{
		if (hasPressedAFlank() || hasPressedStart())
		{
			if (isMugenTextBuiltUp(mTextText))
			{
				playNextDialogStage();
			}
			else
			{
				setMugenTextBuiltUp(mTextText);
			}
		}
	}
};

EXPORT_SCREEN_CLASS(GameScreen);
