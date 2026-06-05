#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "libflac.hpp"

FlacFile::FlacFile(std::ifstream& fin)
  : _fin(fin),
    _lastError(FLAC_SUCCESS),
    _streamInfo(NULL),
    _seekPointsCount(0),
    _picture(NULL),
    _comment(NULL) {
}

FlacFile::~FlacFile() {
  if (_streamInfo) delete _streamInfo;
  if (_picture) {
    if (_picture->data) delete[] _picture->data;
    delete _picture;
  }
  if (_comment) delete _comment;
  for (auto pair: _appData) delete[] pair.second;
}

void FlacFile::Dump() {
  if (_streamInfo) {
    std::cout << "[STREAMINFO]" << std::endl;
    std::cout << std::dec
              << " Minimum blocksize: " << _streamInfo->minBlockSize << " samples" << std::endl
              << " Maximum blocksize: " << _streamInfo->maxBlockSize << " samples" << std::endl
              << " Minimum framesize: " << _streamInfo->minFrameSize << " bytes" << std::endl
              << " Maximum framesize: " << _streamInfo->maxFrameSize << " bytes" << std::endl
              << " Sample rate: " << _streamInfo->sampleRate << " Hz" << std::endl
              << " Channels: " << _streamInfo->channelCount + 1 << std::endl
              << " Bits per sample: " << _streamInfo->bitsPerSample + 1 << std::endl
              << " Total samples: " << _streamInfo->totalSampleCount << std::endl;
    std::cout << " MD5 signature: ";
    for (size_t i = 0; i < 16; i++)
      std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex
                << (uint32_t)_streamInfo->md5sum[i];
    std::cout << std::endl << std::endl;
  }

  if (_appData.size() > 0) {
    std::cout << "[APPLICATION]" << std::endl;
    for (auto& pair: _appData)
      std::cout << std::setfill('0') << std::setw(8) << std::right << std::hex
                << " Application ID: 0x" << pair.first << std::endl;
    std::cout << std::endl;
  }

  if (_seekPointsCount > 0) {
    std::cout << "[SEEKTABLE]" << std::endl;
    size_t i = 0;
    for (auto& seekPoint: _seekPoints) {
      std::cout << std::dec << " [" << i++ << "]" << std::endl;
      std::cout << "  Sample number: " << seekPoint.number << std::endl
                << "  Offset: " << seekPoint.offset << " bytes" << std::endl
                << "  Samples: " << seekPoint.sampleCount << " samples"
                << std::endl << std::endl;
    }
  }

  if (_comment) {
    std::cout << "[VORBIS_COMMENT]" << std::endl;
    std::cout << " Vendor: " << _comment->vendor << std::endl
              << " Comments:" << std::endl;
    size_t i = 0;
    for (auto& comment: _comment->commentList)
      std::cout << std::dec << "  [" << ++i << "]: " << comment << std::endl;
    std::cout << std::endl;
  }

  if (_picture) {
    std::cout << "[PICTURE]" << std::endl;
    std::cout << "  Type: " << _picture->type << std::endl;
    std::cout << "  MIME: " << _picture->mime << std::endl;
    std::cout << "  Description: " << _picture->description << std::endl;
    std::cout << "  Size: " << _picture->width << " x " << _picture->height << std::endl;
    std::cout << "  Color depth: " << _picture->colorDepth << std::endl;
    std::cout << "  Colors: " << _picture->colorCount << std::endl << std::endl;
  }
}

bool FlacFile::Parse() {
  if (getBig32() != FLAC_MAGIC) {
    _lastError = FLAC_INVALID_MAGIC;
    return false;
  }

  bool streamInfoInit = false;
  while (true) {
    uint8_t flags = getBig8();
    uint32_t size = getBig24();
    if (!isOk()) break;

    uint8_t last = flags >> 7;
    uint8_t type = flags & 0x7F;

    if (!streamInfoInit) {
      if (type != TYPE_STREAMINFO) {
        _lastError = FLAC_INVALID_STREAMINFO_POS;
        break;
      } else {
        streamInfoInit = true;
      }
    }

    parseBlock(type, size);
    if (!isOk()) break;

    if (last == 1) break;
  }

  return isOk();
}

