#include "nifti.hpp"

NIFTI::NIFTI(std::string filename)
{
    //check the extension of the file and ensure that the header is being read first
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    if (extension == "img")
    {
        int index = filename.find("img");
        filename.replace(index, 3, "hdr");
    }

    std::ifstream niftiFile(filename.c_str(), std::ios::in | std::ios::binary);

    //READING HEADER: BASED ON http://www.grahamwideman.com/gw/brain/analyze/formatdoc.htm
    int sizeof_hdr = readInt32(niftiFile);

    char data_type[10];
    niftiFile.read(data_type, 10);
    char db_name[18];
    niftiFile.read(data_type, 18);

    int extents = readInt32(niftiFile);
    int session_error = readInt16(niftiFile);
    char regular = readChar(niftiFile);
    char hkey_un0 = readChar(niftiFile);
    int dimNum = readInt16(niftiFile);
    width = readInt16(niftiFile);
    height = readInt16(niftiFile);
    depth = readInt16(niftiFile);
    int time = readInt16(niftiFile);

    int dim5 = readInt16(niftiFile);
    int dim6 = readInt16(niftiFile); //these three are undocumented and do nothing
    int dim7 = readInt16(niftiFile);

    char vox_units[4];
    niftiFile.read(vox_units, 4);
    char cal_units[8];
    niftiFile.read(cal_units, 8);
    
    int unused1 = readInt16(niftiFile);
    int datatype = readInt16(niftiFile);
    int bitpix = readInt16(niftiFile);
    int dim_un0 = readInt16(niftiFile);

    float pixdim0 = readFloat(niftiFile);
    float pixdim1 = readFloat(niftiFile);
    float pixdim2 = readFloat(niftiFile);
    float pixdim3 = readFloat(niftiFile);
    float pixdim4 = readFloat(niftiFile);
    float pixdim5 = readFloat(niftiFile);
    float pixdim6 = readFloat(niftiFile);
    float pixdim7 = readFloat(niftiFile);

    float vox_offset = readFloat(niftiFile);
}
