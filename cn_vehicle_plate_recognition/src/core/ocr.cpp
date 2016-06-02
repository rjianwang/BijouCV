#include <string>
#include "../../include/core/ocr.h"
#include "../../include/core/resource.h"
#include "../../include/core/plate.h"
#include "../../include/ml/ann.h"
#include "../../include/tool/tool.h"

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
	float maxHeight = 35;
	// ��С����
	float minAspect = 0.1;
	float maxAspect = aspect + aspect * error;
	// ��������
	float area = countNonZero(r);
	// bb����
	float bbArea = r.cols * r.rows;
	// ����ռ����İٷֱ�
	float percPixels = area / bbArea;

	// ��һ������ı��ʳ�����׼���ʵ�80%������Ϊ������Ϊ��ɫ�죬������һ���ַ�
	if (DEBUG)
		std::cout << "Aspect: " << aspect << " [" << minAspect << ", " << maxAspect << "] " << "Area " << percPixels << " Char aspect " << charAspect << " Height char " << r.rows << "\n";
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

    cv::Mat threshold;
    cv::threshold(input, threshold, 190, 255, CV_THRESH_BINARY);
    
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
		// �ü�ͼ��
        cv::Mat auxRoi(threshold, mr);
		if (verifySizes(auxRoi)){
            cv::rectangle(result, mr, cv::Scalar(0, 125, 255));
			auxRoi = preprocessChar(auxRoi);
			output.push_back(CharSegment(auxRoi, mr));
		}
		++itc;
	}
    // ��x��������
    Util::qsort(output, 0, output.size() - 1);
    cv::imshow("Segmented chars", result);
    return output;

    std::vector<CharSegment> ret(7);
    // ��ȡ�����ַ�
    int specIndex = getSpecificChar(plate, output);
    // ��ó������ַ�������������ַ�
    for (int i = specIndex, j = 1; i < output.size() && j <= 6; i++, j++)
        ret[j] = output[i];

    // ���������ַ����������ַ���λ�úʹ�С
    ret[0] = getChineseChar(threshold, ret[specIndex]);

    for (int i = 0; i < ret.size(); i++)
    {
        //����ÿ���ַ�ͼƬ  
        sprintf(res, "PlateNumber%d.jpg", i);
        cv::imwrite(res, ret[i].img);
        std::cout << ret[i].img.size() << std::endl;
        // ��ʾ�ַ�����
        cv::rectangle(result, ret[i].pos, cv::Scalar(0, 125, 255));
    }

	if (DEBUG)
    {
		std::cout << "Num chars: " << ret.size() << "\n";
        std::cout << "Spec index: " << specIndex << std::endl;
		cv::imshow("Segmented Chars", result);
    }
    
	return ret;
}

cv::Mat OCR::removeMD(cv::Mat img)
{
    if (DEBUG)
    {
        std::cout << "Remove mao ding..." << std::endl;
    }

    int line = 4;
    int threshold = 15;

    for (int i = 0; i < line; i++)
    {
        int whiteCount = 0;
        for (int j = 0; j < img.cols; j++)
        {
            if (img.at<char>(i, j) == 255)
                whiteCount++;
        }
        for (int j = 0; whiteCount < threshold && j < img.cols; j++)
        {
           img.at<char>(i, j) = 0;
        }
    }

    for (int i = img.rows - line; i < img.rows; i++)
    {
        int whiteCount = 0;
        for (int j = 0; j < img.cols; j++)
        {
            if (img.at<char>(i, j) == 255)
                whiteCount++;
        }
        for (int j = 0; whiteCount < threshold && j < img.cols; j++)
        {
           img.at<char>(i, j) = 0;
        }
    }
    return img;
}

int OCR::getSpecificChar(const Plate &plate, const std::vector<CharSegment> &input)
{
    int maxHeight = 0, maxWidth = 0;

    for (int i = 0; i < input.size(); i++)
    {
        if (input[i].pos.height > maxHeight)
            maxHeight = input[i].pos.height;
        if (input[i].pos.width > maxWidth)
            maxWidth = input[i].pos.width;
    }

    for (int i = 0; i < input.size(); i++)
    {
        cv::Rect mr = input[i].pos;
        int midx = mr.x + mr.width / 2;

        if ((mr.width > maxWidth * 0.8 || mr.height > maxHeight * 0.8)
                && (midx < int(plate.position.width / 7) * 2 - 5)
                && (midx > int(plate.position.width / 7) + 5))             // ��С������ң�������ϸ�
            return i;   // specific char
    }
}

CharSegment OCR::getChineseChar(const cv::Mat &img, const CharSegment &spec)
{
    CharSegment cn_char;

    cn_char.pos.height = spec.pos.height;
    cn_char.pos.width = spec.pos.width * 1.15f;
    cn_char.pos.y = spec.pos.y;

    int x = spec.pos.x - int(cn_char.pos.width * 1.15f);
    cn_char.pos.x = x > 0 ? x : 0;
    
    cv::Mat auxRoi(img, cn_char.pos);
    cn_char.img = auxRoi;

    return cn_char;
}

void OCR::process_chars(Plate *input, const std::vector<CharSegment> segments)
{
    // ANN Classifier for digits and letters
  //  ANNClassifier *annClassifier = new ANNClassifier(10, Resources::numSPCharacters);
    ANNClassifier *annClassifier = new ANNClassifier(100, Resources::numCharacters);

    //annClassifier->load_xml("../OCR.xml");
    annClassifier->load_data("../data/charSamples/");
    annClassifier->DEBUG = this->DEBUG;
    annClassifier->train();

	for (int i = 1; i < segments.size(); i++){
		// ��ÿ���ַ�����Ԥ����ʹ�ö�����ͼ�������ͬ�Ĵ�С 
        cv::Mat ch = preprocessChar(segments[i].img);
		if (saveSegments){
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
			ss << "tmpChars/" << filename << "_" << i << ".jpg";
            cv::imwrite(ss.str(), ch);
		}

		// ����ÿ�����ֽ��з���
        ch.convertTo(ch, CV_32FC1);
		int character = annClassifier->predict(ch);
		input->chars.push_back(std::string(1, Resources::sp_chars[character]));
		input->charsPos.push_back(segments[i].pos);
	}

    delete annClassifier;
}

void OCR::process_cn_chars(Plate *input, const std::vector<CharSegment> segments)
{
    // ANN Classifier for Chinese Characters
    ANNClassifier *annClassifier = new ANNClassifier(17, Resources::numCNCharacters);
    annClassifier->DEBUG = this->DEBUG;
    annClassifier->load_cn_data("../data/cn_chars/"); 
    annClassifier->train();

    // ���ַ�����Ԥ����
    cv::Mat ch = preprocessChar(segments[0].img);
    if (saveSegments){
        std::stringstream ss(std::stringstream::in | std::stringstream::out);
        ss << "tmpChars/" << filename << "_" << 0 << ".jpg";
        cv::imwrite(ss.str(), ch);
    }
    // ����ÿ�����ֽ��з���
    ch.convertTo(ch, CV_32FC1);
    int character = annClassifier->predict(ch);
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
