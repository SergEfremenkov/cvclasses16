﻿///@File: LaplacianEdgeDetector.cpp
///@Brief: Contains implementation of LaplacianEdgeDetector class
///@Author: Roman Golovanov
///@Date: 08 September 2015

#include "stdafx.h"

#include <bitset>

#include "LaplacianEdgeDetector.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

///////////////////////////////////////////////////////////////////////////////
static const std::string sc_windowName = LaplacianEdgeDetector::ReplyName();

///////////////////////////////////////////////////////////////////////////////
void LaplacianEdgeDetector::Show()
{
	/// Load an image
	cv::VideoCapture capture(0);
	if (!capture.isOpened())
	{
		std::cerr << "Can not open the camera !" << std::endl;
		return;
	}
	m_context.videoCapture = &capture;

	/// Create window with original image
	cv::namedWindow(sc_windowName, CV_WINDOW_AUTOSIZE);

	/// Create DEMO window
	cv::namedWindow(sc_windowName, CV_WINDOW_AUTOSIZE);

	cv::createTrackbar("GKernel", sc_windowName, &m_context.gaussianKernelSize, 15);
	//cv::createTrackbar("GSigma", sc_windowName, &m_context.gaussianSigma, 15);

	cv::createTrackbar("LKernel", sc_windowName, &m_context.laplacianKernelSize, 15);
	//cv::createTrackbar("LScale", sc_windowName, &m_context.laplacianScale, 15);
	//cv::createTrackbar("LDelta", sc_windowName, &m_context.laplacianDelta, 15);

	cv::setMouseCallback(sc_windowName, onMouse, static_cast<void*>(&m_mouseLine));

	while (cv::waitKey(1) < 0)
	{
		processFrame();
	}
}

///////////////////////////////////////////////////////////////////////////////
void LaplacianEdgeDetector::processFrame() const
{
	cv::Mat srcImage;
	(*m_context.videoCapture) >> srcImage;

	cv::Mat gray;
	cvtColor(srcImage, gray, CV_RGB2GRAY);

	std::bitset<2> errCode;
	errCode.set(0, m_context.gaussianKernelSize % 2 == 0);
	errCode.set(1, m_context.laplacianKernelSize % 2 == 0);

	if (errCode.any())
	{
		// skip invalid values
		std::string errorText = "Wrong parameters: ";
		errorText += errCode.all() ? "Gaussian and Laplacian" :
			         errCode[0] ? "Gaussian" : "Laplacian";
		errorText += " kernel size shall be odd";
		const auto txtSize = cv::getTextSize(errorText, CV_FONT_HERSHEY_PLAIN, 1.0, 1, nullptr);
		cv::putText(gray, errorText, { 10, 10 + txtSize.height }, CV_FONT_HERSHEY_PLAIN, 1.0, 255);
		cv::imshow(sc_windowName, gray);
		return;
	}
	
	/// Remove noise by blurring with a Gaussian filter
	cv::Mat bluredImg;
	cv::GaussianBlur(gray, bluredImg,
		cv::Size(m_context.gaussianKernelSize, m_context.gaussianKernelSize),
		m_context.gaussianSigma,
		m_context.gaussianSigma,
		cv::BORDER_DEFAULT);

	/// Apply Laplace function
	cv::Mat dst;
	cv::Laplacian(bluredImg, dst,
		CV_16S,
		m_context.laplacianKernelSize,
		m_context.laplacianScale,
		m_context.laplacianDelta,
		cv::BORDER_DEFAULT);

	cv::Mat abs_dst;
	convertScaleAbs(dst, abs_dst);

	if (m_mouseLine.first != m_mouseLine.second)
	{
		cv::arrowedLine(abs_dst, m_mouseLine.first, m_mouseLine.second, 255, 5);
	}

	cv::imshow(sc_windowName, abs_dst);
}

///////////////////////////////////////////////////////////////////////////////
// static
void LaplacianEdgeDetector::onMouse(int event, int x, int y, int flags, void* param)
{
	auto& line = *static_cast<decltype(LaplacianEdgeDetector::m_mouseLine)*>(param);
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
	case CV_EVENT_RBUTTONDOWN:
		line.first = { x, y };
		return;
	case CV_EVENT_LBUTTONUP:
	case CV_EVENT_RBUTTONUP:
		line = {};
		return;

	case CV_EVENT_MOUSEMOVE:
		if (line.first != line.second)
		{
			line.second = { x, y };
		}
		return;
	default:
		return;
	}
}