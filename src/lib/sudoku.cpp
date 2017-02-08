#include "sudoku.h"

int sudokuCols = 0;
int sudokuRows = 0;

int sudokuCol = 0;
int sudokuRow = 0;

float removeTinyVolumeBeforeExtractingPuzzle = 2.0 / 1000;
float removeTinyVolumeBeforeExtractingNumber = 2.0 / 1000;
float removeAreaBeforeExtractingNumber = 3.6 / 100;
float removeMinBoundingAreaBeforeExtractingNumber = 7.2 / 100;
float removeMaxBoundingAreaBeforeExtractingNumber = 7.2 / 100 * 5.52;

Mat preprocess(Mat input, bool tiny)
{
    Mat outerBox = Mat(input.size(), CV_8UC1);
    GaussianBlur(input, input, Size(11, 11), 0);
    adaptiveThreshold(input, outerBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
    if (tiny)
    {
        outerBox = removeTinyVolume(outerBox, input.cols * input.rows * removeTinyVolumeBeforeExtractingPuzzle, Scalar(255, 255, 255));
    }
    bitwise_not(outerBox, outerBox);
    dilate(outerBox, outerBox, Mat());
    return outerBox;
}

/**
* from time to time there are some tiny text around the puzzle and it kills the detection
* this function remove the tiny contour
*/
Mat removeTinyVolume(Mat input, int area, Scalar color)
{
    // we draw to the color of the background
    Mat output = input.clone();
    vector<vector<Point>> contours;
    findContours(input, contours, RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    // cout << "contours : " << contours.size() << endl;

    for (int i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) < area)
        {
            drawContours(output, contours, i, color, -1, 8);
        }
    }
    return output;
}

/**
* You will handle cells here a cell is a square that hold a number or not. A puzle has 81 cells
- you will first preprocess the cell using prepareCell
- then extract the number in a normalize size
- finally extract the feature
**/

Mat prepareCell(Mat cell)
{

    Mat output = cell.clone(), cell_no_noise, cell_no_light;
    // remove noise
    medianBlur(cell, cell_no_noise, 1);
    // remove background/light
    cell_no_light = removeLight(cell_no_noise, calculateLightPattern(cell), 2);
    // binarize image
    // adaptiveThreshold(cell_no_light, output, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
    // ADAPTIVE_THRESH_MEAN_C
    // ADAPTIVE_THRESH_GAUSSIAN_C

    // THRESH_BINARY
    // THRESH_BINARY_INV
    // THRESH_TRUNC
    // THRESH_TOZERO
    // THRESH_TOZERO_INV
    // THRESH_MASK
    // THRESH_OTSU
    // THRESH_TRIANGLE

    adaptiveThreshold(cell_no_light, output, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
    return output;
}

Mat removeLight(Mat img, Mat pattern, int method)
{
    Mat aux;
    // if method is normalization
    if (method == 1)
    {
        // Require change our image to 32 float for division
        Mat img32, pattern32;
        img.convertTo(img32, CV_32F);
        pattern.convertTo(pattern32, CV_32F);
        // Divide the image by the pattern
        aux = 1 - (img32 / pattern32);
        // Scale it to convert to 8bit format
        aux = aux * 255;
        // Convert 8 bits format
        aux.convertTo(aux, CV_8U);
    }
    else
    {
        aux = pattern - img;
    }
    return aux;
}

Mat normalizeSize(Mat in, int size)
{
    // int charSize = 20;
    //Remap image
    int h = in.rows;
    int w = in.cols;
    Mat transformMat = Mat::eye(2, 3, CV_32F);
    int m = max(w, h);
    transformMat.at<float>(0, 2) = m / 2 - w / 2;
    transformMat.at<float>(1, 2) = m / 2 - h / 2;

    Mat warpImage(m, m, in.type());
    warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));

    Mat out;
    resize(warpImage, out, Size(size, size));

    return out;
}

/**
* extract the number and normelize the size
* */
Mat extractNumber(Mat input)
{
    extractNumber(input, false);
}

Mat extractNumber(Mat input, bool debug)
{

    Mat cell = prepareCell(input);
    if (debug)
    {
        cout << "<<< extractNumber with:" << endl;
        cout << "showing_0_" << endl;
        showImage(cell);
    }

    int cell_height = cell.rows;
    int cell_width = cell.cols;

    // setting parameters for long lines filtering
    // that means if a line is longer than 77% of the cell it's droped
    float percent = 0.27;
    float width_threshold = cell_width - cell_width * percent;
    float height_threshold = cell_height - cell_height * percent;

    int area_threshold = cell.rows * cell.cols * removeAreaBeforeExtractingNumber;
    int min_boundingArea_threshold = cell.rows * cell.cols * removeMinBoundingAreaBeforeExtractingNumber;
    int max_boundingArea_threshold = cell.rows * cell.cols * removeMaxBoundingAreaBeforeExtractingNumber;
    // cout << max_boundingArea_threshold << endl;
    // Use connected components with stats
    Mat labels, stats, centroids;
    int num_objects = connectedComponentsWithStats(cell, labels, stats, centroids);
    // Check the number of objects detected
    Mat output;

    if (num_objects < 2)
    {
        if (debug)
        {
            cout << "num_objects < 2 --> No objects detected" << endl;
        }
        return output;
    }
    else
    {
        {
            if (debug)
            {
                cout << "Number of objects detected: " << num_objects - 1 << endl;
            }
        }
    }
    Scalar white(255, 255, 255);

    int area, width, height, left, top;
    // Create output image coloring the objects and show area
    for (int i = 1; i < num_objects; i++)
    {
        area = stats.at<int>(i, CC_STAT_AREA);
        width = stats.at<int>(i, CC_STAT_WIDTH);
        height = stats.at<int>(i, CC_STAT_HEIGHT);
        left = stats.at<int>(i, CC_STAT_LEFT);
        top = stats.at<int>(i, CC_STAT_TOP);
        int boundingArea;

        if (debug)
        {
            cout << "area: " << area << endl;
            cout << "width: " << width << endl;
            cout << "height: " << height << endl;
            cout << "left: " << left << endl;
            cout << "top: " << top << endl;
        }

        // filtering
        boundingArea = width * height;
        if (width > width_threshold)
        {
            if (debug)
            {
                cout << "width_threshold: " << width << ">" << width_threshold << "--> DROP !!!" << endl;
            }
            continue; // drop !!! long horizontal line
        }
        if (height < 8 || height > height_threshold)
        {
            if (debug)
            {
                cout << "height_threshold: " << height << ">" << height_threshold << "--> DROP !!!" << endl;
            }
            continue; // drop !!! long vetical line
        }
        if (boundingArea < min_boundingArea_threshold || boundingArea > max_boundingArea_threshold)
        {
            if (debug)
            {
                cout << "boundingArea: "
                     << min_boundingArea_threshold << "<" << boundingArea << "<" << max_boundingArea_threshold << "not true --> DROP !!!" << endl;
            }

            continue;
        }
        if (area < area_threshold)
        {

            if (debug)
            {
                cout << "area_threshold: " << area << "<" << area_threshold << " --> DROP !!!" << endl;
            }
            continue; // area of the connected object
        }

        // should be the number here
        // Mat mask= labels==i;
        // cell.setTo(white, mask);
        Rect rect(left, top, width, height);
        return input(rect);
    }
    return output;
}

Mat calculateLightPattern(Mat img)
{
    Mat pattern;
    // Basic and effective way to calculate the light pattern from one image
    blur(img, pattern, Size(img.cols / 3, img.cols / 3));
    //    showImage(pattern);
    return pattern;
}

Mat ProjectedHistogram(Mat img, int t)
{
    int sz = (t) ? img.rows : img.cols;
    Mat mhist = Mat::zeros(1, sz, CV_32F);
    for (int j = 0; j < sz; j++)
    {
        Mat data = (t) ? img.row(j) : img.col(j);
        mhist.at<float>(j) = countNonZero(data);
    }
    //Normalize histogram
    double min, max;
    minMaxLoc(mhist, &min, &max);
    if (max > 0)
        mhist.convertTo(mhist, -1, 1.0f / max, 0);
    return mhist;
}

