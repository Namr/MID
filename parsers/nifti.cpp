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

    std::ifstream niftiFile(filename.c_str(), std::ios::in | std::ios::binary);
    if (niftiFile.is_open())
    {
        char *header;
        int shouldSwap = 0;

        //READING HEADER: BASED ON http://www.grahamwideman.com/gw/brain/analyze/formatdoc.htm

        //find the size of the header, and then read it into memory all at once
        int32_t sizeof_hdr;
        niftiFile.read(reinterpret_cast<char *>(&sizeof_hdr), sizeof(sizeof_hdr));
        if (sizeof_hdr != 348)
        {
            endswap(&sizeof_hdr);
            shouldSwap = 1;
        }

        std::cout << sizeof_hdr << std::endl;

        header = new char[sizeof_hdr];
        niftiFile.seekg(0, std::ios::beg);
        niftiFile.read(header, sizeof_hdr);

        //load the important values from the header memory block into their corresponding varibles
        memcpy(&dimNum, &header[40], sizeof(dimNum));
        memcpy(&width, &header[42], sizeof(width));
        memcpy(&height, &header[44], sizeof(height));
        memcpy(&depth, &header[46], sizeof(depth));
        memcpy(&time, &header[48], sizeof(time));
        memcpy(&bitsPerPixel, &header[72], sizeof(bitsPerPixel));
        memcpy(&voxelWidth, &header[80], sizeof(voxelWidth));
        memcpy(&voxelHeight, &header[84], sizeof(voxelHeight));
        memcpy(&voxelDepth, &header[88], sizeof(voxelDepth));

        memcpy(&vox_offset, &header[108], sizeof(vox_offset));

        if (shouldSwap)
        {
            endswap(&dimNum);
            endswap(&width);
            endswap(&height);
            endswap(&depth);
            endswap(&time);
            endswap(&bitsPerPixel);
            endswap(&voxelWidth);
            endswap(&voxelHeight);
            endswap(&voxelDepth);
            endswap(&vox_offset);
        }

        bytesPerPixel = bitsPerPixel / 8;
        //free memory
        delete[] header;

        //switch back to reading image data, if all data is in one file (nii) move to the data
        if (extension == "img" || extension == "hdr")
        {
            int index = filename.find("hdr");
            filename.replace(index, 3, "img");
            niftiFile = std::ifstream(filename.c_str(), std::ios::in | std::ios::binary);
        }
        else if (extension == "nii")
        {
            niftiFile.seekg((int)vox_offset);
        }

        //load data chunk into memory
        int sizeof_data = std::abs(width * height * depth * time * bytesPerPixel);
        data = new char[sizeof_data];
        niftiFile.read(data, sizeof_data);
    }
}
