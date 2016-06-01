#include <string>
#include "OCR.h"
#include "ANNClassifier.h"
#include "Util.h"
#include "Resources.h"

CharSegment::CharSegment(){}
CharSegment::CharSegment(cv::Mat i, cv::Rect p)
{
	img = i;
	pos = p;
}

OCR::OCR()
{
	DEBUG = false;
	trained = false;
	saveSegments = false;
	charSize = 20;
}

OCR::~OCR()
{
}

cv::Mat OCR::preprocessChar(cv::Mat in){
	int h = in.rows;
	int w = in.cols;
    cv::Mat transformMat = cv::Mat::eye(2, 3, CV_32F);
	int m = std::max(w, h);
    transformMat.at<float>(0, 2) = m / 2 - w / 2;
    transformMat.at<float>(1, 2) = m / 2 - h / 2;

    cv::Mat warpImage(m, m, in.type());
    cv::warpAffine(in, warpImage, transformMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));

    cv::Mat out;
    cv::resize(warpImage, out, cv::Size(charSize, charSize));

	return out;
}

bool OCR::verifySizes(cv::Mat r){
	// ��ȷ�ĳ����ַ���߱�Ϊ45/77
	float aspect = 45.0f / 77.0f;
	float charAspect = (float)r.cols / (float)r.rows;
	float error = 0.35;  // �������ﵽ35%
	float minHeight = 15;
	float maxHeight = 28;
	// ��С����
	float minAspect = 0.2;
	float maxAspect = aspect + aspect * error;
	// ��������
	float area = countNonZero(r);
	// bb����
	float bbArea = r.cols * r.rows;
	// ����ռ����İٷֱ�
	float percPixels = area / bbArea;

	// ��һ������ı��ʳ�����׼���ʵ�80%������Ϊ������Ϊ��ɫ�죬������һ���ַ�
	if (DEBUG)
		std::cout << "Aspect: " << aspect << " [" << minAspect << "," << maxAspect << "] " << "Area " << percPixels << " Char aspect " << charAspect << " Height char " << r.rows << "\n";
	if (percPixels < 0.8 && charAspect > minAspect && charAspect < maxAspect && r.rows >= minHeight && r.rows < maxHeight)
		return true;
	else
		return false;
}

// ��ֵ�ָ�
std::vector<CharSegment> OCR::segment(Plate plate){
    if (DEBUG)
    {
        std::cout << "Segmenting..." << std::endl;
        cv::imshow("Plate Image", plate.image);
    }

    cv::Mat input = plate.image;
    std::vector<CharSegment> output;
    
    // ͼ��תΪ�Ҷ�ͼ
    if (input.channels() == 3)
        cv::cvtColor(input, input, CV_BGR2GRAY);

    cv::Mat threshold;
    cv::threshold(input, threshold, 185, 255, CV_THRESH_BINARY);
	if (DEBUG)
		cv::imshow("Threshold plate", threshold);
    cv::Mat img_contours;
	threshold.copyTo(img_contours);
	// �ҵ����ܵĳ��Ƶ�����
    std::vector< std::vector<cv::Point> > contours;
    cv::findContours(img_contours,
		contours, // �����������飬ÿһ��������һ��point���͵�vector��ʾ
		CV_RETR_EXTERNAL, // ��ʾֻ���������
		CV_CHAIN_APPROX_NONE); // �����Ľ��ư취������洢���е�������

	// �ڰ�ɫ��ͼ�ϻ�����ɫ������
	cv::Mat result;

	threshold.copyTo(result);
    cv::cvtColor(result, result, CV_GRAY2RGB);
	cv::drawContours(result, contours,
		-1,  // ���е�����������
		cv::Scalar(255, 0, 0), // ��ɫ
		1); // �ߴ�

	// ��ÿ������������ȡ��С������н��������
    std::vector<std::vector<cv::Point> >::iterator itc = contours.begin();

	char res[20];
	int i = 0;
	// ��û�дﵽ�趨�Ŀ�߱�Ҫ����ȥ������
	while (itc != contours.end()) 
	{
        cv::Rect mr = cv::boundingRect(cv::Mat(*itc));
        cv::rectangle(result, mr, cv::Scalar(0, 255, 0));
		// �ü�ͼ��
        cv::Mat auxRoi(threshold, mr);
		if (verifySizes(auxRoi)){
			auxRoi = preprocessChar(auxRoi);
			output.push_back(CharSegment(auxRoi, mr));
			//����ÿ���ַ�ͼƬ  
			sprintf(res, "PlateNumber%d.jpg", i);
			i++;
            cv::imwrite(res, auxRoi);
            cv::rectangle(result, mr, cv::Scalar(0, 125, 255));
		}
		++itc;
	}
	if (DEBUG)
		std::cout << "Num chars: " << output.size() << "\n";

	if (DEBUG)
		cv::imshow("Segmented Chars", result);
    
    // ��x��������
    Util::qsort(output, 0, output.size() - 1);
	return output;
}

