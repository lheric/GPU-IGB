//
// SNPCov --  compute the covariance of the SNP's for
//				a group of genes.
//
//		Optionally, it will read a set of (snp,pvalues) and write the 
//	needed ones to a file. This is only used to show that VEGAS is converging.
//	The real algorithm will use the ANOVA to produce the pvalues.
//

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <assert.h>

#include "Eigen/Eigen"
#include <Eigen/Core>
#include <Eigen/SVD>

using namespace Eigen;
using namespace std;

// ==============================================================================
//
// ==============================================================================

typedef struct SNPNode
{
	string name;
	vector<char> vals;
} SNPNode_t;

typedef struct GeneNode
{
	string GeneName;
	vector<SNPNode_t> SNP;
} GeneNode_t;

typedef struct GenotypeMatrix
{
	vector<string> StrList;
	int RowCount;
	int ColumnCount;

} GenotypeMatrix_t;

vector<string> split(const string &text, char sep)
{
	vector<string> tokens;
	size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

// ==============================================================================
//
// ==============================================================================

list<GeneNode_t> ReadGeneSnpFile(string GeneSnpFileName)
{
	string line;
	int linecount = 0;

	cout << endl << endl << "ReadGeneSnpFile" << endl;

	ifstream infile(GeneSnpFileName);
	if (!infile.is_open())
	{
		cout << "Error opening GeneSnpFile " << GeneSnpFileName << endl << endl;
		exit(1);
	}

	list<GeneNode_t> GNList;
	GeneNode_t GN;
	SNPNode_t SN;

	while (getline(infile, line))
	{
		linecount++;
		if (linecount == 1)
			continue;			// skip the header

		vector<string> tokens = split(line, '\t');
//		cout << "ReadGeneSnpFile line " << linecount << ", tokencount " << tokens.size() << endl;

		if (GN.GeneName.compare(tokens[5]) != 0)	// start a new gene
		{
			if (linecount > 2) GNList.push_back(GN);
			GN.GeneName = tokens[5];
			GN.SNP.clear();
			SN.name = tokens[1];
			GN.SNP.push_back(SN);
		}
		else
		{
			SN.name = tokens[1];
			GN.SNP.push_back(SN);
		}
	}

	GNList.push_back(GN);

	cout << linecount << " lines in GeneSnpFile" << endl;
	cout << GNList.size() << " genes in the file" << endl;

	return GNList;
}

// ==============================================================================
//
// ==============================================================================

GenotypeMatrix_t ReadSnpFile(string SnpFileName)
{
	string line;
	GenotypeMatrix_t GTM;

	int ColumnCount = 0;
	int RowCount = 0;

	cout << endl << endl << "ReadSnpFile" << endl;

	{
		ifstream infile(SnpFileName);
		if (!infile.is_open())
		{
			cout << "Error opening SnpFile " << SnpFileName << endl << endl;
			exit(1);
		}

		// We are going to read the first line and count the tokens.
		// Then we will continue counting lines.
		// When are done, we can allocate a matrix of the appropriate size.

		while (getline(infile, line))
		{
			RowCount++;
			if (RowCount == 1)
			{
				vector<string> tokens = split(line, '\t');
				ColumnCount = (int)tokens.size();
			}
		}
		infile.close();
	}

	// Reopen the file and process it into the matrix.

	vector<string> SnpList;
	SnpList.resize(ColumnCount*RowCount);

	ifstream infile(SnpFileName);
	if (!infile.is_open())
	{
		cout << "Error opening SnpFile " << SnpFileName << endl << endl;
		exit(1);
	}

	int linecount = -1;
	while (getline(infile, line))
	{
		linecount++;
		vector<string> tokens = split(line, '\t');
		for (int c = 0; c < tokens.size(); c++)
		{
			SnpList[linecount*ColumnCount + c] = tokens[c];
		}
//		cout << "ReadSnpFile line " << linecount << ", tokencount " << tokens.size() << endl;
	}

	cout << ColumnCount << " columns in the matrix" << endl;
	cout << RowCount << " rows in the matrix" << endl;
	cout << linecount << " lines in SnpFile" << endl;

	GTM.RowCount = RowCount;
	GTM.ColumnCount = ColumnCount;
	GTM.StrList = SnpList;

	return GTM;
}

// ==============================================================================
// This function will only work if the matrix is symmetric positive definite.
// Unfortuneately, covariance matrices are symmetric positive semidefinite.
// ==============================================================================
void Choleskyp(double *Ain, double *Lout, int n)
{

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < (i + 1); j++)
		{
			double s = 0;
			for (int k = 0; k < j; k++)
			{
				s += Lout[i * n + k] * Lout[j * n + k];
			}
			Lout[i * n + j] = (i == j) ? sqrt(Ain[i * n + i] - s) : (1.0 / Lout[j * n + j] * (Ain[i * n + j] - s));
		}
	}
}

