/**
 * You will handle cells here
    - you will first preprocess the cell using prepareCell
    - then extract the number in a normalize size
    - finally you can extract the feature
 * */

#include "cell.h"


Mat prepareCell(Mat cell) {

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

Mat removeLight(Mat img, Mat pattern, int method) {
    Mat aux;
// if method is normalization
    if (method == 1) {
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
    } else {
        aux = pattern - img;
    }
    return aux;
}


Mat normalizeSize(Mat in) {

    int charSize = 25;
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
    resize(warpImage, out, Size(charSize, charSize));

    return out;
}

/**
 * extract the number and normelize the size
 * */
Mat extractNumber(Mat cell) {

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

    if (num_objects < 2) {
//        cout << "No objects detected" << endl;
        return output;
    } else {
//        cout << "Number of objects detected: " << num_objects - 1 << endl;
    }

    // Create output image coloring the objects and show area
    for (int i = 1; i < num_objects; i++) {
        int area = stats.at<int>(i, CC_STAT_AREA);
        int width = stats.at<int>(i, CC_STAT_WIDTH);
        int height = stats.at<int>(i, CC_STAT_HEIGHT);
        int left = stats.at<int>(i, CC_STAT_LEFT);
        int top = stats.at<int>(i, CC_STAT_TOP);

        // filtering
        int boundingArea = width * height;
        if (width > width_threshold) continue; // drop long horizontal line
        if (height > height_threshold) continue; // drop long vetical line
        if (boundingArea < 220 || boundingArea > 900) continue;
        if (area < 110) continue; // area of the connected object

        // should be the number here
        Rect rect(left, top, width, height);
//        Mat mask= labels==i;
//        output.setTo(randomColor(rng), mask);
        return normalizeSize(cell(rect));

    }

    return output;

}


Mat calculateLightPattern(Mat img) {
    Mat pattern;
// Basic and effective way to calculate the light pattern from one image
    blur(img, pattern, Size(img.cols / 3, img.cols / 3));
//    showImage(pattern);
    return pattern;
}

Mat ProjectedHistogram(Mat img, int t) {
    int sz = (t) ? img.rows : img.cols;
    Mat mhist = Mat::zeros(1, sz, CV_32F);
    for (int j = 0; j < sz; j++) {
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
Mat features(Mat in, int sizeData) {
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
    for (int i = 0; i < vhist.cols; i++) {
        out.at<float>(j) = vhist.at<float>(i);
        j++;
    }
    for (int i = 0; i < hhist.cols; i++) {
        out.at<float>(j) = hhist.at<float>(i);
        j++;
    }
    for (int x = 0; x < lowData.cols; x++) {
        for (int y = 0; y < lowData.rows; y++) {
            out.at<float>(j) = (float) lowData.at < unsigned
            char > (x, y);
            j++;
        }
    }
    return out;
}
