#include "sudoku.h"

/**
 * this function do too much things need to refact some day
 * */
string grab(Mat image){

    int cellValue;
    Mat cellFeatured;
    std::stringstream response;

    string message;
    fs::path featured("../assets/featuredDataForTraining.xml");
    fs::path trained_data("../assets/trained_data");

    if (!fs::exists(featured) && !fs::exists(trained_data)) {
        createData();

        cout << "Go clean the data folder !! say 'ok' when you are ready "  << endl;
        cin >> message;
        if(message == "ok"){
            std::cout << "2nd: randomize the file because we will use 80% of the data and 20% to test the network" << std::endl;
            randomizeData();
            std::cout << "3rd: extract feature for each cell and save it in featuredDataForTraining.xml" << std::endl;
            featurizeData();
        }
    }

    Ptr<ANN_MLP> model = build_mlp_classifier("../assets/featuredDataForTraining.xml", "../assets/trained_data");

    Mat preprocessed = preprocess(image.clone());
    vector<Point> biggestApprox = findBigestApprox(preprocessed);
    Mat sudoku = extractPuzzle(image, biggestApprox);

    for (unsigned i = 0; i < 81; i++) {
        Mat cell = getCell(sudoku, i), cell_no_noise, cell_no_light, final_cell;

        // remove noise
        medianBlur(cell, cell_no_noise, 1);
        // remove background/light
        cell_no_light = removeLight(cell_no_noise, calculateLightPattern(cell), 2);
        // binarize image
        adaptiveThreshold(cell_no_light, final_cell, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);

        Mat roi = extractRoiFromCell(final_cell);

        if (roi.empty()) {
            response << "0";
        } else {
            cellFeatured = features(roi, 15);
            cellValue = model->predict(cellFeatured);
            // cout << "r:" << cellValue + 1 << endl;
            response << cellValue + 1;
            // showImage(roi);
        }
    }

    Mat data;
    Mat responses;

    // fn
    FileStorage fs;
    fs.open("../assets/featuredDataForTraining.xml", FileStorage::READ);
    fs["TrainingDataF15"] >> data;
    fs["classes"] >> responses;
    // fn - end

    // // manual test
    // Mat sample = data.row(100);
    // float r = model->predict(sample);
    // cout << "r:" << r << endl;

    return response.str();
}