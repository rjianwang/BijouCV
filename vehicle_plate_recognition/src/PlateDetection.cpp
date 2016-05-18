#include <time.h>

#include "PlateDetection.h"
#include "Plate.h"

PlateDetection::PlateDetection()
{
    DEBUG = false;
    saveRecognition = false;
}


PlateDetection::~PlateDetection()
{
}

cv::Mat PlateDetection::histeq(cv::Mat img)
{
	cv::Mat imt(img.size(), img.type());
	// ������ͼ��Ϊ��ɫ����Ҫ��HSV�ռ�����ֱ��ͼ���⴦��
	// ��ת����RGB��ʽ
	if (img.channels() == 3)
	{
		cv::Mat hsv;
		std::vector<cv::Mat> hsvSplit;
		cv::cvtColor(img, hsv, CV_BGR2HSV);
		cv::split(hsv, hsvSplit);
		cv::equalizeHist(hsvSplit[2], hsvSplit[2]);
		cv::merge(hsvSplit, hsv);
		cv::cvtColor(hsv, imt, CV_HSV2BGR);
	}
	// ������ͼ��Ϊ�Ҷ�ͼ��ֱ����ֱ��ͼ���⴦��
	else if (img.channels() == 1){
		equalizeHist(img, imt);
	}

	return imt;
}

bool PlateDetection::verifySizes(cv::RotatedRect ROI)
{
	// �������ó���Ĭ�ϲ���������ʶ������������Ƿ�ΪĿ�공��
	float error = 0.4;
	// ���������ƿ�߱�: 520 / 110 = 4.7272
	float aspect = 4.7272;
	// �趨�����������С/���ߴ磬���ڴ˷�Χ�ڵĲ�����Ϊ����
	int min = 15 * aspect * 15;    // 15������
	int max = 125 * aspect * 125;  // 125������
	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;

	int area = ROI.size.height * ROI.size.width;
	float r = (float)ROI.size.width / (float)ROI.size.height;
	if (r<1)
		r = (float)ROI.size.height / (float)ROI.size.width;

	// �ж��Ƿ�������ϲ���
	if ((area < min || area > max) || (r < rmin || r > rmax))
		return false;

	return true;
}