cv::Mat OCR::ProjectedHistogram(cv::Mat img, int t)
{
	int sz = (t) ? img.rows : img.cols;
    cv::Mat mhist = cv::Mat::zeros(1, sz, CV_32F);

	for (int j = 0; j<sz; j++){
        cv::Mat data = (t) ? img.row(j) : img.col(j);
		mhist.at<float>(j) = cv::countNonZero(data);
	}

	// ��һ��ֱ��ͼ
	double min, max;
    cv::minMaxLoc(mhist, &min, &max);

	if (max>0)
		mhist.convertTo(mhist, -1, 1.0f / max, 0);

	return mhist;
}

cv::Mat OCR::getVisualHistogram(cv::Mat *hist, int type)
{

	int size = 100;
    cv::Mat imHist;

	if (type == HORIZONTAL){
		imHist.create(cv::Size(size, hist->cols), CV_8UC3);
	}
	else{
		imHist.create(cv::Size(hist->cols, size), CV_8UC3);
	}

	imHist = cv::Scalar(55, 55, 55);

	for (int i = 0; i<hist->cols; i++){
		float value = hist->at<float>(i);
		int maxval = (int)(value*size);

        cv::Point pt1;
        cv::Point pt2, pt3, pt4;

		if (type == HORIZONTAL)
		{
			pt1.x = pt3.x = 0;
			pt2.x = pt4.x = maxval;
			pt1.y = pt2.y = i;
			pt3.y = pt4.y = i + 1;

            cv::line(imHist, pt1, pt2, CV_RGB(220, 220, 220), 1, 8, 0);
            cv::line(imHist, pt3, pt4, CV_RGB(34, 34, 34), 1, 8, 0);

			pt3.y = pt4.y = i + 2;
            cv::line(imHist, pt3, pt4, CV_RGB(44, 44, 44), 1, 8, 0);
			pt3.y = pt4.y = i + 3;
            cv::line(imHist, pt3, pt4, CV_RGB(50, 50, 50), 1, 8, 0);
		}
		else
		{
			pt1.x = pt2.x = i;
			pt3.x = pt4.x = i + 1;
			pt1.y = pt3.y = 100;
			pt2.y = pt4.y = 100 - maxval;

            cv::line(imHist, pt1, pt2, CV_RGB(220, 220, 220), 1, 8, 0);
            cv::line(imHist, pt3, pt4, CV_RGB(34, 34, 34), 1, 8, 0);

			pt3.x = pt4.x = i + 2;
            cv::line(imHist, pt3, pt4, CV_RGB(44, 44, 44), 1, 8, 0);
			pt3.x = pt4.x = i + 3;
            cv::line(imHist, pt3, pt4, CV_RGB(50, 50, 50), 1, 8, 0);
		}
	}
	return imHist;
}