void FlacFile::parseBlock(uint8_t type, uint32_t size) {
  switch (type) {
    case TYPE_STREAMINFO    : parseBlockStreamInfo(size); break;
    case TYPE_APPLICATION   : parseBlockApplication(size); break;
    case TYPE_SEEKTABLE     : parseBlockSeekTable(size); break;
    case TYPE_VORBIS_COMMENT: parseBlockVorbisComment(size); break;
    case TYPE_CUESHEET      : parseBlockCueSheet(size); break;
    case TYPE_PICTURE       : parseBlockPicture(size); break;
    default: parseBlockSkip(size); break;
  }
}

void FlacFile::parseBlockStreamInfo(uint32_t size) {
  uint64_t tmp;

  if (_streamInfo || size != 34) {
    _lastError = FLAC_INVALID_STREAMINFO;
    return;
  }

  _streamInfo = new FlacStreamInfo();
  _streamInfo->minBlockSize = getBig16();
  _streamInfo->maxBlockSize = getBig16();
  _streamInfo->minFrameSize = getBig24();
  _streamInfo->maxFrameSize = getBig24();
  tmp = getBig32();
  _streamInfo->sampleRate = tmp >> 12;
  _streamInfo->channelCount = (tmp >> 9) & 0b111;
  _streamInfo->bitsPerSample = (tmp >> 4) & 0b11111;
  _streamInfo->totalSampleCount  = (tmp & 0b1111) << 32;
  _streamInfo->totalSampleCount |= getBig32();
  getBuffer(_streamInfo->md5sum, 16);

  if (isOk() &&
      (_streamInfo->minBlockSize > _streamInfo->maxBlockSize ||
       _streamInfo->minFrameSize > _streamInfo->maxFrameSize)) {
    _lastError = FLAC_INVALID_STREAMINFO;
    return;
  }
  this->Dump();
}

void FlacFile::parseBlockApplication(uint32_t size) {
  if (size < 4) {
    _lastError = FLAC_INVALID_APPLICATION;
    return;
  }

  uint32_t appId = getBig32();
  if (!isOk()) return;
  if (_appData.find(appId) != _appData.end()) {
    _lastError = FLAC_INVALID_APPLICATION;
    return;
  }

  uint8_t* data = new uint8_t[size - 4]();
  getBuffer(data, size - 4);
  _appData.emplace(appId, data);
}

void FlacFile::parseBlockSeekTable(uint32_t size) {
  if (size % 18) {
    _lastError = FLAC_INVALID_SEEKTABLE;
    return;
  }

  uint32_t tableCount = size / 18;
  _seekPoints.resize(tableCount);
  for (size_t i = 0; i < tableCount; i++) {
    _seekPoints[_seekPointsCount].number = getBig64();
    _seekPoints[_seekPointsCount].offset = getBig64();
    _seekPoints[_seekPointsCount].sampleCount = getBig16();
    _seekPointsCount++;
  }
  
}

void FlacFile::parseBlockVorbisComment(uint32_t size) {
  uint32_t count, length, totalSize = 0;

  if (_comment == NULL)
    _comment = new FlacComment();

  length = getLittle32();
  if (!isOk()) return;
  getString(_comment->vendor, length);
  totalSize += 4 + length;
  count = getLittle32();
  if (!isOk()) return;
  totalSize += 4;

  _comment->commentList.reserve(count);
  for (uint32_t i = 0; i < count; i++) {
    length = getLittle32();
    if (!isOk()) return;
    _comment->commentList.emplace_back();
    getString(_comment->commentList.back(), length);
    totalSize += 4 + length;
  }

  if (totalSize != size) {
    _lastError = FLAC_INVALID_VORBIS_COMMENT;
    return;
  }
}

void FlacFile::parseBlockCueSheet(uint32_t size) {
  std::cerr << "[-] CUESHEET is not supported" << std::endl;
  return parseBlockSkip(size);
}