/**
* this function is used on a normalize number (after extractNumber)
* */
Mat features(Mat in, int sizeData)
{
    int HORIZONTAL = 1;
    int VERTICAL = 0;

    //Histogram features
    Mat vhist = ProjectedHistogram(in, VERTICAL);
    Mat hhist = ProjectedHistogram(in, HORIZONTAL);

    //Low data feature
    Mat lowData;
    resize(in, lowData, Size(sizeData, sizeData));

    //Last 10 is the number of moments components
    int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

    Mat out = Mat::zeros(1, numCols, CV_32F);
    //Asign values to feature
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
        for (int y = 0; y < lowData.rows; y++)
        {
            out.at<float>(j) = (float)lowData.at<unsigned char>(x, y);
            j++;
        }
    }
    return out;
}

Mat extractRoiFromCell(Mat sudoku, int k, bool debug)
{
    Mat rawCell, rawRoi, output, thresholded, cleaned;

    rawCell = extractCell(sudoku, k);
    rawRoi = extractNumber(rawCell, debug);

    if (debug)
    {
        cout << ">>> extractNumber with:" << endl;

        showImage(rawCell);
        showImage(rawRoi);
    }

    if (!rawRoi.empty())
    {
        // threshold(rawRoi, thresholded, 125, 255, rawRoi.type());
        adaptiveThreshold(rawRoi, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 1);
        // fin 8bits (CV_8U)
        // showImage(thresholded);

        // fin2 8bits
        vector<double> v = findBiggestComponent(thresholded);

        // double left = v[0];
        // double top = v[1];
        // double width = v[2];
        // double height = v[3];
        // double x = v[4];
        // double y = v[5];

        Rect rect(v[0], v[1], v[2], v[3]);
        return normalizeSize(thresholded(rect), normalizedSizeForCell);
    }
    return output;
}
Mat extractRoiFromCell(Mat sudoku, int k)
{
    return extractRoiFromCell(sudoku, k, false);
}

// ----------------------------------------------------------------------------------
// MNIST

int readFlippedInteger(FILE *fp)
{
    int ret = 0;

    unsigned char *temp;

    temp = (unsigned char *)(&ret);
    fread(&temp[3], sizeof(unsigned char), 1, fp);
    fread(&temp[2], sizeof(unsigned char), 1, fp);
    fread(&temp[1], sizeof(unsigned char), 1, fp);

    fread(&temp[0], sizeof(unsigned char), 1, fp);

    return ret;
}

// ------------------------------------------------------------------------
// PUZZLE

Mat extractCell(Mat sudoku, int numCell)
{
    return sudoku(Rect((numCell % 9) * sudokuCol, (numCell / 9) * sudokuRow, sudokuCol, sudokuRow));
}

// ------------------------------------------------------------------------
// PICTURE

// sort using a custom function object
struct str
{
    bool operator()(Point2f a, Point2f b)
    {
        return a.x <= b.x;
    }
} sort_xs;

// sort using a custom function object
struct str2
{
    bool operator()(Point2f a, Point2f b)
    {
        return a.y <= b.y;
    }
} sort_ys;

bool isContourValid(int contourAreaValue, vector<Point> approx)
{
    if (contourAreaValue > 10000 && isContourConvex(approx) && approx.size() == 4)
    {
        return true;
    }
    return false;
}

/*
* findBiggestBlob
*
* Find the biggest contour in the image
* note that it returns vector< vector<Point> > because it is more convenient to use drawContours after

!!!
Recursivity here is not a good idea as we are waiting for the biggestApprox relatively to the input
  so if we call findBiggestBlob again we will have the result to the first biggest approx found we will return it and apply it to the original
  which is not the good image
  !!!

* */
vector<Point> findBiggestBlob(Mat preprocessed, Mat original)
{
    int largest_area, contourAreaValue = 0;
    vector<vector<Point>> contours;
    vector<Point> contour;
    vector<Point> approx;
    vector<Point> biggestApprox;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // RETR_TREE

    for (int i = 0; i < contours.size(); i++)
    {
        contour = contours[i];
        contourAreaValue = std::fabs(contourArea(contour));
        // Approximate contour with accuracy proportional to the contour perimeter
        approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true) * 0.1, true);
        // Skip small or non-convex objects
        if (isContourValid(contourAreaValue, approx))
        {
            if (contourAreaValue > largest_area)
            {
                // showContour(preprocessed.clone(), contours[i]);
                largest_area = contourAreaValue;
                biggestApprox = approx;
            }
        }
    }
    return biggestApprox;
}

/**
Once the puzzle had been extracted, we wrote the solution on it
*/
Mat writeOnPuzzle(Mat puzzle, string initialState, string solution)
{
    // cout << puzzle.cols << endl;
    cv::Point center;
    cv::String sol(solution);
    cv::Scalar black(0, 0, 0);
    for (int k = 0; k < 81; k++)
    {
        // Mat roi = extractRoiFromCell(sudoku, k);
        if (initialState[k] == '0')
        {
            center.x = (k % 9) * sudokuCol + ((sudokuCol) / 4);
            center.y = (k / 9) * sudokuRow + ((sudokuRow * 6) / 7);
            cv::putText(puzzle,
                        sol.substr(k, 1),
                        center,                              // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL,      // Font
                        float((puzzle.cols / 1000 + 1) * 2), // Scale. 2.0 = 2x bigger
                        black,                               // Color
                        ((puzzle.cols / 1000 + 1) * 2));     // thickness
        }
    }
    return puzzle;
}

/**
    return a vector with
    0: tl
    1: tr1
    2: bl
    3: br
*/
std::vector<Point2f> getSudokuCoordinates(Mat input, vector<Point> biggestApprox)
{

    Point2f tl;
    Point2f tr;
    Point2f bl;
    Point2f br;

    std::vector<Point2f> points(4), tops(2), bottoms(2), src_p(4), dst_p(4);

    points[0] = biggestApprox[0];
    points[1] = biggestApprox[1];
    points[2] = biggestApprox[2];
    points[3] = biggestApprox[3];

    // extract top 2 top points
    std::sort(points.begin(), points.end(), sort_ys);
    tops[0] = points[0];
    tops[1] = points[1];

    // in top 2, take the one the "lefter"
    std::sort(tops.begin(), tops.end(), sort_xs);
    tl = tops[0];
    tr = tops[1];

    bottoms[0] = points[2];
    bottoms[1] = points[3];

    std::sort(bottoms.begin(), bottoms.end(), sort_xs);

    bl = bottoms[0];
    br = bottoms[1];

    // from points
    src_p[0] = tl;
    src_p[1] = tr;
    src_p[2] = br;
    src_p[3] = bl;

    return src_p;
}
/**
    Once we extract our puzzle from the original with the function extractPuzzle, the puzzle inside the extractedBlob could be more accuratly extracted 
    so we extract again until there is no more blob found
    This is true for s6 and s9 pictures
*/
Mat recursiveExtraction(Mat input)
{
    ExtractionInformation extractInfo;
    vector<Point> biggestApprox;
    Mat extractedPuzzle;

    Mat preprocessed = preprocess(input.clone(), false);

    biggestApprox = findBiggestBlob(preprocessed, input);
    if (!biggestApprox.empty())
    {
        extractInfo = extractPuzzle(input, biggestApprox);
        extractedPuzzle = extractInfo.image;
        return recursiveExtraction(extractedPuzzle);
    }
    else
    {
        sudokuCols = input.cols;
        sudokuRows = input.rows;
        sudokuCol = sudokuCols / 9;
        sudokuRow = sudokuRows / 9;
        return input;
    }
}

