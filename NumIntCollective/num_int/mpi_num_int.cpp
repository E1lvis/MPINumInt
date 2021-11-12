#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

double partialIntegrations(int functionid, double a, double b, int n, int i, int intensity ){
  //partial integrations

  double fraction = (b - a) / n; //this is calculating the width
    //double second = 0; //this varibale is for the area
    //double sum = 0; // sum of rectangles
  float (*functionInUse)(float, int);

  switch (functionid)
    {
    case 1:
      functionInUse = f1;
      break;
    case 2:
      functionInUse = f2;
      break;
    case 3:
      functionInUse = f3;
      break;
    case 4:
      functionInUse = f4;
      break;
    default:

      break;
    }


	return functionInUse(a + (i+0.5)*fraction, intensity) * fraction;
}

double integrate(int functionid, double a, double b, int n, int intensity) {
//fully integrates 

    double fraction = (b - a) / n; //this is calculating the width
    //double second = 0; //this varibale is for the area
    double sum = 0; // sum of rectangles
    float (*functionInUse)(float, int);

    switch (functionid)
    {
    case 1:
      functionInUse = f1;
      break;
    case 2:
      functionInUse = f2;
      break;
    case 3:
      functionInUse = f3;
      break;
    case 4:
      functionInUse = f4;
      break;
    default:

      break;
    }

    for (int i = 0; i < n; i++){
      
      sum+= fraction * functionInUse(a + (i+0.5)*fraction, intensity);

}

return sum;

}
int main (int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
  int totalProcesses;
  int processNumber;
   
  int functionid = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  int partitions;

  MPI_Get_size(MPI_COMM_WORLD, &totalProcesses);
  MPI_Get_rank(MPI_COMM_WORLD, &processNumber);

  partitions = n / totalProcesses;
  double sum = 0;
  std::vector<double> individualSums(1);
  std::vector<double> finalSum(1);

  int localstart = processNumber * partitions;

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  //run itegration here 
  int count = 0;
  for(int i = 0; i < n; i++){

  sum += partialIntegrations(functionid, a, b, n, i, intensity);
  count++;
  if(count == partitions ){
	break;
	}

  }
  //end of integration

  individualSums[0] = sum;

  MPI_Reduce(&(individualSums[0]), &(finalSum[0]), 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  int finalValue = finalSum[0];
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;

  //print out integration value
  std::cout<<finalValue;
  std::cerr<<elapsed_seconds.count()<<std::endl;

  MPI_Finalize();
  return 0;
}
