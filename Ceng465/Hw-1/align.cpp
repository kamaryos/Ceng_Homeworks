#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple> // for tuple


#define INTERNAL_GAP -2
#define TERMINAL_GAP 0
#define MATCH 1
#define MISMATCH -3
#define MAXLENGTH 501
#define N_MATCH 0


int max(int A, int B, int C)
{
    if (A>=B && A>=C && A>0) return A;
    else if (B>=A && B>=C && B>0) return B;
    else if (C > 0) return C;
    else return 0;
}

int m(char p, char q)
{
    if( p=='N' || q =='N') return N_MATCH;
    else if (p==q) return MATCH;
    else return MISMATCH;
}

int main(int argc, char *argv[]){

  if(argc < 3){
    printf("Too few arguments!\n");
    printf("First argument is refseq and second argument is shortread!\n");
    exit(1);
  }


  std::ifstream myfile_1 (argv[1]);
  std::string refseq( (std::istreambuf_iterator<char>(myfile_1) ),
                       (std::istreambuf_iterator<char>()    ) );


  std::string line;
  std::vector<std::string> lines;
  std::ifstream myfile_2 (argv[2]);

  if (myfile_2.is_open())
  {
    while (! myfile_2.eof() )
    {
      getline (myfile_2,line);
      lines.push_back(line);
    }
      myfile_2.close();
  }
  else std::cout << "Unable to open file 2\n";

  // int index = 0;

  size_t refseq_size = refseq.length();
  std::vector<int> match(refseq_size,0);
  std::vector<int> start_end ;

  for (auto sr_line : lines) {

      bool flag = true;
      // int count = 0;
      size_t start = 0;
      size_t end = 0;

      size_t line_size = sr_line.length();
      // size_t ref_count = 0;


      std::vector<std::vector<int>> matrix(line_size+1,std::vector<int>(refseq_size+1,0));


      for (size_t i = 1; i <= line_size; i++) {
        for (size_t j = 1; j <= refseq_size; j++) {
          if(j == refseq_size ){
            matrix[i][j] = max(matrix[i-1][j-1]+m(sr_line[i],refseq[j]),matrix[i][j-1]+TERMINAL_GAP,matrix[i-1][j]+TERMINAL_GAP);
            if(flag == false){

              if(matrix[i][j] == matrix[i-1][j-1]+m(sr_line[i],refseq[j])) { start = line_size*i+j -2; flag =true;}
              else if(matrix[i][j] == matrix[i][j-1]+TERMINAL_GAP){ start = line_size*i+j -1; flag =true;}
              else if(matrix[i][j] == matrix[i-1][j]+TERMINAL_GAP){ start = line_size*(i-1)+j -1; flag =true;}

            }
            if(matrix[i][j] >= 30){
                end = i*line_size + j;
                flag = false;
                start_end.push_back(start);
                start_end.push_back(end);
            }
          }
          else{
            matrix[i][j] = max(matrix[i-1][j-1]+m(sr_line[i],refseq[j]),matrix[i][j-1]+INTERNAL_GAP,matrix[i-1][j]+INTERNAL_GAP);
            if(flag == false){

              if(matrix[i][j] == matrix[i-1][j-1]+m(sr_line[i],refseq[j])) { start = line_size*i+j -2; flag =true;}
              else if(matrix[i][j] == matrix[i][j-1]+INTERNAL_GAP){ start = line_size*i+j -1; flag =true;}
              else if(matrix[i][j] == matrix[i-1][j]+INTERNAL_GAP){ start = line_size*(i-1)+j -1; flag =true;}

            }
            if(matrix[i][j] >= 30){
                end = i*line_size + j;
                flag = false;
                start_end.push_back(start);
                start_end.push_back(end);
            }
          }
        }
      }



    // std::cout<<"Line "<< index <<"is finished. Count is " << count << " !!" <<std::endl;
    // index++;

    }
    for (size_t i = 0; i < start_end.size(); i+=2) {
      size_t start = start_end[i];
      size_t end = start_end[i+1];

      for (size_t j = start; j <= end; j++) {
        match[j] += 1;

      }

    }

    for (size_t i = 0; i < match.size(); i++) {
      std::cout << match[i] << '|' << std::endl;
    }
    // std::cout << "" << std::endl << std::endl << std::endl;

  // std::cout<<std::endl;

  // for (unsigned i = 0; i < lines.size(); ++i)
    // std::cout << lines[1] << std::endl;
}