ExtractionInformation extractPuzzle(Mat input, vector<Point> biggestApprox)
{
    ExtractionInformation extractInfo;
    Mat outerBox = Mat(input.size(), CV_8UC1);
    Mat dst_img;

    std::vector<Point2f> coordinates(4), dst_p(4);

    coordinates = getSudokuCoordinates(input, biggestApprox);

    float w = (float)input.cols;
    float h = (float)input.rows;
    float hw = w / 2.0f;
    float hh = h / 2.0f;

    // to points
    dst_p[0] = Point2f(0.0f, 0.0f);
    dst_p[1] = Point2f(w, 0.0f);
    dst_p[2] = Point2f(w, h);
    dst_p[3] = Point2f(0.0f, h);

    // create perspective transform matrix
    Mat trans_mat33 = getPerspectiveTransform(coordinates, dst_p); //CV_64F->double

    // perspective transform operation using transform matrix
    warpPerspective(input, outerBox, trans_mat33, input.size(), INTER_LINEAR);

    extractInfo.transformation = trans_mat33;
    extractInfo.image = outerBox;
    return extractInfo;
}

// --------------------------------------------------------
// KNN

Ptr<ml::KNearest> getKnn(cv::FileStorage raw_features)
{
    int trainingNbr = nbrOfCells * 0.9;
    int testingNbr = nbrOfCells - trainingNbr;
    Mat features(nbrOfCells, normalizedSizeForCell * normalizedSizeForCell, CV_8UC1);
    Mat labels(1, nbrOfCells, CV_8UC1);
    Ptr<ml::KNearest> knn(ml::KNearest::create());

    if (raw_features.isOpened() == false)
    {
        throw std::logic_error("error, unable to open training classifications file, exiting program\n\n");
    }

    raw_features["features"] >> features;
    raw_features["labels"] >> labels;
    raw_features.release();

    Mat sub_features = features(cv::Range(0, trainingNbr), cv::Range::all());
    Mat sub_labels = labels(cv::Range::all(), cv::Range(0, trainingNbr));

    knn->train(sub_features, ml::ROW_SAMPLE, sub_labels);

    return knn;
}

Ptr<ml::SVM> getSvm(FileStorage raw_features)
{
    Mat features(nbrOfCells, normalizedSizeForCell * normalizedSizeForCell, CV_32F);
    Mat svm_labels(nbrOfCells, 1, CV_32S);
    raw_features["features"] >> features;
    raw_features["svm_labels"] >> svm_labels;
    raw_features.release();

    Ptr<ml::SVM> svm = ml::SVM::create();
    svm->setType(ml::SVM::C_SVC);
    svm->setKernel(ml::SVM::POLY);
    svm->setDegree(3); // I had to put it if not it fails
    svm->setGamma(3);

    Ptr<ml::TrainData> tData = ml::TrainData::create(features, ml::SampleTypes::ROW_SAMPLE, svm_labels);
    svm->train(tData);
    return svm;
}

void testKnn(Ptr<ml::KNearest> knn, cv::FileStorage raw_features)
{

    int trainingNbr = nbrOfCells * 0.9;
    int testingNbr = nbrOfCells - trainingNbr;
    Mat features(nbrOfCells, normalizedSizeForCell * normalizedSizeForCell, CV_8UC1);
    Mat labels(1, nbrOfCells, CV_8UC1);
    int totalCorrect = 0;

    if (raw_features.isOpened() == false)
    {
        throw std::logic_error("error, unable to open training classifications file, exiting program\n\n"); // if the file was not opened successfully
        // std::cout << "error, unable to open training classifications file, exiting program\n\n"; // show error message
        // return (0);                                                                              // and exit program
    }

    raw_features["features"] >> features;
    raw_features["labels"] >> labels;
    raw_features.release();
    // vector<Mat> v = readTestMNIST();
    // Mat testFeatures = v[0];
    // Mat expectedLabels = v[1];

    Mat sub_features = features(cv::Range(nbrOfCells - trainingNbr, nbrOfCells - 1), cv::Range::all());
    Mat sub_labels = labels(cv::Range::all(), cv::Range(nbrOfCells - trainingNbr, nbrOfCells - 1));

    int numImages = nbrOfCells - trainingNbr;

    int K = 1;
    Mat response, dist, m;

    for (int i = 0; i < testingNbr; i++)
    {
        m = sub_features.row(i);
        knn->findNearest(m, K, noArray(), response, dist);

        // cout << response << endl;
        // cout << sub_labels.at<float>(0, i) << endl;
        // showImage(m.reshape(1,28));

        if (sub_labels.at<float>(0, i) == response.at<float>(0))
            totalCorrect++;
    }

    printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
}

// void testKnn(Ptr<ml::KNearest> knn, bool debug)
// {
//     int totalCorrect = 0;

//     vector<Mat> v = readTestMNIST();
//     Mat testFeatures = v[0];
//     Mat expectedLabels = v[1];

//     int numImages = testFeatures.rows;

//     int K = 1;
//     Mat response, dist, m;

//     for (int i = 0; i < numImages; i++)
//     {

//         if (debug && i % 1000 == 0 && i != 0)
//         {
//             cout << i << endl;
//         }

//         m = testFeatures.row(i);

//         knn->findNearest(m, K, noArray(), response, dist);

//         if (debug)
//         {
//             cout << "response: " << response << endl;
//             cout << "dist: " << dist << endl;
//             Mat m2 = m.reshape(1, 28);
//             showImage(m2);
//             Mat m3 = m2.reshape(1, 1);
//             showImage(m3);
//         }

//         if (expectedLabels.at<int>(0, i) == response.at<float>(0))
//         {
//             totalCorrect++;
//         }
//     }

//     printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
// }

// --------------------------------------------------------------------------------
// debug

int minuss(int i, int j)
{
    return i - j;
}

Mat drawAllContour(Mat preprocessed)
{
    Mat output = Mat::zeros(preprocessed.rows, preprocessed.cols, preprocessed.type());
    Scalar white(255, 255, 255);
    vector<vector<Point>> contours;
    findContours(preprocessed, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    // cout << "contours : " << contours.size() << endl;

    for (int i = 0; i < contours.size(); i++)
    {
        drawContours(output, contours, i, white, 2, 8);
    }

    return output;
}

Mat drawAllContour(Mat preprocessed, vector<vector<Point>> contours)
{
    Scalar white(255, 255, 255);

    cout << "contours : " << contours.size() << endl;

    for (int i = 0; i < contours.size(); i++)
    {
        drawContours(preprocessed, contours, i, white, 2, 8);
    }

    if (preprocessed.cols > 1200)
    {
        resize(preprocessed, preprocessed, Size(), 0.4, 0.4);
    }

    return preprocessed;
}

vector<double> findBiggestComponent(Mat input)
{
    Mat output = Mat::zeros(input.rows, input.cols, input.type());

    Mat labels, stats, centroids;
    int num_objects = connectedComponentsWithStats(input, labels, stats, centroids);

    int area;
    int width;
    int height;
    int left;
    int top;
    // Check the number of objects detected
    if (num_objects < 2)
    {
        throw std::logic_error("No objects detected");
    }
    else
    {
        // cout << "Number of objects detected: " << num_objects - 1 << endl;
    }

    int biggestArea = 0;
    int index = 0;
    // Create output image coloring the objects and show area
    for (int i = 1; i < num_objects; i++)
    {
        area = stats.at<int>(i, CC_STAT_AREA);
        width = stats.at<int>(i, CC_STAT_WIDTH);
        height = stats.at<int>(i, CC_STAT_HEIGHT);
        left = stats.at<int>(i, CC_STAT_LEFT);
        top = stats.at<int>(i, CC_STAT_TOP);

        if (biggestArea < area)
        {
            biggestArea = area;
            index = i;
        }
    }

    // cout << "area: " << stats.at<int>(index, CC_STAT_AREA) << endl;
    area = stats.at<int>(index, CC_STAT_AREA);
    width = stats.at<int>(index, CC_STAT_WIDTH);
    height = stats.at<int>(index, CC_STAT_HEIGHT);
    left = stats.at<int>(index, CC_STAT_LEFT);
    top = stats.at<int>(index, CC_STAT_TOP);

    return {(double)left, (double)top, (double)width, (double)height, centroids.at<double>(index, 0), centroids.at<double>(index, 1)};
}

Mat drawAllApprox(Mat preprocessed)
{
    Mat output = Mat::zeros(preprocessed.rows, preprocessed.cols, preprocessed.type());

    Scalar white(255, 255, 255);
    vector<vector<Point>> contours;
    std::vector<Point> approx;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++)
    {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.1, true);

        // Skip small or non-convex objects
        if (std::fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
            continue;

        std::vector<vector<Point>> approx_contour(1, approx);
        drawContours(output, approx_contour, 0, white, 2, 8);
    }
    return output;
}
Mat drawAllApprox(Mat preprocessed, Mat origial)
{
    Mat output = origial.clone();

    Scalar white(255, 255, 255);
    vector<vector<Point>> contours;
    std::vector<Point> approx;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++)
    {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.1, true);

        // Skip small or non-convex objects
        if (std::fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
            continue;

        std::vector<vector<Point>> approx_contour(1, approx);
        drawContours(output, approx_contour, 0, white, 2, 8);
    }
    return output;
}