//
// From Golub's book titled 'Matrix Computations'.
// Section 4.2.8 , page 147, 'The Semidefinite Case'
//
void CholeskyGolub(double *Ain, double *Lout, int n)
{
	const double epsilon = 1E-8;

	for (int k = 0; k < n; k++)
	{
		for (int i = 0; i < k+1; i++)
			Lout[k * n + i] = Ain[k * n + i];
	}

	for (int k = 0; k < n; k++)
	{
		if (Lout[k*n + k] > 0)
		{
			Lout[k*n + k] = sqrt(Lout[k*n + k]);

			for (int m = k + 1; m < n; m++)
				Lout[m*n + k] = Lout[m*n + k] / Lout[k*n + k];

			for (int j = k + 1; j < n; j++)
			{
				for (int m = j; m < n; m++)
					Lout[m*n + j] = Lout[m*n + j] - Lout[m*n + k] * Lout[j*n + k];
			}
		}
	}
}

void Factor(double *Ain, double *Lout, int n)
{
	MatrixXd C;

	C.resize(n, n);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			C(i, j) = Ain[i*n + j];
		}
	}

	JacobiSVD<MatrixXd> svd(C, ComputeThinU | ComputeThinV);
	MatrixXd U = svd.matrixU();
	MatrixXd S = svd.singularValues().asDiagonal().toDenseMatrix(); /// svd.singularValues() returns a vector. you may want to keep it. I convert it to dense matrix for print

	MatrixXd CRecon = U * S.cwiseSqrt();

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			Lout[i*n + j] = CRecon(i, j);
		}
	}
}

