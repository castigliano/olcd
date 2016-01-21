#include <unordered_map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::unordered_map<int, std::set<int>> aadjht;
std::unordered_map<int, int> anodecommht;
int commidx = 0;
int m = 0;
int ma = 0;
int mb = 0;
int mc = 0;
int M;
std::unordered_map<int, int> acommdeght;
std::unordered_map<int, std::unordered_map<int, int>> anodecommdeght;
std::unordered_map<int, std::set<int>> acommnodeht;

void ExecuteOLEM(char* edgelistfile, char* resultfile, int NofEdge);
void ExecuteOLTM(char* edgelistfile, char* resultfile);
void Init(int NofEdge);
void Fini(char* resultfile);
bool AddEdge(int olabel, int dlabel);
bool IsExistNode(int label);
void NewEvent(int olabel, int dlabel);
void JoinEvent(int olabel, int dlabel);
void SplitEvent(int olabel, int dlabel);
void DenseEvent(int olabel, int dlabel, int ocidx, int dcidx);
void LinkEvent(int olabel, int dlabel, int ocidx, int dcidx);
void MoveEvent(int olabel, int ocidx, int dcidx);
double DeltaQJoinNode(int cdeg);
double DeltaQMoveNodeOLEM(int i, int a, int b);
double EMRT(double DeltaDeg);
long long DeltaQMoveNodeOLTM(int i, int a, int b);
int GetNodeToCommDeg(int i, int k);
void SetNewNodeToCommDeg(int i, int k);
void SetExistNodeToCommDeg(int i, int k);
void AddNodeToCommDeg(int i, int k);
void ReduceNodeToCommDeg(int i, int k);
int GetCIdxOfNode(int label);
void SetCIdxOfExistNode(int label, int cidx);
void SetCIdxOfNewNode(int label, int cidx);
int GetNodeDeg(int label);
int GetCommDeg(int cidx);
void SetNewCommDeg(int cidx, int deg);
void AddCommDeg(int cidx, int deg);
void ReduceCommDeg(int cidx, int deg);
void SetCommNodeHt();

int main(int argc, char** argv)
{
	if (argc >= 3)
	{
		if (argc == 3)
		{
			ExecuteOLTM(argv[1], argv[2]);
		}
		else
		{
			int NofEdge = 0;
			std::stringstream nestr(argv[3]);	
			nestr >> NofEdge;
			ExecuteOLEM(argv[1], argv[2], NofEdge);
		}
	}
	else
	{
		std::cout << "Usage: OnlineCommunityDetection.exe <EdgeListFile> <ResultFile> [<Number of Edges>]. Please refer to Readme.txt for detail." << std::endl;
	}
}

void ExecuteOLEM(char* edgelistfile, char* resultfile, int NofEdge)
{
	Init(NofEdge);
	std::cout << "Using OLEM Algorithm" << std::endl;
	std::cout << "Parsing " << edgelistfile << " start..." << std::endl;
	std::ifstream srs(edgelistfile);
	while (!srs.eof())
	{
		int olabel;
		int dlabel;
		srs >> olabel >> dlabel;
		bool isnewedge = AddEdge(olabel, dlabel);
		if (isnewedge)
		{
			m++;
			if (!IsExistNode(olabel) && !IsExistNode(dlabel))
			{
				mc++;
				NewEvent(olabel, dlabel);
			}
			else
			{
				if (!IsExistNode(olabel))
				{
					ma++;
					int dcdeg = GetCommDeg(GetCIdxOfNode(dlabel));
					double qJ = DeltaQJoinNode(dcdeg);
					if (qJ > 0.0)
					{
						JoinEvent(olabel, dlabel);
					}
					else
					{
						SplitEvent(olabel, dlabel);
					}
				}
				else
				{
					if (!IsExistNode(dlabel))
					{
						ma++;
						int ocdeg = GetCommDeg(GetCIdxOfNode(olabel));
						double qJ = DeltaQJoinNode(ocdeg);
						if (qJ > 0.0)
						{
							JoinEvent(dlabel, olabel);
						}
						else
						{
							SplitEvent(dlabel, olabel);
						}
					}
					else
					{
						mb++;
						int ocidx = GetCIdxOfNode(olabel);
						int dcidx = GetCIdxOfNode(dlabel);
						if (ocidx != dcidx)
						{
							LinkEvent(olabel, dlabel, ocidx, dcidx);
							double qO2D = DeltaQMoveNodeOLEM(olabel, ocidx, dcidx);
							double qD2O = DeltaQMoveNodeOLEM(dlabel, dcidx, ocidx);
							if (qD2O > 0.0)
							{
								if (qO2D > qD2O)
								{
									MoveEvent(olabel, ocidx, dcidx);
								}
								else
								{
									MoveEvent(dlabel, dcidx, ocidx);
								}
							}
							else
							{
								if (qO2D > 0.0)
								{
									MoveEvent(olabel, ocidx, dcidx);
								}
							}
						}
						else
						{
							DenseEvent(olabel, dlabel, ocidx, dcidx);
						}
					}
				}
			}
		}
	}
	srs.close();

	if (m != M)
	{
		std::cout << "*** Attention! Real Number of Edges " << m << " is NOT Equal to Input Number of Edges" << M << ".***" << std::endl;
	}

	std::cout << "Parsing " << edgelistfile << " finish " << std::endl;
	Fini(resultfile);

}

