#include "opencv.h"

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


    Mat cell = prepareCell(input);
    Mat temp = removeTinyVolume(cell.clone(), 75, Scalar(0, 0, 0));

    // showImage(temp);

    int cell_height = cell.rows;
    int cell_width = cell.cols;

    // setting parameters for long lines filtering
    // that means if a line is longer than 77% of the cell it's droped
    float percent = 0.27;
    float width_threshold = cell_width - cell_width * percent;
    float height_threshold = cell_height - cell_height * percent;

    // Use connected components with stats
    Mat labels, stats, centroids;
    int num_objects = connectedComponentsWithStats(cell, labels, stats, centroids);
    // Check the number of objects detected
    Mat output;

    if (num_objects < 2)
    {
        //        cout << "No objects detected" << endl;
        return output;
    }
    else
    {
        //        cout << "Number of objects detected: " << num_objects - 1 << endl;
    }
    Scalar white(255, 255, 255);

    // Create output image coloring the objects and show area
    for (int i = 1; i < num_objects; i++)
    {
        int area = stats.at<int>(i, CC_STAT_AREA);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);
        int left = stats.at<int>(i, CC_STAT_LEFT);
        int top = stats.at<int>(i, CC_STAT_TOP);

        // filtering
        int boundingArea = width * height;
        if (width > width_threshold)
        {
            // cout << "width > to width_threshold -> skip " << width << endl;

            continue; // drop long horizontal line
        }else{
            // cout << "width: " << width << endl;
        }
        if (height < 8 || height > height_threshold)
        {
            // cout << "height > height_threshold -> skip " << width << endl;

            continue; // drop long vetical line
        }else{
            // cout << "height: " << height << endl;
            // cout << "height_threshold: " << height_threshold << endl;

        }
        if (boundingArea < 220 || boundingArea > 900)
        {
            continue;
        }
        else
        {
            // cout << "boundingArea: " << boundingArea << endl;
        }
        if (area < 105)
        {
            continue; // area of the connected object
        }else{
            // cout << "area: " << area << endl;
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
    rawRoi = extractNumber(rawCell);

    if (debug)
    {
        showImage(rawCell);
        showImage(rawRoi);
    }

    if (!rawRoi.empty())
    {
        // threshold(rawRoi, thresholded, 125, 255, rawRoi.type());
        adaptiveThreshold(rawRoi, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 1);
        // fin 8bits (CV_8U)
        // showImage(thresholded);
        // be careful here the 2nd param can delete tiny number like 1
        cleaned = removeTinyVolume(thresholded.clone(), 75, Scalar(0, 0, 0));
        // fin2 8bits
        // showImage(cleaned);
        vector<double> v = findBiggestComponent(cleaned);

        if (debug)
        {
            showImage(thresholded);
            showImage(cleaned);
        }

        double left = v[0];
        double top = v[1];
        double width = v[2];
        double height = v[3];
        double x = v[4];
        double y = v[5];
        Rect rect(left, top, width, height);
        Mat almostReady = cleaned(rect);

        // threshold(almostReady, output, 125, 255, almostReady.type());

        return normalizeSize(almostReady, normalizedSizeForCell);

        // return cleaned(rect);
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

vector<Mat> readTrainingMNIST()
{
    return readMNIST(true);
}

vector<Mat> readTestMNIST()
{
    return readMNIST(false);
}

vector<Mat> readMNIST(bool training)
{
    vector<Mat> v(2);
    string feat_str, label_str;
    if (training)
    {
        feat_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-images-idx3-ubyte";
        label_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-labels-idx1-ubyte";
    }
    else
    {
        feat_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/t10k-images-idx3-ubyte";
        label_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/t10k-labels-idx1-ubyte";
    }

    FILE *fp = fopen(feat_str.c_str(), "rb");
    FILE *fp2 = fopen(label_str.c_str(), "rb");

    if (!fp || !fp2)
    {
        cout << "can't open file" << endl;
    }

    int magicNumber = readFlippedInteger(fp);
    int numImages = readFlippedInteger(fp);
    int numRows = readFlippedInteger(fp);
    int numCols = readFlippedInteger(fp);
    fseek(fp2, 0x08, SEEK_SET);

    int size = numRows * numCols;

    cout << "size: " << size << endl;
    cout << "rows: " << numRows << endl;
    cout << "cols: " << numCols << endl;

    Mat rawFeatures(numImages, size, CV_8UC1);
    Mat hogFeatures(numImages, 735, CV_8UC1);
    Mat trainLabels(1, numImages, CV_8UC1);
    Mat raw, hoged;

    unsigned char *temp = new unsigned char[size];
    unsigned char tempClass = 0;
    for (int i = 0; i < numImages; i++)
    {
        fread((void *)temp, size, 1, fp);
        fread((void *)(&tempClass), sizeof(unsigned char), 1, fp2);

        trainLabels.at<unsigned char>(0, i) = tempClass;

        // feature creation
        // create the binarize matrix
        for (int k = 0; k < size; k++)
        {
            rawFeatures.at<unsigned char>(i, k) = temp[k];
        }
        raw = rawFeatures.row(i).reshape(1, 28);
        // showImage(raw);
        hoged = hog_feature(raw);
        hoged.copyTo(hogFeatures.row(i));
        // feature creation - end
    }

    // I don't know why I have to convert the matrices in float ...
    rawFeatures.convertTo(rawFeatures, CV_32F);
    hogFeatures.convertTo(hogFeatures, CV_32F);
    trainLabels.convertTo(trainLabels, CV_32F);

    v[0] = hogFeatures;
    v[1] = trainLabels;
    return v;
}

// ------------------------------------------------------------------------
// PUZZLE

Mat extractCell(Mat sudoku, int numCell)
{
    Mat output = sudoku.clone();
    int y = sudoku.cols;
    int x = sudoku.rows;
    int cx = x / 9;
    int cy = y / 9;

    //    cout << "cell size is :" << cx << " * " << cy << " = " << cx * cy << endl;

    Rect rect = Rect((numCell % 9) * cy, (numCell / 9) * cx, cy, cx);
    return output(rect);
}

// ------------------------------------------------------------------------
// PICTURE

Mat preprocess(Mat input)
{
    Mat outerBox = Mat(input.size(), CV_8UC1);
    GaussianBlur(input, input, Size(11, 11), 0);
    adaptiveThreshold(input, outerBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
    outerBox = removeTinyVolume(outerBox.clone(), 400, Scalar(255, 255, 255));
    bitwise_not(outerBox, outerBox);
    dilate(outerBox, outerBox, Mat());
    return outerBox;
}

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

/*
* findBigestApprox
*
* Find the biggest contour in the image
* note that it returns vector< vector<Point> > because it is more convenient to use drawContours after
* */
vector<Point> findBigestApprox(Mat input)
{

    int largest_area = 0;
    vector<vector<Point>> contours;
    vector<Point> approx;
    vector<Point> biggestApprox;

    findContours(input, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++)
    {
        // Approximate contour with accuracy proportional to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.1, true);
        // Skip small or non-convex objects
        if (std::fabs(contourArea(contours[i])) < 1200 || !isContourConvex(approx))
            continue;

        if (approx.size() == 4)
        {
            double a = contourArea(contours[i]);
            if (a > largest_area)
            {
                largest_area = a;
                biggestApprox = approx;
            }
        }
    }
    return biggestApprox;
}

Mat extractPuzzle(Mat input, vector<Point> biggestApprox)
{
    Mat outerBox = Mat(input.size(), CV_8UC1);

    Point2f tl;
    Point2f tr;
    Point2f bl;
    Point2f br;

    Point2f xs[4];
    Point2f ys[4];

    Point2f tops[2];
    Point2f bottoms[2];

    Point2f src_p[4];
    Point2f dst_p[4];

    xs[0] = biggestApprox.at(0);
    xs[1] = biggestApprox.at(1);
    xs[2] = biggestApprox.at(2);
    xs[3] = biggestApprox.at(3);

    ys[0] = biggestApprox.at(0);
    ys[1] = biggestApprox.at(1);
    ys[2] = biggestApprox.at(2);
    ys[3] = biggestApprox.at(3);

    std::sort(ys, ys + 4, sort_ys);

    tops[0] = ys[0];
    tops[1] = ys[1];

    std::sort(tops, tops + 2, sort_xs);

    tl = tops[0];
    tr = tops[1];

    bottoms[0] = ys[2];
    bottoms[1] = ys[3];

    std::sort(bottoms, bottoms + 2, sort_xs);

    bl = bottoms[0];
    br = bottoms[1];

    //
    // cout << tl << endl;
    // cout << tr << endl;
    // cout << bl << endl;
    // cout << br << endl;
    // cout << "------------------------------------" << endl;

    float w = (float)input.cols;
    float h = (float)input.rows;
    float hw = w / 2.0f;
    float hh = h / 2.0f;

    // from points
    src_p[0] = tl;
    src_p[1] = tr;
    src_p[2] = br;
    src_p[3] = bl;

    // to points
    dst_p[0] = Point2f(0.0f, 0.0f);
    dst_p[1] = Point2f(w, 0.0f);
    dst_p[2] = Point2f(w, h);
    dst_p[3] = Point2f(0.0f, h);

    Mat dst_img;

    // create perspective transform matrix
    Mat trans_mat33 = getPerspectiveTransform(src_p, dst_p); //CV_64F->double

    // perspective transform operation using transform matrix
    warpPerspective(input, outerBox, trans_mat33, input.size(), INTER_LINEAR);
    return outerBox;
}

Mat extractPuzzle(Mat original)
{
    Mat outerBox = Mat(original.size(), CV_8UC1);

    Mat preprocessed = preprocess(original.clone());
    vector<Point> biggestApprox = findBigestApprox(preprocessed);

    outerBox = extractPuzzle(original, biggestApprox);

    // trick
    // sometimes the biggest area found is not correct, our puzzle is inside the extract image
    // so we do it a second time to extract the biggest blob which is this time our puzzle
    // this is the case for s6.jpg and s9.jpg for example

    Mat preprocessed2 = preprocess(outerBox.clone());
    vector<Point> biggestApprox2 = findBigestApprox(preprocessed2);

    if (!biggestApprox2.empty())
    {
        outerBox = extractPuzzle(outerBox);
    }
    // trick - end

    return outerBox;
}

// --------------------------------------------------------
// KNN

Ptr<ml::KNearest> getKnn()
{
    int trainingNbr = nbrOfCells * 0.9;
    int testingNbr = nbrOfCells - trainingNbr;
    Mat features(nbrOfCells, normalizedSizeForCell * normalizedSizeForCell, CV_8UC1);
    Mat labels(1, nbrOfCells, CV_8UC1);
    fs::path raw_features_path(getPath("assets/raw-features.yml")); // created by prepareData 
    Ptr<ml::KNearest> knn(ml::KNearest::create());

    // vector<Mat> v = readTrainingMNIST();
    // Mat trainFeatures = v[0];
    // Mat trainLabels = v[1];

    cv::FileStorage raw_features(raw_features_path.string(), cv::FileStorage::READ);

    if (raw_features.isOpened() == false)
    {
        throw "error, unable to open training classifications file, exiting program\n\n"; // if the file was not opened successfully
        // std::cout << "error, unable to open training classifications file, exiting program\n\n"; // show error message
        // return (0);                                                                              // and exit program
    }

    raw_features["features"] >> features;
    raw_features["labels"] >> labels;
    raw_features.release();

    Mat sub_features = features(cv::Range(0, trainingNbr), cv::Range::all());
    Mat sub_labels = labels(cv::Range::all(), cv::Range(0, trainingNbr));

    knn->train(sub_features, ml::ROW_SAMPLE, sub_labels);

    return knn;
}

void testKnn(Ptr<ml::KNearest> knn)
{

    int trainingNbr = nbrOfCells * 0.9;
    int testingNbr = nbrOfCells - trainingNbr;
    Mat features(nbrOfCells, normalizedSizeForCell * normalizedSizeForCell, CV_8UC1);
    Mat labels(1, nbrOfCells, CV_8UC1);
    fs::path raw_features_path(getPath("assets/raw-features.yml"));
    int totalCorrect = 0;

    cv::FileStorage raw_features(raw_features_path.string(), cv::FileStorage::READ);

    if (raw_features.isOpened() == false)
    {
        throw "error, unable to open training classifications file, exiting program\n\n"; // if the file was not opened successfully
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

vector<double> findBiggestComponent(Mat input)
{
    Mat output = Mat::zeros(input.rows, input.cols, input.type());

    Mat labels, stats, centroids;
    int num_objects = connectedComponentsWithStats(input, labels, stats, centroids);
    // Check the number of objects detected

    if (num_objects < 2)
    {
        throw "No objects detected";
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
        int area = stats.at<int>(i, CC_STAT_AREA);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);
        int left = stats.at<int>(i, CC_STAT_LEFT);
        int top = stats.at<int>(i, CC_STAT_TOP);

        if (biggestArea < area)
        {
            biggestArea = area;
            index = i;
        }
    }

    // cout << "area: " << stats.at<int>(index, CC_STAT_AREA) << endl;
    int area = stats.at<int>(index, CC_STAT_AREA);
    int width = stats.at<int>(index, CC_STAT_WIDTH);
    int height = stats.at<int>(index, CC_STAT_HEIGHT);
    int left = stats.at<int>(index, CC_STAT_LEFT);
    int top = stats.at<int>(index, CC_STAT_TOP);

    vector<double> v = {left, top, width, height, centroids.at<double>(index, 0), centroids.at<double>(index, 1)};

    return v;
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

void showImage(Mat img)
{
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", img);
    waitKey(0);
}

fs::path getMyProjectRoot(fs::path p)
{
    string projectRootFolderName = "sudoku";
    if (p.filename() == projectRootFolderName)
    {
        return p;
    }
    else if (p.filename() == "/")
    {
        throw "could not find project root (in function getMyProjectRoot)";
    }
    else
    {
        return getMyProjectRoot(p.parent_path());
    }
}

fs::path getPath(string p)
{
    fs::path rootPath(getMyProjectRoot(getexepath()));
    return rootPath /= p;
}

/**
* from time to time there are some tiny text around the puzzle and it kills the detection
* this function remove the tiny contour
*/
Mat removeTinyVolume(Mat input, int area, Scalar color)
{
    // we draw to the color of the background
    Mat output = input.clone();
    Scalar black(0, 0, 0);
    Scalar white(255, 255, 255);
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

Ptr<ANN_MLP>
build_mlp_classifier(const fs::path data_filename, const fs::path persistence)
{

    const int class_count = 9;
    Mat data;
    Mat responses;

    cout << boost::filesystem::current_path() << endl;

    // fn
    string data_filename_str = data_filename.string();
    FileStorage fs;
    fs.open(data_filename_str, FileStorage::READ);
    fs["TrainingDataF15"] >> data;
    fs["classes"] >> responses;
    // fn - end

    Ptr<ANN_MLP> model;

    int nsamples_all = data.rows;
    int ntrain_samples = (int)(nsamples_all * 0.8);

    //    boost::filesystem::path persistence_path(persistence);
    string persistence_str = persistence.string();
    // Create or load MLP classifier
    if (boost::filesystem::exists(persistence))
    {
        return load_classifier<ANN_MLP>(persistence_str);
    }
    else
    {
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //
        // MLP does not support categorical variables by explicitly.
        // So, instead of the output class label, we will use
        // a binary vector of <class_count> components for training and,
        // therefore, MLP will give us a vector of "probabilities" at the
        // prediction stage
        //
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        Mat train_data = data.rowRange(0, ntrain_samples);
        Mat train_responses = Mat::zeros(ntrain_samples, class_count, CV_32F);

        // 1. unroll the responses
        //        cout << "Unrolling the responses...\n";
        for (int i = 0; i < ntrain_samples; i++)
        {
            train_responses.at<float>(i, responses.at<int>(i)) = 1.f;
        }

        // 2. train classifier
        int layer_sz[] = {data.cols, 100, 100, class_count};
        int nlayers = (int)(sizeof(layer_sz) / sizeof(layer_sz[0]));
        Mat layer_sizes(1, nlayers, CV_32S, layer_sz);

#if 1
        int method = ANN_MLP::BACKPROP;
        double method_param = 0.001;
        int max_iter = 300;
#else
        int method = ANN_MLP::RPROP;
        double method_param = 0.1;
        int max_iter = 1000;
#endif

        Ptr<TrainData> tdata = TrainData::create(train_data, ROW_SAMPLE, train_responses);

        cout << "Training the classifier (may take a few minutes)...\n";
        model = ANN_MLP::create();
        model->setLayerSizes(layer_sizes);
        model->setActivationFunction(ANN_MLP::SIGMOID_SYM, 0, 0);
        model->setTermCriteria(TC(max_iter, 0));
        model->setTrainMethod(method, method_param);
        model->train(tdata);
        cout << endl;
    }

    test_and_save_classifier(model, data, responses, ntrain_samples, 0, persistence_str);
    return model;
}

Mat hog_feature(Mat input)
{

    vector<float> ders;
    vector<Point> locs;

    bool gamma_corr = true;
    Size win_size(input.rows, input.cols); //using just one window
    int c = 4;
    Size block_size(c, c);
    Size block_stride(c, c); //no overlapping blocks
    Size cell_size(c, c);
    int nOri = 15; //number of orientation bins

    HOGDescriptor hog(win_size, block_size, block_stride, cell_size, nOri, 1, -1,
                      HOGDescriptor::L2Hys, 0.2, gamma_corr, HOGDescriptor::DEFAULT_NLEVELS);

    hog.compute(input, ders, Size(0, 0), Size(0, 0), locs);

    Mat Hogfeat(1, ders.size(), CV_32FC1);

    for (int i = 0; i < ders.size(); i++)
    {
        Hogfeat.at<float>(0, i) = ders.at(i);
    }

    // showImage(Hogfeat);

    return Hogfeat;
}

// --------------------------------------------------------------------------
// createDataForTraining

void create_data_structure()
{
    fs::path data("data");
    fs::path data1("data/1");
    fs::path data2("data/2");
    fs::path data3("data/3");
    fs::path data4("data/4");
    fs::path data5("data/5");
    fs::path data6("data/6");
    fs::path data7("data/7");
    fs::path data8("data/8");
    fs::path data9("data/9");

    if (!fs::exists(data))
    {
        boost::filesystem::create_directories(data);
        boost::filesystem::create_directories(data1);
        boost::filesystem::create_directories(data2);
        boost::filesystem::create_directories(data3);
        boost::filesystem::create_directories(data4);
        boost::filesystem::create_directories(data5);
        boost::filesystem::create_directories(data6);
        boost::filesystem::create_directories(data7);
        boost::filesystem::create_directories(data8);
        boost::filesystem::create_directories(data9);
    }
    else
    {
        if (!fs::exists(data1))
            boost::filesystem::create_directories(data1);
        if (!fs::exists(data2))
            boost::filesystem::create_directories(data2);
        if (!fs::exists(data3))
            boost::filesystem::create_directories(data3);
        if (!fs::exists(data4))
            boost::filesystem::create_directories(data4);
        if (!fs::exists(data5))
            boost::filesystem::create_directories(data5);
        if (!fs::exists(data6))
            boost::filesystem::create_directories(data6);
        if (!fs::exists(data7))
            boost::filesystem::create_directories(data7);
        if (!fs::exists(data8))
            boost::filesystem::create_directories(data8);
        if (!fs::exists(data9))
            boost::filesystem::create_directories(data9);
    }
}

std::string remove_extension(const std::string &filename)
{
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos)
        return filename;
    return filename.substr(0, lastdot);
}

std::string uuid_first_part(const std::string &uuid)
{
    size_t first = uuid.find_first_of("-");
    if (first == std::string::npos)
        return uuid;
    return uuid.substr(0, first);
}

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

// give file path from the root of the project
string grab(string filePath_str, Ptr<ml::KNearest> knn)
{
    // Ptr<ml::KNearest> knn = getKnn();
    fs::path filePath(getPath(filePath_str));
    Mat raw, sudoku, roi, response, dist;
    stringstream ss;
    int K = 1;

    raw = imread(filePath.string(), CV_LOAD_IMAGE_GRAYSCALE);
    sudoku = extractPuzzle(raw);

    for (int k = 0; k < 81; k++)
    {
        roi = extractRoiFromCell(sudoku, k);
        if (!roi.empty())
        {
            // showImage(roi);
            roi.convertTo(roi, CV_32F);
            knn->findNearest(roi.reshape(1, 1), K, noArray(), response, dist);
            ss << response.at<float>(0);
        }
        else
        {
            ss << "0";
        }
    }

    return ss.str();
}

string getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return string( result, (count > 0) ? count : 0 );
}
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