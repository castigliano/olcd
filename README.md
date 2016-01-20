# olcd
Online Community Detection for Large Complex Network

INTRODUCTION
This source code is the C++ implementation of our online community detection method for large complex networks. The latest version can be found at http://www.cs.zju.edu.cn/people/gpan/. 

HOW TO COMPILE
Windows: This source code should be sufficient to compile the executable in Visual C++ 2010 Express or similar IDE.
Linux: g++ -std=c++11 -O3 OnlineCD.cpp

HOW TO RUN
The program handles networks in the form of a flat text file containing edge adjacencies (a list of tab-delimited pairs of numeric indices, e.g., "12\t16\n"). Demo dataset file can be found at "DemoDatasetFolder" folder.

Run the program like: 
.\OnlineCD.exe  <EdgeListFile> <ResultFile> [<Number of Edges>]

<EdgeListFile> is the file path to input edge list file. e.g., .\DemoDatasetFolder\DemoEdgeList.txt  
<ResultFile> is the file path for the program to output partition result. e.g., .\DemoDatasetFolder\PartitionResult.txt 
<Number of Edges> is the number of edges in edge list file and it is optional. If it is added, the program will use OLEM algorithm, otherwise OLTM algorithm. 

The program will output partition result of edge list file (a list of comma-delimited pairs of node and community it belongs to) to <ResultFile> and modularity of partition result to console.

If you have any further questions, please feel free to contact zws10@zju.edu.cn

CITATION
If you use this source code, please cite our paper: Pan G, Zhang W, Wu Z, Li S (2014) Online Community Detection for Large Complex Networks. PLoS ONE 9(7): e102799. doi:10.1371/journal.pone.0102799
