#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "MarkerRecognizer.h"

#include <iostream>

using namespace cv;

int main()
{
    /*open camera*/
    VideoCapture cap(0);
    if(!cap.isOpened())
    {
        std::cout << "camera is not opened" << std::endl;
        return -1;
    }

    /*init the img*/
    Mat source, undistored, gray;

    /*intrins parameter*/
    float camera_matrix_[] =
    {
        595.65433848626435f, 0.0f, 319.5f,
        0.0f, 595.65433848626435f, 239.5f,
        0.0f, 0.0f, 1.0f
    };
    float dist_coeff_[] = {-0.44688069423840865f, 0.26100528802816081f, -0.00f, -0.000f,    -0.062279852268793255f};
    Point3f corners_3d_598[] =
    {
        Point3f(-0.824f, -0.824f, 0),
        Point3f(-0.824f, -0.776f, 0),
        Point3f(-0.776f, -0.776f, 0),
        Point3f(-0.776f, -0.824f, 0)
    };
    Point3f corners_3d_108[] =
    {
        Point3f(-0.824f, -0.024f, 0),
        Point3f(-0.824f,  0.024f, 0),
        Point3f(-0.776f,  0.024f, 0),
        Point3f(-0.776f, -0.024f, 0)
    };
    Point3f corners_3d_915[] =
    {
        Point3f(-0.824f,  0.776f, 0),
        Point3f(-0.824f,  0.824f, 0),
        Point3f(-0.776f,  0.824f, 0),
        Point3f(-0.776f,  0.776f, 0)
    };
    Point3f corners_3d_198[] =
    {
        Point3f( 0.776f,  0.776f, 0),
        Point3f( 0.776f,  0.824f, 0),
        Point3f( 0.824f,  0.824f, 0),
        Point3f( 0.824f,  0.776f, 0)
    };
    Point3f corners_3d_213[] =
    {
        Point3f( 0.776f, -0.024f, 0),
        Point3f( 0.776f,  0.024f, 0),
        Point3f( 0.824f,  0.024f, 0),
        Point3f( 0.824f, -0.024f, 0)
    };
    Point3f corners_3d_10[] =
    {
        Point3f( 0.776f,  0.776f, 0),
        Point3f( 0.776f,  0.824f, 0),
        Point3f( 0.824f,  0.824f, 0),
        Point3f( 0.824f,  0.776f, 0)
    };

    Mat camera_matrix = Mat(3, 3, CV_32FC1, camera_matrix_).clone();
    Mat dist_coeffs = Mat(1, 4, CV_32FC1, dist_coeff_).clone();
    Mat transMatrix = Mat::zeros(4,4,CV_64F);
    Mat coorMatrix = Mat::zeros(4,4,CV_64F);
    Mat rvec,tvec,rmat;
    vector<Point3f>m_corners_3d(24);// = vector<Point3f>(corners_3d, corners_3d + 4);
    vector<Point3f>m_corners_3d_598 = vector<Point3f>(corners_3d_598,corners_3d_598+4);
    vector<Point3f>m_corners_3d_108 = vector<Point3f>(corners_3d_108,corners_3d_108+4);
    vector<Point3f>m_corners_3d_915 = vector<Point3f>(corners_3d_915,corners_3d_915+4);
    vector<Point3f>m_corners_3d_198 = vector<Point3f>(corners_3d_198,corners_3d_198+4);
    vector<Point3f>m_corners_3d_10  = vector<Point3f>(corners_3d_10 ,corners_3d_10+4 );
    vector<Point3f>m_corners_3d_213 = vector<Point3f>(corners_3d_213,corners_3d_213+4);
    vector<Point2f>m_corners_2d(24);

//    namedWindow("haha",1);
    MarkerRecognizer m_recognizer;
    /*main loop*/
    for(;;)
    {
//        double t = (double)getTickCount();
        cap >> source;
        cvtColor(source,gray,CV_BGR2GRAY);
        undistort(gray,undistored,camera_matrix,dist_coeffs);

        m_recognizer.update(gray,100);

        vector<Marker>& markers = m_recognizer.getMarkers();
        m_corners_3d.clear();
        m_corners_2d.clear();
//
//        std::cout << markers.size() << std::endl;
        for (int i = 0; i < markers.size();i++)
        {
            switch (markers[i].m_id)
            {
            case 598 : m_corners_3d.insert(m_corners_3d.end(),m_corners_3d_598.begin(),m_corners_3d_598.end());break;
            case 108 : m_corners_3d.insert(m_corners_3d.end(),m_corners_3d_108.begin(),m_corners_3d_108.end());break;
            case 915 : m_corners_3d.insert(m_corners_3d.end(),m_corners_3d_915.begin(),m_corners_3d_915.end());break;
            case 198 : m_corners_3d.insert(m_corners_3d.end(),m_corners_3d_198.begin(),m_corners_3d_198.end());break;
            case 213 : m_corners_3d.insert(m_corners_3d.end(),m_corners_3d_213.begin(),m_corners_3d_213.end());break;
            case 10  : m_corners_3d.insert(m_corners_3d.end(),m_corners_3d_10.begin() ,m_corners_3d_10.end() );break;
            }
            m_corners_2d.insert(m_corners_2d.end(),markers[i].m_corners.begin(),markers[i].m_corners.end());
//            std::cout << i << "th:"<< markers[i].m_id << std::endl;
//            double t = (double)getTickCount();
//            solvePnP(m_corners_3d,markers[i].m_corners,camera_matrix,dist_coeffs,rvec,tvec);
//            t = ((double)getTickCount()-t)/getTickFrequency();
//            std::cout << i << "  " << t <<std::endl;
//            Rodrigues(rvec, rmat);
//            for(int nr = 0;nr < 3; nr++)
//            {
//                double *datar = rmat.ptr<double>(nr);
//                double *datat = transMatrix.ptr<double>(nr);
//                for(int nc = 0;nc < 3; nc++)
//                {
//                    datat[nc]=datar[nc];
//                }
//            }
//            transMatrix.at<double>(0,3) = tvec.at<double>(0,0);
//            transMatrix.at<double>(1,3) = tvec.at<double>(1,0);
//            transMatrix.at<double>(2,3) = tvec.at<double>(2,0);
//            transMatrix.at<double>(3,3) = 1.f;
//            coorMatrix = transMatrix.inv();

 //           std::cout << "roation" << coorMatrix << std::endl;
        }
//        std::cout << m_corners_3d.size() << "  "<<markers.size()<< std::endl;
        if(m_corners_3d.size() >0 )
        {
            solvePnP(m_corners_3d,m_corners_2d,camera_matrix,dist_coeffs,rvec,tvec);
            Rodrigues(rvec, rmat);
            for(int nr = 0;nr < 3; nr++)
            {
                double *datar = rmat.ptr<double>(nr);
                double *datat = transMatrix.ptr<double>(nr);
                for(int nc = 0;nc < 3; nc++)
                {
                    datat[nc]=datar[nc];
                }
            }
            transMatrix.at<double>(0,3) = tvec.at<double>(0,0);
            transMatrix.at<double>(1,3) = tvec.at<double>(1,0);
            transMatrix.at<double>(2,3) = tvec.at<double>(2,0);
            transMatrix.at<double>(3,3) = 1.f;
            coorMatrix = transMatrix.inv();


//            std::cout << "roation" << coorMatrix << std::endl;

        }
//        t = ((double)getTickCount()-t)/getTickFrequency();
//        std::cout << t <<std::endl;
//        imshow("haha",gray);
//        if (waitKey(30) >= 0) break;
    }
}
