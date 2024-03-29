#include <iostream>
#include <opencv2/opencv.hpp>

#include <RedisImageHelper.hpp>
#include <cxxopts.hpp>
#include "ImageUtils.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

bool VERBOSE = false;
bool STREAM_MODE = true;
std::string redisInputKey = "camera0";
std::string redisHost = "127.0.0.1";
int redisPort = 6379;

struct cameraParams {
    uint width;
    uint height;
    uint channels;
    void* client;
};

static int parseCommandLine(cxxopts::Options options, int argc, char** argv)
{
    auto result = options.parse(argc, argv);
    if (result.count("h")) {
        std::cout << options.help({"", "Group"});
        return EXIT_FAILURE;
    }

    if (result.count("v")) {
        VERBOSE = true;
        std::cerr << "Verbose mode enabled." << std::endl;
    }

    if (result.count("i")) {
        redisInputKey = result["i"].as<std::string>();
        if (VERBOSE) {
            std::cerr << "Input key was set to `" << redisInputKey << "`." << std::endl;
        }
    }
    else {
        if (VERBOSE) {
            std::cerr << "No input key was specified. Input key was set to default (" << redisInputKey << ")." << std::endl;
        }
    }

    if (result.count("u")) {
        STREAM_MODE = false;
        if (VERBOSE) {
            std::cerr << "Unique mode enabled." << std::endl;
        }
    }

    if (result.count("redis-port")) {
        redisPort = result["redis-port"].as<int>();
        if (VERBOSE) {
            std::cerr << "Redis port set to " << redisPort << "." << std::endl;
        }
    }
    else {
        if (VERBOSE) {
            std::cerr << "No redis port specified. Redis port was set to " << redisPort << "." << std::endl;
        }
    }

    if (result.count("redis-host")) {
        redisHost = result["redis-host"].as<std::string>();
        if (VERBOSE) {
            std::cerr << "Redis host set to " << redisHost << "." << std::endl;
        }
    }
    else {
        if (VERBOSE) {
            std::cerr << "No redis host was specified. Redis host was set to " << redisHost << "." << std::endl;
        }
    }


    return 0;
}

void onImagePublished(redisAsyncContext* c, void* data, void* privdata)
{
    struct cameraParams* cameraParams = static_cast<struct cameraParams*>(privdata);
    if (cameraParams == NULL) {
        if(VERBOSE) {
            std::cerr << "Could not read camera parameters from private data." << std::endl;
        }
        return;
    }
    uint width = cameraParams->width;
    uint height = cameraParams->height;
    uint channels = cameraParams->channels;
    RedisImageHelperSync *clientSync =  (RedisImageHelperSync*) cameraParams->client;

    // std::cout << "Width height " << width << " " << height << std::endl;

    redisReply *reply = (redisReply*) data;
    if  (reply == NULL)
    {
        return;
    } 
    else { 

    // if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 3)
    
        // cv::Mat displayFrame;
        // Image* cFrame = RedisImageHelper::dataToImage(reply->element[2]->str, width, height, channels, reply->element[2]->len);
        // if (cFrame == NULL) {
        //     if (VERBOSE) {
        //         std::cerr << "Could not retrieve image from data." << std::endl;
        //     }
        //     return;
        // }
      // Do another get
       cv::Mat displayFrame;
       Image* image = clientSync->getImage(width, height, channels, redisInputKey);
        if (image == NULL) { 
          std::cerr << "Error: Could not get camera frame, exiting..." << std::endl;
        }else { 

          cv::Mat frame;

          // std::cout << "Channels" << channels << std::endl; 
          if(channels == 2){
 // cv::Mat frame = cv::Mat(image->height(), image->width(), CV_8UC3, (void*)image->data());
            frame = cv::Mat(image->height(), image->width(), CV_16UC1, (void*)image->data());     

            // cv::Mat new_image = cv::Mat::zeros( frame.size(), frame.type() );
            // double alpha = 2.0; /*< Simple contrast control */
            // int beta = 10; /*< Simple brightness control */

            // for( int y = 0; y < frame.rows; y++ ) {
            //   for( int x = 0; x < frame.cols; x++ ) {
            //     for( int c = 0; c < frame.channels(); c++ ) {
            //       new_image.at<cv::Vec3b>(y,x)[c] =
            //       cv::saturate_cast<uchar>( alpha*frame.at<cv::Vec3b>(y,x)[c] + beta );
            //     }
            //   }
            // }
            // frame = new_image;
          }

          if(channels == 3){
            frame = cv::Mat(image->height(), image->width(), CV_8UC3, (void*)image->data());
          }
          // cv::Mat frame = cv::Mat(image->height(), image->width(), CV_8UC3, (void*)image->data());
          // cv::Mat frame = cv::Mat(image->height(), image->width(), CV_8UC3, (void*)image->data());

          //  cv::cvtColor(frame, displayFrame, cv::COLOR_GRAY2RGB);
          //  cv::cvtColor(frame, displayFrame, cv::COLOR_RGB2BGR);
          //  cv::Mat hsv = cv::Mat(image->height(), image->width(), CV_8UC3); 
          // cv::cvtColor(frame, displayFrame, cv::COLOR_BGR2HSV);
           // cv::imshow("frame", frame);
          cv::imshow("frame", frame);
          cv::waitKey(1);
        }
    }
}