void OCR::drawVisualFeatures(cv::Mat character, cv::Mat hhist, 
        cv::Mat vhist, cv::Mat lowData){
    cv::Mat img(121, 121, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat ch;
    cv::Mat ld;

    cv::cvtColor(character, ch, CV_GRAY2RGB);

    cv::resize(lowData, ld, cv::Size(100, 100), 0, 0, cv::INTER_NEAREST);
    cv::cvtColor(ld, ld, CV_GRAY2RGB);

    cv::Mat hh = getVisualHistogram(&hhist, HORIZONTAL);
    cv::Mat hv = getVisualHistogram(&vhist, VERTICAL);

    cv::Mat subImg = img(cv::Rect(0, 101, 20, 20));
	ch.copyTo(subImg);

	subImg = img(cv::Rect(21, 101, 100, 20));
	hh.copyTo(subImg);

	subImg = img(cv::Rect(0, 0, 20, 100));
	hv.copyTo(subImg);

	subImg = img(cv::Rect(21, 0, 100, 100));
	ld.copyTo(subImg);

    cv::line(img, cv::Point(0, 100), cv::Point(121, 100), 
            cv::Scalar(0, 0, 255));
    cv::line(img, cv::Point(20, 0), cv::Point(20, 121), 
            cv::Scalar(0, 0, 255));

    cv::imshow("Visual Features", img);

    cv::waitKey(0);
}

// ������ȡ
cv::Mat OCR::features(cv::Mat in, int sizeData){
	//Histogram features
    cv::Mat vhist = ProjectedHistogram(in, VERTICAL);
    cv::Mat hhist = ProjectedHistogram(in, HORIZONTAL);

    cv::Mat lowData;
    cv::resize(in, lowData, cv::Size(sizeData, sizeData));

	if (DEBUG)
		drawVisualFeatures(in, hhist, vhist, lowData);

	int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

    cv::Mat out = cv::Mat::zeros(1, numCols, CV_32F);

	int j = 0;
	for (int i = 0; i < vhist.cols; i++)
	{
		out.at<float>(j) = vhist.at<float>(i);
		j++;
	}
	for (int i = 0; i < hhist.cols; i++)
	{
		out.at<float>(j) = hhist.at<float>(i);
		j++;
	}
	for (int x = 0; x < lowData.cols; x++)
	{
		for (int y = 0; y < lowData.rows; y++){
			out.at<float>(j) = (float)lowData.at<unsigned char>(x, y);
			j++;
		}
	}
	if (DEBUG)
		std::cout << out << "\n===========================================\n";
	return out;
}

void OCR::process_chars(Plate *input, const std::vector<CharSegment> segments)
{
    // ANN Classifier
    ANNClassifier *annClassifier = new ANNClassifier();

    annClassifier->load_xml("../OCR.xml");
    //annClassifier->load_data("../data/chars/");
    annClassifier->DEBUG = this->DEBUG;
    annClassifier->train(Resources::numSPCharacters);

	for (int i = 1; i < segments.size(); i++){
		// ��ÿ���ַ�����Ԥ����ʹ�ö�����ͼ�������ͬ�Ĵ�С 
        cv::Mat ch = preprocessChar(segments[i].img);
		if (saveSegments){
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << "tmpChars/" << filename << "_" << i << ".jpg";
            cv::imwrite(ss.str(), ch);
		}
		// Ϊÿ���ֶ���ȡ����
        cv::Mat f = features(ch, 15);
		// ����ÿ�����ֽ��з���
        /*ch.convertTo(ch, CV_32FC1);
        cv::Mat resized;
        resized.create(12, 24, CV_32FC1);
        cv::resize(ch, resized, resized.size(), 0, 0, cv::INTER_CUBIC);
        resized = resized.reshape(1, 1);*/
		int character = annClassifier->predict(f);
		input->chars.push_back(std::string(1, Resources::sp_chars[character]));
		input->charsPos.push_back(segments[i].pos);
	}

    delete annClassifier;
}

void OCR::process_cn_chars(Plate *input, const std::vector<CharSegment> segments)
{
    std::vector<std::string> labels;
    labels = Util::getFiles("../data/cn_chars/");

    // ANN Classifier
    ANNClassifier *annClassifier = new ANNClassifier();
    annClassifier->DEBUG = this->DEBUG;
    annClassifier->load_cn_data("../data/cn_chars/"); 
    annClassifier->train(Resources::numCNCharacters);

    // ���ַ�����Ԥ����
    cv::Mat ch = preprocessChar(segments[0].img);
    if (saveSegments){
        std::stringstream ss(std::stringstream::in | std::stringstream::out);
        ss << "tmpChars/" << filename << "_" << 0 << ".jpg";
        cv::imwrite(ss.str(), ch);
    }
    // Ϊÿ���ֶ���ȡ����
    // cv::Mat f = features(ch, 15);
    // ����ÿ�����ֽ��з���
    ch.convertTo(ch, CV_32FC1);
    ch = ch.reshape(1, 1);
    int character = annClassifier->predict(ch);
    std::cout << Resources::cn_chars[character] << std::endl;
    input->chars.push_back(Resources::cn_chars[character]);
    input->charsPos.push_back(segments[0].pos);

    delete annClassifier;
}

std::string OCR::ocr(Plate *input)
{
    if (DEBUG)
        std::cout << "Char regcognition..." << std::endl;
    
	// �ַ��ָ�
    std::vector<CharSegment> segments = segment(*input);
    // ѵ�����ķ���������ʶ��
    process_cn_chars(input, segments);
    // ѵ���ַ�����������ʶ��
    process_chars(input, segments);

	return "-";//input->str();
}
