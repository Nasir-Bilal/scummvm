/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "agi/agi.h"
#include "agi/graphics.h"
#include "agi/sprite.h"

namespace Agi {

// Apple II V2+ views use different color values.
static const byte apple2ViewColorMap[16] = {
	/*00*/ 0x00,
	/*01*/ 0x04,
	/*02*/ 0x01,
	/*03*/ 0x05,
	/*04*/ 0x02,
	/*05*/ 0x08,
	/*06*/ 0x09,
	/*07*/ 0x0b,
	/*08*/ 0x06,
	/*09*/ 0x0d,
	/*0a*/ 0x07,
	/*0b*/ 0x0c,
	/*0c*/ 0x0a,
	/*0d*/ 0x0e,
	/*0e*/ 0x03,
	/*0f*/ 0x0f
};

void AgiEngine::updateView(ScreenObjEntry *screenObj) {
	if (screenObj->flags & fDontUpdate) {
		screenObj->flags &= ~fDontUpdate;
		return;
	}

	int16 celNr = screenObj->currentCelNr;
	int16 lastCelNr = screenObj->celCount - 1;

	switch (screenObj->cycle) {
	case kCycleNormal:
		celNr++;
		if (celNr > lastCelNr)
			celNr = 0;
		break;
	case kCycleEndOfLoop:
		if (celNr < lastCelNr) {
			debugC(5, kDebugLevelResources, "cel %d (last = %d)", celNr + 1, lastCelNr);
			if (++celNr != lastCelNr)
				break;
		}
		if (!screenObj->ignoreLoopFlag) {
			setFlag(screenObj->loop_flag, true);
		} else {
			warning("kCycleEndOfLoop: skip setting flag %d", screenObj->loop_flag);
		}
		screenObj->flags &= ~fCycling;
		screenObj->direction = 0;
		screenObj->cycle = kCycleNormal;
		break;
	case kCycleRevLoop:
		if (celNr) {
			celNr--;
			if (celNr)
				break;
		}
		if (!screenObj->ignoreLoopFlag) {
			setFlag(screenObj->loop_flag, true);
		} else {
			warning("kCycleRevLoop: skip setting flag %d", screenObj->loop_flag);
		}
		screenObj->flags &= ~fCycling;
		screenObj->direction = 0;
		screenObj->cycle = kCycleNormal;
		break;
	case kCycleReverse:
		if (celNr == 0) {
			celNr = lastCelNr;
		} else {
			celNr--;
		}
		break;
	default:
		break;
	}

	setCel(screenObj, celNr);
}

/**
 * Decode an AGI view resource.
 * This function decodes the raw data of the specified AGI view resource
 * and fills the corresponding views array element.
 * @param viewNr number of view resource to decode
 */
int AgiEngine::decodeView(byte *resourceData, uint16 resourceSize, int16 viewNr) {
	AgiView *viewData = &_game.views[viewNr];

	debugC(5, kDebugLevelResources, "decodeView(%d)", viewNr);

	if (resourceSize < 5)
		error("unexpected end of view data for view %d", viewNr);

	if (getVersion() < 0x2000) {
		viewData->headerStepSize = resourceData[0];
		viewData->headerCycleTime = resourceData[1];
	} else {
		viewData->headerStepSize = 0;
		viewData->headerCycleTime = 0;
	}

	bool isAGI256Data = false;
	if (getFeatures() & GF_AGI256) {
		uint16 headerId = READ_LE_UINT16(resourceData);
		isAGI256Data = (headerId == 0xF00F); // AGI 256-2 view detected, 256 color view
	}

	// Apple II V2+ views stopped including the first two bytes
	int headerLoopCountOffset = 2;
	int viewHeaderSize = 5;
	const bool apple2 = (getPlatform() == Common::kPlatformApple2) && getVersion() >= 0x2000;
	if (apple2) {
		headerLoopCountOffset = 0;
		viewHeaderSize = 3;
	}

	byte headerLoopCount = resourceData[headerLoopCountOffset];
	uint16 headerDescriptionOffset = READ_LE_UINT16(resourceData + headerLoopCountOffset + 1);

	viewData->loopCount = headerLoopCount;
	viewData->description = nullptr;
	viewData->loop = nullptr;

	if (headerDescriptionOffset) {
		// Figure out length of description
		uint16 descriptionPos = headerDescriptionOffset;
		uint16 descriptionLen = 0;
		while (descriptionPos < resourceSize) {
			if (resourceData[descriptionPos] == 0)
				break;
			descriptionPos++;
			descriptionLen++;
		}
		// Allocate memory for description
		viewData->description = new byte[descriptionLen + 1];
		// Copy description over
		memcpy(viewData->description, resourceData + headerDescriptionOffset, descriptionLen);
		viewData->description[descriptionLen] = 0; // set terminator
	}

	if (!viewData->loopCount) // no loops, exit now
		return errOK;

	// Check, if at least the loop-offsets are available
	if (resourceSize < viewHeaderSize + (headerLoopCount * 2))
		error("unexpected end of view data for view %d", viewNr);

	// Allocate space for loop-information
	AgiViewLoop *loopData = new AgiViewLoop[headerLoopCount];
	viewData->loop = loopData;

	for (int16 loopNr = 0; loopNr < headerLoopCount; loopNr++) {
		uint16 loopOffset = READ_LE_UINT16(resourceData + viewHeaderSize + (loopNr * 2));

		// Check, if at least the loop-header is available
		if (resourceSize < (loopOffset + 1))
			error("unexpected end of view data for view %d", viewNr);

		// loop-header:
		//  celCount:BYTE [ upper nibble used as mirror data in 2.230 ]
		//  relativeCelOffset[0]:WORD
		//  relativeCelOffset[1]:WORD
		//  etc.
		byte loopHeaderCelCountByte = resourceData[loopOffset];
		const bool isMirrorDataInLoopHeader = (getVersion() == 0x2230);
		if (isMirrorDataInLoopHeader) {
			loopData->celCount = loopHeaderCelCountByte & 0x0f;
		} else {
			loopData->celCount = loopHeaderCelCountByte;
		}
		loopData->cel = nullptr;

		// Check, if at least the cel-offsets for current loop are available
		if (resourceSize < (loopOffset + 1 + (loopData->celCount * 2)))
			error("unexpected end of view data for view %d", viewNr);

		if (loopData->celCount) {
			// Allocate space for cel-information of current loop
			AgiViewCel *celData = new AgiViewCel[loopData->celCount];
			loopData->cel = celData;

			for (int16 celNr = 0; celNr < loopData->celCount; celNr++) {
				uint16 celOffset = READ_LE_UINT16(resourceData + loopOffset + 1 + (celNr * 2));
				celOffset += loopOffset; // cel offset is relative to loop offset, so adjust accordingly

				// Check, if at least the cel-header is available
				if (resourceSize < (celOffset + 3))
					error("unexpected end of view data for view %d", viewNr);

				// cel-header:
				//  width:BYTE
				//  height:BYTE
				//  Transparency + Mirroring:BYTE
				//  celData follows
				byte celHeaderWidth = resourceData[celOffset + 0];
				byte celHeaderHeight = resourceData[celOffset + 1];
				byte celHeaderTransparencyMirror = resourceData[celOffset + 2];
				if (apple2) {
					// Apple II views switched the transparency and mirror bits
					celHeaderTransparencyMirror = (celHeaderTransparencyMirror << 4) | (celHeaderTransparencyMirror >> 4);
				}

				byte celHeaderClearKey;
				bool celHeaderMirrored = false;
				if (!isAGI256Data) {
					// regular AGI view data
					// Transparency + Mirroring byte is as follows:
					//  Bit 0-3 - clear key
					//  Bit 4-6 - original loop, that is not supposed to be mirrored in any case
					//  Bit 7   - apply mirroring
					celHeaderClearKey = celHeaderTransparencyMirror & 0x0F; // bit 0-3 is the clear key
					if (apple2) {
						// Apple II views use different color values
						celHeaderClearKey = apple2ViewColorMap[celHeaderClearKey];
					}

					if (isMirrorDataInLoopHeader) {
						// 2.230 (early version of xmascard): mirror data is in loop header.
						if (loopHeaderCelCountByte & 0x80) {
							// mirror bit is set
							// there is a second mirror bit whose purpose is currently unknown;
							// both bits are set in every xmascard loop with mirror data.
							byte celHeaderMirrorLoop = (loopHeaderCelCountByte >> 4) & 0x03;
							if (celHeaderMirrorLoop != loopNr) {
								// only set to mirrored in case we are not the original loop
								celHeaderMirrored = true;
							}
						}
					} else {
						// 2.272+: mirror data is in cel header
						if (celHeaderTransparencyMirror & 0x80) {
							// mirror bit is set
							byte celHeaderMirrorLoop = (celHeaderTransparencyMirror >> 4) & 0x07;
							if (celHeaderMirrorLoop != loopNr) {
								// only set to mirrored in case we are not the original loop
								celHeaderMirrored = true;
							}
						}
					}
				} else {
					// AGI256-2 view data
					celHeaderClearKey = celHeaderTransparencyMirror; // full 8 bits for clear key
				}

				celData->width = celHeaderWidth;
				celData->height = celHeaderHeight;
				celData->clearKey = celHeaderClearKey;
				celData->mirrored = celHeaderMirrored;

				// Now decompress cel-data
				if ((celHeaderWidth == 0) && (celHeaderHeight == 0))
					error("view cel is 0x0");

				byte *celCompressedData = resourceData + celOffset + 3;
				uint16 celCompressedSize = resourceSize - (celOffset + 3);

				if (celCompressedSize == 0)
					error("compressed size of loop within view %d is 0 bytes", viewNr);

				if (!isAGI256Data) {
					unpackViewCelData(celData, celCompressedData, celCompressedSize, viewNr);
				} else {
					unpackViewCelDataAGI256(celData, celCompressedData, celCompressedSize, viewNr);
				}
				celData++;
			}
		}

		loopData++;
	}

	return errOK;
}

void AgiEngine::unpackViewCelData(AgiViewCel *celData, byte *compressedData, uint16 compressedSize, int16 viewNr) {
	byte *rawBitmap = new byte[celData->width * celData->height];
	int16 remainingHeight = celData->height;
	int16 remainingWidth = celData->width;
	int16 adjustPreChangeSingle = 0;
	int16 adjustAfterChangeSingle = +1;
	const bool apple2 = (getPlatform() == Common::kPlatformApple2) && getVersion() >= 0x2000;

	celData->rawBitmap = rawBitmap;

	if (celData->mirrored) {
		adjustPreChangeSingle = -1;
		adjustAfterChangeSingle = 0;
		rawBitmap += celData->width;
	}

	while (remainingHeight) {
		if (!compressedSize)
			error("unexpected end of data, while unpacking view %d", viewNr);

		byte curByte = *compressedData++;
		compressedSize--;

		byte curColor;
		byte curChunkLen;
		if (curByte == 0) {
			curColor = celData->clearKey;
			curChunkLen = remainingWidth;
		} else {
			curColor = curByte >> 4;
			curChunkLen = curByte & 0x0F;
			if (curChunkLen > remainingWidth)
				error("invalid chunk in view %d", viewNr);
			if (apple2) {
				curColor = apple2ViewColorMap[curColor];
			}
		}

		switch (curChunkLen) {
		case 0:
			break;
		case 1:
			rawBitmap += adjustPreChangeSingle;
			*rawBitmap = curColor;
			rawBitmap += adjustAfterChangeSingle;
			break;
		default:
			if (celData->mirrored)
				rawBitmap -= curChunkLen;
			memset(rawBitmap, curColor, curChunkLen);
			if (!celData->mirrored)
				rawBitmap += curChunkLen;
			break;
		}

		remainingWidth -= curChunkLen;

		// Each row is terminated by a zero byte; any remaining pixels are transparent.
		// Apple II views don't use terminators, instead they explicitly draw remaining
		// transparent pixels with a normal chunk byte, and rows end when they're full.
		// The Apple II method uses one less byte on full rows.
		if (curByte == 0 || (apple2 && remainingWidth == 0)) {
			remainingWidth = celData->width;
			remainingHeight--;

			if (celData->mirrored)
				rawBitmap += celData->width * 2;
		}
	}

	// for CGA rendering, apply dithering
	switch (_renderMode) {
	case Common::kRenderCGA: {
		uint16 totalPixels = celData->width * celData->height;

		// dither clear key
		celData->clearKey = _gfx->getCGAMixtureColor(celData->clearKey);

		rawBitmap = celData->rawBitmap;
		for (uint16 pixelNr = 0; pixelNr < totalPixels; pixelNr++) {
			byte curColor = *rawBitmap;
			*rawBitmap = _gfx->getCGAMixtureColor(curColor);
			rawBitmap++;
		}
		break;
	}
	default:
		break;
	}
}

void AgiEngine::unpackViewCelDataAGI256(AgiViewCel *celData, byte *compressedData, uint16 compressedSize, int16 viewNr) {
	byte *rawBitmap = new byte[celData->width * celData->height];
	int16 remainingHeight = celData->height;
	int16 remainingWidth = celData->width;

	celData->rawBitmap = rawBitmap;

	while (remainingHeight) {
		if (!compressedSize)
			error("unexpected end of data, while unpacking AGI256 view %d", viewNr);

		byte curByte = *compressedData++;
		compressedSize--;

		if (curByte == 0) {
			// Go to next vertical position
			if (remainingWidth) {
				// fill remaining bytes with clear key
				memset(rawBitmap, celData->clearKey, remainingWidth);
				rawBitmap += remainingWidth;
				remainingWidth = 0;
			}
		} else {
			if (!remainingWidth) {
				error("broken view data, while unpacking AGI256 view %d", viewNr);
				break;
			}
			*rawBitmap = curByte;
			rawBitmap++;
			remainingWidth--;
		}

		if (curByte == 0) {
			remainingWidth = celData->width;
			remainingHeight--;
		}
	}
}

/**
 * Unloads all data in a view resource
 * @param viewNr number of view resource
 */
void AgiEngine::unloadView(int16 viewNr) {
	AgiView *viewData = &_game.views[viewNr];

	debugC(5, kDebugLevelResources, "discard view %d", viewNr);
	if (!(_game.dirView[viewNr].flags & RES_LOADED))
		return;

	// Rebuild sprite list, see Sarien bug #779302
	_sprites->eraseSprites();

	// free data
	for (int16 loopNr = 0; loopNr < viewData->loopCount; loopNr++) {
		AgiViewLoop *loopData = &viewData->loop[loopNr];
		for (int16 celNr = 0; celNr < loopData->celCount; celNr++) {
			AgiViewCel *celData = &loopData->cel[celNr];

			delete[] celData->rawBitmap;
		}
		delete[] loopData->cel;
	}
	delete[] viewData->loop;
	delete[] viewData->description;

	viewData->headerCycleTime = 0;
	viewData->headerStepSize = 0;
	viewData->description = nullptr;
	viewData->loop = nullptr;
	viewData->loopCount = 0;

	// Mark this view as not loaded anymore
	_game.dirView[viewNr].flags &= ~RES_LOADED;

	_sprites->buildAllSpriteLists();
	_sprites->drawAllSpriteLists();
}

/**
 * Set a view table entry to use the specified view resource.
 * @param screenObj pointer to screen object
 * @param viewNr number of AGI view resource
 */
void AgiEngine::setView(ScreenObjEntry *screenObj, int16 viewNr) {
	if (!(_game.dirView[viewNr].flags & RES_LOADED)) {
		// View resource currently not loaded, this is probably a game bug
		// Load the resource now to fix the issue, and give out a warning
		// This happens in at least Larry 1 for Apple IIgs right after getting beaten up by taxi driver
		// Original interpreter bombs out in this situation saying "view not loaded, Press ESC to quit"
		warning("setView() called on screen object %d to use view %d, but view not loaded", screenObj->objectNr, viewNr);
		warning("probably game script bug, trying to load view into memory");
		if (loadResource(RESOURCETYPE_VIEW, viewNr) != errOK) {
			// loading failed, we better error() out now
			error("setView() called to set view %d for screen object %d, which is not loaded atm and loading failed", viewNr, screenObj->objectNr);
			return;
		};
	}

	screenObj->viewResource = &_game.views[viewNr];
	screenObj->currentViewNr = viewNr;
	screenObj->loopCount = screenObj->viewResource->loopCount;
	screenObj->viewReplaced = true;

	if (getVersion() < 0x2000) {
		screenObj->stepSize = screenObj->viewResource->headerStepSize;
		screenObj->cycleTime = screenObj->viewResource->headerCycleTime;
		screenObj->cycleTimeCount = 0;
	}
	if (screenObj->currentLoopNr >= screenObj->loopCount) {
		setLoop(screenObj, 0);
	} else {
		setLoop(screenObj, screenObj->currentLoopNr);
	}
}

/**
 * Set a view table entry to use the specified loop of the current view.
 * @param screenObj pointer to screen object
 * @param loopNr number of loop
 */
void AgiEngine::setLoop(ScreenObjEntry *screenObj, int16 loopNr) {
	if (!(_game.dirView[screenObj->currentViewNr].flags & RES_LOADED)) {
		error("setLoop() called on screen object %d, which has no loaded view resource assigned to it", screenObj->objectNr);
		return;
	}
	assert(screenObj->viewResource);

	if (screenObj->loopCount == 0) {
		warning("setLoop() called on screen object %d, which has no loops (view %d)", screenObj->objectNr, screenObj->currentViewNr);
		return;
	}

	if (loopNr >= screenObj->loopCount) {
		// requested loop not existent
		// instead of error()ing out, we instead clip it
		// At least required for possibly Manhunter 1 according to previous comment when leaving the arcade machine
		// TODO: Check MH1
		// WORKAROUND: This code caused an issue in KQ1 when bowing to the king in room 53. Bug #7045
		// When ego finishes bowing, the script sets his view to 0 and loop to 1 so that he faces left,
		// but it does this by setting the loop first and then the view. The previous view is 71 and only
		// has one loop. This code treated that as an invalid set.loop and would clip it to 0, but that
		// caused ego to face away from the king. For now, we detect this and set the view to 0 first.
		if (getGameID() == GID_KQ1 && screenObj->currentViewNr == 71 && loopNr == 1) {
			setView(screenObj, 0);
		} else {
			int16 requestedLoopNr = loopNr;

			loopNr = screenObj->loopCount - 1;

			warning("Non-existent loop requested for screen object %d", screenObj->objectNr);
			warning("view %d, requested loop %d -> clipped to loop %d", screenObj->currentViewNr, requestedLoopNr, loopNr);
		}
	}

	AgiViewLoop *curViewLoop = &_game.views[screenObj->currentViewNr].loop[loopNr];

	screenObj->currentLoopNr = loopNr;
	screenObj->loopData = curViewLoop;
	screenObj->celCount = curViewLoop->celCount;

	if (screenObj->currentCelNr >= screenObj->celCount) {
		setCel(screenObj, 0);
	} else {
		setCel(screenObj, screenObj->currentCelNr);
	}
}

/**
 * Set a view table entry to use the specified cel of the current loop.
 * @param screenObj pointer to screen object
 * @param celNr number of cel
 */
void AgiEngine::setCel(ScreenObjEntry *screenObj, int16 celNr) {
	if (!(_game.dirView[screenObj->currentViewNr].flags & RES_LOADED)) {
		error("setCel() called on screen object %d, which has no loaded view resource assigned to it", screenObj->objectNr);
		return;
	}
	assert(screenObj->viewResource);

	if (screenObj->loopCount == 0) {
		warning("setLoop() called on screen object %d, which has no loops (view %d)", screenObj->objectNr, screenObj->currentViewNr);
		return;
	}

	AgiViewLoop *curViewLoop = &_game.views[screenObj->currentViewNr].loop[screenObj->currentLoopNr];

	// Added by Amit Vainsencher <amitv@subdimension.com> to prevent
	// crash in KQ1 -- not in the Sierra interpreter
	if (curViewLoop->celCount == 0) {
		warning("setCel() called on screen object %d, which has no cels (view %d)", screenObj->objectNr, screenObj->currentViewNr);
		return;
	}

	if (celNr >= screenObj->celCount) {
		// requested cel not existent
		// instead of error()ing out, we instead clip it
		// At least required for King's Quest 3 on Apple IIgs - walking the planks death cutscene
		// see bug #5832, which is a game bug!
		int16 requestedCelNr = celNr;

		celNr = screenObj->celCount - 1;

		warning("Non-existent cel requested for screen object %d", screenObj->objectNr);
		warning("view %d, loop %d, requested cel %d -> clipped to cel %d", screenObj->currentViewNr, screenObj->currentLoopNr, requestedCelNr, celNr);
	}

	screenObj->currentCelNr = celNr;

	AgiViewCel *curViewCel;
	curViewCel         = &curViewLoop->cel[celNr];
	screenObj->celData = curViewCel;
	screenObj->xSize   = curViewCel->width;
	screenObj->ySize   = curViewCel->height;

	// If position isn't appropriate, update it accordingly
	clipViewCoordinates(screenObj);
}

/**
 * Restrict view table entry's position so it stays wholly inside the screen.
 * Also take horizon into account when clipping if not set to ignore it.
 * @param v pointer to view table entry
 */
void AgiEngine::clipViewCoordinates(ScreenObjEntry *screenObj) {
	if (screenObj->xPos + screenObj->xSize > SCRIPT_WIDTH) {
		screenObj->flags |= fUpdatePos;
		screenObj->xPos = SCRIPT_WIDTH - screenObj->xSize;
	}
	if (screenObj->yPos - screenObj->ySize + 1 < 0) {
		screenObj->flags |= fUpdatePos;
		screenObj->yPos = screenObj->ySize - 1;
	}
	if (screenObj->yPos <= _game.horizon && (~screenObj->flags & fIgnoreHorizon)) {
		screenObj->flags |= fUpdatePos;
		screenObj->yPos = _game.horizon + 1;
	}

	if (getVersion() < 0x2000) {
		screenObj->flags |= fDontUpdate;
	}
}

/**
 * Set the view table entry as updating.
 * @param viewPtr pointer to view table entry
 */
void AgiEngine::startUpdate(ScreenObjEntry *viewPtr) {
	if (~viewPtr->flags & fUpdate) {
		_sprites->eraseSprites();
		viewPtr->flags |= fUpdate;
		_sprites->buildAllSpriteLists();
		_sprites->drawAllSpriteLists();
	}
}

/**
 * Set the view table entry as non-updating.
 * @param viewPtr pointer to view table entry
 */
void AgiEngine::stopUpdate(ScreenObjEntry *viewPtr) {
	if (viewPtr->flags & fUpdate) {
		_sprites->eraseSprites();
		viewPtr->flags &= ~fUpdate;
		_sprites->buildAllSpriteLists();
		_sprites->drawAllSpriteLists();
	}
}

// loops to use according to direction and number of loops in
// the view resource
static int loopTable2[] = {
	0x04, 0x04, 0x00, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01
};

static int loopTable4[] = {
	0x04, 0x03, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x01
};

/**
 * Update view table entries.
 * This function is called at the end of each interpreter cycle
 * to update the view table entries and blit the sprites.
 */
void AgiEngine::updateScreenObjTable() {
	ScreenObjEntry *screenObj;
	int16 changeCount, loopNr;

	changeCount = 0;
	for (screenObj = _game.screenObjTable; screenObj < &_game.screenObjTable[SCREENOBJECTS_MAX]; screenObj++) {
		if ((screenObj->flags & (fAnimated | fUpdate | fDrawn)) != (fAnimated | fUpdate | fDrawn)) {
			continue;
		}

		changeCount++;

		loopNr = 4;
		if (!(screenObj->flags & fFixLoop)) {
			switch (screenObj->loopCount) {
			case 2:
			case 3:
				loopNr = loopTable2[screenObj->direction];
				break;
			case 4:
				loopNr = loopTable4[screenObj->direction];
				break;
			default:
				// for KQ4
				if (getVersion() == 0x3086 || getGameID() == GID_KQ4)
					loopNr = loopTable4[screenObj->direction];
				break;
			}
		}

		// AGI 2.272 (ddp, xmas) doesn't test step_time_count!
		if (loopNr != 4 && loopNr != screenObj->currentLoopNr) {
			if (getVersion() <= 0x2272 || screenObj->stepTimeCount == 1) {
				setLoop(screenObj, loopNr);
			}
		}

		if (screenObj->flags & fCycling) {
			if (screenObj->cycleTimeCount) {
				screenObj->cycleTimeCount--;
				if (screenObj->cycleTimeCount == 0) {
					updateView(screenObj);
					screenObj->cycleTimeCount = screenObj->cycleTime;
				}
			}
		}
	}

	if (changeCount) {
		_sprites->eraseRegularSprites();
		updatePosition();
		_sprites->buildRegularSpriteList();
		_sprites->drawRegularSpriteList();
		_sprites->showRegularSpriteList();

		_game.screenObjTable[SCREENOBJECTS_EGO_ENTRY].flags &= ~(fOnWater | fOnLand);
	}
}

bool AgiEngine::isEgoView(const ScreenObjEntry *screenObj) {
	return screenObj == _game.screenObjTable;
}

} // End of namespace Agi