cv::Mat displayFrame;
struct cameraParams contextData;

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "Camera client sample program.");
    options.add_options()
            ("redis-host", "The host adress to which the redis client should try to connect", cxxopts::value<std::string>())
            ("redis-port", "The port to which the redis client should try to connect.", cxxopts::value<int>())
            ("i, input", "The redis input key where data are going to arrive.", cxxopts::value<std::string>())
            ("s, stream", "Deactivate stream mode. In stream mode the program will constantly process input data and publish output data. By default stream mode is enabled.")
            ("u, unique", "Activate unique mode. In unique mode the program will only read and output data one time.")
            ("v, verbose", "Enable verbose mode. This will print helpfull process informations on the standard error stream.")
            ("h, help", "Print this help message.");

    int retCode = parseCommandLine(options, argc, argv);
    if (retCode)
    {
        return EXIT_FAILURE;
    }

    RedisImageHelperSync clientSync(redisHost, redisPort, redisInputKey);
    if (!clientSync.connect()) {
        std::cerr << "Could not connect to redis server." << std::endl;
        return EXIT_FAILURE;
    }

    // struct cameraParams contextData;
    // Removed cameraParameters...
    contextData.width = clientSync.getInt(redisInputKey + ":width");
    contextData.height = clientSync.getInt(redisInputKey + ":height");
    contextData.channels = clientSync.getInt(redisInputKey + ":channels");
    contextData.client = &clientSync;

    if (STREAM_MODE) {
        RedisImageHelperAsync clientAsync(redisHost, redisPort, redisInputKey);
        if (!clientAsync.connect()) {
            std::cerr << "Could not connect to redis server." << std::endl;
            return EXIT_FAILURE;
        }

	      clientAsync.subscribe(redisInputKey, onImagePublished, static_cast<void*>(&contextData));
        // while(true){
        //   cv::Mat displayFrame;
        //   Image* image = clientSync.getImage(contextData.width, contextData.height, contextData.channels, redisInputKey);
        //   if (image == NULL) { std::cerr << "Error: Could not get camera frame, exiting..." << std::endl; return EXIT_FAILURE;}
        //   cv::Mat frame = cv::Mat(image->height(), image->width(), CV_8UC3, (void*)image->data());
        //   cv::cvtColor(frame, displayFrame, cv::COLOR_RGB2BGR);
        //   cv::imshow("frame", displayFrame);
        //   cv::waitKey(1);
        //   delete image;
        // }

        return EXIT_SUCCESS;
    }
    else {
        cv::Mat displayFrame;
        Image* image = clientSync.getImage(contextData.width, contextData.height, contextData.channels, redisInputKey);
        if (image == NULL) { std::cerr << "Error: Could not get camera frame, exiting..." << std::endl; return EXIT_FAILURE;}
        cv::Mat frame = cv::Mat(image->height(), image->width(), CV_8UC3, (void*)image->data());
        cv::cvtColor(frame, displayFrame, cv::COLOR_RGB2BGR);
        cv::imshow("frame", displayFrame);
        cv::waitKey();
        delete image;

        return EXIT_SUCCESS;
    }
}