void FlacFile::parseBlockPicture(uint32_t size) {
  uint32_t length;
  uint32_t totalSize = 4;

  if (_picture == NULL)
    _picture = new FlacPicture();

  _picture->type = getBig32();

  length = getBig32();
  if (!isOk()) return;
  getString(_picture->mime, length);
  totalSize += 4 + length;

  length = getBig32();
  if (!isOk()) return;
  getString(_picture->mime, length);
  totalSize += 4 + length;

  _picture->width = getBig32();
  _picture->height = getBig32();
  _picture->colorDepth = getBig32();
  _picture->colorCount = getBig32();
  totalSize += 16;

  _picture->dataLength = getBig32();
  if (!isOk()) return;
  totalSize += 4 + _picture->dataLength;
  if (totalSize != size) {
    _lastError = FLAC_INVALID_PICTURE;
    return;
  }
  
  if (_picture->data) delete[] _picture->data;
  _picture->data = new uint8_t[_picture->dataLength];
  getBuffer(_picture->data, _picture->dataLength);
}

void FlacFile::parseBlockSkip(uint32_t size) {
  _fin.seekg(size, std::ios_base::cur);
}

uint8_t FlacFile::getBig8() {
  uint8_t v = 0;
  _fin.read(reinterpret_cast<char*>(&v), 1);
  if (_fin.gcount() != 1) _lastError = FLAC_TRUNC;
  return v;
}

uint16_t FlacFile::getBig16() {
  uint16_t v = 0;
  _fin.read(reinterpret_cast<char*>(&v), 2);
  if (_fin.gcount() != 2) _lastError = FLAC_TRUNC;
  return __builtin_bswap16(v);
}

uint32_t FlacFile::getBig24() {
  uint32_t v = 0;
  _fin.read(reinterpret_cast<char*>(&v) + 1, 3);
  if (_fin.gcount() != 3) _lastError = FLAC_TRUNC;
  return __builtin_bswap32(v);
}

uint32_t FlacFile::getBig32() {
  uint32_t v = 0;
  _fin.read(reinterpret_cast<char*>(&v), 4);
  if (_fin.gcount() != 4) _lastError = FLAC_TRUNC;
  return __builtin_bswap32(v);
}

uint64_t FlacFile::getBig64() {
  uint64_t v = 0;
  _fin.read(reinterpret_cast<char*>(&v), 8);
  if (_fin.gcount() != 8) _lastError = FLAC_TRUNC;
  return __builtin_bswap64(v);
}

uint32_t FlacFile::getLittle32() {
  uint32_t v = 0;
  _fin.read(reinterpret_cast<char*>(&v), 4);
  if (_fin.gcount() != 4) _lastError = FLAC_TRUNC;
  return v;
}

void FlacFile::getBuffer(uint8_t *buf, uint32_t size) {
  _fin.read(reinterpret_cast<char*>(buf), size);
  if (_fin.gcount() != size) _lastError = FLAC_TRUNC;
}

void FlacFile::getString(std::string& str, uint32_t size) {
  str.resize(size);
  getBuffer(reinterpret_cast<uint8_t*>((char*)str.data()), size);
}

bool FlacFile::isOk() {
  return _lastError == FLAC_SUCCESS;
}

const char* FlacFile::Reason() {
  switch (_lastError) {
    case FLAC_SUCCESS: return "Success";
    case FLAC_INVALID_MAGIC: return "Not a FLAC file";
    case FLAC_INVALID_STREAMINFO: return "Invalid stream information";
    case FLAC_INVALID_STREAMINFO_POS: return "First block is not stream information";
    case FLAC_INVALID_APPLICATION: return "Invalid application";
    case FLAC_INVALID_VORBIS_COMMENT: return "Invalid Vorbis comment";
    case FLAC_INVALID_SEEKTABLE: return "Invalid seek table";
    case FLAC_INVALID_PICTURE: return "Invalid picture";
    case FLAC_TRUNC: return "File is truncated";
  }
  return "Unknown error";
}
