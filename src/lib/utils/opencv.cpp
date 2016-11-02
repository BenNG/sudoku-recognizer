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

    int cell_height = cell.rows;
    int cell_width = cell.cols;

    // setting parameters for long lines filtering
    // that means if a line is longer than 77% of the cell it's droped
    float percent = 0.23;
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
            continue; // drop long horizontal line
        if (height > height_threshold)
            continue; // drop long vetical line
        if (boundingArea < 220 || boundingArea > 900)
            continue;
        if (area < 110)
            continue; // area of the connected object

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

Mat extractRoiFromCell(Mat sudoku, int k)
{
    Mat rawCell, rawRoi, output, thresholded, cleaned;

    rawCell = extractCell(sudoku, k);
    rawRoi = extractNumber(rawCell);

    if (!rawRoi.empty())
    {
        adaptiveThreshold(rawRoi, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
        // fin 8bits (CV_8U)
        cleaned = removeTinyVolume(thresholded, 90, Scalar(0, 0, 0));
        // fin2 8bits
        vector<double> v = findBiggestComponent(cleaned);

        double left = v[0];
        double top = v[1];
        double width = v[2];
        double height = v[3];
        double x = v[4];
        double y = v[5];
        Rect rect(left, top, width, height);
        return cleaned(rect);
    }

    return output;
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

vector<Mat> readTrainingMNIST(){
    return readMNIST(true);
}

vector<Mat> readTestMNIST(){
    return readMNIST(false);
}

vector<Mat> readMNIST(bool training)
{
    vector<Mat> v(2);
    string feat_str, label_str; 
    if(training){
        feat_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-images-idx3-ubyte";
        label_str = "/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-labels-idx1-ubyte";
    }else{
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

    Mat trainFeatures(numImages, size, CV_8UC1);
    Mat trainLabels(1, numImages, CV_8UC1);

    unsigned char *temp = new unsigned char[size];
    unsigned char tempClass = 0;
    for (int i = 0; i < numImages; i++)
    {
        fread((void *)temp, size, 1, fp);
        fread((void *)(&tempClass), sizeof(unsigned char), 1, fp2);

        trainLabels.at<unsigned char>(0, i) = tempClass;
        // feature creation
        for (int k = 0; k < size; k++)
        {
            trainFeatures.at<unsigned char>(i, k) = temp[k];
        }
        // feature creation - end
    }

    // I don't know why I have to convert the matrices in float ...
    trainFeatures.convertTo(trainFeatures, CV_32F);
    trainLabels.convertTo(trainLabels, CV_32F);

    v[0] = trainFeatures;
    v[1] = trainLabels;
    return v;
}

// ------------------------------------------------------------------------
// PUZZLE

Mat extractCell(Mat sudoku, int numCell) {
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
    vector<vector<Point> > contours;
    vector<cv::Point> approx;
    vector<cv::Point> biggestApprox;

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

    cv::Point2f tl;
    cv::Point2f tr;
    cv::Point2f bl;
    cv::Point2f br;

    cv::Point2f xs[4];
    cv::Point2f ys[4];

    cv::Point2f tops[2];
    cv::Point2f bottoms[2];

    cv::Point2f src_p[4];
    cv::Point2f dst_p[4];

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
    dst_p[0] = cv::Point2f(0.0f, 0.0f);
    dst_p[1] = cv::Point2f(w, 0.0f);
    dst_p[2] = cv::Point2f(w, h);
    dst_p[3] = cv::Point2f(0.0f, h);

    cv::Mat dst_img;

    // create perspective transform matrix
    cv::Mat trans_mat33 = cv::getPerspectiveTransform(src_p, dst_p); //CV_64F->double

    // perspective transform operation using transform matrix
    warpPerspective(input, outerBox, trans_mat33, input.size(), cv::INTER_LINEAR);
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
    Ptr<ml::KNearest> knn(ml::KNearest::create());

    vector<Mat> v = readTrainingMNIST();
    Mat trainFeatures = v[0];
    Mat trainLabels = v[1];

    knn->train(trainFeatures, ml::ROW_SAMPLE, trainLabels);
    return knn;
}

void testKnn(Ptr<ml::KNearest> knn)
{
    int totalCorrect = 0;

    vector<Mat> v = readTestMNIST();
    Mat testFeatures = v[0];
    Mat expectedLabels = v[1];

    int numImages = testFeatures.rows;

    int K = 1;
    Mat response, dist, m;

    for (int i = 0; i < numImages; i++)
    {
        m = testFeatures.row(i);
        knn->findNearest(m, K, noArray(), response, dist);

        if (expectedLabels.at<int>(0, i) == response.at<float>(0))
            totalCorrect++;
    }

    printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
}

void testKnn(Ptr<ml::KNearest> knn, bool debug)
{
    int totalCorrect = 0;

    vector<Mat> v = readTestMNIST();
    Mat testFeatures = v[0];
    Mat expectedLabels = v[1];

    int numImages = testFeatures.rows;

    int K = 1;
    Mat response, dist, m;

    for (int i = 0; i < numImages; i++)
    {

        if (debug && i % 1000 == 0 && i != 0)
        {
            cout << i << endl;
        }

        m = testFeatures.row(i);

        knn->findNearest(m, K, noArray(), response, dist);

        if (debug)
        {
            cout << "response: " << response << endl;
            cout << "dist: " << dist << endl;
            Mat m2 = m.reshape(1, 28);
            showImage(m2);
            Mat m3 = m2.reshape(1, 1);
            showImage(m3);
        }

        if (expectedLabels.at<int>(0, i) == response.at<float>(0))
        {
            totalCorrect++;
        }
    }

    printf("Accuracy: %f ", (double)totalCorrect * 100 / (double)numImages);
}


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
    vector<vector<Point> > contours;
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

    vector<double> v = {left, top, width, height, centroids.at<double>(index,0), centroids.at<double>(index,1)};

    return v;
}

Mat drawAllApprox(Mat preprocessed)
{
    Mat output = Mat::zeros(preprocessed.rows, preprocessed.cols, preprocessed.type());

    Scalar white(255, 255, 255);
    vector<vector<Point> > contours;
    std::vector<cv::Point> approx;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++)
    {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.1, true);

        // Skip small or non-convex objects
        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
            continue;

        std::vector<vector<Point> > approx_contour(1, approx);
        drawContours(output, approx_contour, 0, white, 2, 8);
    }
    return output;
}
Mat drawAllApprox(Mat preprocessed, Mat origial)
{
    Mat output = origial.clone();

    Scalar white(255, 255, 255);
    vector<vector<Point> > contours;
    std::vector<cv::Point> approx;

    findContours(preprocessed.clone(), contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++)
    {

        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.1, true);

        // Skip small or non-convex objects
        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
            continue;

        std::vector<vector<Point> > approx_contour(1, approx);
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

fs::path getPath(fs::path p)
{
    fs::path rootPath(getMyProjectRoot(fs::current_path()));
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
    vector<vector<Point> > contours;
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

Mat deskew(Mat t){
    Moments m = moments(t, true);
    double skew = m.mu11/(int)m.mu02;
    Mat transform = Mat(2,3,CV_32F);
    transform.at<float>(0,0) = 1;
    transform.at<float>(0,1) = skew;
    transform.at<float>(0,2) = -0.5*28*skew;
    transform.at<float>(1,0) = 0;
    transform.at<float>(1,1) = 1;
    transform.at<float>(1,2) = 0;
    // cout << skew << endl;
    warpAffine(t, t, transform, t.size(), cv::WARP_INVERSE_MAP);
    return t;
}

// ---------------------------------------------------------------------
// mlp


template<typename T>
static Ptr<T> load_classifier(const string& persistence)
{
    // load classifier from the specified file
    Ptr<T> model = StatModel::load<T>( persistence );
    if( model.empty() )
        cout << "Could not read the classifier " << persistence << endl;
    else
        cout << "The classifier " << persistence << " is loaded.\n";

    return model;
}

inline TermCriteria TC(int iters, double eps) {
    return TermCriteria(TermCriteria::MAX_ITER + (eps > 0 ? TermCriteria::EPS : 0), iters, eps);
}


static void test_and_save_classifier(const Ptr<StatModel>& model,
                                     const Mat& data, const Mat& responses,
                                     int ntrain_samples, int rdelta,
                                     const string& filename_to_save)
{
    int i, nsamples_all = data.rows;
    double train_hr = 0, test_hr = 0;

    // compute prediction error on train and test data
    for( i = 0; i < nsamples_all; i++ )
    {
        Mat sample = data.row(i);

        float r = model->predict( sample );
        r = std::abs(r + rdelta - responses.at<int>(i)) <= FLT_EPSILON ? 1.f : 0.f;

        if( i < ntrain_samples )
            train_hr += r;
        else
            test_hr += r;
    }

    test_hr /= nsamples_all - ntrain_samples;
    train_hr = ntrain_samples > 0 ? train_hr/ntrain_samples : 1.;

    printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n",
            train_hr*100., test_hr*100. );

    if( !filename_to_save.empty() )
    {
        model->save( filename_to_save );
    }
}




Ptr<ANN_MLP>
build_mlp_classifier(const fs::path data_filename, const fs::path persistence){

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
    int ntrain_samples = (int)(nsamples_all*0.8);

//    boost::filesystem::path persistence_path(persistence);
    string persistence_str = persistence.string();
    // Create or load MLP classifier
    if (boost::filesystem::exists(persistence)) {
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
        Mat train_responses = Mat::zeros( ntrain_samples, class_count, CV_32F );

        // 1. unroll the responses
//        cout << "Unrolling the responses...\n";
        for (int i = 0; i < ntrain_samples; i++) {
            train_responses.at<float>(i, responses.at<int>(i)) = 1.f;
        }

        // 2. train classifier
        int layer_sz[] = {data.cols, 100, 100, class_count};
        int nlayers = (int) (sizeof(layer_sz) / sizeof(layer_sz[0]));
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
        model->setTermCriteria(TC(max_iter,0));
        model->setTrainMethod(method, method_param);
        model->train(tdata);
        cout << endl;
    }

    test_and_save_classifier(model, data, responses, ntrain_samples, 0, persistence_str);
    return model;
}