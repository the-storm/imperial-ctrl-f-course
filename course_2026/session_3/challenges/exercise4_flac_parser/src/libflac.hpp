#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>

#define FLAC_MAGIC 0x664c6143U

enum FlacError {
  FLAC_SUCCESS = 0,
  FLAC_INVALID_MAGIC,
  FLAC_INVALID_STREAMINFO,
  FLAC_INVALID_STREAMINFO_POS,
  FLAC_INVALID_APPLICATION,
  FLAC_INVALID_VORBIS_COMMENT,
  FLAC_INVALID_SEEKTABLE,
  FLAC_INVALID_PICTURE,
  FLAC_TRUNC,
};

enum FlacType {
  TYPE_STREAMINFO = 0,
  TYPE_PADDING,
  TYPE_APPLICATION,
  TYPE_SEEKTABLE,
  TYPE_VORBIS_COMMENT,
  TYPE_CUESHEET,
  TYPE_PICTURE,
};

struct FlacStreamInfo {
  uint16_t minBlockSize;
  uint16_t maxBlockSize;
  uint32_t minFrameSize;
  uint32_t maxFrameSize;
  uint32_t sampleRate;
  uint8_t  channelCount;
  uint8_t  bitsPerSample;
  uint64_t totalSampleCount;
  uint8_t  md5sum[16];
};

struct FlacSeekPoint {
  uint64_t number;
  uint64_t offset;
  uint16_t sampleCount;
};

struct FlacComment {
  std::string vendor;
  std::vector<std::string> commentList;
};

struct FlacPicture {
  FlacPicture(): data(NULL) {}

  uint32_t type;
  std::string mime;
  std::string description;
  uint32_t width;
  uint32_t height;
  uint32_t colorDepth;
  uint32_t colorCount;
  uint32_t dataLength;
  uint8_t  *data;
};

class FlacFile {
public:
  FlacFile(std::ifstream& fin);
  ~FlacFile();
  const char* Reason();
  bool Parse();
  void Dump();

private:
  bool isOk();
  uint8_t  getBig8();
  uint16_t getBig16();
  uint32_t getBig24();
  uint32_t getBig32();
  uint64_t getBig64();
  uint32_t getLittle32();
  void getBuffer(uint8_t *buf, uint32_t size);
  void getString(std::string& str, uint32_t size);
  void parseBlock(uint8_t type, uint32_t size);
  void parseBlockStreamInfo(uint32_t size);
  void parseBlockApplication(uint32_t size);
  void parseBlockSeekTable(uint32_t size);
  void parseBlockVorbisComment(uint32_t size);
  void parseBlockCueSheet(uint32_t size);
  void parseBlockPicture(uint32_t size);
  void parseBlockSkip(uint32_t size);

  std::ifstream& _fin;
  FlacError _lastError;

  FlacStreamInfo *_streamInfo;
  std::unordered_map<uint32_t, uint8_t*> _appData;
  std::vector<FlacSeekPoint> _seekPoints;
  size_t _seekPointsCount;
  FlacPicture *_picture;
  FlacComment *_comment;
};
