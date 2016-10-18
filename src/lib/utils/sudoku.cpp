#include "sudoku.h"

/**
 * this function do too much things need to refact some day
 * */
string grab(string fileName){

    fs::path image_path(getPath(fileName));

    string image_path_str = image_path.string();

//    cout << image_path_str << endl;

    Mat image = imread(image_path_str, 0); // Read the file

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl;
        return "";
    }
    
    int cellValue;
    Mat cellFeatured;
    std::stringstream response;

    string message;
    fs::path featured(getPath("assets/featuredDataForTraining.xml"));
    fs::path trained_data(getPath("assets/trained_data"));
//
//    cout << featured << endl;
//    cout << trained_data << endl;



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

    Ptr<ANN_MLP> model = build_mlp_classifier(featured, trained_data);

    // Mat preprocessed = preprocess(image.clone());
    // vector<Point> biggestApprox = findBigestApprox(preprocessed);
    Mat sudoku = extractPuzzle(image);


    for (unsigned i = 0; i < 81; i++) {
        Mat cell = extractCell(sudoku, i), prepared_cell;
        prepared_cell = prepareCell(cell);
        Mat roi = extractNumber(prepared_cell);

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
    // trainingDataf5;
    // trainingDataf10;
    // trainingDataf15;
    // trainingDataf20;
    fs["trainingDataf15"] >> data;
    fs["classes"] >> responses;
    // fn - end

    // // manual test
    // Mat sample = data.row(100);
    // float r = model->predict(sample);
    // cout << "r:" << r << endl;

    string resp = response.str();
    showImage(sudoku);
    return resp;
}