void ComputeSNPCovariance(list<GeneNode_t> &GL, GenotypeMatrix_t &GTM, string CovFileName)
{
	// First copy the data from the GTM to the vectors in GL.
	// This will make it faster to compute the variance.
	// It also allows us to clean up the data during the transfer.
	cout << endl << endl << "ComputeSNPCovariance" << endl;

	for (list<GeneNode_t>::iterator git = GL.begin(); git != GL.end(); git++)
	{
//		cout << "ComputeSNPCovariance Gene= " << git->GeneName << endl;

		for (int j = 0; j < git->SNP.size(); j++)
		{

			int k = 6;
			for (; k < GTM.ColumnCount; k++)
				if (git->SNP[j].name.compare(GTM.StrList[k]) == 0)
					break;

			if (k == GTM.ColumnCount)
				cout << "** Warning can't find SNP=" << git->SNP[j].name << " in the header row of GTM";
			else
			{
				git->SNP[j].vals.resize(GTM.RowCount-1);	// dont need to account for the header

				int ZeroCount = 0;
				int OneCount = 0;
				int TwoCount = 0;

				for (int m = 1; m < GTM.RowCount; m++)
				{
					char V = 'X';
					if (GTM.StrList[GTM.ColumnCount*m + k].compare("0") == 0) {V = 0; ZeroCount++;}
					else
					if (GTM.StrList[GTM.ColumnCount*m + k].compare("1") == 0) {V = 1; OneCount++;}
					else
					if (GTM.StrList[GTM.ColumnCount*m + k].compare("2") == 0) {V = 2; TwoCount++;}

					git->SNP[j].vals[m - 1] = V;
				}
				//
				// We've copied the column from the GTM to a vector associated with a SNP.
				// There might have been crap in the GTM matrix. So make a pass over the 
				// new data and replace the crap with the most frequent value.

				if (ZeroCount + OneCount + TwoCount != git->SNP[j].vals.size())
				{
					char V = 0;		// vegas2 script replaces nan's with 0
					//if (OneCount > ZeroCount)
					//{
					//	V = 1;
					//	if (TwoCount > OneCount)
					//		V = 2;
					//}
					//else
					//{
					//	if (TwoCount > ZeroCount) V = 2;
					//}

					for (int m = 0; m < git->SNP[j].vals.size(); m++)
					{
						if (git->SNP[j].vals[m] == 'X')
							git->SNP[j].vals[m] = V;
					}
				}
			}
		}
	}

	// Now compute the variance and write each matrix to one line of
	// the CovFileName.

	ofstream outfile(CovFileName);

	if (!outfile.is_open())
	{
		cout << "Error opening CovFileName " << CovFileName << endl << endl;
		exit(1);
	}

	for (list<GeneNode_t>::iterator git = GL.begin(); git != GL.end(); git++)
	{
		const int SNPCount = (int)git->SNP.size();

		cout << "ComputeSNPCovariance Gene= " << git->GeneName << " SNPCount= " << SNPCount << endl;
		outfile << "# Gene=" << git->GeneName << " SNPCount= " << SNPCount << endl;

		double *Pin = new double[SNPCount*SNPCount];
		double *Pout = new double[SNPCount*SNPCount];
		int Inx = 0;

		for (int j = 0; j < SNPCount; j++)
		{
			for (int k = 0; k < SNPCount; k++)
			{
				if (git->SNP[j].vals.size() != git->SNP[k].vals.size())
				{
					cout << "** Warning Gene " << git->GeneName << "has different length snp vectors" << endl;
					cout << "\t\t SNP= " << git->SNP[j].name << " size= " << git->SNP[j].vals.size() << endl;
					cout << "\t\t SNP= " << git->SNP[k].name << " size= " << git->SNP[k].vals.size() << endl; 
					outfile << "** ";
				}
				else
				{
					int length = (int) git->SNP[j].vals.size();

					double cov = 0.0;
					if (length > 1)
					{
						double K1 = git->SNP[j].vals[0];
						double K2 = git->SNP[k].vals[0];
						double Ex = 0;
						double Ey = 0;
						double Exy = 0;
						for (int m = 0; m < length; m++)
						{
							double X1 = git->SNP[j].vals[m];
							double X2 = git->SNP[k].vals[m];

							Ex += (X1 - K1);
							Ey += (X2 - K2);
							Exy += (X1 - K1)*(X2 - K2);
						}
						cov = (Exy - Ex*Ey / (double)length) / (double)length;

						// variance of SNP[j]
						{
						double nj = 0;
						double meanj = 0.0;
						double MJ = 0.0;
						for (int m = 0; m < length; m++)
						{
							nj++;
							double DX = git->SNP[j].vals[m] - meanj;
							meanj += DX / nj;
							double DX2 = git->SNP[j].vals[m] - meanj;
							MJ += DX*DX2;
						}
						cov = cov / sqrt((MJ / (nj - 1)));
						}
						// variance of SNP[k]
						{
							double nj = 0;
							double meanj = 0.0;
							double MJ = 0.0;
							for (int m = 0; m < length; m++)
							{
								nj++;
								double DX = git->SNP[k].vals[m] - meanj;
								meanj += DX / nj;
								double DX2 = git->SNP[k].vals[m] - meanj;
								MJ += DX*DX2;
							}
							cov = cov / sqrt((MJ / (nj - 1)));
						}
					}
//					outfile << std::fixed << std::setprecision(6) << cov << " ";
					Pin[Inx++] = cov;
				}
			}
		}

		memset(Pout, 0, SNPCount*SNPCount * sizeof(double));

//		CholeskyGolub(Pin, Pout, SNPCount);
		Factor(Pin, Pout, SNPCount);

		// The output file will have 1 line with a comment on it, a line below
		// with the covariance matrix, and the final line is the cholesky factorization.
		// I put it in this format to make it easy to parse.

		for (int i = 0; i < SNPCount*SNPCount; i++)
			outfile << std::fixed << std::setprecision(6) << Pin[i] << " ";
		outfile << endl;

		for (int i = 0; i < SNPCount*SNPCount; i++)
			outfile << std::fixed << std::setprecision(6) << Pout[i] << " ";

		outfile << endl;

		delete[] Pin;
		delete[] Pout;
	}

	outfile.close();
}