void drawMarkers(Mat input, vector<Point> biggestApprox)
{
    Scalar white(255, 255, 255);

    drawMarker(input, biggestApprox.at(0), white);
    drawMarker(input, biggestApprox.at(1), white);
    drawMarker(input, biggestApprox.at(2), white);
    drawMarker(input, biggestApprox.at(3), white);
}

Mat drawGrid(Mat input)
{
    Scalar white(255, 255, 255);

    Mat output = input.clone();
    int h = input.cols;
    int w = input.rows;
    int cw = w / 9;
    int ch = h / 9;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            //  drawMarker(input, Point(j * ch, i * cw), white);
            Rect rect = Rect(j * ch, i * cw, ch, cw);
            rectangle(output, rect, white);
        }
    }
    return output;
}
void showContour(Mat img, vector<Point> contour)
{
    cout << "contourArea : " << contourArea(contour) << endl;
    Scalar white(255, 255, 255);
    vector<vector<Point>> contours = {contour};
    drawContours(img, contours, 0, white, 2, 8);

    if (img.cols > 1200)
    {
        resize(img, img, Size(), 0.4, 0.4);
    }

    showImage(img);
}
void showImage(Mat img)
{

    if (img.cols > 1200)
    {
        resize(img, img, Size(), 0.4, 0.4);
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", img);
    waitKey(0);
}

string getMyProjectRoot(string path, string projectRootName)
{
    vector<string> strs(splitPath(path));

    string last = strs[strs.size() - 1];
    if (last == projectRootName)
    {
        return path;
    }
    else
    {
        strs.pop_back();

        if (strs.empty())
        {
            throw std::logic_error("could not find project root (in function getMyProjectRoot)");
        }
        return getMyProjectRoot(joinPath(strs), projectRootName);
    }
}

// string getPath(string p)
// {
//     stringstream ss;

//     ss << getMyProjectRoot(getexepath(), "sudoku-recognizer");
//     ss << "/";
//     ss << p;
//     return ss.str();
// }

Mat deskew(Mat t)
{
    Moments m = moments(t, true);
    double skew = m.mu11 / (int)m.mu02;
    Mat transform = Mat(2, 3, CV_32F);
    transform.at<float>(0, 0) = 1;
    transform.at<float>(0, 1) = skew;
    transform.at<float>(0, 2) = -0.5 * 28 * skew;
    transform.at<float>(1, 0) = 0;
    transform.at<float>(1, 1) = 1;
    transform.at<float>(1, 2) = 0;
    // cout << skew << endl;
    warpAffine(t, t, transform, t.size(), WARP_INVERSE_MAP);
    return t;
}

void showCells(Mat sudoku, bool debug)
{
    Mat roi;
    for (int k = 0; k < 81; k++)
    {
        roi = extractRoiFromCell(sudoku, k, debug);
        if (!roi.empty())
        {
            cout << k << endl;
            showImage(roi);
        }
    }
}

void showCells(Mat sudoku, int cellNum, bool debug)
{
    Mat roi;
    roi = extractRoiFromCell(sudoku, cellNum, debug);
    if (!roi.empty())
    {
        showImage(roi);
    }
}

// ---------------------------------------------------------------------
// mlp

template <typename T>
static Ptr<T> load_classifier(const string &persistence)
{
    // load classifier from the specified file
    Ptr<T> model = StatModel::load<T>(persistence);
    if (model.empty())
        cout << "Could not read the classifier " << persistence << endl;
    else
        cout << "The classifier " << persistence << " is loaded.\n";

    return model;
}

inline TermCriteria TC(int iters, double eps)
{
    return TermCriteria(TermCriteria::MAX_ITER + (eps > 0 ? TermCriteria::EPS : 0), iters, eps);
}

static void test_and_save_classifier(const Ptr<StatModel> &model,
                                     const Mat &data, const Mat &responses,
                                     int ntrain_samples, int rdelta,
                                     const string &filename_to_save)
{
    int i, nsamples_all = data.rows;
    double train_hr = 0, test_hr = 0;

    // compute prediction error on train and test data
    for (i = 0; i < nsamples_all; i++)
    {
        Mat sample = data.row(i);

        float r = model->predict(sample);
        r = std::abs(r + rdelta - responses.at<int>(i)) <= FLT_EPSILON ? 1.f : 0.f;

        if (i < ntrain_samples)
            train_hr += r;
        else
            test_hr += r;
    }

    test_hr /= nsamples_all - ntrain_samples;
    train_hr = ntrain_samples > 0 ? train_hr / ntrain_samples : 1.;

    printf("Recognition rate: train = %.1f%%, test = %.1f%%\n",
           train_hr * 100., test_hr * 100.);

    if (!filename_to_save.empty())
    {
        model->save(filename_to_save);
    }
}

// --------------------------------------------------------------------------
// createDataForTraining

// data created manually
std::map<int, std::map<int, int>> cellValues()
{
    std::map<int, std::map<int, int>> map;
    std::map<int, int> s0;

    std::map<int, int> s1;
    std::map<int, int> s2;

    std::map<int, int> s3;
    std::map<int, int> s4;
    std::map<int, int> s5;

    std::map<int, int> s6;
    std::map<int, int> s7;
    std::map<int, int> s8;

    std::map<int, int> s9;
    std::map<int, int> s10;
    std::map<int, int> s11;

    std::map<int, int> s12;
    std::map<int, int> s13;
    std::map<int, int> s14;

    std::map<int, int> s15;
    std::map<int, int> s16;
    std::map<int, int> s17;

    std::map<int, int> s18;
    std::map<int, int> s19;
    std::map<int, int> s20;

    std::map<int, int> s21;
    std::map<int, int> s22;
    std::map<int, int> s23;

    std::map<int, int> s24;
    std::map<int, int> s25;
    std::map<int, int> s26;

    std::map<int, int> s27;
    std::map<int, int> s28;
    std::map<int, int> s29;

    std::map<int, int> s30;
    std::map<int, int> s31;
    std::map<int, int> s32;

    std::map<int, int> s33;
    std::map<int, int> s34;
    std::map<int, int> s35;

    s0.insert(std::make_pair(3, 6));
    s0.insert(std::make_pair(5, 4));
    s0.insert(std::make_pair(6, 7));
    s0.insert(std::make_pair(9, 7));
    s0.insert(std::make_pair(11, 6));
    s0.insert(std::make_pair(17, 9));
    s0.insert(std::make_pair(23, 5));
    s0.insert(std::make_pair(25, 8));
    s0.insert(std::make_pair(28, 7));
    s0.insert(std::make_pair(31, 2));
    s0.insert(std::make_pair(34, 9));
    s0.insert(std::make_pair(35, 3));
    s0.insert(std::make_pair(36, 8));
    s0.insert(std::make_pair(44, 5));
    s0.insert(std::make_pair(45, 4));
    s0.insert(std::make_pair(46, 3));
    s0.insert(std::make_pair(49, 1));
    s0.insert(std::make_pair(52, 7));
    s0.insert(std::make_pair(55, 5));
    s0.insert(std::make_pair(57, 2));
    s0.insert(std::make_pair(63, 3));
    s0.insert(std::make_pair(69, 2));
    s0.insert(std::make_pair(71, 8));
    s0.insert(std::make_pair(74, 2));
    s0.insert(std::make_pair(75, 3));
    s0.insert(std::make_pair(77, 1));

    s1.insert(std::make_pair(2, 7));
    s1.insert(std::make_pair(3, 6));
    s1.insert(std::make_pair(4, 2));
    s1.insert(std::make_pair(7, 5));
    s1.insert(std::make_pair(8, 8));
    s1.insert(std::make_pair(10, 1));
    s1.insert(std::make_pair(11, 4));
    s1.insert(std::make_pair(13, 9));
    s1.insert(std::make_pair(17, 6));
    s1.insert(std::make_pair(20, 6));
    s1.insert(std::make_pair(23, 3));
    s1.insert(std::make_pair(26, 2));
    s1.insert(std::make_pair(29, 8));
    s1.insert(std::make_pair(30, 3));
    s1.insert(std::make_pair(33, 7));
    s1.insert(std::make_pair(35, 5));
    s1.insert(std::make_pair(37, 7));
    s1.insert(std::make_pair(43, 4));
    s1.insert(std::make_pair(45, 3));
    s1.insert(std::make_pair(47, 9));
    s1.insert(std::make_pair(50, 4));
    s1.insert(std::make_pair(51, 6));
    s1.insert(std::make_pair(54, 7));
    s1.insert(std::make_pair(57, 4));
    s1.insert(std::make_pair(60, 1));
    s1.insert(std::make_pair(63, 1));
    s1.insert(std::make_pair(67, 8));
    s1.insert(std::make_pair(69, 2));
    s1.insert(std::make_pair(70, 6));
    s1.insert(std::make_pair(72, 8));
    s1.insert(std::make_pair(73, 9));
    s1.insert(std::make_pair(76, 6));
    s1.insert(std::make_pair(77, 7));
    s1.insert(std::make_pair(78, 5));

    s2 = s1;

    s3.insert(std::make_pair(0, 6));
    s3.insert(std::make_pair(5, 4));
    s3.insert(std::make_pair(6, 1));
    s3.insert(std::make_pair(12, 9));
    s3.insert(std::make_pair(15, 5));
    s3.insert(std::make_pair(17, 8));
    s3.insert(std::make_pair(18, 5));
    s3.insert(std::make_pair(19, 8));
    s3.insert(std::make_pair(24, 7));
    s3.insert(std::make_pair(25, 4));
    s3.insert(std::make_pair(28, 9));
    s3.insert(std::make_pair(30, 7));
    s3.insert(std::make_pair(31, 1));
    s3.insert(std::make_pair(33, 8));
    s3.insert(std::make_pair(40, 4));
    s3.insert(std::make_pair(47, 5));
    s3.insert(std::make_pair(49, 9));
    s3.insert(std::make_pair(50, 8));
    s3.insert(std::make_pair(52, 3));
    s3.insert(std::make_pair(55, 7));
    s3.insert(std::make_pair(56, 8));
    s3.insert(std::make_pair(61, 2));
    s3.insert(std::make_pair(62, 1));
    s3.insert(std::make_pair(63, 4));
    s3.insert(std::make_pair(65, 2));
    s3.insert(std::make_pair(68, 7));
    s3.insert(std::make_pair(74, 6));
    s3.insert(std::make_pair(75, 8));
    s3.insert(std::make_pair(80, 5));

    s4 = s3;
    s5 = s3;

    s6.insert(std::make_pair(2, 6));
    s6.insert(std::make_pair(3, 4));
    s6.insert(std::make_pair(6, 7));
    s6.insert(std::make_pair(7, 5));
    s6.insert(std::make_pair(11, 5));
    s6.insert(std::make_pair(13, 8));
    s6.insert(std::make_pair(14, 2));
    s6.insert(std::make_pair(16, 6));
    s6.insert(std::make_pair(20, 7));
    s6.insert(std::make_pair(21, 3));
    s6.insert(std::make_pair(23, 6));
    s6.insert(std::make_pair(25, 8));
    s6.insert(std::make_pair(26, 9));
    s6.insert(std::make_pair(28, 5));
    s6.insert(std::make_pair(30, 1));
    s6.insert(std::make_pair(31, 3));
    s6.insert(std::make_pair(33, 9));
    s6.insert(std::make_pair(37, 9));
    s6.insert(std::make_pair(38, 3));
    s6.insert(std::make_pair(42, 8));
    s6.insert(std::make_pair(43, 4));
    s6.insert(std::make_pair(47, 2));
    s6.insert(std::make_pair(49, 4));
    s6.insert(std::make_pair(50, 8));
    s6.insert(std::make_pair(52, 7));
    s6.insert(std::make_pair(54, 5));
    s6.insert(std::make_pair(55, 8));
    s6.insert(std::make_pair(57, 2));
    s6.insert(std::make_pair(59, 9));
    s6.insert(std::make_pair(60, 6));
    s6.insert(std::make_pair(64, 7));
    s6.insert(std::make_pair(66, 8));
    s6.insert(std::make_pair(67, 6));
    s6.insert(std::make_pair(69, 2));
    s6.insert(std::make_pair(73, 2));
    s6.insert(std::make_pair(74, 9));
    s6.insert(std::make_pair(77, 3));
    s6.insert(std::make_pair(78, 4));

    s7 = s6;
    s8 = s6;

    s9.insert(std::make_pair(1, 8));
    s9.insert(std::make_pair(4, 1));
    s9.insert(std::make_pair(7, 4));
    s9.insert(std::make_pair(11, 2));
    s9.insert(std::make_pair(13, 8));
    s9.insert(std::make_pair(14, 3));
    s9.insert(std::make_pair(17, 1));
    s9.insert(std::make_pair(19, 6));
    s9.insert(std::make_pair(21, 2));
    s9.insert(std::make_pair(26, 3));
    s9.insert(std::make_pair(31, 9));
    s9.insert(std::make_pair(32, 4));
    s9.insert(std::make_pair(33, 3));
    s9.insert(std::make_pair(34, 5));
    s9.insert(std::make_pair(36, 8));
    s9.insert(std::make_pair(38, 9));
    s9.insert(std::make_pair(42, 7));
    s9.insert(std::make_pair(44, 4));
    s9.insert(std::make_pair(46, 1));
    s9.insert(std::make_pair(47, 5));
    s9.insert(std::make_pair(48, 3));
    s9.insert(std::make_pair(49, 7));
    s9.insert(std::make_pair(54, 3));
    s9.insert(std::make_pair(59, 2));
    s9.insert(std::make_pair(61, 7));
    s9.insert(std::make_pair(63, 2));
    s9.insert(std::make_pair(66, 7));
    s9.insert(std::make_pair(67, 3));
    s9.insert(std::make_pair(69, 6));
    s9.insert(std::make_pair(73, 9));
    s9.insert(std::make_pair(76, 6));
    s9.insert(std::make_pair(79, 3));

    s10 = s9;
    s11 = s9;

    s12.insert(std::make_pair(3, 6));
    s12.insert(std::make_pair(4, 5));
    s12.insert(std::make_pair(5, 2));
    s12.insert(std::make_pair(7, 9));
    s12.insert(std::make_pair(9, 6));
    s12.insert(std::make_pair(10, 4));
    s12.insert(std::make_pair(13, 7));
    s12.insert(std::make_pair(15, 5));
    s12.insert(std::make_pair(17, 1));
    s12.insert(std::make_pair(22, 3));
    s12.insert(std::make_pair(24, 7));
    s12.insert(std::make_pair(25, 8));
    s12.insert(std::make_pair(27, 9));
    s12.insert(std::make_pair(29, 8));
    s12.insert(std::make_pair(31, 4));
    s12.insert(std::make_pair(36, 1));
    s12.insert(std::make_pair(38, 4));
    s12.insert(std::make_pair(39, 2));
    s12.insert(std::make_pair(41, 7));
    s12.insert(std::make_pair(42, 9));
    s12.insert(std::make_pair(44, 8));
    s12.insert(std::make_pair(49, 9));
    s12.insert(std::make_pair(51, 6));
    s12.insert(std::make_pair(53, 5));
    s12.insert(std::make_pair(55, 3));
    s12.insert(std::make_pair(56, 9));
    s12.insert(std::make_pair(58, 2));
    s12.insert(std::make_pair(63, 8));
    s12.insert(std::make_pair(65, 6));
    s12.insert(std::make_pair(67, 1));
    s12.insert(std::make_pair(70, 5));
    s12.insert(std::make_pair(71, 9));
    s12.insert(std::make_pair(73, 1));
    s12.insert(std::make_pair(75, 9));
    s12.insert(std::make_pair(76, 8));
    s12.insert(std::make_pair(77, 4));

    s13 = s12;
    s14 = s12;

    s15.insert(std::make_pair(2, 9));
    s15.insert(std::make_pair(3, 1));
    s15.insert(std::make_pair(7, 5));
    s15.insert(std::make_pair(8, 6));
    s15.insert(std::make_pair(13, 6));
    s15.insert(std::make_pair(17, 3));
    s15.insert(std::make_pair(20, 5));
    s15.insert(std::make_pair(21, 3));
    s15.insert(std::make_pair(24, 9));
    s15.insert(std::make_pair(26, 4));
    s15.insert(std::make_pair(28, 3));
    s15.insert(std::make_pair(30, 7));
    s15.insert(std::make_pair(31, 5));
    s15.insert(std::make_pair(36, 5));
    s15.insert(std::make_pair(37, 2));
    s15.insert(std::make_pair(39, 8));
    s15.insert(std::make_pair(41, 6));
    s15.insert(std::make_pair(43, 9));
    s15.insert(std::make_pair(44, 7));
    s15.insert(std::make_pair(49, 1));
    s15.insert(std::make_pair(50, 2));
    s15.insert(std::make_pair(52, 8));
    s15.insert(std::make_pair(54, 6));
    s15.insert(std::make_pair(56, 1));
    s15.insert(std::make_pair(59, 3));
    s15.insert(std::make_pair(60, 5));
    s15.insert(std::make_pair(63, 8));
    s15.insert(std::make_pair(67, 7));
    s15.insert(std::make_pair(72, 7));
    s15.insert(std::make_pair(73, 4));
    s15.insert(std::make_pair(77, 8));
    s15.insert(std::make_pair(78, 6));

    s16 = s15;
    s17 = s15;

    s18.insert(std::make_pair(0, 1));
    s18.insert(std::make_pair(3, 9));
    s18.insert(std::make_pair(4, 7));
    s18.insert(std::make_pair(5, 4));
    s18.insert(std::make_pair(7, 8));
    s18.insert(std::make_pair(9, 9));
    s18.insert(std::make_pair(10, 7));
    s18.insert(std::make_pair(14, 6));
    s18.insert(std::make_pair(18, 3));
    s18.insert(std::make_pair(23, 5));
    s18.insert(std::make_pair(25, 2));
    s18.insert(std::make_pair(26, 7));
    s18.insert(std::make_pair(29, 7));
    s18.insert(std::make_pair(30, 6));
    s18.insert(std::make_pair(32, 2));
    s18.insert(std::make_pair(40, 5));
    s18.insert(std::make_pair(48, 8));
    s18.insert(std::make_pair(50, 3));
    s18.insert(std::make_pair(51, 1));
    s18.insert(std::make_pair(54, 6));
    s18.insert(std::make_pair(55, 9));
    s18.insert(std::make_pair(57, 5));
    s18.insert(std::make_pair(62, 8));
    s18.insert(std::make_pair(66, 2));
    s18.insert(std::make_pair(70, 9));
    s18.insert(std::make_pair(71, 5));
    s18.insert(std::make_pair(73, 2));
    s18.insert(std::make_pair(75, 4));
    s18.insert(std::make_pair(76, 9));
    s18.insert(std::make_pair(77, 8));
    s18.insert(std::make_pair(80, 6));

    s19 = s18;
    s20 = s18;

    s21.insert(std::make_pair(1, 2));
    s21.insert(std::make_pair(4, 9));
    s21.insert(std::make_pair(5, 3));
    s21.insert(std::make_pair(10, 7));
    s21.insert(std::make_pair(11, 9));
    s21.insert(std::make_pair(15, 8));
    s21.insert(std::make_pair(16, 4));
    s21.insert(std::make_pair(17, 1));
    s21.insert(std::make_pair(19, 5));
    s21.insert(std::make_pair(23, 1));
    s21.insert(std::make_pair(29, 3));
    s21.insert(std::make_pair(31, 5));
    s21.insert(std::make_pair(33, 4));
    s21.insert(std::make_pair(35, 8));
    s21.insert(std::make_pair(38, 5));
    s21.insert(std::make_pair(39, 8));
    s21.insert(std::make_pair(41, 6));
    s21.insert(std::make_pair(42, 3));
    s21.insert(std::make_pair(45, 1));
    s21.insert(std::make_pair(47, 8));
    s21.insert(std::make_pair(49, 3));
    s21.insert(std::make_pair(51, 5));
    s21.insert(std::make_pair(57, 1));
    s21.insert(std::make_pair(61, 8));
    s21.insert(std::make_pair(63, 5));
    s21.insert(std::make_pair(64, 1));
    s21.insert(std::make_pair(65, 4));
    s21.insert(std::make_pair(69, 9));
    s21.insert(std::make_pair(70, 2));
    s21.insert(std::make_pair(75, 3));
    s21.insert(std::make_pair(76, 4));
    s21.insert(std::make_pair(79, 5));

    s22 = s21;
    s23 = s21;

    s24.insert(std::make_pair(6, 8));
    s24.insert(std::make_pair(8, 5));
    s24.insert(std::make_pair(9, 9));
    s24.insert(std::make_pair(10, 6));
    s24.insert(std::make_pair(14, 5));
    s24.insert(std::make_pair(15, 7));
    s24.insert(std::make_pair(19, 3));
    s24.insert(std::make_pair(21, 7));
    s24.insert(std::make_pair(23, 9));
    s24.insert(std::make_pair(24, 4));
    s24.insert(std::make_pair(27, 4));
    s24.insert(std::make_pair(28, 1));
    s24.insert(std::make_pair(29, 2));
    s24.insert(std::make_pair(30, 6));
    s24.insert(std::make_pair(32, 3));
    s24.insert(std::make_pair(37, 5));
    s24.insert(std::make_pair(43, 4));
    s24.insert(std::make_pair(48, 4));
    s24.insert(std::make_pair(50, 1));
    s24.insert(std::make_pair(51, 3));
    s24.insert(std::make_pair(52, 7));
    s24.insert(std::make_pair(53, 2));
    s24.insert(std::make_pair(56, 1));
    s24.insert(std::make_pair(57, 9));
    s24.insert(std::make_pair(59, 7));
    s24.insert(std::make_pair(61, 6));
    s24.insert(std::make_pair(65, 6));
    s24.insert(std::make_pair(66, 8));
    s24.insert(std::make_pair(70, 5));
    s24.insert(std::make_pair(71, 7));
    s24.insert(std::make_pair(72, 2));
    s24.insert(std::make_pair(74, 8));

    s25 = s24;
    s26 = s24;

    s27.insert(std::make_pair(0, 9));
    s27.insert(std::make_pair(3, 6));
    s27.insert(std::make_pair(4, 8));
    s27.insert(std::make_pair(6, 7));
    s27.insert(std::make_pair(7, 1));
    s27.insert(std::make_pair(9, 6));
    s27.insert(std::make_pair(11, 7));
    s27.insert(std::make_pair(13, 4));
    s27.insert(std::make_pair(14, 9));
    s27.insert(std::make_pair(16, 2));
    s27.insert(std::make_pair(23, 3));
    s27.insert(std::make_pair(24, 9));
    s27.insert(std::make_pair(30, 3));
    s27.insert(std::make_pair(32, 4));
    s27.insert(std::make_pair(33, 5));
    s27.insert(std::make_pair(34, 6));
    s27.insert(std::make_pair(35, 8));
    s27.insert(std::make_pair(45, 3));
    s27.insert(std::make_pair(46, 1));
    s27.insert(std::make_pair(47, 5));
    s27.insert(std::make_pair(48, 8));
    s27.insert(std::make_pair(50, 6));
    s27.insert(std::make_pair(56, 9));
    s27.insert(std::make_pair(57, 4));
    s27.insert(std::make_pair(64, 3));
    s27.insert(std::make_pair(66, 7));
    s27.insert(std::make_pair(67, 9));
    s27.insert(std::make_pair(69, 2));
    s27.insert(std::make_pair(71, 1));
    s27.insert(std::make_pair(73, 7));
    s27.insert(std::make_pair(74, 8));
    s27.insert(std::make_pair(76, 6));
    s27.insert(std::make_pair(77, 5));
    s27.insert(std::make_pair(80, 4));

    s28 = s27;
    s29 = s27;

    s30.insert(std::make_pair(4, 8));
    s30.insert(std::make_pair(7, 5));
    s30.insert(std::make_pair(8, 7));
    s30.insert(std::make_pair(9, 7));
    s30.insert(std::make_pair(12, 9));
    s30.insert(std::make_pair(13, 1));
    s30.insert(std::make_pair(14, 5));
    s30.insert(std::make_pair(17, 3));
    s30.insert(std::make_pair(19, 3));
    s30.insert(std::make_pair(23, 7));
    s30.insert(std::make_pair(24, 9));
    s30.insert(std::make_pair(25, 1));
    s30.insert(std::make_pair(31, 7));
    s30.insert(std::make_pair(32, 2));
    s30.insert(std::make_pair(33, 6));
    s30.insert(std::make_pair(36, 3));
    s30.insert(std::make_pair(37, 1));
    s30.insert(std::make_pair(43, 2));
    s30.insert(std::make_pair(44, 5));
    s30.insert(std::make_pair(47, 6));
    s30.insert(std::make_pair(48, 4));
    s30.insert(std::make_pair(49, 5));
    s30.insert(std::make_pair(55, 9));
    s30.insert(std::make_pair(56, 5));
    s30.insert(std::make_pair(57, 1));
    s30.insert(std::make_pair(61, 7));
    s30.insert(std::make_pair(63, 6));
    s30.insert(std::make_pair(66, 5));
    s30.insert(std::make_pair(67, 3));
    s30.insert(std::make_pair(68, 9));
    s30.insert(std::make_pair(71, 4));
    s30.insert(std::make_pair(72, 1));
    s30.insert(std::make_pair(73, 8));
    s30.insert(std::make_pair(76, 6));

    s31 = s30;
    s32 = s30;

    s33.insert(std::make_pair(2, 8));
    s33.insert(std::make_pair(5, 7));
    s33.insert(std::make_pair(6, 2));
    s33.insert(std::make_pair(7, 9));
    s33.insert(std::make_pair(9, 4));
    s33.insert(std::make_pair(10, 3));
    s33.insert(std::make_pair(12, 1));
    s33.insert(std::make_pair(15, 6));
    s33.insert(std::make_pair(18, 6));
    s33.insert(std::make_pair(22, 8));
    s33.insert(std::make_pair(23, 3));
    s33.insert(std::make_pair(26, 5));
    s33.insert(std::make_pair(27, 3));
    s33.insert(std::make_pair(28, 2));
    s33.insert(std::make_pair(29, 4));
    s33.insert(std::make_pair(38, 1));
    s33.insert(std::make_pair(39, 4));
    s33.insert(std::make_pair(41, 6));
    s33.insert(std::make_pair(42, 3));
    s33.insert(std::make_pair(51, 4));
    s33.insert(std::make_pair(52, 7));
    s33.insert(std::make_pair(53, 8));
    s33.insert(std::make_pair(54, 8));
    s33.insert(std::make_pair(57, 9));
    s33.insert(std::make_pair(58, 2));
    s33.insert(std::make_pair(62, 6));
    s33.insert(std::make_pair(65, 6));
    s33.insert(std::make_pair(68, 5));
    s33.insert(std::make_pair(70, 1));
    s33.insert(std::make_pair(71, 9));
    s33.insert(std::make_pair(73, 7));
    s33.insert(std::make_pair(74, 3));
    s33.insert(std::make_pair(75, 8));
    s33.insert(std::make_pair(78, 5));

    s34 = s33;
    s35 = s33;

    map.insert(std::make_pair(0, s0));

    map.insert(std::make_pair(1, s1));
    map.insert(std::make_pair(2, s2));

    map.insert(std::make_pair(3, s3));
    map.insert(std::make_pair(4, s3));
    map.insert(std::make_pair(5, s3));

    map.insert(std::make_pair(6, s6));
    map.insert(std::make_pair(7, s6));
    map.insert(std::make_pair(8, s6));

    map.insert(std::make_pair(9, s9));
    map.insert(std::make_pair(10, s10));
    map.insert(std::make_pair(11, s11));

    map.insert(std::make_pair(12, s12));
    map.insert(std::make_pair(13, s13));
    map.insert(std::make_pair(14, s14));

    map.insert(std::make_pair(15, s15));
    map.insert(std::make_pair(16, s16));
    map.insert(std::make_pair(17, s17));

    map.insert(std::make_pair(18, s18));
    map.insert(std::make_pair(19, s19));
    map.insert(std::make_pair(20, s20));

    map.insert(std::make_pair(21, s21));
    map.insert(std::make_pair(22, s22));
    map.insert(std::make_pair(23, s23));

    map.insert(std::make_pair(24, s24));
    map.insert(std::make_pair(25, s25));
    map.insert(std::make_pair(26, s26));

    map.insert(std::make_pair(27, s27));
    map.insert(std::make_pair(28, s28));
    map.insert(std::make_pair(29, s29));

    map.insert(std::make_pair(30, s30));
    map.insert(std::make_pair(31, s31));
    map.insert(std::make_pair(32, s32));

    map.insert(std::make_pair(33, s33));
    map.insert(std::make_pair(34, s34));
    map.insert(std::make_pair(35, s35));

    return map;
}

// ---------------------------------------------------------------------------------------
// sudoku
void grabNumbers(Mat extractedPuzzle, Ptr<ml::SVM> svm, string *result)
{
    unsigned int n = std::thread::hardware_concurrency() - 1;
    if (n == 0)
    {
        n = 1;
    }
    // std::cout << n << " concurrent threads are supported.\n";

    vector<vector<int>> indexes(n);
    vector<std::thread> threads(n);

    for (int i = 0; i < 81; i++)
    {
        indexes[i % n].push_back(i);
    }

    // for(int i = 0 ; i < indexes[3].size(); i++){
    //     cout << indexes[3][i] << endl;
    // }

    _grabNumbers(extractedPuzzle, svm, result, indexes[0]);

    for (int k = 1; k < n; k++)
    {
        threads[k] = std::thread(_grabNumbers, extractedPuzzle, svm, result, indexes[k]);
    }

    for (int k = 1; k < n; k++)
    {
        threads[k].join();
    }

    // cout << ">>> grabNumbers" << endl;
}

void _grabNumbers(Mat extractedPuzzle, Ptr<ml::SVM> svm, string *result, vector<int> tab)
{
    Mat roi;
    stringstream ss;
    for (int k = 0; k < tab.size(); k++)
    {
        // cout << tab[k] << endl;

        _grabNumber(extractedPuzzle, tab[k], svm, result);
    }
}

void _grabNumber(Mat extractedPuzzle, int cellNumber, Ptr<ml::SVM> svm, string *pResult)
{
    stringstream ss;

    Mat roi = extractRoiFromCell(extractedPuzzle, cellNumber);
    if (!roi.empty())
    {
        roi.convertTo(roi, CV_32F);
        ss << svm->predict(roi.reshape(1, 1));
    }
    else
    {
        ss << "0";
    }
    *(pResult + cellNumber) = ss.str();
}

// string getexepath()
// {

//     char *path = NULL;
//     int length, dirname_length;
//     int i;

//     length = wai_getExecutablePath(NULL, 0, &dirname_length);

//     if (length > 0)
//     {
//         path = (char *)malloc(length + 1);
//         if (!path)
//             abort();
//         wai_getExecutablePath(path, length, &dirname_length);
//         path[length] = '\0';

//         // printf("executable path: %s\n", path);
//         path[dirname_length] = '\0';
//         // printf("  dirname: %s\n", path);
//         // printf("  basename: %s\n", path + dirname_length + 1);
//         return path;
//         // free(path); // memory leak ?
//     }

//     cout << length << endl;

//     // char result[PATH_MAX];
//     // ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
//     // return string(result, (count > 0) ? count : 0);
//     return "";
// }

/**
// Windows
#include <string>
#include <windows.h>

std::string getexepath()
{
  char result[ MAX_PATH ];
  return std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
}
// Windows - end
*/

vector<string> splitPath(string path)
{
    stringstream ss(path);
    vector<string> result;

    while (ss.good())
    {
        string substr;
        getline(ss, substr, '/');
        result.push_back(substr);
    }

    return result;
}

string joinPath(vector<string> strs)
{
    stringstream ss;

    for (int i = 0; i < strs.size(); i++)
    {
        ss << strs[i];
        if (i != strs.size() - 1)
        {
            ss << "/";
        }
    }

    return ss.str();
}

int isDirectory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int getdir(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        stringstream ss("Error opening ");
        ss << dir;
        throw std::logic_error(ss.str());
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int getNumberOfFilesInFolder(string dir)
{
    vector<string> files;
    getdir(dir, files);
    return files.size() - 2;
}

string hello()
{
    return "hello";
}

Mat mouline(Mat original)
{
    ExtractionInformation extractInfo, extractInfo2;

    // prepare knn
    string raw_features_path("./../assets/raw-features.yml"); // created by prepareData
    cv::FileStorage raw_features(raw_features_path, cv::FileStorage::READ);

    Ptr<ml::SVM> svm = getSvm(raw_features);

    Mat preprocessed = preprocess(original.clone(), true);

    vector<Point> biggestApprox = findBiggestBlob(preprocessed, original);

    extractInfo = extractPuzzle(original, biggestApprox);
    Mat extractedPuzzle = extractInfo.image;
    // showImage(extractedPuzzle);

    Mat finalExtraction = recursiveExtraction(extractedPuzzle);
    // showImage(finalExtraction);

    string resultTab[81], initialStateOfTheSudoku;

    grabNumbers(finalExtraction, svm, resultTab);

    stringstream solution, ss;

    for (int k = 0; k < 81; k++)
    {
        ss << resultTab[k];
    }

    initialStateOfTheSudoku = ss.str();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<bool, std::array<int, 81>> pair = solve(initialStateOfTheSudoku.c_str());

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    // cout << duration / 1000 << " ms" << endl;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    if (pair.first)
    {
        std::array<int, 81> ans = pair.second;
        for (int i = 0; i < 81; i++)
        {
            solution << ans[i];
        }
        // LOGI("%s", solution.str().c_str());

        if (areSameNonZeroValues(initialStateOfTheSudoku, solution.str()))
        {
            Mat writen = writeOnPuzzle(finalExtraction, initialStateOfTheSudoku, solution.str());
            warpPerspective(writen, original, extractInfo.transformation, original.size(), WARP_INVERSE_MAP, BORDER_TRANSPARENT);
        }
    }

    return original;
}

// string askServerForSolution(string initialStateOfTheSudoku)
// {

//     // string solution = askServerForSolution(initialStateOfTheSudoku);
//     Sudoku::init();

//     auto S = solve(unique_ptr<Sudoku>(new Sudoku(initialStateOfTheSudoku)));
//     string solution = S->getSolution();

//     return solution;

//     // // we are not using server because it is too slow
//     //     // request the server for solution --------------------------------

//     //     stringstream ss, url;

//     //     url << "http://moulinet.tech/sudokus?input=";
//     //     url << initialStateOfTheSudoku;

//     //     CURL *curl_handle;
//     //     CURLcode res;

//     //     struct MemoryStruct chunk;

//     //     chunk.memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
//     //     chunk.size = 0;                   /* no data at this point */

//     //     curl_global_init(CURL_GLOBAL_ALL);

//     //     /* init the curl session */
//     //     curl_handle = curl_easy_init();

//     //     /* specify URL to get */
//     //     curl_easy_setopt(curl_handle, CURLOPT_URL, url.str().c_str());

//     //     /* send all data to this function  */
//     //     curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

//     //     /* we pass our 'chunk' struct to the callback function */
//     //     curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

//     //     /* some servers don't like requests that are made without a user-agent
//     //        field, so we provide one */
//     //     curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

//     //     /* get it! */
//     //     res = curl_easy_perform(curl_handle);

//     //     /* check for errors */
//     //     if (res != CURLE_OK)
//     //     {
//     //         fprintf(stderr, "curl_easy_perform() failed: %s\n",
//     //                 curl_easy_strerror(res));
//     //     }
//     //     else
//     //     {
//     //         /*
//     //          * Now, our chunk.memory points to a memory block that is chunk.size
//     //          * bytes big and contains the remote file.
//     //          *
//     //          * Do something nice with it!
//     //          */

//     //         ss << chunk.memory;

//     //         printf("%lu bytes retrieved\n", (long)chunk.size);
//     //     }

//     //     curl_easy_cleanup(curl_handle);

//     //     free(chunk.memory);

//     //     curl_global_cleanup();

//     //     string solution = ss.str();

//     //     // request the server for solution - end --------------------------------
//     // return solution;
// }

std::pair<bool, std::array<int, 81>> solve(const char *input)
{
    std::array<int, 81> ans;
    int *map = ans.data();         // The map of numbers with 0 for empty
    int empty[81];                 // The index of each empty gird
    int c = 0;                     // The current index we accessed
    int i, j, k;                   // The current row, column, region index we accessed
    int b;                         // The temporary bit set
    int empty_count = 0;           // The count of empty gird
    int filled = 0;                // The count of filled empty gird
    int col[9], row[9], region[9]; //Bit set of each column, row and region
    for (i = 0; i < 9; i++)
    {
        col[i] = row[i] = region[i] = 01776; // Binary number 1111111110
    }
    while (*input)
    {
        if (*input == '0') // Empty gird
        {
            empty[empty_count++] = c;
            map[c] = 0;
        }
        else if ((*input) <= '9' && (*input) >= '1') // Non empty gird
        {
            i = c / 9;
            j = c % 9;
            k = (c / 27) * 3 + ((c % 9) / 3);
            map[c] = (*input) - '0';
            b = (1 << map[c]); // Check if the gird is wrong
            if ((b & row[i] & col[j] & region[k]) == 0)
            {
                return std::pair<bool, std::array<int, 81>>(false, ans);
            }
            // Fill the bit to 0 for the row, column and region
            row[i] ^= b;
            col[j] ^= b;
            region[k] ^= b;
        }
        else
        {
            break;
        }
        input++;
        c++;
    }
    for (; c < 81; c++) //Fill empty for left girds
    {
        empty[empty_count++] = c;
        map[c] = 0;
    }
    while (filled < empty_count) //Fill numbers until all number has been filled
    {
        c = empty[filled];
        i = c / 9;
        j = c % 9;
        k = (c / 27) * 3 + ((c % 9) / 3);
        if (map[c] > 0) //If this gird has been filled before, then reset it
        {
            b = (1 << map[c]); //Reset the bit to 1
            row[i] |= b;
            col[j] |= b;
            region[k] |= b;
        }
        while (map[c] <= 9) //Fill current gird if the number was not appeared in current row, col or region
        {
            map[c]++;
            b = (1 << map[c]); //Get the bit of the number and check if it has alerdy existed.
            if (row[i] & col[j] & region[k] & b)
            {
                break;
            }
        }
        if (map[c] > 9) // If there is no number to fill, back to the gird we filled before
        {
            map[c] = 0;
            filled--;
            if (filled < 0) // If current is the first gird, then there is no answer for the puzzle
            {
                return std::pair<bool, std::array<int, 81>>(false, ans);
            }
        }
        else
        {
            //After we filled this gird, set the bit to 0
            row[i] ^= b;
            col[j] ^= b;
            region[k] ^= b;
            filled++;
        }
    }
    return std::pair<bool, std::array<int, 81>>(true, ans);
}

bool areSameNonZeroValues(string init, string sol)
{
    int s = init.size();
    bool result = true;
    for (int i = 0; i < s; i++)
    {
        if (init[i] != '0' && init[i] != sol[i])
        {
            return false;
        }
    }
    return result;
}