void ExecuteOLTM(char* edgelistfile, char* resultfile)
{
	Init(0);
	std::cout << "Using OLTM Algorithm" << std::endl;
	std::cout << "Parsing " << edgelistfile << " start " << std::endl;
	std::ifstream srs(edgelistfile);
	while (!srs.eof())
	{
		int olabel;
		int dlabel;
		srs >> olabel >> dlabel;
		bool isnewedge = AddEdge(olabel, dlabel);
		if (isnewedge)
		{
			m++;
			if (!IsExistNode(olabel) && !IsExistNode(dlabel))
			{
				NewEvent(olabel, dlabel);
			}
			else
			{
				if (!IsExistNode(olabel))
				{
					JoinEvent(olabel, dlabel);
				}
				else
				{
					if (!IsExistNode(dlabel))
					{
						JoinEvent(dlabel, olabel);
					}
					else
					{
						int ocidx = GetCIdxOfNode(olabel);
						int dcidx = GetCIdxOfNode(dlabel);
						if (ocidx != dcidx)
						{
							LinkEvent(olabel, dlabel, ocidx, dcidx);
							long long qO2D = DeltaQMoveNodeOLTM(olabel, ocidx, dcidx);
							long long qD2O = DeltaQMoveNodeOLTM(dlabel, dcidx, ocidx);
							if (qD2O > 0)
							{
								if (qO2D > qD2O)
								{
									MoveEvent(olabel, ocidx, dcidx);
								}
								else
								{
									MoveEvent(dlabel, dcidx, ocidx);
								}
							}
							else
							{
								if (qO2D > 0)
								{
									MoveEvent(olabel, ocidx, dcidx);
								}
							}
						}
						else
						{
							DenseEvent(olabel, dlabel, ocidx, dcidx);
						}
					}
				}
			}
		}
	}
	srs.close();

	std::cout << "Parsing " << edgelistfile << " finish " << std::endl;
	Fini(resultfile);

}

void Init(int NofEdge)
{
	M = NofEdge;
}

