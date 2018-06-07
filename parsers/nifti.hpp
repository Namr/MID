#include <string>
#include <fstream>
#include <iostream>

class NIFTI
{
    int32_t readInt32(std::ifstream &stream)
    {
        uint32_t val;
        stream.read(reinterpret_cast<char *>(&val), sizeof(val));
        return val;
    }

    int16_t readInt16(std::ifstream &stream)
    {
        uint16_t val;
        stream.read(reinterpret_cast<char *>(&val), sizeof(val));
        return val;
    }

    float_t readFloat(std::ifstream &stream)
    {
        float_t val;
        stream.read(reinterpret_cast<char *>(&val), sizeof(val));
        return val;
    }
    char readChar(std::ifstream &stream)
    {
        char val[1];
        stream.read(val, 1);
        return val[0];
    }
  public:
    int width, height, depth;
    char *data;
    NIFTI(std::string filepath);
};
