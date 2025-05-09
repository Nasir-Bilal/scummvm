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
 *
 * This file is dual-licensed.
 * In addition to the GPLv3 license mentioned above, MojoTouch has
 * non-exclusively licensed this code on March 23th, 2024, to be used in
 * closed-source products.
 * Therefore, any contributions (commits) to it will also be dual-licensed.
 *
 */

// Based on http://wiki.multimedia.cx/index.php?title=Smacker
// and the FFmpeg Smacker decoder (libavcodec/smacker.c), revision 16143
// https://git.ffmpeg.org/gitweb/ffmpeg.git/commit/40a19c443430de520d86bbd644033c8e2ca87e9b

#include "video/smk_decoder.h"

#include "common/endian.h"
#include "common/util.h"
#include "common/stream.h"
#include "common/bitarray.h"
#include "common/bitstream.h"
#include "common/system.h"
#include "common/textconsole.h"

#include "audio/audiostream.h"
#include "audio/mixer.h"
#include "audio/decoders/raw.h"

namespace Video {

enum SmkBlockTypes {
	SMK_BLOCK_MONO = 0,
	SMK_BLOCK_FULL = 1,
	SMK_BLOCK_SKIP = 2,
	SMK_BLOCK_FILL = 3
};

/*
 * class SmallHuffmanTree
 * A Huffman-tree to hold 8-bit values.
 */

class SmallHuffmanTree {
public:
	SmallHuffmanTree(SmackerBitStream &bs);

	uint16 getCode(SmackerBitStream &bs);
private:
	enum {
		SMK_NODE = 0x8000
	};

	uint16 decodeTree(uint32 prefix, int length);

	uint16 _treeSize;
	uint16 _tree[511];

	uint16 _prefixtree[256];
	byte _prefixlength[256];

	SmackerBitStream &_bs;
	bool _empty;
};

SmallHuffmanTree::SmallHuffmanTree(SmackerBitStream &bs)
	: _treeSize(0), _bs(bs), _empty(false) {
	if (!_bs.getBit()) {
		_empty = true;
		return;
	}

	for (uint16 i = 0; i < 256; ++i)
		_prefixtree[i] = _prefixlength[i] = 0;

	decodeTree(0, 0);

	(void)_bs.getBit();
}

uint16 SmallHuffmanTree::decodeTree(uint32 prefix, int length) {
	if (_empty)
		return 0;

	if (!_bs.getBit()) { // Leaf
		_tree[_treeSize] = _bs.getBits<8>();

		if (length <= 8) {
			for (int i = 0; i < 256; i += (1 << length)) {
				_prefixtree[prefix | i] = _treeSize;
				_prefixlength[prefix | i] = length;
			}
		}
		++_treeSize;

		return 1;
	}

	uint16 t = _treeSize++;

	if (length == 8) {
		_prefixtree[prefix] = t;
		_prefixlength[prefix] = 8;
	}

	uint16 r1 = decodeTree(prefix, length + 1);

	_tree[t] = (SMK_NODE | r1);

	uint16 r2 = decodeTree(prefix | (1 << length), length + 1);

	return r1+r2+1;
}

uint16 SmallHuffmanTree::getCode(SmackerBitStream &bs) {
	if (_empty)
		return 0;

	// Peeking data out of bounds is well-defined and returns 0 bits.
	// This is for convenience when using speed-up techniques reading
	// more bits than actually available.
	byte peek = bs.peekBits<8>();
	uint16 *p = &_tree[_prefixtree[peek]];
	bs.skip(_prefixlength[peek]);

	while (*p & SMK_NODE) {
		if (bs.getBit())
			p += *p & ~SMK_NODE;
		p++;
	}

	return *p;
}

/*
 * class BigHuffmanTree
 * A Huffman-tree to hold 16-bit values.
 */

class BigHuffmanTree {
public:
	BigHuffmanTree(SmackerBitStream &bs, int allocSize);
	~BigHuffmanTree();

	void reset();
	uint32 getCode(SmackerBitStream &bs);
private:
	enum {
		SMK_NODE = 0x80000000
	};

	uint32 decodeTree(uint32 prefix, int length);

	uint32  _treeSize;
	uint32 *_tree;
	uint32  _last[3];

	uint32 _prefixtree[256];
	byte _prefixlength[256];