void Fini(char* resultfile)
{
	std::cout << "Writing results..." << std::endl;
	std::ofstream sw(resultfile);
	for (auto label = anodecommht.begin(); label != anodecommht.end(); ++label)
	{
		sw << label->first << "," << label->second << std::endl;
	}
	sw.close();

	std::cout << "Calculating modularity...(You can stop the program if you do not need to know modularity.)" << std::endl;

	for (auto label = anodecommht.begin(); label != anodecommht.end(); ++label)
	{
		int cidx = label->second;
		if (acommnodeht.find(cidx) == acommnodeht.end())
		{
			acommnodeht.insert(make_pair(cidx, std::set<int>()));
		}
		acommnodeht[cidx].insert(label->first);
	}

	double modularity = 0.0;
	for (auto cidx = acommnodeht.begin(); cidx != acommnodeht.end(); ++cidx)
	{
		double cmnq = 0.0;
		int cmnq1 = 0;
		int cmnq2 = 0;
		for (auto label = cidx->second.begin(); label != cidx->second.end(); ++label)
		{
			for (auto adjlabel = aadjht[*label].begin(); adjlabel != aadjht[*label].end(); ++adjlabel)
			{
				if (cidx->second.find(*adjlabel) != cidx->second.end())
				{
					cmnq1++;
				}
			}
		}
		cmnq1 = cmnq1 / 2;
		for (auto label = cidx->second.begin(); label != cidx->second.end(); ++label)
		{
			cmnq2 = cmnq2 + GetNodeDeg(*label);
		}
		cmnq = static_cast<double>(cmnq1) / static_cast<double>(m) - pow((static_cast<double>(cmnq2) / (2.0 * static_cast<double>(m))), 2.0);
		modularity = modularity + cmnq;
	}
	std::cout << "Modularity: " << modularity << std::endl;
}

bool AddEdge(int olabel, int dlabel)
{
	bool isnewedge = false;
	if (olabel != dlabel)
	{
		if (aadjht.find(olabel) == aadjht.end())
		{
			aadjht.insert(make_pair(olabel, std::set<int>()));
			isnewedge = true;
		}
		if (aadjht.find(dlabel) == aadjht.end())
		{
			aadjht.insert(make_pair(dlabel, std::set<int>()));
			isnewedge = true;
		}
		if (aadjht[olabel].find(dlabel) == aadjht[olabel].end())
		{
			aadjht[olabel].insert(dlabel);
			aadjht[dlabel].insert(olabel);
			isnewedge = true;
		}
	}
	return isnewedge;
}

bool IsExistNode(int label)
{
	return anodecommht.find(label) != anodecommht.end();
}

void NewEvent(int olabel, int dlabel)
{
	commidx++;
	SetCIdxOfNewNode(olabel, commidx);
	SetCIdxOfNewNode(dlabel, commidx);
	SetNewCommDeg(commidx, 2);
	SetNewNodeToCommDeg(olabel, commidx);
	SetNewNodeToCommDeg(dlabel, commidx);
}

void JoinEvent(int olabel, int dlabel)
{
	int dcidx = GetCIdxOfNode(dlabel);
	SetCIdxOfNewNode(olabel, dcidx);
	AddCommDeg(dcidx, 2);
	SetNewNodeToCommDeg(olabel, dcidx);
	AddNodeToCommDeg(dlabel, dcidx);
}

void SplitEvent(int olabel, int dlabel)
{
	int dcidx = GetCIdxOfNode(dlabel);
	commidx++;
	SetCIdxOfNewNode(olabel, commidx);
	AddCommDeg(dcidx, 1);
	SetNewCommDeg(commidx, 1);
	SetNewNodeToCommDeg(olabel, dcidx);
	SetExistNodeToCommDeg(dlabel, commidx);
}

void DenseEvent(int olabel, int dlabel, int ocidx, int dcidx)
{
	AddCommDeg(ocidx, 2);
	AddNodeToCommDeg(olabel, dcidx);
	AddNodeToCommDeg(dlabel, ocidx);
}

void LinkEvent(int olabel, int dlabel, int ocidx, int dcidx)
{
	AddCommDeg(dcidx, 1);
	AddCommDeg(ocidx, 1);
	AddNodeToCommDeg(olabel, dcidx);
	AddNodeToCommDeg(dlabel, ocidx);
}

void MoveEvent(int olabel, int ocidx, int dcidx)
{
	SetCIdxOfExistNode(olabel, dcidx);
	ReduceCommDeg(ocidx, GetNodeDeg(olabel));
	AddCommDeg(dcidx, GetNodeDeg(olabel));
	for (auto oadjlabel = aadjht[olabel].begin(); oadjlabel != aadjht[olabel].end(); ++oadjlabel)
	{
		AddNodeToCommDeg(*oadjlabel, dcidx);
		ReduceNodeToCommDeg(*oadjlabel, ocidx);
	}
}

