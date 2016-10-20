#include "knn.h"

int readFlippedInteger(FILE *fp)
{
    int ret = 0;

    BYTE *temp;

    temp = (BYTE*)(&ret);
    fread(&temp[3], sizeof(BYTE), 1, fp);
    fread(&temp[2], sizeof(BYTE), 1, fp);
    fread(&temp[1], sizeof(BYTE), 1, fp);

    fread(&temp[0], sizeof(BYTE), 1, fp);

    return ret;
}

Ptr<ml::KNearest> getKnn(){
  Ptr<ml::KNearest>  knn(ml::KNearest::create());

  FILE *fp = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-images-idx3-ubyte", "rb");
  FILE *fp2 = fopen("/keep/Repo/USELESS/_sandbox/cpp/learning-cpp/sudoku/assets/train-labels-idx1-ubyte", "rb");

  if(!fp || !fp2){
    cout << "can't open file" << endl;
  }

  int magicNumber = readFlippedInteger(fp);
  int numImages = readFlippedInteger(fp);
  int numRows = readFlippedInteger(fp);
  int numCols = readFlippedInteger(fp);
  fseek(fp2, 0x08, SEEK_SET);

  int size = numRows*numCols;

  cout << "size: " << size << endl;
  cout << "rows: " << numRows << endl;
  cout << "cols: " << numCols << endl;

  Mat_<float> trainFeatures(numImages, size);
  Mat_<int> trainLabels(1,numImages);

  BYTE *temp = new BYTE[size];
  BYTE tempClass=0;
  for(int i=0;i<numImages;i++)
  {
      fread((void*)temp, size, 1, fp);
      fread((void*)(&tempClass), sizeof(BYTE), 1, fp2);

      trainLabels[0][i] = (int)tempClass;

      for(int k=0;k<size;k++){
          trainFeatures[i][k] = (float)temp[k];
      }
  }
  knn->train(trainFeatures, ml::ROW_SAMPLE, trainLabels);
  
  Mat_<float> test(numRows, numCols);

  // Mat_<float> testFeature(1, size);
  int expected_resp = trainLabels[0][5];

  Mat_<float> testFeature = trainFeatures.row(5);
  Mat_<float> testImage = createMatFromMNIST( trainFeatures.row(5) );

  showImage(testImage);



  int K=1;
  Mat response,dist;
  knn->findNearest(testFeature, K, noArray(), response, dist);
  cerr << response << endl;
  cerr << dist<< endl;

  cout << "expected: " << expected_resp << endl;
  return knn;
}
