#ifndef __MARKER_RECOGNIZER_H__
#define __MARKER_RECOGNIZER_H__

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
//#include <opencv2/cudaimgproc.hpp>
#include <vector>

class Marker
{
public:
    int m_id;
    std::vector<cv::Point2f> m_corners;
    // c0------c3
    // |		|
    // |		|
    // c1------c2

public:
    Marker();
    Marker(int _id, cv::Point2f _c0, cv::Point2f _c1, cv::Point2f _c2, cv::Point2f _c3);

//    void estimateTransformToCamera(std::vector<cv::Point3f> corners_3d, cv::Mat& camera_matrix, cv::Mat& dist_coeff, cv::Mat& rmat, cv::Mat& tvec);
//    void drawToImage(cv::Mat& image, cv::Scalar color, float thickness);
};

class MarkerRecognizer
{
private:
    std::vector<cv::Point2f> m_marker_coords;
    std::vector<Marker> m_markers;
    float camera_matrix_[9] =
    {
        1448.4650273883371f, 0.0f, 639.0f,
        0.0f, 1448.4650273883371f, 359.25f,
        0.0f, 0.0f, 1.0f
    };
    float dist_coeff_[5] = {-0.075f, 0.5f, -0.00f, -0.000f,    -0.00f};
    cv::Point3f corners_3d_598[4] =
    {
        cv::Point3f(-0.898f, -0.898f, 0),
        cv::Point3f(-0.898f, -0.702f, 0),
        cv::Point3f(-0.702f, -0.702f, 0),
        cv::Point3f(-0.702f, -0.898f, 0)
    };
    cv::Point3f corners_3d_108[4] =
    {
        cv::Point3f(-0.898f, -0.098f, 0),
        cv::Point3f(-0.898f,  0.098f, 0),
        cv::Point3f(-0.702f,  0.098f, 0),
        cv::Point3f(-0.702f, -0.098f, 0)
    };
    cv::Point3f corners_3d_915[4] =
    {
        cv::Point3f(-0.898f,  0.702f, 0),
        cv::Point3f(-0.898f,  0.898f, 0),
        cv::Point3f(-0.702f,  0.898f, 0),
        cv::Point3f(-0.702f,  0.702f, 0)
    };
    cv::Point3f corners_3d_198[4] =
    {
        cv::Point3f( 0.702f,  0.702f, 0),
        cv::Point3f( 0.702f,  0.898f, 0),
        cv::Point3f( 0.898f,  0.898f, 0),
        cv::Point3f( 0.898f,  0.702f, 0)
    };
    cv::Point3f corners_3d_213[4] =
    {
        cv::Point3f( 0.702f, -0.098f, 0),
        cv::Point3f( 0.702f,  0.098f, 0),
        cv::Point3f( 0.898f,  0.098f, 0),
        cv::Point3f( 0.898f, -0.098f, 0)
    };
    cv::Point3f corners_3d_10[4] =
    {
        cv::Point3f( 0.702f,  0.702f, 0),
        cv::Point3f( 0.702f,  0.898f, 0),
        cv::Point3f( 0.898f,  0.898f, 0),
        cv::Point3f( 0.898f,  0.702f, 0)
    };

    cv::Mat camera_matrix = cv::Mat(3, 3, CV_32FC1, camera_matrix_).clone();
    cv::Mat dist_coeffs = cv::Mat(1, 4, CV_32FC1, dist_coeff_).clone();
    cv::Mat transMatrix = cv::Mat::zeros(4,4,CV_64F);
    cv::Mat coorMatrix = cv::Mat::zeros(4,4,CV_64F);
    cv::Mat rvec,tvec,rmat;
    std::vector<cv::Point3f>m_corners_3d;// = vector<Point3f>(corners_3d, corners_3d + 4);
    std::vector<cv::Point3f>m_corners_3d_598 = std::vector<cv::Point3f>(corners_3d_598,corners_3d_598+4);
    std::vector<cv::Point3f>m_corners_3d_108 = std::vector<cv::Point3f>(corners_3d_108,corners_3d_108+4);
    std::vector<cv::Point3f>m_corners_3d_915 = std::vector<cv::Point3f>(corners_3d_915,corners_3d_915+4);
    std::vector<cv::Point3f>m_corners_3d_198 = std::vector<cv::Point3f>(corners_3d_198,corners_3d_198+4);
    std::vector<cv::Point3f>m_corners_3d_10  = std::vector<cv::Point3f>(corners_3d_10 ,corners_3d_10+4 );
    std::vector<cv::Point3f>m_corners_3d_213 = std::vector<cv::Point3f>(corners_3d_213,corners_3d_213+4);
    std::vector<cv::Point2f>m_corners_2d;

private:
    void markerDetect(cv::Mat& img_gray, std::vector<Marker>& possible_markers, int min_size, int min_side_length);
    void markerRecognize(cv::Mat& img_gray, std::vector<Marker>& possible_markers, std::vector<Marker>& final_markers);
    void markerRefine(cv::Mat& img_gray, std::vector<Marker>& final_markers);
    cv::Mat bitMatrixRotate(cv::Mat& bit_matrix);
    int hammingDistance(cv::Mat& bit_matrix);
    int bitMatrixToId(cv::Mat& bit_matrix);


   // cv::cuda::GpuMat gpuGray, gpuBinary;

public:
    MarkerRecognizer();
    int update(cv::Mat& image_gray, int min_size, int min_side_length = 10);
    std::vector<Marker>& getMarkers();
    bool position_est(cv::Mat opencv_frame);
    cv::Mat get_position();
    //std::vector<cv::Point3f> obj_recognize(cv::Mat opencv_frame);
 //   void drawToImage(cv::Mat& image, cv::Scalar color, float thickness);
};

#endif
