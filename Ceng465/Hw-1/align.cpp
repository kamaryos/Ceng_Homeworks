#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


#define INTERNAL_GAP -2
#define TERMINAL_GAP 0
#define MATCH 1
#define MISMATCH -3
#define MAXLENGTH 501
#define N_MATCH 0


int max(int A, int B, int C, bool flag)
{
    if (A>=B && A>=C && A>0) {flag = false;return A;}
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

  int index = 0;

  // std::vector<int> match(refseq_size,0);

  for (auto sr_line : lines) {

      bool flag = true;
      int count = 0;

      size_t line_size = sr_line.length();
      size_t ref_count = 0;

      size_t refseq_size = refseq.length();
      // std::cout<< "Dna started from " << ref_count << " to " << ref_count+line_size << std::endl;

      std::vector<std::vector<int>> matrix(line_size+1,std::vector<int>(refseq_size+1,0));

      for (size_t i = 1; i <= line_size; i++) {
        for (size_t j = 1; j <= refseq_size; j++) {

          if(j == refseq_size || flag == false)
            matrix[i][j] = max(matrix[i-1][j-1]+m(sr_line[i],refseq[j]),matrix[i][j-1]+TERMINAL_GAP,matrix[i-1][j]+TERMINAL_GAP,flag);
          else
            matrix[i][j] = max(matrix[i-1][j-1]+m(sr_line[i],refseq[j]),matrix[i][j-1]+INTERNAL_GAP,matrix[i-1][j]+INTERNAL_GAP,flag);
        }
      }

      // std::cout<<  matrix[line_size-1][line_size-1] <<"  "<< matrix[line_size][line_size] << std::endl;
      // if(matrix[line_size][refseq_size] >= 30) {count++;}
      ref_count += line_size;



    // std::cout<<"Line "<< index <<"is finished. Count is " << count << " !!" <<std::endl;
    // index++;
    
    // for (size_t i = 0; i < refseq_size; i++) {
    //   for (size_t j = 0; j < line_size; j++) {
    //     // if(matrix[j][i] > 10)
    //     std::cout << matrix[j][i] << " | ";
    //   }
    //   std::cout << std::endl;

    }


    std::cout << "" << std::endl << std::endl << std::endl;

  }
  // std::cout<<std::endl;

  // for (unsigned i = 0; i < lines.size(); ++i)
    // std::cout << lines[1] << std::endl;
}
