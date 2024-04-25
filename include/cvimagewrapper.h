/*
 * BnpImageData.h
 *
 *  Created on: Jun 15, 2009
 *      Author: taras
 */

#ifndef CVIMAGEWRAPPER_H_
#define CVIMAGEWRAPPER_H_

#include <bionic.h>

#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include <matio.h>

using namespace std;
using namespace BioNic;

/**
 * Gia data wrapper class for images.
 * 2011-09-20 Added Matlab mat file support
 */
class CvImageWrapper : public BnDataWrapper<IplImage> {
public:
	CvImageWrapper () { }
	CvImageWrapper (IplImage *image) : BnDataWrapper<IplImage>(image) { }
    
	virtual ~CvImageWrapper () {
		if (getData()) {
			IplImage * image = data();
			cvReleaseImage(&image);
			setData(0);
		}
	}
    
    string supportedFileExt () { return "jpg;tif;tiff;jpeg;jp2;png;cr2;bmp;dib;jpe;dib;pbm;pgm;ppm;"; } //mat;"; }
    
    // Serialization method. Stores object's properties into SerializationInfo structure.
    void serialize(string & value, const map<string,string> & config) {
        if (!data())
            return;
        string format = config.find("serialize_image_format")->second;
        value = generateDataFileName() + "." + format;
        cvSaveImage(value.c_str(), data());
	}
    
#define MAT_EXT ".mat"
    
    // Deserialization method. Sets current object's properties to those provided by SerializationInfo structure.
    void deserialize(const string & value, const map<string,string> & config) {
        IplImage *image = 0;
        if (value.find(MAT_EXT) == value.size() - ((string)MAT_EXT).size()) {
            return; // not supported
            // Matlab mat file detected!
            /*
            matvar_t *matvar;
            mat_t * mat = Mat_Open(value.c_str(), MAT_ACC_RDONLY);
            if ( mat ) {
                matvar = Mat_VarReadNextInfo(mat);
                if ( matvar != NULL ) {
//                    void *data;         //< Pointer to the data
                    Mat_VarReadDataAll(mat, matvar);                    
                    logMsg( LogLevel::LLFATAL, "Dimensions %d [ %d by %d ]; data type %d; data size %d; class type %d; Data pointer %d <%s> compressed:%d; size: %d", matvar->rank, matvar->dims[0], matvar->dims[1], matvar->data_type, matvar->data_size, matvar->class_type, matvar->data, matvar->name, matvar->compression, Mat_VarGetSize(matvar));
                    
                    int width = matvar->dims[0];
                    int height = matvar->dims[1];
                    
                    image = cvCreateImage(cvSize(height, width), IPL_DEPTH_8U, 1);
                    cvSetZero(image);
                    unsigned char max_val = 0;
                    for (int i = 0; i < height; ++i)
                        for (int j = 0; j < width; ++j) {
                            unsigned char t = *(((unsigned char*)matvar->data) + i*width + j);
                            max_val = max(max_val, t);
                            cvSetReal2D(image, j, i, t);
                        }
                    cvConvertScale(image, image, 255./(double)max_val, 0);
                    Mat_VarFree(matvar);
                } else {
                    logMsg( LogLevel::LLFATAL, "Matlab image loading small failure");
                    throw "test";
                }
                Mat_Close(mat);
            } else {
                logMsg( LogLevel::LLFATAL, "Matlab image loading failure");
                throw "test";
            }
            */
        } else {
            image = cvLoadImage(value.c_str(), CV_LOAD_IMAGE_ANYCOLOR);
        }
        
        if (!image) {
            logMsg( LogLevel::LLERROR, "(%s) image failed", value.c_str());
            setData( 0 );
            return ;
            throw formatMsg("(%s) image failed to load correctly! Check if the BioNic framework was compiled with libjpeg-dev or libtiff-dev or applicable.", value.c_str());
        } else {
            logMsg( LogLevel::LLDEBUG, "(%s) loaded;dim=[%dx%d];ch=[%d];depth=[%d]", value.c_str(), image->height, image->width, image->nChannels, image->depth);
        }
        setData( image );
	}

    long long getMemorySize () {
        if (data())
            return data()->imageSize;
        else
            return 0;
    }
    
	BN_DECLARE_DATA_WRAPPER(CvImageWrapper);
};

#undef MAT_EXT

#endif /* CVIMAGEWRAPPER_H_ */