	/* Used during construction */
	SmackerBitStream &_bs;
	uint32 _markers[3];
	SmallHuffmanTree *_loBytes;
	SmallHuffmanTree *_hiBytes;
};

BigHuffmanTree::BigHuffmanTree(SmackerBitStream &bs, int allocSize)
	: _bs(bs) {
	uint32 bit = _bs.getBit();
	if (!bit) {
		_tree = new uint32[1];
		_tree[0] = 0;
		_last[0] = _last[1] = _last[2] = 0;
		return;
	}

	for (uint32 i = 0; i < 256; ++i)
		_prefixtree[i] = _prefixlength[i] = 0;

	_loBytes = new SmallHuffmanTree(_bs);
	_hiBytes = new SmallHuffmanTree(_bs);

	_markers[0] = _bs.getBits<16>();
	_markers[1] = _bs.getBits<16>();
	_markers[2] = _bs.getBits<16>();

	_last[0] = _last[1] = _last[2] = 0xffffffff;

	_treeSize = 0;
	_tree = new uint32[allocSize / 4];
	decodeTree(0, 0);
	(void)_bs.getBit();

	for (uint32 i = 0; i < 3; ++i) {
		if (_last[i] == 0xffffffff) {
			_last[i] = _treeSize;
			_tree[_treeSize++] = 0;
		}
	}

	delete _loBytes;
	delete _hiBytes;
}

BigHuffmanTree::~BigHuffmanTree() {
	delete[] _tree;
}

void BigHuffmanTree::reset() {
	_tree[_last[0]] = _tree[_last[1]] = _tree[_last[2]] = 0;
}

uint32 BigHuffmanTree::decodeTree(uint32 prefix, int length) {
	uint32 bit = _bs.getBit();

	if (!bit) { // Leaf
		uint32 lo = _loBytes->getCode(_bs);
		uint32 hi = _hiBytes->getCode(_bs);

		uint32 v = (hi << 8) | lo;

		_tree[_treeSize] = v;

		if (length <= 8) {
			for (int i = 0; i < 256; i += (1 << length)) {
				_prefixtree[prefix | i] = _treeSize;
				_prefixlength[prefix | i] = length;
			}
		}

		for (int i = 0; i < 3; ++i) {
			if (_markers[i] == v) {
				_last[i] = _treeSize;
				_tree[_treeSize] = 0;
			}
		}
		++_treeSize;

		return 1;
	}

	uint32 t = _treeSize++;

	if (length == 8) {
		_prefixtree[prefix] = t;
		_prefixlength[prefix] = 8;
	}

	uint32 r1 = decodeTree(prefix, length + 1);

	_tree[t] = SMK_NODE | r1;

	uint32 r2 = decodeTree(prefix | (1 << length), length + 1);
	return r1+r2+1;
}

uint32 BigHuffmanTree::getCode(SmackerBitStream &bs) {
	// Peeking data out of bounds is well-defined and returns 0 bits.
	// This is for convenience when using speed-up techniques reading
	// more bits than actually available.
	byte peek = bs.peekBits<8>();
	uint32 *p = &_tree[_prefixtree[peek]];
	bs.skip(_prefixlength[peek]);

	while (*p & SMK_NODE) {
		if (bs.getBit())
			p += (*p) & ~SMK_NODE;
		p++;
	}

	uint32 v = *p;
	if (v != _tree[_last[0]]) {
		_tree[_last[2]] = _tree[_last[1]];
		_tree[_last[1]] = _tree[_last[0]];
		_tree[_last[0]] = v;
	}

	return v;
}

SmackerDecoder::SmackerDecoder() {
	_fileStream = 0;
	_firstFrameStart = 0;
	_frameTypes = 0;
	_frameSizes = 0;
}

SmackerDecoder::~SmackerDecoder() {
	close();
}

uint32 SmackerDecoder::getSignatureVersion(uint32 signature) const {
	if (signature == MKTAG('S', 'M', 'K', '2')) {
		return 2;
	} else if (signature == MKTAG('S', 'M', 'K', '4')) {
		return 4;
	} else {
		return 0;
	}
}

bool SmackerDecoder::loadStream(Common::SeekableReadStream *stream) {
	close();

	_fileStream = stream;

	// Read in the Smacker header
	_header.signature = _fileStream->readUint32BE();

	uint32 version = getSignatureVersion(_header.signature);
	if (version == 0)
		return false;

	uint32 width = _fileStream->readUint32LE();
	uint32 height = _fileStream->readUint32LE();
	uint32 frameCount = _fileStream->readUint32LE();
	int32 frameDelay = _fileStream->readSint32LE();

	// frame rate contains 2 digits after the comma, so 1497 is actually 14.97 fps
	Common::Rational frameRate;
	if (frameDelay > 0)
		frameRate = Common::Rational(1000, frameDelay);
	else if (frameDelay < 0)
		frameRate = Common::Rational(100000, -frameDelay);
	else
		frameRate = 1000;

	// Flags are determined by which bit is set, which can be one of the following:
	// 0 - set to 1 if file contains a ring frame.
	// 1 - set to 1 if file is Y-interlaced
	// 2 - set to 1 if file is Y-doubled
	// If bits 1 or 2 are set, the frame should be scaled to twice its height
	// before it is displayed.
	_header.flags = _fileStream->readUint32LE();
	if (_header.flags & 1)
		frameCount++;

	SmackerVideoTrack *videoTrack = createVideoTrack(width, height, frameCount, frameRate, _header.flags, version);
	addTrack(videoTrack);

	// TODO: should we do any extra processing for Smacker files with ring frames?

	// TODO: should we do any extra processing for Y-doubled videos? Are they the
	// same as Y-interlaced videos?

	uint32 i;
	for (i = 0; i < 7; ++i)
		_header.audioSize[i] = _fileStream->readUint32LE();

	_header.treesSize = _fileStream->readUint32LE();
	_header.mMapSize = _fileStream->readUint32LE();
	_header.mClrSize = _fileStream->readUint32LE();
	_header.fullSize = _fileStream->readUint32LE();
	_header.typeSize = _fileStream->readUint32LE();

	for (i = 0; i < 7; ++i) {
		// AudioRate - Frequency and format information for each sound track, up to 7 audio tracks.
		// The 32 constituent bits have the following meaning:
		// * bit 31 - indicates Huffman + DPCM compression
		// * bit 30 - indicates that audio data is present for this track
		// * bit 29 - 1 = 16-bit audio; 0 = 8-bit audio
		// * bit 28 - 1 = stereo audio; 0 = mono audio
		// * bit 27 - indicates Bink RDFT compression
		// * bit 26 - indicates Bink DCT compression
		// * bits 25-24 - unused
		// * bits 23-0 - audio sample rate
		uint32 audioInfo = _fileStream->readUint32LE();
		_header.audioInfo[i].hasAudio = audioInfo & 0x40000000;
		_header.audioInfo[i].is16Bits = audioInfo & 0x20000000;
		_header.audioInfo[i].isStereo = audioInfo & 0x10000000;
		_header.audioInfo[i].sampleRate = audioInfo & 0xFFFFFF;

		if (audioInfo & 0x8000000)
			_header.audioInfo[i].compression = kCompressionRDFT;
		else if (audioInfo & 0x4000000)
			_header.audioInfo[i].compression = kCompressionDCT;
		else if (audioInfo & 0x80000000)
			_header.audioInfo[i].compression = kCompressionDPCM;
		else
			_header.audioInfo[i].compression = kCompressionNone;

		if (_header.audioInfo[i].hasAudio) {
			if (_header.audioInfo[i].compression == kCompressionRDFT || _header.audioInfo[i].compression == kCompressionDCT)
				warning("Unhandled Smacker v2 audio compression");

			addTrack(new SmackerAudioTrack(_header.audioInfo[i], getSoundType()));
		} else {
			addTrack(new SmackerEmptyTrack());
		}
	}

	_header.dummy = _fileStream->readUint32LE();

	_frameSizes = new uint32[frameCount];
	for (i = 0; i < frameCount; ++i)
		_frameSizes[i] = _fileStream->readUint32LE();

	_frameTypes = new byte[frameCount];
	for (i = 0; i < frameCount; ++i)
		_frameTypes[i] = _fileStream->readByte();

	byte *huffmanTrees = (byte *) malloc(_header.treesSize);
	_fileStream->read(huffmanTrees, _header.treesSize);

	SmackerBitStream bs(new Common::BitStreamMemoryStream(huffmanTrees, _header.treesSize, DisposeAfterUse::YES), DisposeAfterUse::YES);
	videoTrack->readTrees(bs, _header.mMapSize, _header.mClrSize, _header.fullSize, _header.typeSize);

	_firstFrameStart = _fileStream->pos();

	return true;
}

void SmackerDecoder::close() {
	VideoDecoder::close();

	delete _fileStream;
	_fileStream = 0;

	delete[] _frameTypes;
	_frameTypes = 0;

	delete[] _frameSizes;
	_frameSizes = 0;
}

bool SmackerDecoder::rewind() {
	// Call the parent method to rewind the tracks first
	if (!VideoDecoder::rewind())
		return false;

	// And seek back to where the first frame begins
	_fileStream->seek(_firstFrameStart);
	return true;
}

const Graphics::Surface *SmackerDecoder::forceSeekToFrame(uint frame) {
	uint seekFrame;
	if (frame >= 10)
		seekFrame = MAX<uint>(frame - 10, 0);
	else
		seekFrame = 0;

	if (!isVideoLoaded())
		return nullptr;

	if (seekFrame >= getFrameCount())
		return nullptr;

	if (!rewind())
		return nullptr;

	stopAudio();
	SmackerVideoTrack *videoTrack = (SmackerVideoTrack *)getTrack(0);
	uint32 startPos = _fileStream->pos();
	uint32 offset = 0;
	for (uint32 i = 0; i < seekFrame; i++) {
		videoTrack->increaseCurFrame();
		// Frames with palette data contain palette entries which use
		// the previous palette as their base. Therefore, we need to
		// parse all palette entries up to the requested frame
		if (_frameTypes[videoTrack->getCurFrame()] & 1) {
			_fileStream->seek(startPos + offset, SEEK_SET);
			videoTrack->unpackPalette(_fileStream);
		}
		offset += _frameSizes[i] & ~3;
	}

	if (!_fileStream->seek(startPos + offset, SEEK_SET))
		return nullptr;

	const Graphics::Surface *surface = nullptr;
	while (getCurFrame() < (int)frame) {
		surface = decodeNextFrame();
	}

	_lastTimeChange = videoTrack->getFrameTime(frame);
	if (isPlaying()) {
		_startTime = g_system->getMillis() - (_lastTimeChange.msecs() / getRate()).toInt();
	}
	resetPauseStartTime();
	return surface;
}

void SmackerDecoder::readNextPacket() {
	SmackerVideoTrack *videoTrack = (SmackerVideoTrack *)getTrack(0);

	if (videoTrack->endOfTrack())
		return;

	videoTrack->increaseCurFrame();

	uint i;
	uint32 chunkSize = 0;
	uint32 dataSizeUnpacked = 0;

	uint32 startPos = _fileStream->pos();

	// Check if we got a frame with palette data, and
	// call back the virtual setPalette function to set
	// the current palette
	if (_frameTypes[videoTrack->getCurFrame()] & 1)
		videoTrack->unpackPalette(_fileStream);

	// Load audio tracks
	for (i = 0; i < 7; ++i) {
		if (!(_frameTypes[videoTrack->getCurFrame()] & (2 << i)))
			continue;

		chunkSize = _fileStream->readUint32LE();
		chunkSize -= 4;    // subtract the first 4 bytes (chunk size)

		if (_header.audioInfo[i].compression == kCompressionNone) {
			dataSizeUnpacked = chunkSize;
		} else {
			dataSizeUnpacked = _fileStream->readUint32LE();
			chunkSize -= 4;    // subtract the next 4 bytes (unpacked data size)
		}

		handleAudioTrack(i, chunkSize, dataSizeUnpacked);
	}

	uint32 frameSize = _frameSizes[videoTrack->getCurFrame()] & ~3;
//	uint32 remainder =  _frameSizes[videoTrack->getCurFrame()] & 3;

	if (_fileStream->pos() - startPos > frameSize)
		error("Smacker actual frame size exceeds recorded frame size");

	uint32 frameDataSize = frameSize - (_fileStream->pos() - startPos);

	byte *frameData = (byte *)malloc(frameDataSize + 1);
	// Padding to keep the BigHuffmanTrees from reading past the data end
	frameData[frameDataSize] = 0x00;

	_fileStream->read(frameData, frameDataSize);

	SmackerBitStream bs(new Common::BitStreamMemoryStream(frameData, frameDataSize + 1, DisposeAfterUse::YES), DisposeAfterUse::YES);
	videoTrack->decodeFrame(bs);

	_fileStream->seek(startPos + frameSize);
}

void SmackerDecoder::handleAudioTrack(byte track, uint32 chunkSize, uint32 unpackedSize) {
	if (chunkSize == 0)
		return;

	if (_header.audioInfo[track].hasAudio) {
		// Get the audio track, which start at offset 1 (first track is video)
		SmackerAudioTrack *audioTrack = (SmackerAudioTrack *)getTrack(track + 1);

		// If it's track 0, play the audio data
		byte *soundBuffer = (byte *)malloc(chunkSize + 1);
		// Padding to keep the SmallHuffmanTrees from reading past the data end
		soundBuffer[chunkSize] = 0x00;

		_fileStream->read(soundBuffer, chunkSize);

		if (_header.audioInfo[track].compression == kCompressionRDFT || _header.audioInfo[track].compression == kCompressionDCT) {
			// TODO: Compressed audio (Bink RDFT/DCT encoded)
			free(soundBuffer);
			return;
		} else if (_header.audioInfo[track].compression == kCompressionDPCM) {
			// Compressed audio (Huffman DPCM encoded)
			audioTrack->queueCompressedBuffer(soundBuffer, chunkSize + 1, unpackedSize);
			free(soundBuffer);
		} else {
			// Uncompressed audio (PCM)
			audioTrack->queuePCM(soundBuffer, chunkSize);
		}
	} else {
		// Ignore possibly unused data
		_fileStream->skip(chunkSize);
	}
}

VideoDecoder::AudioTrack *SmackerDecoder::getAudioTrack(int index) {
	// Smacker audio track indexes are relative to the first audio track
	Track *track = getTrack(index + 1);

	if (!track || track->getTrackType() != Track::kTrackTypeAudio)
		return 0;

	return (AudioTrack *)track;
}

SmackerDecoder::SmackerVideoTrack::SmackerVideoTrack(uint32 width, uint32 height, uint32 frameCount, const Common::Rational &frameRate, uint32 flags, uint32 version) : _palette(256) {
	_surface = new Graphics::Surface();
	_surface->create(width, height * ((flags & 6) ? 2 : 1), Graphics::PixelFormat::createFormatCLUT8());
	_dirtyBlocks.set_size(width * height / 16);
	_frameCount = frameCount;
	_frameRate = frameRate;
	_flags = flags;
	_version = version;
	_curFrame = -1;
	_dirtyPalette = false;
	_MMapTree = _MClrTree = _FullTree = _TypeTree = 0;
}

SmackerDecoder::SmackerVideoTrack::~SmackerVideoTrack() {
	_surface->free();
	delete _surface;

	delete _MMapTree;
	delete _MClrTree;
	delete _FullTree;
	delete _TypeTree;
}

uint16 SmackerDecoder::SmackerVideoTrack::getWidth() const {
	return _surface->w;
}

uint16 SmackerDecoder::SmackerVideoTrack::getHeight() const {
	return _surface->h;
}

Graphics::PixelFormat SmackerDecoder::SmackerVideoTrack::getPixelFormat() const {
	return _surface->format;
}

void SmackerDecoder::SmackerVideoTrack::readTrees(SmackerBitStream &bs, uint32 mMapSize, uint32 mClrSize, uint32 fullSize, uint32 typeSize) {
	_MMapTree = new BigHuffmanTree(bs, mMapSize);
	_MClrTree = new BigHuffmanTree(bs, mClrSize);
	_FullTree = new BigHuffmanTree(bs, fullSize);
	_TypeTree = new BigHuffmanTree(bs, typeSize);
}

void SmackerDecoder::SmackerVideoTrack::decodeFrame(SmackerBitStream &bs) {
	_MMapTree->reset();
	_MClrTree->reset();
	_FullTree->reset();
	_TypeTree->reset();
	_dirtyBlocks.clear();

	// Height needs to be doubled if we have flags (Y-interlaced or Y-doubled)
	uint doubleY = (_flags & 6) ? 2 : 1;

	uint bw = getWidth() / 4;
	uint bh = getHeight() / doubleY / 4;
	uint stride = getWidth();
	uint block = 0, blocks = bw*bh;

	byte *out;
	uint type, run, j, mode;
	uint32 p1, p2, clr, map;
	byte hi, lo;
	uint i;

	while (block < blocks) {
		type = _TypeTree->getCode(bs);
		run = getBlockRun((type >> 2) & 0x3f);

		switch (type & 3) {
		case SMK_BLOCK_MONO:
			while (run-- && block < blocks) {
				clr = _MClrTree->getCode(bs);
				map = _MMapTree->getCode(bs);
				out = (byte *)_surface->getPixels() + (block / bw) * (stride * 4 * doubleY) + (block % bw) * 4;
				hi = clr >> 8;
				lo = clr & 0xff;
				for (i = 0; i < 4; i++) {
					for (j = 0; j < doubleY; j++) {
						out[0] = (map & 1) ? hi : lo;
						out[1] = (map & 2) ? hi : lo;
						out[2] = (map & 4) ? hi : lo;
						out[3] = (map & 8) ? hi : lo;
						out += stride;
					}
					map >>= 4;
				}
				_dirtyBlocks.set(block);
				++block;
			}
			break;
		case SMK_BLOCK_FULL:
			// Smacker v2 has one mode, Smacker v4 has three
			if (_version == 2) {
				mode = 0;
			} else {
				// 00 - mode 0
				// 10 - mode 1
				// 01 - mode 2
				mode = 0;
				if (bs.getBit()) {
					mode = 1;
				} else if (bs.getBit()) {
					mode = 2;
				}
			}

			while (run-- && block < blocks) {
				out = (byte *)_surface->getPixels() + (block / bw) * (stride * 4 * doubleY) + (block % bw) * 4;
				switch (mode) {
					case 0:
						for (i = 0; i < 4; ++i) {
							p1 = _FullTree->getCode(bs);
							p2 = _FullTree->getCode(bs);
							for (j = 0; j < doubleY; ++j) {
								out[2] = p1 & 0xff;
								out[3] = p1 >> 8;
								out[0] = p2 & 0xff;
								out[1] = p2 >> 8;
								out += stride;
							}
						}
						break;
					case 1:
						p1 = _FullTree->getCode(bs);
						out[0] = out[1] = p1 & 0xFF;
						out[2] = out[3] = p1 >> 8;
						out += stride;
						out[0] = out[1] = p1 & 0xFF;
						out[2] = out[3] = p1 >> 8;
						out += stride;
						p2 = _FullTree->getCode(bs);
						out[0] = out[1] = p2 & 0xFF;
						out[2] = out[3] = p2 >> 8;
						out += stride;
						out[0] = out[1] = p2 & 0xFF;
						out[2] = out[3] = p2 >> 8;
						out += stride;
						break;
					case 2:
						for (i = 0; i < 2; i++) {
							// We first get p2 and then p1
							// Check ffmpeg thread "[PATCH] Smacker video decoder bug fix"
							// https://ffmpeg.org/pipermail/ffmpeg-devel/2008-December/044246.html
							p2 = _FullTree->getCode(bs);
							p1 = _FullTree->getCode(bs);
							for (j = 0; j < doubleY; ++j) {
								out[0] = p1 & 0xff;
								out[1] = p1 >> 8;
								out[2] = p2 & 0xff;
								out[3] = p2 >> 8;
								out += stride;
							}
							for (j = 0; j < doubleY; ++j) {
								out[0] = p1 & 0xff;
								out[1] = p1 >> 8;
								out[2] = p2 & 0xff;
								out[3] = p2 >> 8;
								out += stride;
							}
						}
						break;
					default:
						break;
				}
				_dirtyBlocks.set(block);
				++block;
			}
			break;
		case SMK_BLOCK_SKIP:
			while (run-- && block < blocks)
				block++;
			break;
		case SMK_BLOCK_FILL:
			uint32 col;
			mode = type >> 8;
			while (run-- && block < blocks) {
				out = (byte *)_surface->getPixels() + (block / bw) * (stride * 4 * doubleY) + (block % bw) * 4;
				col = mode * 0x01010101;
				for (i = 0; i < 4 * doubleY; ++i) {
					out[0] = out[1] = out[2] = out[3] = col;
					out += stride;
				}
				_dirtyBlocks.set(block);
				++block;
			}
			break;
		default:
			break;
		}
	}
}

void SmackerDecoder::SmackerVideoTrack::unpackPalette(Common::SeekableReadStream *stream) {
	uint startPos = stream->pos();
	uint32 len = 4 * stream->readByte();

	byte *chunk = (byte *)malloc(len);
	stream->read(chunk, len);
	byte *p = chunk;

	const byte *oldPalette = _palette.data();

	byte newPalette[3 * 256];
	_palette.grab(newPalette, 0, 256);
	byte *pal = newPalette;

	int sz = 0;
	byte b0;
	while (sz < 256) {
		b0 = *p++;
		if (b0 & 0x80) {               // if top bit is 1 (0x80 = 10000000)
			sz += (b0 & 0x7f) + 1;     // get lower 7 bits + 1 (0x7f = 01111111)
			pal += 3 * ((b0 & 0x7f) + 1);
		} else if (b0 & 0x40) {        // if top 2 bits are 01 (0x40 = 01000000)
			byte c = (b0 & 0x3f) + 1;  // get lower 6 bits + 1 (0x3f = 00111111)
			uint s = 3 * *p++;
			sz += c;

			while (c--) {
				*pal++ = oldPalette[s + 0];
				*pal++ = oldPalette[s + 1];
				*pal++ = oldPalette[s + 2];
				s += 3;
			}
		} else {                       // top 2 bits are 00
			sz++;
			// get the lower 6 bits for each component (0x3f = 00111111)
			byte r = b0 & 0x3f;
			byte g = (*p++) & 0x3f;
			byte b = (*p++) & 0x3f;

			// upscale to full 8-bit color values. The Multimedia Wiki suggests
			// a lookup table for this, but this should produce the same result.
			*pal++ = (r * 4 + r / 16);
			*pal++ = (g * 4 + g / 16);
			*pal++ = (b * 4 + b / 16);
		}
	}
	stream->seek(startPos + len);
	free(chunk);

	_palette.set(newPalette, 0, 256);
	_dirtyPalette = true;
}

SmackerDecoder::SmackerAudioTrack::SmackerAudioTrack(const AudioInfo &audioInfo, Audio::Mixer::SoundType soundType) :
		AudioTrack(soundType),
		_audioInfo(audioInfo) {
	_audioStream = Audio::makeQueuingAudioStream(_audioInfo.sampleRate, _audioInfo.isStereo);
}

SmackerDecoder::SmackerAudioTrack::~SmackerAudioTrack() {
	delete _audioStream;
}

bool SmackerDecoder::SmackerAudioTrack::rewind() {
	delete _audioStream;
	_audioStream = Audio::makeQueuingAudioStream(_audioInfo.sampleRate, _audioInfo.isStereo);
	return true;
}

Audio::AudioStream *SmackerDecoder::SmackerAudioTrack::getAudioStream() const {
	return _audioStream;
}

void SmackerDecoder::SmackerAudioTrack::queueCompressedBuffer(byte *buffer, uint32 bufferSize, uint32 unpackedSize) {
	SmackerBitStream audioBS(new Common::BitStreamMemoryStream(buffer, bufferSize), DisposeAfterUse::YES);
	bool dataPresent = audioBS.getBit();

	if (!dataPresent)
		return;

	bool isStereo = audioBS.getBit();
	assert(isStereo == _audioInfo.isStereo);
	bool is16Bits = audioBS.getBit();
	assert(is16Bits == _audioInfo.is16Bits);

	int numBytes = 1 * (isStereo ? 2 : 1) * (is16Bits ? 2 : 1);

	byte *unpackedBuffer = (byte *)malloc(unpackedSize);
	byte *curPointer = unpackedBuffer;
	uint32 curPos = 0;

	SmallHuffmanTree *audioTrees[4];
	for (int k = 0; k < numBytes; k++)
		audioTrees[k] = new SmallHuffmanTree(audioBS);

	// Base values, stored as big endian

	int32 bases[2];

	if (isStereo) {
		if (is16Bits) {
			bases[1] = SWAP_BYTES_16(audioBS.getBits<16>());
		} else {
			bases[1] = audioBS.getBits<8>();
		}
	}

	if (is16Bits) {
		bases[0] = SWAP_BYTES_16(audioBS.getBits<16>());
	} else {
		bases[0] = audioBS.getBits<8>();
	}

	// The bases are the first samples, too
	for (int i = 0; i < (isStereo ? 2 : 1); i++, curPointer += (is16Bits ? 2 : 1), curPos += (is16Bits ? 2 : 1)) {
		if (is16Bits)
			WRITE_BE_UINT16(curPointer, bases[i]);
		else
			*curPointer = (bases[i] & 0xFF) ^ 0x80;
	}

	// Next follow the deltas, which are added to the corresponding base values and
	// are stored as little endian
	// We store the unpacked bytes in big endian format

	while (curPos < unpackedSize) {
		// If the sample is stereo, the data is stored for the left and right channel, respectively
		// (the exact opposite to the base values)
		if (!is16Bits) {
			for (int k = 0; k < (isStereo ? 2 : 1); k++) {
				int8 delta = (int8) ((int16) audioTrees[k]->getCode(audioBS));
				bases[k] = (bases[k] + delta) & 0xFF;
				*curPointer++ = bases[k] ^ 0x80;
				curPos++;
			}
		} else {
			for (int k = 0; k < (isStereo ? 2 : 1); k++) {
				byte lo = audioTrees[k * 2]->getCode(audioBS);
				byte hi = audioTrees[k * 2 + 1]->getCode(audioBS);
				bases[k] += (int16) (lo | (hi << 8));

				WRITE_BE_UINT16(curPointer, bases[k]);
				curPointer += 2;
				curPos += 2;
			}
		}

	}

	for (int k = 0; k < numBytes; k++)
		delete audioTrees[k];

	queuePCM(unpackedBuffer, unpackedSize);
}

void SmackerDecoder::SmackerAudioTrack::queuePCM(byte *buffer, uint32 bufferSize) {
	byte flags = 0;
	if (_audioInfo.is16Bits)
		flags |= Audio::FLAG_16BITS;
	if (_audioInfo.isStereo)
		flags |= Audio::FLAG_STEREO;

	_audioStream->queueBuffer(buffer, bufferSize, DisposeAfterUse::YES, flags);
}

SmackerDecoder::SmackerVideoTrack *SmackerDecoder::createVideoTrack(uint32 width, uint32 height, uint32 frameCount, const Common::Rational &frameRate, uint32 flags, uint32 version) const {
	return new SmackerVideoTrack(width, height, frameCount, frameRate, flags, version);
}

Common::Rational SmackerDecoder::getFrameRate() const {
	const SmackerVideoTrack *videoTrack = (const SmackerVideoTrack *)getTrack(0);

	return videoTrack->getFrameRate();
}

const Common::Rect *SmackerDecoder::getNextDirtyRect() {
	SmackerVideoTrack *videoTrack = (SmackerVideoTrack *)getTrack(0);

	return videoTrack->getNextDirtyRect();
}

const Common::Rect *SmackerDecoder::SmackerVideoTrack::getNextDirtyRect() {
	uint doubleY = (_flags & 6) ? 2 : 1;

	uint bw = getWidth() / 4;
	uint bh = getHeight() / 4;
	uint blocks = bw*bh;

	// Scan forward in dirty blocks bitarray for next dirty rect
	uint block_idx = (_lastDirtyRect.left) / 4 + (_lastDirtyRect.top / 4 / doubleY) * bw;
	while (block_idx < blocks && !_dirtyBlocks.get(block_idx)) {
		++block_idx;
	}
	if (block_idx == blocks) {
		_lastDirtyRect = Common::Rect();
		return nullptr;
	}

	uint block_x0 = block_idx % bw;
	uint block_y0 = block_idx / bw;

	// Find the width of the dirty rect
	uint block_x1 = block_x0 + 1;
	while (block_x1 < bw && _dirtyBlocks.get(block_x1 + block_y0 * bw)) {
		++block_x1;
	}

	// Find the height of the dirty rect
	uint block_y1 = block_y0 + 1;
	while (block_y1 < bh) {
		// Check that the rect to the left of the next line isn't dirty
		if (block_x0 != 0 && _dirtyBlocks.get(block_x0 - 1 + block_y1 * bw)) {
			break;
		}

		// Check that all the rects on this line are dirty
		uint bx;
		for (bx = block_x0; bx != block_x1; ++bx) {
			if (!_dirtyBlocks.get(bx + block_y1 * bw)) {
				break;
			}
		}
		if (bx != block_x1) {
			break;
		}

		// Check that the rect to the right of this line isn't dirty
		if (bx != bw && _dirtyBlocks.get(bx + block_y1 * bw)) {
			break;
		}
		++block_y1;
	}

	// Undirty all the rects that we're returning
	for (uint y = block_y0; y != block_y1; ++y) {
		for (uint x = block_x0; x != block_x1; ++x) {
			_dirtyBlocks.unset(x + y * bw);
		}
	}

	_lastDirtyRect = Common::Rect(
		int16(4 * block_x0),
		int16(4 * block_y0 * doubleY),
		int16(4 * block_x1),
		int16(4 * block_y1 * doubleY)
	);
	return &_lastDirtyRect;
}

} // End of namespace Video
