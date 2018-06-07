#include "parsers/nifti.hpp"

NIFTI::NIFTI(std::string filename)
{
    //check the extension of the file and ensure that the header is being read first
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    if (extension == "img")
    {
        int index = filename.find("img");
        filename.replace(index, 3, "hdr");
    }

    std::ifstream niftiFile(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (niftiFile.is_open())
    {
        char *header;

        //READING HEADER: BASED ON http://www.grahamwideman.com/gw/brain/analyze/formatdoc.htm

        //find the size of the header, and then read it into memory
        int sizeof_hdr = readInt32(niftiFile);
        header = new char[sizeof_hdr];
        niftiFile.seekg(0, std::ios::beg);
        niftiFile.read(header, sizeof_hdr);


        dimNum = (((int)header[40]) << 8) | header[41];
        width = (((int)header[42]) << 8) | header[43];
        height = (((int)header[44]) << 8) | header[45];
        depth = (((int)header[46]) << 8) | header[47];
        time = (((int)header[48]) << 8) | header[49];

        bitsPerPixel = (((int)header[72]) << 8) | header[73];
        bytesPerPixel = bitsPerPixel / 8;

        memcpy(&voxelWidth, &header[80], sizeof(voxelWidth));
        memcpy(&voxelHeight, &header[84], sizeof(voxelWidth));
        memcpy(&voxelDepth, &header[88], sizeof(voxelWidth));

        float vox_offset;
        memcpy(&voxelWidth, &header[108], sizeof(voxelWidth));
    }
    //switch back to reading image data
    if (extension == "img" || extension == "hdr")
    {
        int index = filename.find("hdr");
        filename.replace(index, 3, "img");
        niftiFile = std::ifstream(filename.c_str(), std::ios::in | std::ios::binary);
    }

    //loop through MRI file and seperate each pixel into its x,y,z cordinate
    for (int z = 0; z < depth; z++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                //place data into a char buffer and then reinterpret into an int
                //allocate memory to store a single pixel, this size is determined by the bitsperpixel value in the header
                char *pix = (char *)malloc(bytesPerPixel * sizeof(char));
                niftiFile.read(pix, bytesPerPixel);
                data[x][y][z] = reinterpret_cast<unsigned char &>(pix[0]);
            }
        }
    }
}
