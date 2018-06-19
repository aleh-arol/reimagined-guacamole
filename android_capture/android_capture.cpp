
#include <chrono>
#include <exception>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "android_capture.hpp"


//TODO: timing to calc real fps
using _timer = std::chrono::high_resolution_clock;
using time_stamp_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
using steady_timestamp_t = std::chrono::time_point<std::chrono::steady_clock>;

bool run_capture(const std::string& dest_path, const std::string& codec_fourcc, unsigned frames_count, unsigned camera_id, unsigned api_id)
{
    assert(frames_count > 0);
    assert(codec_fourcc.empty() || codec_fourcc.size() == 4);

    unsigned frame_counter = 0;

    cv::Mat frame, face_frame;
    cv::VideoCapture cap;
    cv::VideoWriter encoder;

    if (cap.open(camera_id + api_id))
    {
        //get camera info
        cv::Size frame_dim = cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        unsigned cam_fps = static_cast<unsigned>(cap.get(cv::CAP_PROP_FPS));
        unsigned cam_format = static_cast<unsigned>(cap.get(cv::CAP_PROP_FORMAT));
        unsigned codec = static_cast<unsigned>(cap.get(cv::CAP_PROP_FOURCC));
        char codec_str[] = {(char)(codec & 0XFF) , (char)((codec & 0XFF00) >> 8),(char)((codec & 0XFF0000) >> 16),(char)((codec & 0XFF000000) >> 24), 0};

        std::cout << "Camera dimensions: [" << frame_dim.width << ":" << frame_dim.height << "]" << std::endl;
        std::cout << "Camera FPS: " << cam_fps << std::endl;
        std::cout << "Camera Format: " << cam_format << std::endl;
        std::cout << "Camera Codec: " << codec_str << std::endl;

        //try to open writer
        //MJPG or X264
        unsigned output_codec = codec_fourcc.empty() ? codec : cv::VideoWriter::fourcc(codec_fourcc[0], codec_fourcc[1], codec_fourcc[2], codec_fourcc[3]);

        //encoding fps should be equal to real encoding fps, otherwise video will be too fast
        //so the one need to estimate avg fps before encoding
        if (encoder.open(dest_path, output_codec, cam_fps, frame_dim))
        {
#if FACE_DETECTION
            auto face_cascade = cv::CascadeClassifier("lbpcascade_frontalface_improved.xml");
#endif
            auto start_time = std::chrono::high_resolution_clock::now();
            while(frames_count--)
            {
                cap.read(frame);

#if FACE_DETECTION
                //detect faces

                std::vector<cv::Rect> rois;
                std::vector<int> detection_levels;

                auto gray_frame = cv::Mat();
                cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
                face_cascade.detectMultiScale(gray_frame, rois, detection_levels);

                // only extract a single face
                if (!rois.empty())
                {
                    auto roi = rois[0];
                    frame(roi).copyTo(face_frame);

                    cv::copyMakeBorder(face_frame, face_frame, roi.y, frame_dim.height - (roi.y + roi.height), roi.x, frame_dim.width - (roi.x + roi.width), cv::BORDER_CONSTANT, 40);
                }
                else
                {
                    face_frame = cv::Mat(frame_dim, frame.type());
                }
#else
                auto face_frame = frame;
#endif
                encoder.write(face_frame);
                ++frame_counter;

                // check if we succeeded
                if (frame.empty()) {
                    throw std::runtime_error(std::string("Captured empty frame!"));
                }
            }
            auto end_time = std::chrono::high_resolution_clock::now();

            std::cout << "Processed " << frame_counter << " frames in " << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() << " seconds." << std::endl;
            std::cout << "Average FPS " << static_cast<float>(frame_counter) * 1000.f / std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << std::endl;
        }
        else
        {
            throw std::runtime_error("Can't open encoder");
        }
    }
    else
    {
        throw std::runtime_error("Can't open camera");
    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