// ==============================================================================
//
// ==============================================================================

void CreateSNPpvaluesFile(list<GeneNode_t> &GL, string SnpPvaluesInFileName, string SnpPvaluesOutFileName)
{
	cout << endl << endl << "CreateSNPpvaluesFile" << endl;
	cout << "\tReading input file" << endl;

	// Load the (SNP, pvalue) pairs into a map.
	map<string, string> SnpPvalueMap;

	string line;
	{
		ifstream infile(SnpPvaluesInFileName);
		if (!infile.is_open())
		{
			cout << "Error opening SnpFile " << SnpPvaluesInFileName << endl << endl;
			exit(1);
		}

		int LineCount = 0;
		while (getline(infile, line))
		{
			vector<string> tokens = split(line, '\t');
			int count = (int)tokens.size();
			if (count != 2)
			{
				cout << "Line " << LineCount << " of " << SnpPvaluesInFileName << "has " << count;
				cout << "tokens. I expected 2 tokens." << endl << endl;
				exit(1);
			}

			pair<map<string, string>::iterator, bool> ret;
			ret = SnpPvalueMap.insert(pair<string,string>(tokens[0], tokens[1]));
			if (ret.second == false) 
			{
				cout << "Line " << LineCount << " of " << SnpPvaluesInFileName << "has duplicate input snp ";
				cout << tokens[0] << endl << endl;
				exit(1);
			}

			LineCount++;
		}
		infile.close();
	}
	
	//
	// Now write the output file.
	//
	ofstream outfile(SnpPvaluesOutFileName);

	if (!outfile.is_open())
	{
		cout << "Error opening SnpPvaluesOutFileName " << SnpPvaluesOutFileName << endl << endl;
		exit(1);
	}

	cout << "\tWriting output file" << endl;

	for (list<GeneNode_t>::iterator git = GL.begin(); git != GL.end(); git++)
	{
		for (int j = 0; j < git->SNP.size(); j++)
		{
			map<string, string>::iterator it;
			it = SnpPvalueMap.find(git->SNP[j].name);

			// This shouldnt happen.
			if (it == SnpPvalueMap.end())
			{
				cout << "Gene  " << git->GeneName << " has no snps in map. " << endl << endl;
				exit(1);
			}
			
			outfile << it->first << " " << it->second << endl;
		}
	}

	outfile.close();
}

// ==============================================================================
//
// ==============================================================================
int main()
{
	const char *SnpFileName = "data\\genotype-1.txt";
	const char *GeneSnpFileName = "data\\gene-snp-1.txt";
	const char *CovFileName = "data\\snpcov-factored-3.txt";
	const char *SnpPvaluesInFileName = "data\\snp-pvalue-500000.txt";
	const char *SnpPvaluesOutFileName = "data\\snp-pvalue-1.txt";

	list<GeneNode_t> GL = ReadGeneSnpFile(GeneSnpFileName);
	GenotypeMatrix_t GTM = ReadSnpFile(SnpFileName);

	ComputeSNPCovariance(GL, GTM, CovFileName);

	CreateSNPpvaluesFile(GL, SnpPvaluesInFileName, SnpPvaluesOutFileName);
    return 0;
}