std::vector<Plate> PlateDetection::segment(cv::Mat img)
{
	std::vector<Plate> plates;

	// ͼ��ת��Ϊ�Ҷ�ͼ
	cv::Mat gray;
	cv::cvtColor(img, gray, CV_BGR2GRAY);

	// ��ֵ�˲���ȥ��
	cv::blur(gray, gray, cv::Size(5, 5));

	// Sobel���Ӽ���Ե
	cv::Mat sobel;
	Sobel(gray,			// ����ͼ��
		sobel,			// ���ͼ��
		CV_8U,			//���ͼ������
		1,				// x�����ϵĲ�ֽ���
		0,				// y�����ϵĲ�ֽ���
		3,				// ��չSobel�˵Ĵ�С��������1,3,5��7
		1,				// ���㵼��ֵʱ��ѡ���������ӣ�Ĭ��ֵ��1
		0,				// ��ʾ�ڽ������Ŀ��ͼ֮ǰ��ѡ��deltaֵ��Ĭ��ֵΪ0
		cv::BORDER_DEFAULT); // �߽�ģʽ��Ĭ��ֵΪBORDER_DEFAULT
	if (DEBUG)
		cv::imshow("Sobel", sobel);

	// ��ֵ�ָ�õ���ֵͼ�������õ���ֵ��Otsu�㷨�õ�
	cv::Mat threshold;
	cv::threshold(sobel, threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	if (DEBUG)
		cv::imshow("Threshold Image", threshold);


	// ʹ��morphologyEx�����õ��������Ƶ����򣨵����������ƺţ�
	// ����һ���ṹԪ��structuringElement��ά��Ϊ17 * 3
	cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3));
	cv::morphologyEx(threshold, threshold, CV_MOP_CLOSE, structuringElement);
	if (DEBUG)
		cv::imshow("Close", threshold);

	// �ҵ����ܵĳ��Ƶ�����
	std::vector<std::vector<cv::Point> > contours;
	findContours(threshold,
		contours, // �����������飬ÿһ��������һ��point���͵�vector��ʾ
		CV_RETR_EXTERNAL, // ��ʾֻ���������
		CV_CHAIN_APPROX_NONE); // �����Ľ��ư취������洢���е�������

	// ��ÿ������������ȡ��С������н��������
	std::vector<std::vector<cv::Point> >::iterator itc = contours.begin();
	std::vector<cv::RotatedRect> rects;
	// ��û�дﵽ�趨�Ŀ�߱�Ҫ����ȥ������
	while (itc != contours.end())
	{
		cv::RotatedRect ROI = cv::minAreaRect(cv::Mat(*itc));
		if (!verifySizes(ROI)){
			itc = contours.erase(itc);
		}
		else{
			++itc;
			rects.push_back(ROI);
		}
	}

	// �ڰ�ɫ��ͼ�ϻ�����ɫ������
	cv::Mat result;
	img.copyTo(result);
	cv::drawContours(result,
		contours,
		-1,				    // ���е�����������
		cv::Scalar(255, 0, 0), // ��ɫ
		1);		// �ߴ�

	// ʹ����ˮ����㷨�ü����ƻ�ȡ������������
	for (int i = 0; i< rects.size(); i++)
	{
		cv::circle(result, rects[i].center, 3, cv::Scalar(0, 255, 0), -1);
		// �õ���Ⱥ͸߶��н�С��ֵ���õ����Ƶ���С�ߴ�
		float minSize = (rects[i].size.width < rects[i].size.height) ? rects[i].size.width : rects[i].size.height;
		minSize = minSize - minSize * 0.5;
		// �ڿ����ĸ����������ɸ��������
		srand(time(NULL));
		// ��ʼ����ˮ����㷨�Ĳ���
		cv::Mat mask;
		mask.create(img.rows + 2, img.cols + 2, CV_8UC1);
		mask = cv::Scalar::all(0);
		// loDiff��ʾ��ǰ�۲�����ֵ���䲿����������ֵ���ߴ�����
		// �ò�������������֮������Ȼ���ɫ֮��������ֵ
		int loDiff = 30;
		// upDiff��ʾ��ǰ�۲�����ֵ���䲿����������ֵ���ߴ�����
		// �ò�������������֮������Ȼ���ɫ֮��������ֵ
		int upDiff = 30;
		int connectivity = 4; // ���ڿ����㷨����ͨ�ԣ���ȡ4����8
		int newMaskVal = 255;
		int NumSeeds = 10;
		cv::Rect ccomp;
		// ������־����Ϊ��������
		int flags = connectivity + // ���ڿ����㷨����ͨ�ԣ���ȡ4����8
			(newMaskVal << 8) +
			CV_FLOODFILL_FIXED_RANGE + // ���øñ�ʶ�����ῼ�ǵ�ǰ��������������֮��Ĳ�
			CV_FLOODFILL_MASK_ONLY; // ��������ȥ���ı�ԭʼͼ��, ����ȥ�����ģͼ��
		for (int j = 0; j < NumSeeds; j++){
			cv::Point seed;
			seed.x = rects[i].center.x + rand() % (int)minSize - (minSize / 2);
			seed.y = rects[i].center.y + rand() % (int)minSize - (minSize / 2);
			circle(result, seed, 1, cv::Scalar(0, 255, 255), -1);
			// ��������㷨������������
			int area = floodFill(img,
				mask,
				seed,
				cv::Scalar(255, 0, 0),
				&ccomp,
				cv::Scalar(loDiff, loDiff, loDiff),
				cv::Scalar(upDiff, upDiff, upDiff),
				flags);
		}
		if (DEBUG)
			cv::imshow("MASK", mask);

		// �õ��ü�����󣬼������Ч�ߴ�
		// ����ÿ������İ�ɫ���أ��ȵõ���λ��
		// ��ʹ��minAreaRect������ȡ��ӽ��Ĳü�����
		std::vector<cv::Point> pointsInterest;
		cv::Mat_<uchar>::iterator itMask = mask.begin<uchar>();
		cv::Mat_<uchar>::iterator end = mask.end<uchar>();
		for (; itMask != end; ++itMask)
		if (*itMask == 255)
			pointsInterest.push_back(itMask.pos());

		cv::RotatedRect minRect = cv::minAreaRect(pointsInterest);

		if (verifySizes(minRect)){
			// ��ת����ͼ
			cv::Point2f rect_points[4]; minRect.points(rect_points);
			for (int j = 0; j < 4; j++)
				line(result, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 0, 255), 1, 8);

			// �õ���תͼ������ľ���
			float r = (float)minRect.size.width / (float)minRect.size.height;
			float angle = minRect.angle;
			if (r<1)
				angle = 90 + angle;
			cv::Mat rotmat = cv::getRotationMatrix2D(minRect.center, angle, 1);

			// ͨ������任��ת�����ͼ��
			cv::Mat img_rotated;
			cv::warpAffine(img, img_rotated, rotmat, img.size(), CV_INTER_CUBIC);

			// ���ü�ͼ��
			cv::Size rect_size = minRect.size;
			if (r < 1)
				std::swap(rect_size.width, rect_size.height);
			cv::Mat img_crop;
			cv::getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);

			cv::Mat resultResized;
			resultResized.create(33, 144, CV_8UC3);
			resize(img_crop, resultResized, resultResized.size(), 0, 0, cv::INTER_CUBIC);
			// Ϊ����������Ӱ�죬�Բü�ͼ��ʹ��ֱ��ͼ���⻯����
			cv::Mat grayResult;
			cv::cvtColor(resultResized, grayResult, CV_BGR2GRAY);
			cv::blur(grayResult, grayResult, cv::Size(3, 3));
			grayResult = histeq(grayResult);
			if (saveRecognition){
				std::stringstream ss(std::stringstream::in | std::stringstream::out);
				ss << "tmp/" << filename << "_" << i << ".jpg";
				imwrite(ss.str(), grayResult);
			}
			plates.push_back(Plate(grayResult, minRect.boundingRect()));
		}
	}

	if (DEBUG)
		cv::imshow("Contours", result);

	return plates;
}