double DeltaQJoinNode(int cdeg)
{
	double dq = static_cast<double>(2 * m - cdeg - 1) / (2.0 * static_cast<double>(m) * static_cast<double>(m));
	double emrt = EMRT(2 * cdeg + 2);
	return static_cast<double>(m) / static_cast<double>(M) * dq + emrt;
}

double DeltaQMoveNodeOLEM(int i, int a, int b)
{
	double dq = static_cast<double>(GetNodeToCommDeg(i, b) - GetNodeToCommDeg(i, a)) / static_cast<double>(m) + static_cast<double>(GetCommDeg(a) - GetCommDeg(b) - GetNodeDeg(i)) * static_cast<double>(GetNodeDeg(i)) / (2.0 * static_cast<double>(m) * static_cast<double>(m));
	double emrt = EMRT(4.0 * static_cast<double>(GetCommDeg(b) - GetCommDeg(a) + GetNodeDeg(i) - 1) * static_cast<double>(GetNodeDeg(i) - 1));
	return static_cast<double>(m) / static_cast<double>(M) * dq + emrt;
}

double EMRT(double DeltaDeg)
{
	double pa = static_cast<double>(ma) / static_cast<double>(m);
	double pb = static_cast<double>(mb) / static_cast<double>(m);
	double pc = static_cast<double>(mc) / static_cast<double>(m);
	double res = (-pc - pa) * static_cast<double>(M - m) / (4.0 * static_cast<double>(M) * static_cast<double>(m) * static_cast<double>(m)) * DeltaDeg + (2.0 - pb) * log(static_cast<double>(M) / static_cast<double>(m)) / (8.0 * static_cast<double>(M) * static_cast<double>(m) * static_cast<double>(m)) * DeltaDeg;
	return res;
}

long long DeltaQMoveNodeOLTM(int i, int a, int b)
{
	return 2 * static_cast<long long>(m) * static_cast<long long>(GetNodeToCommDeg(i, b) - GetNodeToCommDeg(i, a)) + static_cast<long long>(GetCommDeg(a) - GetCommDeg(b) - GetNodeDeg(i)) * static_cast<long long>(GetNodeDeg(i));
}

int GetNodeToCommDeg(int i, int k)
{
	int res = 0;
	if (anodecommdeght[i].find(k) != anodecommdeght[i].end())
	{
		res = anodecommdeght[i][k];
	}
	return res;
}

void SetNewNodeToCommDeg(int i, int k)
{
	std::unordered_map<int, int> acommht; 
	acommht.insert(std::make_pair(k, 1));
	anodecommdeght.insert(make_pair(i, acommht));
}

void SetExistNodeToCommDeg(int i, int k)
{
	anodecommdeght[i].insert(std::make_pair(k, 1));
}

void AddNodeToCommDeg(int i, int k)
{
	if (anodecommdeght[i].find(k) != anodecommdeght[i].end())
	{
		anodecommdeght[i][k] = anodecommdeght[i][k] + 1;
	}
	else
	{
		anodecommdeght[i].insert(std::make_pair(k, 1));
	}
}

void ReduceNodeToCommDeg(int i, int k)
{
	anodecommdeght[i][k] = anodecommdeght[i][k] - 1;
}

int GetCIdxOfNode(int label)
{
	return anodecommht[label];
}

void SetCIdxOfExistNode(int label, int cidx)
{
	anodecommht[label] = cidx;
}

void SetCIdxOfNewNode(int label, int cidx)
{
	anodecommht.insert(std::make_pair(label, cidx));
}

int GetNodeDeg(int label)
{
	return aadjht[label].size();
}

int GetCommDeg(int cidx)
{
	return acommdeght[cidx];
}

void SetNewCommDeg(int cidx, int deg)
{
	acommdeght.insert(std::make_pair(cidx, deg));
}

void AddCommDeg(int cidx, int deg)
{
	acommdeght[cidx] = acommdeght[cidx] + deg;
}

void ReduceCommDeg(int cidx, int deg)
{
	acommdeght[cidx] = acommdeght[cidx] - deg;
}