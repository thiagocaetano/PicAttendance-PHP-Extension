#include "crop_image.h"
#include "find_faces.h"
#include "recognize_faces.h"

#include "opencv2/highgui/highgui.hpp"
#include "phpcpp.h"

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension myExtension("picattendance", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
        myExtension.add("PICATTENDANCE_find_faces", FindFaces);
        myExtension.add("PICATTENDANCE_recognize_faces", RecognizeFaces);
        myExtension.add("PICATTENDANCE_crop_image", CropImage);
        // return the extension
        return myExtension;
    }
}
