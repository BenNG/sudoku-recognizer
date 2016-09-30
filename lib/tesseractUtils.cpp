/**
 *
 * not sure it works but it is not important
 *
 * */


char *identifyText(Mat input, char *language = "eng") {
    tesseract::TessBaseAPI ocr;
    ocr.Init("./tessdata", language, tesseract::OEM_TESSERACT_ONLY);
    ocr.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    ocr.SetImage(input.data, input.cols, input.rows, 1,
                 input.step);
    ocr.SetVariable("tessedit_char_whitelist", "0123456789");

    char *text = ocr.GetUTF8Text();
//    cout << "Text:" << endl;
//    cout << text << endl;
//    cout << "Confidence: " << ocr.MeanTextConf() << endl << endl;
// Get the text

    return text;
}

/**
 * pre training
 *
 * ├── data
│*   ├── 1
│*   ├── 2
│*   ├── 3
│*   ├── 4
│*   ├── 5
│*   ├── 6
│*   ├── 7
│*   ├── 8
│*   └── 9
 *
 * create file image
 * An input representing a 4 will be saved in 4 if recognize if not in the parent folder
 *
 * */
void createImagesForTraining(Mat input, int fileNumber, int cellNumber) {

    string number = identifyText(input);

    std::stringstream trimmer;
    trimmer << number;
    number.clear();
    trimmer >> number;

    cout << "number is:" << number << endl;

    std::stringstream ss;
    if (number == "") {
        ss << "./data/s" << fileNumber << "-" << cellNumber << ".jpg";
    } else {
        ss << "./data/" << number << "/s" << fileNumber << "-" << cellNumber << ".jpg";
    }

    std::string filename = ss.str();
    cout << filename << endl;

    imwrite(filename, input);
}

char *identifyText(Mat input, char *language = "eng") {
    tesseract::TessBaseAPI ocr;
    ocr.Init("./tessdata", language, tesseract::OEM_TESSERACT_ONLY);
    ocr.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    ocr.SetImage(input.data, input.cols, input.rows, 1,
                 input.step);
    ocr.SetVariable("tessedit_char_whitelist", "0123456789");

    char *text = ocr.GetUTF8Text();
//    cout << "Text:" << endl;
//    cout << text << endl;
//    cout << "Confidence: " << ocr.MeanTextConf() << endl << endl;
// Get the text

    return text;
}

/**
 * pre training
 *
 * ├── data
│*   ├── 1
│*   ├── 2
│*   ├── 3
│*   ├── 4
│*   ├── 5
│*   ├── 6
│*   ├── 7
│*   ├── 8
│*   └── 9
 *
 * create file image
 * An input representing a 4 will be saved in 4 if recognize if not in the parent folder
 *
 * */
void createImagesForTraining(Mat input, int fileNumber, int cellNumber) {

    string number = identifyText(input);

    std::stringstream trimmer;
    trimmer << number;
    number.clear();
    trimmer >> number;

    cout << "number is:" << number << endl;

    std::stringstream ss;
    if (number == "") {
        ss << "./data/s" << fileNumber << "-" << cellNumber << ".jpg";
    } else {
        ss << "./data/" << number << "/s" << fileNumber << "-" << cellNumber << ".jpg";
    }

    std::string filename = ss.str();
    cout << filename << endl;

    imwrite(filename, input);
}
