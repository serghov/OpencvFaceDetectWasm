#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <emscripten.h>

#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


using namespace std;
using namespace cv;
using namespace chrono;

/**
 * Gets current time in milliseconds
 * @return
 */
long long getMillis()
{
    chrono::milliseconds ms = duration_cast<chrono::milliseconds>(high_resolution_clock::now().time_since_epoch());
    return ms.count();
}

SDL_Surface *screen = nullptr;
SDL_Surface *sdlImage = nullptr;

/**
 * Displays an image using sdl
 * apparently not all functionality is implemented in emscripten
 * so only rgba images are currently supported
 * @param img
 */
void sdlShowImage(Mat &img)
{
    int width = img.cols, height = img.rows;
    SDL_Rect rect = {0, 0, width, height};

    if (sdlImage)
    {
        SDL_FreeSurface(sdlImage);
    }
    sdlImage = SDL_CreateRGBSurfaceFrom(img.data, width, height, 32, width * 4,
                                        0x00ff0000,
                                        0x0000ff00,
                                        0x000000ff,
                                        0xff000000);



//    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_BlitSurface(sdlImage, &rect, screen, &rect);
    SDL_UpdateRect(screen, 0, 0, width, height);
}


String face_cascade_name = "haarcascade_frontalface_default.xml";
CascadeClassifier face_cascade;
RNG rng(12345);


/**
 * Finds faces in an image, draws circles around them
 * and displays the image using sdl
 * http://docs.opencv.org/2.4/doc/tutorials/objdetect/cascade_classifier/cascade_classifier.html
 * @param frame
 */
void detectAndDisplay(Mat frame)
{
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor(frame, frame_gray, CV_RGBA2GRAY);
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale(frame_gray, faces, 2, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(50, 50),
                                  Size(frame.rows * 0.9, frame.cols * 0.9));

    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
        ellipse(frame, center, Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360,
                Scalar(0, 0, 255, 255), 1, 8, 0);
    }

    sdlShowImage(frame);
}

/**
 * This function is called from js, when a new video frame is ready
 * it takes the frame, puts it into a an opencv Mat
 * finds faces, and draws the output using sdl
 */
extern "C"
void onNewImage(uchar *data, int width, int height)
{
    long long startTime = getMillis();

    Mat image(height, width, CV_8UC4);
    image.data = data;
    detectAndDisplay(image);
    delete data;
    long long endTime = getMillis();
//    cout << "Took " << endTime - startTime << "ms, " << (int) (1000.0 / (endTime - startTime)) << "fps;" << endl;

    // we show this on the upper part of the webpage
    string res = "updateData('Took " + std::to_string(endTime - startTime) + "ms, " +
                 std::to_string((int) (1000.0 / (endTime - startTime))) + "fps;');";
    emscripten_run_script(res.c_str());
}

void update()
{
    // nothing here, just an empty update loop
}

int main(int, char **)
{
    // initialize sdl
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(640, 360, 32, SDL_HWSURFACE);

    // load opencv haar cascade file, which needs to be in
    // in emscripten memory
    if (!face_cascade.load(face_cascade_name))
    {
        printf("--(!)Error loading\n");
        cout << "Error loading face haar cascade file " << face_cascade_name << endl;
        return -1;
    }


    cout << "Ready" << endl;

    // tell js that we are ready
    emscripten_run_script("isReady = true");
    // run an empty loop to our program doesnt end with return 0
    emscripten_set_main_loop(update, 0, 1);


    return 0;
}
//emcc ../main.cpp -I /home/serg/build/opencv/build/install/include -l/home/serg/build/opencv/build/install/lib/libopencv_core -l/home/serg/build/opencv/build/install/lib/libopencv_imgproc -l/home/serg/build/opencv/build/install/lib/libopencv_imgcodecs -l/home/serg/build/opencv/build/install/lib/libopencv_highgui -l/home/serg/build/opencv/build/install/lib/libopencv_photo -l/home/serg/build/opencv/build/install/lib/libopencv_video -l/home/serg/build/opencv/build/install/lib/libopencv_videoio -O3 --preload-file test.bmp -s WASM=1 -o main.js